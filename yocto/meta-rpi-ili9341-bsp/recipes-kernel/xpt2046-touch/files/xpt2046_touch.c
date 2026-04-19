// SPDX-License-Identifier: GPL-2.0
/*
 * XPT2046 Touch Controller Driver - OPTIMIZED VERSION
 * 
 * Author: TungNHS
 * Optimized: Removed debug logs, reduced delays for better responsiveness
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/delay.h>

/* XPT2046 commands */
#define XPT2046_CMD_X   0xD0
#define XPT2046_CMD_Y   0x90
#define XPT2046_CMD_Z1  0xB0
#define XPT2046_CMD_Z2  0xC0

/* Default calibration */
#define DEFAULT_X_MIN   200
#define DEFAULT_X_MAX   3900
#define DEFAULT_Y_MIN   200
#define DEFAULT_Y_MAX   3900
#define DEFAULT_P_MIN   30
#define DEFAULT_P_MAX   255

/* Performance tuning */
#define POLL_INTERVAL_MS    5    /* Polling interval khi pen down (was 10) */
#define SETTLE_DELAY_US     100   /* ADC settle time (was 20) - Increased for stability */
#define READ_SAMPLES        3    /* Number of samples to average */

struct xpt2046_cal {
    u16 x_min, x_max;
    u16 y_min, y_max;
    bool swap_xy;
    bool invert_x;
    bool invert_y;
};

struct xpt2046_data {
    struct spi_device *spi;
    struct input_dev *input;
    struct gpio_desc *irq_gpio;
    struct delayed_work work;
    
    int irq;
    bool pendown;
    unsigned int poll_interval_ms;
    
    struct xpt2046_cal cal;
    u16 pressure_min;
    u16 pressure_max;
};

/* Read 12-bit coordinate from ADC - OPTIMIZED */
static u16 xpt2046_read_coord(struct xpt2046_data *data, u8 cmd)
{
    u8 tx[3] = {cmd, 0, 0};
    u8 rx[3] = {0};
    struct spi_transfer xfer = {
        .tx_buf = tx,
        .rx_buf = rx,
        .len = 3,
        .speed_hz = 2000000,  /* Explicit speed for consistency */
    };
    struct spi_message msg;
    
    spi_message_init(&msg);
    spi_message_add_tail(&xfer, &msg);
    
    if (spi_sync(data->spi, &msg) < 0)
        return 0;
    
    return ((rx[1] << 8 | rx[2]) >> 3) & 0xFFF;
}

/* Read position and pressure - OPTIMIZED: reduced delays, single read */
static void xpt2046_read_position(struct xpt2046_data *data, u16 *x, u16 *y, u16 *pressure)
{
    u16 z1, z2;
    u32 sum_x = 0, sum_y = 0, sum_z1 = 0, sum_z2 = 0;
    int i;
    
    /* Read multiple samples and average */
    for (i = 0; i < READ_SAMPLES; i++) {
        sum_x += xpt2046_read_coord(data, XPT2046_CMD_X);
        udelay(SETTLE_DELAY_US);
        
        sum_y += xpt2046_read_coord(data, XPT2046_CMD_Y);
        udelay(SETTLE_DELAY_US);
        
        sum_z1 += xpt2046_read_coord(data, XPT2046_CMD_Z1);
        sum_z2 += xpt2046_read_coord(data, XPT2046_CMD_Z2);
    }

    *x = sum_x / READ_SAMPLES;
    *y = sum_y / READ_SAMPLES;
    z1 = sum_z1 / READ_SAMPLES;
    z2 = sum_z2 / READ_SAMPLES;
    
    /* Calculate pressure */
    if (z1 > 10 && *x > 0) {
        *pressure = (*x * (z2 - z1)) / z1;
        if (*pressure > data->pressure_max)
            *pressure = data->pressure_max;
    } else {
        *pressure = 0;
    }
}

