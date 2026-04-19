/*
 * ILI9341 Framebuffer Display Driver over SPI - OPTIMIZED VERSION
 * 
 * Optimizations:
 * - Use separate SPI buffer (don't modify framebuffer in-place)
 * - Combine R↔B swap and byte swap in single pass
 * - Remove debug logs in hot path
 * - Optimized loop with minimal operations
 * 
 * Author: TungNHS
 * Version: 3.0-OPTIMIZED
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fb.h>
#include <linux/spi/spi.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

#define WIDTH           320
#define HEIGHT          240
#define BPP             16
#define FPS             30
#define VMEM_SIZE       (WIDTH * HEIGHT * BPP / 8)  /* 153,600 bytes */
#define SPI_CHUNK_SIZE  4096                        /* Chunk size for SPI transfer */

#define ILI9341_SWRESET     0x01
#define ILI9341_SLPOUT      0x11
#define ILI9341_DISPOFF     0x28
#define ILI9341_DISPON      0x29
#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_MADCTL      0x36
#define ILI9341_COLMOD      0x3A

/* Extended Commands */
#define ILI9341_POWERA      0xCB
#define ILI9341_POWERB      0xCF
#define ILI9341_DTCA        0xE8
#define ILI9341_DTCB        0xEA
#define ILI9341_POWER_SEQ   0xED
#define ILI9341_PRC         0xF7
#define ILI9341_POWER1      0xC0
#define ILI9341_POWER2      0xC1
#define ILI9341_VCOM1       0xC5
#define ILI9341_VCOM2       0xC7
#define ILI9341_FRMCTR1     0xB1
#define ILI9341_DFC         0xB6
#define ILI9341_GAMMA0      0xE0
#define ILI9341_GAMMA1      0xE1

struct ili9341_par {
    struct spi_device *spi;
    struct fb_info *info;
    struct fb_deferred_io fbdefio;
    
    struct gpio_desc *dc_gpio;
    struct gpio_desc *reset_gpio;
    struct gpio_desc *led_gpio;
    
    u8 *spi_tx_buf;     /* Separate buffer for SPI transmission */
    
    u32 rotation;
    
    /* Performance stats (optional) */
    unsigned long frame_count;
};

/* ============================================
 * LOW-LEVEL SPI FUNCTIONS
 * ============================================ */

static void ili9341_write_cmd(struct ili9341_par *par, u8 cmd)
{
    gpiod_set_value(par->dc_gpio, 0);
    spi_write(par->spi, &cmd, 1);
}

static void ili9341_write_data(struct ili9341_par *par, const void *buf, size_t len)
{
    gpiod_set_value(par->dc_gpio, 1);
    spi_write(par->spi, buf, len);
}

static void ili9341_reset(struct ili9341_par *par)
{
    gpiod_set_value(par->reset_gpio, 1);
    msleep(5);
    gpiod_set_value(par->reset_gpio, 0);
    msleep(20);
    gpiod_set_value(par->reset_gpio, 1);
    msleep(150);
}

/* ============================================
 * DISPLAY INITIALIZATION
 * ============================================ */

