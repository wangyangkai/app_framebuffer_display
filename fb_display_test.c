/* fb_display_test.c
 *
 * Author: Yangkai Wang 
 * wang_yangkai@163.com
 *
 * Coding in 2015/2/22
 */

#include <stdio.h>
#include "fb_pixel.h"

static void fb_display_test(fb_dev_t *fb_dev)
{
	int xres = fb_dev->xres;
	int yres = fb_dev->yres;
	int x, y;
	pixel_color_t color;
	unsigned char gray;
	int times = 0;

	printf("%s()\n", __func__);

	fb_fill_color_white(fb_dev);
	usleep(1000*1000);
	fb_fill_color_black(fb_dev);
	usleep(1000*1000);
	fb_fill_color_white(fb_dev);
	usleep(1000*1000);
	fb_fill_color_black(fb_dev);
	usleep(1000*1000);

	do {
		/* base test */
		printf("blue\n");
		PIXEL_COLOR_SET_BLUE(&color);
		fb_fill_color(fb_dev, &color);
		usleep(1000*1000*3);

		printf("green\n");
		PIXEL_COLOR_SET_GREEN(&color);
		fb_fill_color(fb_dev, &color);
		usleep(1000*1000*3);

		printf("red\n");
		PIXEL_COLOR_SET_RED(&color);
		fb_fill_color(fb_dev, &color);
		usleep(1000*1000*3);

		printf("white\n");
		fb_fill_color_white(fb_dev);
		usleep(1000*1000*3);


		/* draw line */
		printf("draw line\n");
		PIXEL_COLOR_SET_RED(&color);
		fb_fill_color(fb_dev, &color);

		for (y = 0; y < yres; y += 1) {
			for (x = 0; x < xres; x += 1) {
				PIXEL_COLOR_SET_BLACK(&color);
				fb_set_pixel_color(fb_dev, x, y, &color);

				if ((y == yres/2) || (x == xres/2) 
					|| (y == yres - 1) || (x == xres - 1)
					|| (y == 0) || (x == 0)) {
					PIXEL_COLOR_SET_WHITE(&color);
					fb_set_pixel_color(fb_dev, x, y, &color);
				}

				if (x % 200 == 0)
					usleep(1);
			}
		}
		usleep(1000*1000*3);


		/* display gray level */
		printf("gray\n");
		PIXEL_COLOR_SET_RED(&color);
		fb_fill_color(fb_dev, &color);
		gray = 0x00;
		for (y = 0; y < yres; y += 1) {
			for (x = 0; x < xres; x+=1) {
				pixel_color_set(&color, gray, gray, gray, 0xff);
				fb_set_pixel_color(fb_dev, x, y, &color);

				if (x % 200 == 0)
					usleep(1);
			}
			if (y % 10 == 0)
				gray += (unsigned char)(yres / 0xff * 5);
		}
		usleep(1000*1000*6);

	} while (++times && times < 5);
}

int main(int argc, char *argv[])
{
	int i;
	int lcd_fd;
	int ret;
	int count;
	fb_dev_t fb_dev;
	char *fb_dev_file;

	if (argc != 2) {
		printf("usage %s </dev/fb*>\n", argv[0]);
		printf("example:%s /dev/fb0\n", argv[0]);

		if (argc == 1) {
			printf("try open /dev/fb0\n", argv[0]);
			fb_dev_file = "/dev/fb0";
		} else {
			return -1;
		}
	} else {
		fb_dev_file = argv[1];
	}
	printf("fb_dev_file:%s\n", fb_dev_file);

	ret = fb_init(fb_dev_file, &fb_dev);
	if (ret < 0) {
		printf("fb_init() error, ret:%d", ret);
		return ret;
	}

	fb_display_test(&fb_dev);

	fb_deinit(&fb_dev);

	return 0;
}