/* Apply calibration to coordinates - OPTIMIZED: inline-friendly */
static void xpt2046_apply_cal(struct xpt2046_data *data, u16 *x, u16 *y)
{
    u16 temp_x = *x;
    u16 temp_y = *y;

    /* Step 1: Swap X/Y if needed */
    if (data->cal.swap_xy) {
        u16 tmp = temp_x;
        temp_x = temp_y;
        temp_y = tmp;
    }

    /* Step 2: Clamp to calibration range */
    if (temp_x < data->cal.x_min) temp_x = data->cal.x_min;
    else if (temp_x > data->cal.x_max) temp_x = data->cal.x_max;
    
    if (temp_y < data->cal.y_min) temp_y = data->cal.y_min;
    else if (temp_y > data->cal.y_max) temp_y = data->cal.y_max;

    /* Step 3: Invert if needed */
    if (data->cal.invert_x)
        temp_x = data->cal.x_max - (temp_x - data->cal.x_min);

    if (data->cal.invert_y)
        temp_y = data->cal.y_max - (temp_y - data->cal.y_min);

    *x = temp_x;
    *y = temp_y;
}

/* Delayed work handler - OPTIMIZED: No debug logs */
static void xpt2046_work_func(struct work_struct *work)
{
    struct xpt2046_data *data = container_of(work, struct xpt2046_data, work.work);
    u16 x, y, pressure;
    int irq_state;

    irq_state = gpiod_get_value(data->irq_gpio);

    if (irq_state == 0) {  /* Pen is down */
        xpt2046_read_position(data, &x, &y, &pressure);
        xpt2046_apply_cal(data, &x, &y);

        if (pressure >= data->pressure_min && pressure <= data->pressure_max) {
            input_report_abs(data->input, ABS_X, x);
            input_report_abs(data->input, ABS_Y, y);
            input_report_abs(data->input, ABS_PRESSURE, pressure);
            input_report_key(data->input, BTN_TOUCH, 1);
            input_sync(data->input);
            data->pendown = true;

            /* Continue polling while pen is down */
            schedule_delayed_work(&data->work, msecs_to_jiffies(data->poll_interval_ms));
        }
    } else {  /* Pen is up */
        if (data->pendown) {
            input_report_key(data->input, BTN_TOUCH, 0);
            input_report_abs(data->input, ABS_PRESSURE, 0);
            input_sync(data->input);
            data->pendown = false;
        }
    }
}

/* IRQ handler - OPTIMIZED: Minimal processing */
static irqreturn_t xpt2046_irq_handler(int irq, void *dev_id)
{
    struct xpt2046_data *data = dev_id;
    
    /* Cancel pending work and schedule new one immediately */
    cancel_delayed_work(&data->work);
    schedule_delayed_work(&data->work, 0);  /* No delay on first touch! */
    
    return IRQ_HANDLED;
}

/* Parse device tree calibration */
static int xpt2046_parse_dt(struct device *dev, struct xpt2046_data *data)
{
    struct device_node *np = dev->of_node;
    u32 val;

    if (!np)
        goto use_defaults;

    data->cal.x_min = (!of_property_read_u32(np, "ti,x-min", &val)) ? val : DEFAULT_X_MIN;
    data->cal.x_max = (!of_property_read_u32(np, "ti,x-max", &val)) ? val : DEFAULT_X_MAX;
    data->cal.y_min = (!of_property_read_u32(np, "ti,y-min", &val)) ? val : DEFAULT_Y_MIN;
    data->cal.y_max = (!of_property_read_u32(np, "ti,y-max", &val)) ? val : DEFAULT_Y_MAX;
    data->pressure_min = (!of_property_read_u32(np, "ti,pressure-min", &val)) ? val : DEFAULT_P_MIN;
    data->pressure_max = (!of_property_read_u32(np, "ti,pressure-max", &val)) ? val : DEFAULT_P_MAX;

    data->cal.swap_xy = of_property_read_bool(np, "ti,swap-xy");
    data->cal.invert_x = of_property_read_bool(np, "ti,invert-x");
    data->cal.invert_y = of_property_read_bool(np, "ti,invert-y");

    return 0;

use_defaults:
    data->cal.x_min = DEFAULT_X_MIN;
    data->cal.x_max = DEFAULT_X_MAX;
    data->cal.y_min = DEFAULT_Y_MIN;
    data->cal.y_max = DEFAULT_Y_MAX;
    data->pressure_min = DEFAULT_P_MIN;
    data->pressure_max = DEFAULT_P_MAX;
    data->cal.swap_xy = false;
    data->cal.invert_x = false;
    data->cal.invert_y = false;

    return 0;
}