static int ili9341_init_display(struct ili9341_par *par)
{
    u8 madctl;

    ili9341_reset(par);
    
    /* Software Reset */
    ili9341_write_cmd(par, ILI9341_SWRESET);
    msleep(150);
    
    /* Power Control A */
    ili9341_write_cmd(par, ILI9341_POWERA);
    ili9341_write_data(par, (u8[]){0x39, 0x2C, 0x00, 0x34, 0x02}, 5);

    /* Power Control B */
    ili9341_write_cmd(par, ILI9341_POWERB);
    ili9341_write_data(par, (u8[]){0x00, 0xC1, 0x30}, 3);

    /* Driver Timing Control A */
    ili9341_write_cmd(par, ILI9341_DTCA);
    ili9341_write_data(par, (u8[]){0x85, 0x00, 0x78}, 3);

    /* Driver Timing Control B */
    ili9341_write_cmd(par, ILI9341_DTCB);
    ili9341_write_data(par, (u8[]){0x00, 0x00}, 2);

    /* Power On Sequence Control */
    ili9341_write_cmd(par, ILI9341_POWER_SEQ);
    ili9341_write_data(par, (u8[]){0x64, 0x03, 0x12, 0x81}, 4);

    /* Pump Ratio Control */
    ili9341_write_cmd(par, ILI9341_PRC);
    ili9341_write_data(par, (u8[]){0x20}, 1);

    /* Power Control 1 */
    ili9341_write_cmd(par, ILI9341_POWER1);
    ili9341_write_data(par, (u8[]){0x23}, 1);

    /* Power Control 2 */
    ili9341_write_cmd(par, ILI9341_POWER2);
    ili9341_write_data(par, (u8[]){0x10}, 1);

    /* VCOM Control 1 */
    ili9341_write_cmd(par, ILI9341_VCOM1);
    ili9341_write_data(par, (u8[]){0x3E, 0x28}, 2);

    /* VCOM Control 2 */
    ili9341_write_cmd(par, ILI9341_VCOM2);
    ili9341_write_data(par, (u8[]){0x86}, 1);

    /* Memory Access Control */
    switch (par->rotation) {
        case 0:   madctl = 0x40; break;
        case 90:  madctl = 0x20; break;
        case 180: madctl = 0x80; break;
        case 270: madctl = 0xE0; break;
        default:  madctl = 0x20; break;
    }
    ili9341_write_cmd(par, ILI9341_MADCTL);
    ili9341_write_data(par, &madctl, 1);

    /* Pixel Format Set */
    ili9341_write_cmd(par, ILI9341_COLMOD);
    ili9341_write_data(par, (u8[]){0x55}, 1);

    /* Frame Rate Control */
    ili9341_write_cmd(par, ILI9341_FRMCTR1);
    ili9341_write_data(par, (u8[]){0x00, 0x18}, 2);

    /* Display Function Control */
    ili9341_write_cmd(par, ILI9341_DFC);
    ili9341_write_data(par, (u8[]){0x08, 0x82, 0x27}, 3);

    /* Gamma Set */
    ili9341_write_cmd(par, 0x26);
    ili9341_write_data(par, (u8[]){0x01}, 1);

    /* Positive Gamma Correction */
    ili9341_write_cmd(par, ILI9341_GAMMA0);
    ili9341_write_data(par, (u8[]){0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00}, 15);

    /* Negative Gamma Correction */
    ili9341_write_cmd(par, ILI9341_GAMMA1);
    ili9341_write_data(par, (u8[]){0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F}, 15);

    /* Sleep Out */
    ili9341_write_cmd(par, ILI9341_SLPOUT);
    msleep(120);
    
    /* Display ON */
    ili9341_write_cmd(par, ILI9341_DISPON);
    msleep(100);
    
    /* Backlight ON */
    gpiod_set_value(par->led_gpio, 1);
    
    return 0;
}

static void ili9341_set_window(struct ili9341_par *par, u16 x0, u16 y0, u16 x1, u16 y1)
{
    u8 buf[4];
    
    buf[0] = x0 >> 8;
    buf[1] = x0 & 0xFF;
    buf[2] = x1 >> 8;
    buf[3] = x1 & 0xFF;
    ili9341_write_cmd(par, ILI9341_CASET);
    ili9341_write_data(par, buf, 4);
    
    buf[0] = y0 >> 8;
    buf[1] = y0 & 0xFF;
    buf[2] = y1 >> 8;
    buf[3] = y1 & 0xFF;
    ili9341_write_cmd(par, ILI9341_PASET);
    ili9341_write_data(par, buf, 4);
    
    ili9341_write_cmd(par, ILI9341_RAMWR);
}

/* SOFTWARE R↔B SWAP for RGB565 */
static inline u16 swap_rb_rgb565(u16 pixel)
{
    u16 r = (pixel >> 11) & 0x1F;
    u16 g = (pixel >> 5) & 0x3F;
    u16 b = pixel & 0x1F;
    return (b << 11) | (g << 5) | r;
}

/* ============================================
 * OPTIMIZED PIXEL CONVERSION
 * 
 * Combines R↔B swap + byte swap in SINGLE PASS
 * Input:  RGB565 little-endian (framebuffer)
 * Output: BGR565 big-endian (SPI to display)
 * ============================================ */

