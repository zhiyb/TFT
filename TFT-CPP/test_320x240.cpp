#include <util/delay.h>
#include <stdio.h>
#include <tft.h>

extern void test(void);

void test(void)
{
	using namespace tft;
	setOrient(Flipped | Landscape);
	background = 0x667f;
	foreground = 0x0000;
	setBGLight(true);

start:
	clean();
	zoom = 1;
	puts("*** TFT library testing ***");
	puts("STDOUT output, orientation, FG/BG colour, BG light");
	zoom = 3;
	puts("Font size test");
	zoom = 1;
	x = 300;
	y = 38;
	puts("Change text position & word warp test");
	frame(115, 56, 200, 10, 2, 0xF800);
	puts("Draw frame test");
	rectangle(118, 68, 180, 6, 0x07E0);
	puts("Draw rectangle test");
	point(120, 76, 0x001F);
	point(122, 76, 0x001F);
	point(124, 76, 0x001F);
	point(126, 76, 0x001F);
	point(128, 76, 0x001F);
	point(130, 76, 0x001F);
	puts("Draw points test");
	line(200, 100, 300, 200, 0xF800);
	line(300, 210, 200, 110, 0x001F);
	line(200, 200, 300, 100, 0xF800);
	line(300, 110, 200, 210, 0x001F);

	line(100, 100, 300, 200, 0xF800);
	line(300, 210, 100, 110, 0x001F);
	line(100, 200, 300, 100, 0xF800);
	line(300, 110, 100, 210, 0x001F);

	line(200, 0, 300, 200, 0xF800);
	line(300, 210, 200, 10, 0x001F);
	line(200, 200, 300, 0, 0xF800);
	line(300, 10, 200, 210, 0x001F);

	line(100, 150, 300, 150, 0xF800);
	line(300, 160, 100, 160, 0x001F);
	line(250, 0, 250, 200, 0xF800);
	line(260, 200, 260, 0, 0x001F);
	puts("Draw lines test");

#ifdef TFT_VERTICAL_SCROLLING
	tft::setVerticalScrollingArea(64, 32);
	uint16_t max = tft::vsMaximum() - 32;
	uint16_t v = 64;
loop:
	tft::setVerticalScrolling(v);
	if (max == ++v)
		v = 64;
	_delay_ms(10);
#else
loop:
#endif
	goto loop;
	goto start;
}