/* Device tree match table */
static const struct of_device_id xpt2046_dt_ids[] = {
    { .compatible = "ti,ads7846" },
    { .compatible = "xptek,xpt2046" },
    { }
};
MODULE_DEVICE_TABLE(of, xpt2046_dt_ids);

/* SPI device ID table */
static const struct spi_device_id xpt2046_spi_ids[] = {
    { "xpt2046", 0 },
    { "ads7846", 0 },
    { "ti,ads7846", 0 }, 
    { }
};
MODULE_DEVICE_TABLE(spi, xpt2046_spi_ids);

/* SPI driver probe */
static int xpt2046_probe(struct spi_device *spi)
{
    struct xpt2046_data *data;
    struct input_dev *input;
    int ret;
    
    data = devm_kzalloc(&spi->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;
    
    data->spi = spi;
    data->pendown = false;
    data->poll_interval_ms = POLL_INTERVAL_MS;  /* 5ms for ~200Hz polling */
    
    ret = xpt2046_parse_dt(&spi->dev, data);
    if (ret)
        return ret;
    
    data->irq_gpio = devm_gpiod_get(&spi->dev, "pendown", GPIOD_IN);
    if (IS_ERR(data->irq_gpio))
        return PTR_ERR(data->irq_gpio);
    
    data->irq = gpiod_to_irq(data->irq_gpio);
    if (data->irq < 0)
        return data->irq;
    
    input = devm_input_allocate_device(&spi->dev);
    if (!input)
        return -ENOMEM;
    
    data->input = input;
    
    input->name = "XPT2046 Touchscreen";
    input->phys = "spi/input0";
    input->id.bustype = BUS_SPI;
    input->dev.parent = &spi->dev;
    
    input_set_capability(input, EV_KEY, BTN_TOUCH);
    input_set_abs_params(input, ABS_X, data->cal.x_min, data->cal.x_max, 0, 0);  /* No fuzz */
    input_set_abs_params(input, ABS_Y, data->cal.y_min, data->cal.y_max, 0, 0);  /* No fuzz */
    input_set_abs_params(input, ABS_PRESSURE, 0, data->pressure_max, 0, 0);
    
    ret = input_register_device(input);
    if (ret)
        return ret;
    
    INIT_DELAYED_WORK(&data->work, xpt2046_work_func);

    ret = devm_request_threaded_irq(&spi->dev, data->irq, NULL, xpt2046_irq_handler,
                                     IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
                                     "xpt2046", data);
    if (ret) {
        dev_err(&spi->dev, "Failed to request IRQ: %d\n", ret);
        return ret;
    }
    
    spi_set_drvdata(spi, data);
    
    dev_info(&spi->dev, "XPT2046 touch initialized (optimized, poll=%dms)\n", 
             data->poll_interval_ms);
    
    return 0;
}

/* SPI driver remove */
static int xpt2046_remove(struct spi_device *spi)
{
    struct xpt2046_data *data = spi_get_drvdata(spi);
    
    cancel_delayed_work_sync(&data->work);

    return 0;
}

static struct spi_driver xpt2046_driver = {
    .driver = {
        .name           = "xpt2046_touch",
        .of_match_table = xpt2046_dt_ids,
    },
    .probe  = xpt2046_probe,
    .remove = xpt2046_remove,
    .id_table = xpt2046_spi_ids,
};

module_spi_driver(xpt2046_driver);

MODULE_AUTHOR("TungNHS");
MODULE_DESCRIPTION("XPT2046 Touch Controller Driver");
MODULE_LICENSE("GPL v2");