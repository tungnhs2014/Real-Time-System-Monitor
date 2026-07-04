// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS
/*
 * Userspace RGB565 framebuffer color test.
 */

#include <errno.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define DEFAULT_FB_DEV "/dev/fb1"
#define WIDTH          320
#define HEIGHT         240

#define BLACK          0x0000
#define RED            0xF800
#define GREEN          0x07E0
#define BLUE           0x001F
#define YELLOW         0xFFE0
#define CYAN           0x07FF
#define MAGENTA        0xF81F
#define WHITE          0xFFFF

static void fill_screen(uint16_t *fb, uint16_t color)
{
	int i;

	for (i = 0; i < WIDTH * HEIGHT; i++)
		fb[i] = color;
}

static void draw_pattern(uint16_t *fb)
{
	int x;
	int y;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			if (x < WIDTH / 2 && y < HEIGHT / 2)
				fb[y * WIDTH + x] = RED;
			else if (x >= WIDTH / 2 && y < HEIGHT / 2)
				fb[y * WIDTH + x] = GREEN;
			else if (x < WIDTH / 2)
				fb[y * WIDTH + x] = BLUE;
			else
				fb[y * WIDTH + x] = YELLOW;
		}
	}
}

static void draw_red_gradient(uint16_t *fb)
{
	int x;
	int y;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			uint16_t red = (uint16_t)((x * 31) / WIDTH);

			fb[y * WIDTH + x] = red << 11;
		}
	}
}

static void draw_color_bars(uint16_t *fb)
{
	static const uint16_t colors[] = {
		BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, WHITE,
	};
	const int bar_width = WIDTH / (int)(sizeof(colors) / sizeof(colors[0]));
	int x;
	int y;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			int bar = x / bar_width;

			if (bar >= (int)(sizeof(colors) / sizeof(colors[0])))
				bar = (int)(sizeof(colors) / sizeof(colors[0])) - 1;

			fb[y * WIDTH + x] = colors[bar];
		}
	}
}

static void draw_checkerboard(uint16_t *fb)
{
	const int square_size = 20;
	int x;
	int y;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			int check_x = (x / square_size) % 2;
			int check_y = (y / square_size) % 2;

			fb[y * WIDTH + x] = ((check_x + check_y) % 2) ? BLACK : WHITE;
		}
	}
}

static int verify_rgb565(const struct fb_var_screeninfo *vinfo)
{
	if (vinfo->red.offset == 11 &&
	    vinfo->green.offset == 5 &&
	    vinfo->blue.offset == 0 &&
	    vinfo->bits_per_pixel == 16) {
		printf("Framebuffer format: RGB565\n");
		return 0;
	}

	printf("Framebuffer format warning:\n");
	printf("  expected: bpp=16 red@11 green@5 blue@0\n");
	printf("  actual:   bpp=%u red@%u green@%u blue@%u\n",
	       vinfo->bits_per_pixel,
	       vinfo->red.offset,
	       vinfo->green.offset,
	       vinfo->blue.offset);

	return -EINVAL;
}

static void show_color(const char *name, uint16_t *fb, uint16_t color, long screensize)
{
	printf("%s 0x%04X\n", name, color);
	fill_screen(fb, color);
	msync(fb, screensize, MS_SYNC);
	sleep(2);
}

int main(int argc, char *argv[])
{
	const char *fb_dev = (argc > 1) ? argv[1] : DEFAULT_FB_DEV;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long screensize;
	uint16_t *fb;
	int fd;

	printf("ILI9341 framebuffer RGB565 test\n");
	printf("Device: %s\n", fb_dev);

	fd = open(fb_dev, O_RDWR);
	if (fd < 0) {
		perror("open framebuffer");
		return EXIT_FAILURE;
	}

	if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) < 0) {
		perror("FBIOGET_FSCREENINFO");
		close(fd);
		return EXIT_FAILURE;
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
		perror("FBIOGET_VSCREENINFO");
		close(fd);
		return EXIT_FAILURE;
	}

	printf("Resolution: %ux%u\n", vinfo.xres, vinfo.yres);
	verify_rgb565(&vinfo);

	screensize = (long)vinfo.yres * finfo.line_length;
	fb = mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (fb == MAP_FAILED) {
		perror("mmap framebuffer");
		close(fd);
		return EXIT_FAILURE;
	}

	show_color("RED", fb, RED, screensize);
	show_color("GREEN", fb, GREEN, screensize);
	show_color("BLUE", fb, BLUE, screensize);
	show_color("WHITE", fb, WHITE, screensize);
	show_color("CYAN", fb, CYAN, screensize);
	show_color("MAGENTA", fb, MAGENTA, screensize);
	show_color("YELLOW", fb, YELLOW, screensize);

	printf("Four-quadrant pattern\n");
	draw_pattern(fb);
	msync(fb, screensize, MS_SYNC);
	sleep(3);

	printf("Red gradient\n");
	draw_red_gradient(fb);
	msync(fb, screensize, MS_SYNC);
	sleep(3);

	printf("Color bars\n");
	draw_color_bars(fb);
	msync(fb, screensize, MS_SYNC);
	sleep(3);

	printf("Checkerboard\n");
	draw_checkerboard(fb);
	msync(fb, screensize, MS_SYNC);
	sleep(3);

	printf("Clear screen\n");
	fill_screen(fb, BLACK);
	msync(fb, screensize, MS_SYNC);

	munmap(fb, screensize);
	close(fd);

	printf("Done\n");
	return EXIT_SUCCESS;
}
