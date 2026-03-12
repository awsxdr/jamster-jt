#include <iostream>
#include <thread>

#include "st7735.h"
#include "graphics.h"

using namespace std;

int main() {
	auto const screen = St7735(64);
	auto graphics = Graphics(screen);

	short x = screen.width() / 2 - 10;
	short y = screen.height() / 2 - 10;
	unsigned short width = 20;
	unsigned short height = 20;
	unsigned short shift_x = 1;
	unsigned short shift_y = 1;

	auto const background_color = graphics.from_rgb(128, 128, 255);
	auto const box_color = graphics.from_rgb(255, 0, 128);

	graphics.clear(background_color);
	//graphics.fill_rect(10, 10, graphics.width() - 20, graphics.height() - 40, graphics.from_rgb(255, 255, 255));
	// graphics.fill_rect(10, 10, 20, 20, graphics.from_rgb(255, 0, 0));
	// graphics.fill_rect(30, 30, 20, 20, graphics.from_rgb(255, 255, 0));
	// graphics.fill_rect(50, 50, 20, 20, graphics.from_rgb(0, 255, 0));
	// graphics.fill_rect(70, 70, 20, 20, graphics.from_rgb(0, 255, 255));
	// graphics.fill_rect(90, 90, 20, 20, graphics.from_rgb(0, 0, 255));
	// graphics.fill_rect(110, 110, 20, 20, graphics.from_rgb(255, 0, 255));

	auto frame_count = 0;

	while(frame_count < 500) {
		graphics.fill_rect(x, y, 20, 20, background_color);

		x += shift_x;
		y += shift_y;

		if(x > graphics.width() - 20 || x < 0) {
			shift_x = -shift_x;
		}

		if(y > graphics.height() - 20 || y < 0) {
			shift_y = -shift_y;
		}

		graphics.fill_rect(x, y, 20, 20, box_color);

		graphics.render();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		++frame_count;
	}

	graphics.clear(graphics.from_rgb(0, 0, 0));
	graphics.render();

	return 0;
}