static inline void convert_pixel_optimized(const u8 *src, u8 *dst)
{
    /*
     * Input (little-endian RGB565):
     *   src[0] = GGGBBBBB (low byte)
     *   src[1] = RRRRRGGG (high byte)
     * 
     * After R↔B swap → BGR565:
     *   BBBBBGGG GGGRRRRR
     * 
     * After byte swap for SPI (big-endian):
     *   dst[0] = BBBBBGGG (high byte first)
     *   dst[1] = GGGRRRRR (low byte second)
     * 
     * Optimized: Do both swaps in one operation
     */
    u16 pixel = src[0] | (src[1] << 8);
    u16 swapped = swap_rb_rgb565(pixel);
    
    dst[0] = swapped >> 8;
    dst[1] = swapped & 0xFF;
}

/* ============================================
 * DEFERRED I/O - MAIN DISPLAY UPDATE
 * ============================================ */

static void ili9341_deferred_io(struct fb_info *info, struct list_head *pagelist)
{
    struct ili9341_par *par = info->par;
    const u8 *src = (const u8 *)info->screen_buffer;
    u8 *dst = par->spi_tx_buf;
    int i;
    
    par->frame_count++;
    
    /* Set full screen window */
    ili9341_set_window(par, 0, 0, WIDTH - 1, HEIGHT - 1);
    gpiod_set_value(par->dc_gpio, 1);
    
    /* 
     * OPTIMIZED: Chunk-based processing
     * - Saves memory (no need for full 153KB second buffer)
     * - DMA safe (uses small kmalloc buffer)
     * - Allows touch driver to preempt between chunks
     */
    for (i = 0; i < VMEM_SIZE; i += SPI_CHUNK_SIZE) {
        size_t len = (VMEM_SIZE - i) < SPI_CHUNK_SIZE ? (VMEM_SIZE - i) : SPI_CHUNK_SIZE;
        int j;
        
        /* Convert chunk */
        for (j = 0; j < len; j += 2) {
            convert_pixel_optimized(&src[i + j], &dst[j]);
        }
        
        /* Send chunk */
        spi_write(par->spi, dst, len);
    }
}

/* ============================================
 * FRAMEBUFFER OPERATIONS
 * ============================================ */

static struct fb_ops ili9341_ops = {
    .owner        = THIS_MODULE,
    .fb_read      = fb_sys_read,
    .fb_write     = fb_sys_write,
    .fb_fillrect  = sys_fillrect,
    .fb_copyarea  = sys_copyarea,
    .fb_imageblit = sys_imageblit,
};

/* ============================================
 * SYSFS ATTRIBUTES (optional debug)
 * ============================================ */

static ssize_t frame_count_show(struct device *dev,
                                struct device_attribute *attr, char *buf)
{
    struct fb_info *info = dev_get_drvdata(dev);
    struct ili9341_par *par = info->par;
    return sprintf(buf, "%lu\n", par->frame_count);
}

static DEVICE_ATTR_RO(frame_count);

static struct attribute *ili9341_attrs[] = {
    &dev_attr_frame_count.attr,
    NULL,
};

static const struct attribute_group ili9341_attr_group = {
    .attrs = ili9341_attrs,
};

/* ============================================
 * DRIVER PROBE / REMOVE
 * ============================================ */

