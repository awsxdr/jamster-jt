#include "graphics.h"

Graphics::Graphics(const GraphicsDevice& device) : _device(device), _width(device.width()), _height(device.height()) {
    _bitmap = new unsigned short[device.width() * device.height()];
}

Graphics::~Graphics() {
    delete[] _bitmap;
}

unsigned short Graphics::from_rgb(byte red, byte green, byte blue) const {
    return _device.color().from_rgb(red, green, blue);
}

void Graphics::clear(const unsigned short color) {
    auto const bitmap_length = width() * height();
    for(int i = 0; i < bitmap_length; ++i) {
        _bitmap[i] = color;
    }
}

void Graphics::render() const {
    _device.blit(0, 0, _width, _height, _bitmap);
}

void Graphics::fill_rect(const short left, const short top, const short width, const short height, const unsigned short color) {

    for (short y = top; y < top + height - 1; ++y) {
        for (short x = left; x < left + width - 1; ++x) {
            auto const adjusted_point = translate_point({ x, y });
            if (adjusted_point.x < 0 || adjusted_point.y < 0 || adjusted_point.x > _width || adjusted_point.y > _height) {
                continue;
            }
            _bitmap[adjusted_point.y * _width + adjusted_point.x] = color;
        }
    }
}

void Graphics::line(const unsigned short x1, const unsigned short y1, const unsigned short x2, const unsigned short y2, const unsigned short color) {
}

unsigned short Graphics::width() const {
    return _device.height(); // Flipped due to screen rotation
}

unsigned short Graphics::height() const {
    return _device.width();
}

Point Graphics::translate_point(const Point& point) const {
    return Point { point.y, static_cast<short>(_height - point.x) };
}