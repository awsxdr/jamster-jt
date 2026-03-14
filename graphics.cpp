#include "graphics.h"

#include <Magick++.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>

using namespace std;
using namespace Magick;

struct Pixel64 { 
    float red;
    float green;
    float blue;
    float alpha;
};

struct PixelRgb {
    float red;
    float green;
    float blue;
};

int float_to_int(float value) {
    const float magic = 1 << 23;

    auto const intermediate = value + magic;

    const int* result_ptr = reinterpret_cast<const int*>(&intermediate);

    return (*result_ptr) & 0x7fffff;
}

Graphics::Graphics(const int width, const int height) : _width(width), _height(height) {
    _bitmap = new unsigned short[width * height];
}

Graphics::~Graphics() {
    delete[] _bitmap;
}

void Graphics::clear(const unsigned short color) {
    auto const bitmap_length = width() * height();
    for(int i = 0; i < bitmap_length; ++i) {
        _bitmap[i] = color;
    }
}

void Graphics::render(const GraphicsDevice& device) const {
    device.blit(0, 0, _width, _height, _bitmap);
}

void Graphics::fill_rect(const short left, const short top, const short width, const short height, const unsigned short color) {

    for (short y = top; y < top + height - 1; ++y) {
        for (short x = left; x < left + width - 1; ++x) {
            if (x < 0 || y < 0 || x > _width || y > _height) {
                continue;
            }
            _bitmap[y * _width + x] = color;
        }
    }
}

void Graphics::line(const unsigned short x1, const unsigned short y1, const unsigned short x2, const unsigned short y2, const unsigned short color) {
}

void Graphics::draw_image(const Graphics& image, const short x, const short y) {
    const unsigned short target_width = min(_width, static_cast<unsigned short>(x + image._width)) - x;
    const unsigned short target_height = min(_height, static_cast<unsigned short>(y + image._height)) - y;
    const unsigned short target_x = max((short) 0, x);
    const unsigned short target_y = max((short) 0, y);

    for(unsigned short row = 0; row < target_height; ++row) {
        memcpy(&_bitmap[(row + target_y) * _width + target_x], &image._bitmap[row * image._width], target_width * sizeof(short));
    }
}

unsigned short color_from_rgb(const byte red, const byte green, const byte blue) {
    auto const adjusted_blue = static_cast<unsigned short>(0x1f * (blue / 255.0)) << 11;
    auto const adjusted_green = static_cast<unsigned short>(0x3f * (green / 255.0)) << 5;
    auto const adjusted_red = static_cast<unsigned short>(0x1f * (red / 255.0));

    auto const color = adjusted_blue | adjusted_green | adjusted_red;
    auto const reordered_bytes = static_cast<unsigned short>(((color & 0xff) << 8) | (color >> 8));

    return reordered_bytes;
}

RgbColor color_to_rgb(unsigned short color) {
    auto const reordered_bytes = static_cast<unsigned short>(((color & 0xff) << 8) | (color >> 8));
    const byte red = static_cast<byte>(((reordered_bytes & 0x1f) / 31.0f) * 255.0f);
    const byte green = static_cast<byte>((((reordered_bytes >> 5) & 0x3f) / 63.0f) * 255.0f);
    const byte blue = static_cast<byte>((((reordered_bytes >> 11) & 0x1f) / 31.0f) * 255.0f);

    return { red, green, blue };
}


void Graphics::draw_text(const string& text, const short x, const short y, const RgbColor color, const float point_size) {
    Image buffer(Geometry(_width, _height), "black");
    buffer.magick("RGBA");

    auto pixels = reinterpret_cast<PixelRgb*>(buffer.getPixels(0, 0, _width, _height));
    auto const size = _width * _height;

    for(int i = 0; i < size; ++i) {
        auto const pixel = (pixels + i);
        auto const pixel_color = color_to_rgb(_bitmap[i]);
        pixel->red = static_cast<float>(pixel_color.red << 8);
        pixel->green = static_cast<float>(pixel_color.green << 8);
        pixel->blue = static_cast<float>(pixel_color.blue << 8);
    }

    buffer.syncPixels();

    auto const text_color = ColorRGB(color.red / 255.0f, color.green / 255.0f, color.blue / 255.0f);
    vector<Drawable> commands;
    commands.push_back(DrawableStrokeColor(text_color));
    commands.push_back(DrawableFillColor(text_color));
    commands.push_back(DrawablePointSize(point_size));
    commands.push_back(DrawableText(x, y, text.c_str()));
    buffer.draw(commands);

    auto out_pixels = reinterpret_cast<Pixel64*>(buffer.getPixels(0, 0, _width, _height));
    for(auto i = 0; i < size; ++i) {
        auto const pixel = *(out_pixels+i);

        _bitmap[i] = color_from_rgb(
            static_cast<byte>(float_to_int(pixel.red) >> 8),
            static_cast<byte>(float_to_int(pixel.green) >> 8),
            static_cast<byte>(float_to_int(pixel.blue) >> 8));
    }
}

void Graphics::rotate_cw() {

}

void Graphics::rotate_ccw() {
    auto const new_width = _height;
    auto const new_height = _width;
    auto new_bitmap = new unsigned short[_width * _height];

    for(int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            auto const target_x = y;
            auto const target_y = _width - 1 - x;
            new_bitmap[target_y * new_width + target_x] = _bitmap[y * _width + x];
        }
    }

    delete[] _bitmap;
    _bitmap = new_bitmap;
    _width = new_width;
    _height = new_height;
}

unsigned short Graphics::width() const {
    return _width;
}

unsigned short Graphics::height() const {
    return _height;
}