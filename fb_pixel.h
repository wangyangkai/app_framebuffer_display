/* fb_pixel.h
 *
 * Author: Yangkai Wang 
 * wang_yangkai@163.com
 *
 * Coding in 2015/2/22
 */

#ifndef _FB_PIXEL_H
#define _FB_PIXEL_H

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} pixel_color_t;

struct fb_dev;
typedef void (*fb_set_pixel_color_t)(struct fb_dev *fb_info, int x, int y, pixel_color_t *color);

typedef struct fb_dev {
	int fd;
	char *fb_ptr;
	int xres;
	int yres;
	int buf_size;
	fb_set_pixel_color_t set_pixel_fn;
} fb_dev_t;

extern void fb_set_pixel_color(fb_dev_t *fb_dev, 
				int x, int y, pixel_color_t *color);

extern void pixel_color_set(pixel_color_t *color, 
	unsigned char r, unsigned char g, unsigned char b, unsigned char a);

extern void fb_fill_color(fb_dev_t *fb_dev, pixel_color_t *color);
extern void fb_fill_color_black(fb_dev_t *fb_dev);
extern void fb_fill_color_white(fb_dev_t *fb_dev);

#define PIXEL_COLOR_SET_RED(color)  pixel_color_set(color, 0xff, 0x00, 0x00, 0xff)
#define PIXEL_COLOR_SET_GREEN(color)  pixel_color_set(color, 0x00, 0xff, 0x00, 0xff)
#define PIXEL_COLOR_SET_BLUE(color)  pixel_color_set(color, 0x00, 0x00, 0xff, 0xff)
#define PIXEL_COLOR_SET_BLACK(color)  pixel_color_set(color, 0x00, 0x00, 0x00, 0x00)
#define PIXEL_COLOR_SET_WHITE(color)  pixel_color_set(color, 0xff, 0xff, 0xff, 0xff)


extern int fb_init(char *fb_dev_file, fb_dev_t *fb_dev);
extern int fb_deinit(fb_dev_t *fb_dev);

#endif