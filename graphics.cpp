#include "graphics.h"

#include <Magick++.h>
#include <vector>
#include <iostream>

using namespace std;
using namespace Magick;
using namespace MagickCore;

// struct Pixel64 { 
//     //unsigned short alpha;
//     unsigned short red;
//     unsigned short red2;
//     unsigned short green;
//     unsigned short green2;
//     unsigned short blue;
//     unsigned short blue2;
// };

// struct Pixel64 { 
//     unsigned int red;
//     unsigned int green;
//     unsigned int blue;
// };

struct Pixel64 { 
    //float alpha;
    float red;
    float green;
    float blue;
};

Graphics::Graphics(const GraphicsDevice& device) : _device(device), _width(device.width()), _height(device.height()) {
    Magick::InitializeMagick(nullptr);

    //Magick::Image image("img/gradient_bw.png");
    Magick::Image image("img/lena_scaled.jpg");

    image.modifyImage();
    //image.scale(Magick::Geometry(128, 160));

    Magick::Image buffer;
    buffer.size(Magick::Geometry(128, 160));
    //buffer.magick("RGB565");
    buffer.modifyImage();

    vector<Magick::Drawable> commands;
    commands.push_back(Magick::DrawableCompositeImage(0, 0, image));
    buffer.draw(commands);

    Magick::Pixels view(image);
    //auto const pixels = reinterpret_cast<Pixel64*>(view.get(0, 0, 160, 128));
    auto pixels = reinterpret_cast<Pixel64*>(image.getPixels(0, 0, 128, 160));
    //auto const pixels = view.get(0, 0, 160, 128);

    _bitmap = new unsigned short[device.width() * device.height()];

    auto const size = device.width() * device.height();
    for(auto i = 0; i < size; ++i) {
        //auto const pixel = static_cast<unsigned short>(*(pixels+i*3));
        auto const pixel = *(pixels+i);
        //_bitmap[i] = from_rgb((int)pixel.red, (int)pixel.green, (int)pixel.blue);
        //cout << pixel.quantumRed() << endl;
        //cout << pixel << endl;

        _bitmap[i] = from_rgb(
            static_cast<byte>(pixel.red / 65535.0f * 255.0f),
            static_cast<byte>(pixel.green / 65535.0f * 255.0f),
            static_cast<byte>(pixel.blue / 65535.0f * 255.0f));
        //cout << (pixel.red2 >> 8) << ", " << (pixel.green2 >> 8) << ", " << (pixel.blue2 >> 8) << endl;
        if(i % 128 == 0)
            cout << pixel.red / 65535.0f * 255.0f << ", " << pixel.green / 65535.0f * 255.0f << ", " << pixel.blue / 65535.0f * 255.0f << endl;

        // if(i % 128 == 0)
        //     cout << pixel << endl;
        // _bitmap[i] = from_rgb(pixel / 256, 0, 0);
    }
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

Location Graphics::translate_point(const Location& point) const {
    return Location { point.y, static_cast<short>(_height - point.x) };
}