static int ili9341_probe(struct spi_device *spi)
{
    struct fb_info *info;
    struct ili9341_par *par;
    int ret;
    u8 *vmem;
    
    dev_info(&spi->dev, "ILI9341 OPTIMIZED driver probe\n");
    
    info = framebuffer_alloc(sizeof(struct ili9341_par), &spi->dev);
    if (!info)
        return -ENOMEM;
    
    par = info->par;
    par->spi = spi;
    par->info = info;
    par->frame_count = 0;
    
    spi_set_drvdata(spi, info);
    
    /* SPI setup */
    spi->mode = SPI_MODE_0;
    spi->bits_per_word = 8;
    ret = spi_setup(spi);
    if (ret < 0)
        goto err_fb;
    
    /* GPIO setup */
    par->dc_gpio = devm_gpiod_get(&spi->dev, "dc", GPIOD_OUT_LOW);
    if (IS_ERR(par->dc_gpio)) {
        ret = PTR_ERR(par->dc_gpio);
        goto err_fb;
    }
    
    par->reset_gpio = devm_gpiod_get(&spi->dev, "reset", GPIOD_OUT_HIGH);
    if (IS_ERR(par->reset_gpio)) {
        ret = PTR_ERR(par->reset_gpio);
        goto err_fb;
    }
    
    par->led_gpio = devm_gpiod_get(&spi->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(par->led_gpio)) {
        ret = PTR_ERR(par->led_gpio);
        goto err_fb;
    }

    /* Read rotation from device tree */
    ret = of_property_read_u32(spi->dev.of_node, "rotation", &par->rotation);
    if (ret)
        par->rotation = 90;
    
    /* Allocate framebuffer memory */
    vmem = vzalloc(VMEM_SIZE);
    if (!vmem) {
        ret = -ENOMEM;
        goto err_fb;
    }
    
    /* Allocate small DMA-safe buffer for SPI chunks */
    par->spi_tx_buf = devm_kzalloc(&spi->dev, SPI_CHUNK_SIZE, GFP_KERNEL);
    if (!par->spi_tx_buf) {
        ret = -ENOMEM;
        goto err_vmem;
    }
    
    info->screen_buffer = vmem;
    info->fbops = &ili9341_ops;
    
    info->fix = (struct fb_fix_screeninfo) {
        .id             = "ili9341",
        .type           = FB_TYPE_PACKED_PIXELS,
        .visual         = FB_VISUAL_TRUECOLOR,
        .xpanstep       = 0,
        .ypanstep       = 0,
        .ywrapstep      = 0,
        .line_length    = WIDTH * BPP / 8,
        .accel          = FB_ACCEL_NONE,
        .smem_start     = (unsigned long)vmem,
        .smem_len       = VMEM_SIZE,
    };
    
    info->var = (struct fb_var_screeninfo) {
        .xres           = WIDTH,
        .yres           = HEIGHT,
        .xres_virtual   = WIDTH,
        .yres_virtual   = HEIGHT,
        .bits_per_pixel = BPP,
        .red            = { .offset = 11, .length = 5 },
        .green          = { .offset = 5,  .length = 6 },
        .blue           = { .offset = 0,  .length = 5 },
        .transp         = { .offset = 0,  .length = 0 },
    };
    
    info->flags = FBINFO_HWACCEL_DISABLED | FBINFO_VIRTFB;
    
    /* Deferred I/O setup */
    par->fbdefio.delay = HZ / FPS;
    par->fbdefio.deferred_io = ili9341_deferred_io;
    info->fbdefio = &par->fbdefio;
    
    fb_deferred_io_init(info);
    
    /* Initialize display hardware */
    ret = ili9341_init_display(par);
    if (ret)
        goto err_defio;
    
    /* Register framebuffer */
    ret = register_framebuffer(info);
    if (ret < 0)
        goto err_defio;
    
    /* Create sysfs entries */
    ret = sysfs_create_group(&spi->dev.kobj, &ili9341_attr_group);
    if (ret)
        dev_warn(&spi->dev, "Failed to create sysfs group\n");
    
    dev_info(&spi->dev, "ILI9341 OPTIMIZED driver loaded (rotation=%d, fps=%d)\n",
             par->rotation, FPS);
    
    return 0;

err_defio:
    fb_deferred_io_cleanup(info);
    /* par->spi_tx_buf is devm_ managed, no need to free */
err_vmem:
    vfree(vmem);
err_fb:
    framebuffer_release(info);
    return ret;
}

static int ili9341_remove(struct spi_device *spi)
{
    struct fb_info *info = spi_get_drvdata(spi);
    struct ili9341_par *par = info->par;
    
    /* Turn off display and backlight */
    gpiod_set_value(par->led_gpio, 0);
    ili9341_write_cmd(par, ILI9341_DISPOFF);
    
    sysfs_remove_group(&spi->dev.kobj, &ili9341_attr_group);
    unregister_framebuffer(info);
    fb_deferred_io_cleanup(info);
    
    /* par->spi_tx_buf is devm_ managed */
    vfree(info->screen_buffer);
    framebuffer_release(info);
    
    return 0;
}

static const struct of_device_id ili9341_of_match[] = {
    { .compatible = "ilitek,ili9341" },
    { }
};
MODULE_DEVICE_TABLE(of, ili9341_of_match);

static struct spi_driver ili9341_driver = {
    .driver = {
        .name           = "ili9341",
        .of_match_table = ili9341_of_match,
    },
    .probe  = ili9341_probe,
    .remove = ili9341_remove,
};

module_spi_driver(ili9341_driver);

MODULE_AUTHOR("TungNHS");
MODULE_DESCRIPTION("ILI9341 framebuffer display driver over SPI");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("3.0");
