// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS
/*
 * XPT2046 SPI touch controller driver using PENIRQ and delayed polling.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/delay.h>

#define XPT2046_CMD_X   0xD0
#define XPT2046_CMD_Y   0x90
#define XPT2046_CMD_Z1  0xB0
#define XPT2046_CMD_Z2  0xC0

#define DEFAULT_X_MIN   200
#define DEFAULT_X_MAX   3900
#define DEFAULT_Y_MIN   200
#define DEFAULT_Y_MAX   3900
#define DEFAULT_P_MIN   30
#define DEFAULT_P_MAX   255

#define POLL_INTERVAL_MS        20
#define SETTLE_DELAY_US         100
#define READ_SAMPLES            5
#define RELEASE_DEBOUNCE_COUNT  2
#define COORD_JITTER            8

/**
 * struct xpt2046_cal - touch calibration and orientation
 * @x_min: minimum raw/normalized X coordinate.
 * @x_max: maximum raw/normalized X coordinate.
 * @y_min: minimum raw/normalized Y coordinate.
 * @y_max: maximum raw/normalized Y coordinate.
 * @swap_xy: swap X/Y axes for the mounted display orientation.
 * @invert_x: invert X coordinates after optional axis swap.
 * @invert_y: invert Y coordinates after optional axis swap.
 */
struct xpt2046_cal {
	u16 x_min;
	u16 x_max;
	u16 y_min;
	u16 y_max;
	bool swap_xy;
	bool invert_x;
	bool invert_y;
};

/**
 * struct xpt2046_data - per-device touch controller state
 * @spi: SPI device for the XPT2046 controller on chip select 1.
 * @input: Linux input device reporting ABS_X, ABS_Y, pressure, and touch.
 * @irq_gpio: active-low PENIRQ GPIO descriptor.
 * @work: delayed work used to poll coordinates outside IRQ context.
 * @irq: Linux IRQ number derived from @irq_gpio.
 * @pendown: tracks whether the last reported state was pressed.
 * @have_last: tracks whether @last_x and @last_y contain valid coordinates.
 * @last_x: last reported X coordinate after calibration.
 * @last_y: last reported Y coordinate after calibration.
 * @penup_count: consecutive release samples required before reporting pen up.
 * @poll_interval_ms: polling interval while the pen remains down.
 * @cal: calibration and orientation settings from device tree.
 * @pressure_min: minimum pressure accepted as a valid touch.
 * @pressure_max: maximum pressure reported to the input subsystem.
 */
struct xpt2046_data {
	struct spi_device *spi;
	struct input_dev *input;
	struct gpio_desc *irq_gpio;
	struct delayed_work work;

	int irq;
	bool pendown;
	bool have_last;
	u16 last_x;
	u16 last_y;
	u8 penup_count;
	unsigned int poll_interval_ms;

	struct xpt2046_cal cal;
	u16 pressure_min;
	u16 pressure_max;
};

static u16 xpt2046_read_coord(struct xpt2046_data *data, u8 cmd)
{
	u8 tx[3] = {cmd, 0, 0};
	u8 rx[3] = {0};
	struct spi_transfer xfer = {
	.tx_buf = tx,
	.rx_buf = rx,
	.len = 3,
	.speed_hz = 2000000,
	};
	struct spi_message msg;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	if (spi_sync(data->spi, &msg) < 0)
		return 0;

	return ((rx[1] << 8 | rx[2]) >> 3) & 0xFFF;
}

static void xpt2046_sort_u16(u16 *values, size_t count)
{
	size_t i;

	for (i = 1; i < count; i++) {
		u16 key = values[i];
		size_t j = i;

		while (j > 0 && values[j - 1] > key) {
			values[j] = values[j - 1];
			j--;
		}

		values[j] = key;
	}
}

static bool xpt2046_read_position(struct xpt2046_data *data,
				  u16 *x, u16 *y, u16 *pressure)
{
	u16 z1, z2;
	u16 xs[READ_SAMPLES];
	u16 ys[READ_SAMPLES];
	u16 z1s[READ_SAMPLES];
	u16 z2s[READ_SAMPLES];
	u32 pressure_calc;
	int i;

	for (i = 0; i < READ_SAMPLES; i++) {
		xs[i] = xpt2046_read_coord(data, XPT2046_CMD_X);
		udelay(SETTLE_DELAY_US);

		ys[i] = xpt2046_read_coord(data, XPT2046_CMD_Y);
		udelay(SETTLE_DELAY_US);

		z1s[i] = xpt2046_read_coord(data, XPT2046_CMD_Z1);
		z2s[i] = xpt2046_read_coord(data, XPT2046_CMD_Z2);
	}

	xpt2046_sort_u16(xs, READ_SAMPLES);
	xpt2046_sort_u16(ys, READ_SAMPLES);
	xpt2046_sort_u16(z1s, READ_SAMPLES);
	xpt2046_sort_u16(z2s, READ_SAMPLES);

	*x = xs[READ_SAMPLES / 2];
	*y = ys[READ_SAMPLES / 2];
	z1 = z1s[READ_SAMPLES / 2];
	z2 = z2s[READ_SAMPLES / 2];

	if (z1 <= 10 || z2 < z1 || *x == 0)
		return false;

	pressure_calc = ((u32)*x * (z2 - z1)) / z1;
	*pressure = min_t(u32, pressure_calc, data->pressure_max);

	return *pressure >= data->pressure_min;
}

