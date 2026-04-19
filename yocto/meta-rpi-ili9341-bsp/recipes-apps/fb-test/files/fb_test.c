/*
 * Framebuffer Test for ILI9341 - RGB565 FORMAT (STANDARD)
 * 
 * Author: TungNHS
 * Version: 2.2-RGB
 * 
 * RGB565 COLOR FORMAT (Standard Linux framebuffer)
 * ===================================================
 * 
 * This test uses STANDARD RGB565 format for RGB hardware (BGR=0).
 * Your ILI9341 hardware is RGB wired (uncommon variant).
 * 
 * RGB565 16-bit Format:
 * ---------------------
 * Bit layout: RRRRR GGGGGG BBBBB
 * 
 * Bits 15-11: RED channel (5 bits)
 * Bits 10-5:  GREEN channel (6 bits)
 * Bits 4-0:   BLUE channel (5 bits)
 * 
 * Color Examples:
 * ---------------
 * RED   = 0xF800 = 11111|000000|00000 (max red, no green, no blue)
 * GREEN = 0x07E0 = 00000|111111|00000 (no red, max green, no blue)
 * BLUE  = 0x001F = 00000|000000|11111 (no red, no green, max blue)
 * WHITE = 0xFFFF = 11111|111111|11111 (all channels max)
 * BLACK = 0x0000 = 00000|000000|00000 (all channels off)
 * 
 * Expected Results with RGB Driver (BGR=0):
 * ------------------------------------------
 * Test 1: RED (0xF800)   → Display shows RED 
 * Test 2: GREEN (0x07E0) → Display shows GREEN 
 * Test 3: BLUE (0x001F)  → Display shows BLUE 
 * Test 4: WHITE (0xFFFF) → Display shows WHITE
 * Test 5: Pattern        → Correct colors
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define WIDTH  320
#define HEIGHT 240

/* STANDARD RGB565 COLOR DEFINITIONS
 * These match standard Linux framebuffer and RGB hardware (BGR=0)
 */
#define BLACK   0x0000  /* 00000|000000|00000 - All off */
#define RED     0xF800  /* 11111|000000|00000 - Bits 15-11: RED max */
#define GREEN   0x07E0  /* 00000|111111|00000 - Bits 10-5: GREEN max */
#define BLUE    0x001F  /* 00000|000000|11111 - Bits 4-0: BLUE max */
#define YELLOW  0xFFE0  /* 11111|111111|00000 - RED + GREEN */
#define CYAN    0x07FF  /* 00000|111111|11111 - GREEN + BLUE */
#define MAGENTA 0xF81F  /* 11111|000000|11111 - RED + BLUE */
#define WHITE   0xFFFF  /* 11111|111111|11111 - All max */

/* Custom colors in RGB565 format */
#define ORANGE  0xFC20  /* 11111|100001|00000 - Max red, some green, no blue */
#define PURPLE  0x801F  /* 10000|000000|11111 - Some red, no green, max blue */
#define PINK    0xF81F  /* 11111|000000|11111 - Max red, no green, max blue */

static void fill_screen(uint16_t *fb, uint16_t color)
{
    for (int i = 0; i < WIDTH * HEIGHT; i++)
        fb[i] = color;
}

static void draw_pattern(uint16_t *fb)
{
    /* Draw 4-quadrant pattern in RGB565:
     * Top-left:     RED
     * Top-right:    GREEN
     * Bottom-left:  BLUE
     * Bottom-right: YELLOW
     */
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x < WIDTH/2 && y < HEIGHT/2)
                fb[y * WIDTH + x] = RED;
            else if (x >= WIDTH/2 && y < HEIGHT/2)
                fb[y * WIDTH + x] = GREEN;
            else if (x < WIDTH/2 && y >= HEIGHT/2)
                fb[y * WIDTH + x] = BLUE;
            else
                fb[y * WIDTH + x] = YELLOW;
        }
    }
}

static void draw_gradient(uint16_t *fb)
{
    /* Draw RED gradient from left (dark) to right (bright)
     * In RGB565, RED is at bits 15-11
     */
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            uint8_t val = (x * 31) / WIDTH;  /* Scale 0-31 for 5-bit RED */
            uint16_t color = (val << 11);    /* Shift to RED position (bits 15-11) */
            fb[y * WIDTH + x] = color;
        }
    }
}

static void draw_rgb_bars(uint16_t *fb)
{
    /* Draw vertical RGB color bars
     * RGB565 format: RED|GREEN|BLUE
     */
    int bar_width = WIDTH / 7;
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int bar = x / bar_width;
            uint16_t color;
            
            switch (bar) {
                case 0: color = BLACK;   break;
                case 1: color = RED;     break;
                case 2: color = GREEN;   break;
                case 3: color = BLUE;    break;
                case 4: color = CYAN;    break;
                case 5: color = MAGENTA; break;
                default: color = WHITE;  break;
            }
            
            fb[y * WIDTH + x] = color;
        }
    }
}

