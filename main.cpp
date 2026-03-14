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

	main_graphics.clear(screen.color().from_rgb(255, 255, 255));
	main_graphics.render(screen);

	Graphics* large_digit[10];
	Graphics* small_digit[10];

	for(auto i = 0; i < 10; ++i) {
		auto large_image = new Graphics(20, 32);
		large_image->clear(screen.color().from_rgb(255, 255, 255));

		auto small_image = new Graphics(10, 16);
		small_image->clear(screen.color().from_rgb(255, 255, 255));

		string digit = "";
		digit += (char)('0' + i);

		large_image->draw_text(digit, 0, 24, { 0, 0, 0}, 32);
		large_image->rotate_ccw();

		small_image->draw_text(digit, 0, 12, { 0, 0, 0}, 16);
		small_image->rotate_ccw();

		large_digit[i] = large_image;
		small_digit[i] = small_image;

		main_graphics.fill_rect(90, 130 - i * 10, 10, i * 10, screen.color().from_rgb(192, 224, 255));
		main_graphics.render(screen);
	}

	main_graphics.clear(screen.color().from_rgb(255, 255, 255));
	main_graphics.render(screen);

	auto start_time = std::chrono::high_resolution_clock::now();

	auto frame_count = 0;
	auto last_second = 0;

	while(true) {
		auto time = std::chrono::high_resolution_clock::now();
		auto expired_time = time - start_time;

		auto expired_milliseconds = expired_time / 1ms;
		auto expired_seconds = expired_time / 1s;
		auto expired_minutes = expired_seconds / 60;

		main_graphics.draw_image(*large_digit[((expired_seconds % 60) / 10) % 10], 20, 120);
		main_graphics.draw_image(*large_digit[expired_seconds % 10], 20, 100);
		main_graphics.draw_image(*small_digit[(expired_milliseconds / 100) % 10], 32, 90);
		main_graphics.draw_image(*small_digit[(expired_milliseconds / 10) % 10], 32, 80);

		main_graphics.render(screen);

		if(expired_seconds > 31) {
			break;
		}

		++frame_count;

		if(expired_seconds != last_second) {
			cout << frame_count << " FPS" << endl;
			frame_count = 0;
			last_second = expired_seconds;
		}
	}

	for (auto i = 0; i < 10; ++i) {
		delete large_digit[i];
		delete small_digit[i];
	}

	return 0;
}