static void xpt2046_apply_cal(struct xpt2046_data *data, u16 *x, u16 *y)
{
	u16 temp_x = *x;
	u16 temp_y = *y;

	if (data->cal.swap_xy) {
		u16 tmp = temp_x;

		temp_x = temp_y;
		temp_y = tmp;
	}

	if (temp_x < data->cal.x_min)
		temp_x = data->cal.x_min;
	else if (temp_x > data->cal.x_max)
		temp_x = data->cal.x_max;

	if (temp_y < data->cal.y_min)
		temp_y = data->cal.y_min;
	else if (temp_y > data->cal.y_max)
		temp_y = data->cal.y_max;

	if (data->cal.invert_x)
		temp_x = data->cal.x_max - (temp_x - data->cal.x_min);

	if (data->cal.invert_y)
		temp_y = data->cal.y_max - (temp_y - data->cal.y_min);

	*x = temp_x;
	*y = temp_y;
}

static bool xpt2046_coord_changed(struct xpt2046_data *data, u16 x, u16 y)
{
	if (!data->have_last)
		return true;

	return abs((int)x - (int)data->last_x) >= COORD_JITTER ||
	       abs((int)y - (int)data->last_y) >= COORD_JITTER;
}

static void xpt2046_report_touch(struct xpt2046_data *data,
				 u16 x, u16 y, u16 pressure)
{
	input_report_abs(data->input, ABS_X, x);
	input_report_abs(data->input, ABS_Y, y);
	input_report_abs(data->input, ABS_PRESSURE, pressure);
	input_report_key(data->input, BTN_TOUCH, 1);
	input_sync(data->input);

	data->pendown = true;
	data->have_last = true;
	data->last_x = x;
	data->last_y = y;
}

static void xpt2046_report_release(struct xpt2046_data *data)
{
	input_report_key(data->input, BTN_TOUCH, 0);
	input_report_abs(data->input, ABS_PRESSURE, 0);
	input_sync(data->input);

	data->pendown = false;
	data->have_last = false;
	data->penup_count = 0;
}

static void xpt2046_handle_penup(struct xpt2046_data *data)
{
	if (!data->pendown)
		return;

	if (++data->penup_count < RELEASE_DEBOUNCE_COUNT) {
		schedule_delayed_work(&data->work,
				      msecs_to_jiffies(data->poll_interval_ms));
		return;
	}

	xpt2046_report_release(data);
}

static void xpt2046_work_func(struct work_struct *work)
{
	struct xpt2046_data *data = container_of(work, struct xpt2046_data, work.work);
	u16 x, y, pressure;
	int irq_state;

	irq_state = gpiod_get_value(data->irq_gpio);

	if (irq_state == 0) {
		if (!xpt2046_read_position(data, &x, &y, &pressure)) {
			xpt2046_handle_penup(data);
			return;
		}

		data->penup_count = 0;
		xpt2046_apply_cal(data, &x, &y);

		if (xpt2046_coord_changed(data, x, y))
			xpt2046_report_touch(data, x, y, pressure);

		schedule_delayed_work(&data->work,
				      msecs_to_jiffies(data->poll_interval_ms));
		return;
	}

	xpt2046_handle_penup(data);
}

static irqreturn_t xpt2046_irq_handler(int irq, void *dev_id)
{
	struct xpt2046_data *data = dev_id;

	cancel_delayed_work(&data->work);
	schedule_delayed_work(&data->work, 0);

	return IRQ_HANDLED;
}

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

static const struct of_device_id xpt2046_dt_ids[] = {
	{ .compatible = "ti,ads7846" },
	{ .compatible = "xptek,xpt2046" },
	{ }
};
MODULE_DEVICE_TABLE(of, xpt2046_dt_ids);

static const struct spi_device_id xpt2046_spi_ids[] = {
	{ "xpt2046", 0 },
	{ "ads7846", 0 },
	{ "ti,ads7846", 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, xpt2046_spi_ids);

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
	data->have_last = false;
	data->penup_count = 0;
	data->poll_interval_ms = POLL_INTERVAL_MS;

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
	input_set_abs_params(input, ABS_X, data->cal.x_min, data->cal.x_max, COORD_JITTER, 0);
	input_set_abs_params(input, ABS_Y, data->cal.y_min, data->cal.y_max, COORD_JITTER, 0);
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

	dev_info(&spi->dev, "XPT2046 touch initialized (poll=%dms)\n",
		 data->poll_interval_ms);

	return 0;
}

static int xpt2046_remove(struct spi_device *spi)
{
	struct xpt2046_data *data = spi_get_drvdata(spi);

	cancel_delayed_work_sync(&data->work);
	return 0;
}

static struct spi_driver xpt2046_driver = {
	.driver = {
		.name           = "ili9341_touch",
		.of_match_table = xpt2046_dt_ids,
	},
	.probe  = xpt2046_probe,
	.remove = xpt2046_remove,
	.id_table = xpt2046_spi_ids,
};

module_spi_driver(xpt2046_driver);

MODULE_AUTHOR("TungNHS");
MODULE_DESCRIPTION("XPT2046 SPI touch controller");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0.0");