static void draw_checkerboard(uint16_t *fb)
{
    /* Draw checkerboard pattern: alternating WHITE and BLACK
     * Good for testing pixel alignment
     */
    int square_size = 20;
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int check_x = (x / square_size) % 2;
            int check_y = (y / square_size) % 2;
            
            if ((check_x + check_y) % 2 == 0)
                fb[y * WIDTH + x] = WHITE;
            else
                fb[y * WIDTH + x] = BLACK;
        }
    }
}

static void draw_color_blocks(uint16_t *fb)
{
    /* Draw large color blocks for easy visual verification
     * 2x3 grid of solid colors
     */
    int block_w = WIDTH / 2;
    int block_h = HEIGHT / 3;
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int block_x = x / block_w;
            int block_y = y / block_h;
            uint16_t color;
            
            if (block_y == 0) {
                color = (block_x == 0) ? RED : GREEN;
            } else if (block_y == 1) {
                color = (block_x == 0) ? BLUE : CYAN;
            } else {
                color = (block_x == 0) ? MAGENTA : YELLOW;
            }
            
            fb[y * WIDTH + x] = color;
        }
    }
}

static void print_color_info(void)
{
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("  ILI9341 RGB565 COLOR TEST (STANDARD)\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("\n");
    printf("Format: RGB565 (16-bit) - Standard Linux Framebuffer\n");
    printf("  Bits 15-11: RED   (5 bits)\n");
    printf("  Bits 10-5:  GREEN (6 bits)\n");
    printf("  Bits 4-0:   BLUE  (5 bits)\n");
    printf("\n");
    printf("Color Definitions:\n");
    printf("  BLACK   = 0x%04X  (Binary: %05d|%06d|%05d)\n", BLACK,
           (BLACK >> 11) & 0x1F, (BLACK >> 5) & 0x3F, BLACK & 0x1F);
    printf("  RED     = 0x%04X  (Binary: %05d|%06d|%05d)\n", RED,
           (RED >> 11) & 0x1F, (RED >> 5) & 0x3F, RED & 0x1F);
    printf("  GREEN   = 0x%04X  (Binary: %05d|%06d|%05d)\n", GREEN,
           (GREEN >> 11) & 0x1F, (GREEN >> 5) & 0x3F, GREEN & 0x1F);
    printf("  BLUE    = 0x%04X  (Binary: %05d|%06d|%05d)\n", BLUE,
           (BLUE >> 11) & 0x1F, (BLUE >> 5) & 0x3F, BLUE & 0x1F);
    printf("  WHITE   = 0x%04X  (Binary: %05d|%06d|%05d)\n", WHITE,
           (WHITE >> 11) & 0x1F, (WHITE >> 5) & 0x3F, WHITE & 0x1F);
    printf("  CYAN    = 0x%04X  (GREEN + BLUE)\n", CYAN);
    printf("  MAGENTA = 0x%04X  (RED + BLUE)\n", MAGENTA);
    printf("  YELLOW  = 0x%04X  (RED + GREEN)\n", YELLOW);
    printf("\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    const char *fb_dev = (argc > 1) ? argv[1] : "/dev/fb1";
    int fd;
    uint16_t *fb;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long screensize;
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  ILI9341 Framebuffer Test - RGB565 Format (STANDARD) ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Device: %s\n", fb_dev);
    
    fd = open(fb_dev, O_RDWR);
    if (fd < 0) {
        perror("Failed to open framebuffer");
        return 1;
    }
    
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) < 0) {
        perror("Failed to get fixed info");
        close(fd);
        return 1;
    }
    
    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("Failed to get variable info");
        close(fd);
        return 1;
    }
    
    printf("\nFramebuffer Info:\n");
    printf("  Resolution: %dx%d\n", vinfo.xres, vinfo.yres);
    printf("  Bits per pixel: %d\n", vinfo.bits_per_pixel);
    printf("  Color format:\n");
    printf("    RED:   offset=%d, length=%d (bits %d-%d)\n", 
           vinfo.red.offset, vinfo.red.length,
           vinfo.red.offset, vinfo.red.offset + vinfo.red.length - 1);
    printf("    GREEN: offset=%d, length=%d (bits %d-%d)\n", 
           vinfo.green.offset, vinfo.green.length,
           vinfo.green.offset, vinfo.green.offset + vinfo.green.length - 1);
    printf("    BLUE:  offset=%d, length=%d (bits %d-%d)\n", 
           vinfo.blue.offset, vinfo.blue.length,
           vinfo.blue.offset, vinfo.blue.offset + vinfo.blue.length - 1);
    
    /* Verify RGB565 format */
    if (vinfo.red.offset == 11 && vinfo.green.offset == 5 && vinfo.blue.offset == 0) {
        printf("\nFramebuffer is correctly configured as RGB565 (STANDARD)\n");
    } else {
        printf("\n WARNING: Framebuffer format doesn't match RGB565!\n");
        printf("    Expected: R@11, G@5, B@0\n");
        printf("    Got:      R@%d, G@%d, B@%d\n", 
               vinfo.red.offset, vinfo.green.offset, vinfo.blue.offset);
    }
    
    screensize = vinfo.yres * finfo.line_length;
    
    fb = (uint16_t *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fb == MAP_FAILED) {
        perror(" Failed to mmap framebuffer");
        close(fd);
        return 1;
    }
    
    print_color_info();
    
    printf("Starting color tests (2 seconds each)...\n\n");
    
    printf("Test 1: RED (0x%04X)\n", RED);
    printf("  → Display should show pure RED\n");
    fill_screen(fb, RED);
    msync(fb, screensize, MS_SYNC);
    sleep(2);
    
    printf("\nTest 2: GREEN (0x%04X)\n", GREEN);
    printf("  → Display should show pure GREEN\n");
    fill_screen(fb, GREEN);
    msync(fb, screensize, MS_SYNC);
    sleep(2);
    
    printf("\nTest 3: BLUE (0x%04X)\n", BLUE);
    printf("  → Display should show pure BLUE\n");
    fill_screen(fb, BLUE);
    msync(fb, screensize, MS_SYNC);
    sleep(2);
    
    printf("\nTest 4: WHITE (0x%04X)\n", WHITE);
    printf("  → Display should show pure WHITE\n");
    fill_screen(fb, WHITE);
    msync(fb, screensize, MS_SYNC);
    sleep(2);
    
    printf("\nTest 5: CYAN (0x%04X) - GREEN+BLUE\n", CYAN);
    printf("  → Display should show CYAN\n");
    fill_screen(fb, CYAN);
    msync(fb, screensize, MS_SYNC);
    sleep(2);
    
    printf("\nTest 6: MAGENTA (0x%04X) - RED+BLUE\n", MAGENTA);
    printf("  → Display should show MAGENTA\n");
    fill_screen(fb, MAGENTA);
    msync(fb, screensize, MS_SYNC);
    sleep(2);
    
    printf("\nTest 7: YELLOW (0x%04X) - RED+GREEN\n", YELLOW);
    printf("  → Display should show YELLOW\n");
    fill_screen(fb, YELLOW);
    msync(fb, screensize, MS_SYNC);
    sleep(2);
    
    printf("\nTest 8: Four-Quadrant Pattern\n");
    printf("  → Top-left: RED, Top-right: GREEN\n");
    printf("  → Bottom-left: BLUE, Bottom-right: YELLOW\n");
    draw_pattern(fb);
    msync(fb, screensize, MS_SYNC);
    sleep(3);
    
    printf("\nTest 9: RED Gradient (dark → bright)\n");
    printf("  → Should show smooth RED gradient\n");
    draw_gradient(fb);
    msync(fb, screensize, MS_SYNC);
    sleep(3);
    
    printf("\nTest 10: Color Bars\n");
    printf("  → Seven vertical bars: BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, WHITE\n");
    draw_rgb_bars(fb);
    msync(fb, screensize, MS_SYNC);
    sleep(3);
    
    printf("\nTest 11: Color Blocks (2x3 grid)\n");
    printf("  → Large solid color blocks for verification\n");
    draw_color_blocks(fb);
    msync(fb, screensize, MS_SYNC);
    sleep(3);
    
    printf("\nTest 12: Checkerboard Pattern\n");
    printf("  → Alternating WHITE and BLACK squares\n");
    draw_checkerboard(fb);
    msync(fb, screensize, MS_SYNC);
    sleep(3);
    
    printf("\nTest 13: BLACK (clear screen)\n");
    printf("  → Display should show BLACK (screen off)\n");
    fill_screen(fb, BLACK);
    msync(fb, screensize, MS_SYNC);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║     All tests completed!                              ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("If colors displayed correctly:\n");
    printf("  Driver RGB mode is working (BGR=0)\n");
    printf("  Framebuffer RGB565 format is correct\n");
    printf("  SPI byte swap is functioning\n");
    printf("  Your hardware is RGB wired (uncommon variant)\n");
    printf("\n");
    printf("If colors are still swapped (RED↔BLUE):\n");
    printf("  May need BGR mode instead (most common)\n");
    printf("  Check hardware wiring\n");
    printf("\n");
    
    munmap(fb, screensize);
    close(fd);
    
    return 0;
}