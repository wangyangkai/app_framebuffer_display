CC=/opt/toolchain/arm-cortex_a8-linux-gnueabi-4.9.3/bin/arm-cortex_a8-linux-gnueabi-gcc

all: app_fb_display

app_fb_display: fb_display_test.c fb_pixel.c
	$(CC) -o $@ $^

clean:
	rm app_fb_display
