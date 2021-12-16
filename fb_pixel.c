/* fb_pixel.c
 *
 * Author: Yangkai Wang 
 * wang_yangkai@163.com
 *
 * Coding in 2015/2/22
 */

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "fb_pixel.h"

static void fb_set_pixel_rgb565(fb_dev_t *fb_dev, 
				int x, int y, pixel_color_t *color)
{
	char *fb_ptr = fb_dev->fb_ptr;
	int xres = fb_dev->xres;
	int yres = fb_dev->yres;

#define RGB(r,g,b) ((unsigned int)((((unsigned short)(((r)&0xf8) >> 3)) << 11) | (((unsigned short)(((g)&0xfC) >> 2)) << 5) | ((unsigned char)(((b)&0xf8) >> 3))))
#define RGB_R(rgb) ((unsigned char)(((rgb)>>11) << 3))
#define RGB_G(rgb) ((unsigned char)(((rgb)>>5) << 2))
#define RGB_B(rgb) ((unsigned char)((rgb) << 3))

	*(unsigned short *)&fb_ptr[y*xres*2 + x*2 + 0] = RGB(color->r, color->g, color->b);
}

static void fb_set_pixel_rgb888(fb_dev_t *fb_dev, 
				int x, int y, pixel_color_t *color)
{
	char *fb_ptr = fb_dev->fb_ptr;
	int xres = fb_dev->xres;
	int yres = fb_dev->yres;

	fb_ptr[y*xres*3 + x*3 + 0] = color->r;
	fb_ptr[y*xres*3 + x*3 + 1] = color->g;
	fb_ptr[y*xres*3 + x*3 + 2] = color->b;
}

static void fb_set_pixel_bgra8888(fb_dev_t *fb_dev, 
				int x, int y, pixel_color_t *color)
{
	char *fb_ptr = fb_dev->fb_ptr;
	int xres = fb_dev->xres;
	int yres = fb_dev->yres;

	fb_ptr[y*xres*4 + x*4 + 0] = color->b;
	fb_ptr[y*xres*4 + x*4 + 1] = color->g;
	fb_ptr[y*xres*4 + x*4 + 2] = color->r;
	fb_ptr[y*xres*4 + x*4 + 3] = color->a;
}


extern void fb_set_pixel_color(fb_dev_t *fb_dev, 
				int x, int y, pixel_color_t *color)
{
	fb_set_pixel_color_t fn;

	fn = fb_dev->set_pixel_fn;
	fn(fb_dev, x, y, color);
}

extern void pixel_color_set(pixel_color_t *color, 
	unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	color->r = r;
	color->g = g;
	color->b = b;
	color->a = a;
}

extern void fb_fill_color(fb_dev_t *fb_dev, pixel_color_t *color)
{
	int xres = fb_dev->xres;
	int yres = fb_dev->yres;
	int x, y;

	for (y = 0; y < yres; y += 1) {
		for (x = 0; x < xres; x += 1)
			fb_set_pixel_color(fb_dev, x, y, color);
	}
}

extern void fb_fill_color_black(fb_dev_t *fb_dev)
{
	memset(fb_dev->fb_ptr, 0x00, fb_dev->buf_size);
}

extern void fb_fill_color_white(fb_dev_t *fb_dev)
{
	memset(fb_dev->fb_ptr, 0xff, fb_dev->buf_size);
}


extern int fb_init(char *fb_dev_file, fb_dev_t *fb_dev)
{
	int fb_fd;
	int fb_buf_size;
	char *fb_ptr;
	fb_set_pixel_color_t fn;

	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	fb_fd = open(fb_dev_file, O_RDWR);
	if (fb_fd < 0) {
		printf("%s() open %s failed\n", __func__, fb_dev_file);
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) != 0) {
		printf("%s() ioctl FBIOGET_FSCREENINFO failed\n", __func__);
		close(fb_fd);
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) != 0) {
		printf("%s() ioctl FBIOGET_VSCREENINFO failed\n", __func__);
		close(fb_fd);
		return -1;
	}

	printf("xres:%d\n", vinfo.xres);
	printf("yres:%d\n", vinfo.yres);
	printf("bits_per_pixel:%d\n", vinfo.bits_per_pixel);

	printf("red.offset:%d, red.length:%d; blue.offset:%d, blue.length:%d; green.offset:%d, green.length:%d; transp.offset:%d, transp.length:%d\n", 
		vinfo.red.offset, vinfo.red.length, vinfo.blue.offset, vinfo.blue.length, vinfo.green.offset, vinfo.green.length, vinfo.transp.offset, vinfo.transp.length);

	fb_buf_size = (vinfo.xres *vinfo.yres * vinfo.bits_per_pixel / 8);
	printf("fb_buf_size:%d\n", fb_buf_size);

	if (vinfo.bits_per_pixel == 16 && vinfo.red.offset == 0) {
		fn = fb_set_pixel_rgb565;
	} else if (vinfo.bits_per_pixel == 24 && vinfo.red.offset == 0) {
		fn = fb_set_pixel_rgb888;
	} else if (vinfo.bits_per_pixel == 32 && vinfo.blue.offset == 0) {
		fn = fb_set_pixel_bgra8888;
	} else {
		printf("%s() don't support the fb pixel color format\n", __func__);
		close(fb_fd);
		return -1;	
	}

	fb_ptr = (char *)mmap(NULL,
				fb_buf_size,
				PROT_READ | PROT_WRITE,
				MAP_SHARED,
				fb_fd,
				0);
	if (fb_ptr < 0) {
		printf("%s() mmap failed\n", __func__);
		close(fb_fd);
		return -1;
	} else {
		printf("framebuffer ptr mmaped done\n");
	}

	fb_dev->fd = fb_fd;
	fb_dev->fb_ptr = fb_ptr;
	fb_dev->xres = vinfo.xres;
	fb_dev->yres = vinfo.yres;
	fb_dev->buf_size = fb_buf_size;
	fb_dev->set_pixel_fn = fn;

	return 0;
}

extern int fb_deinit(fb_dev_t *fb_dev)
{
	munmap(fb_dev->fb_ptr, 0);
	close(fb_dev->fd);

	return 0;
}