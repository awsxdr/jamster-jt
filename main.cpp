#include <iostream>
#include <thread>
#include <Magick++.h>

#include "st7735.h"
#include "graphics.h"

using namespace std;

int main() {
	Magick::InitializeMagick(nullptr);

	auto const screen = St7735(64);
	auto main_graphics = Graphics(screen.width(), screen.height());

	main_graphics.clear(screen.color().from_rgb(255, 0, 255));
	main_graphics.render(screen);

	auto text = Graphics(50, 20);
	text.clear(screen.color().from_rgb(255, 0, 255));
	text.draw_text("Hello", 0, 12, { 0, 255, 0 }, 16);
	text.rotate_ccw();

	main_graphics.render(screen);

	auto const background_color = screen.color().from_rgb(255, 0, 255);
	short x = screen.width() / 2 - 10;
	short y = screen.height() / 2 - 10;

	unsigned short shift_x = 1;
	unsigned short shift_y = 1;

	auto frame_count = 0;

	while(frame_count < 500) {
		main_graphics.fill_rect(x, y, text.width(), text.height(), background_color);

		x += shift_x;
		y += shift_y;

		if(x > main_graphics.width() - text.width() || x < 0) {
			shift_x = -shift_x;
		}

		if(y > main_graphics.height() - text.height() || y < 0) {
			shift_y = -shift_y;
		}

		main_graphics.draw_image(text, x, y);
		main_graphics.render(screen);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		++frame_count;
	}

	return 0;
}

