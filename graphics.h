#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <string>

#ifndef byte
#define byte unsigned char
#endif

class Color {
    public:
        virtual unsigned short from_rgb(const byte red, const byte green, const byte blue) const = 0;
};

struct RgbColor {
    byte red;
    byte green;
    byte blue;
};

class GraphicsDevice {
    public:
        virtual void fill_rect(const unsigned short left, const unsigned short top, const unsigned short width, const unsigned short height, const unsigned short color) const = 0;
        virtual void blit(const unsigned short left, const unsigned short top, const unsigned short width, const unsigned short height, const unsigned short* pixels) const = 0;
        virtual void clear(const unsigned short color) const = 0;

        virtual const Color& color() const = 0;

        virtual unsigned short width() const = 0;
        virtual unsigned short height() const = 0;
};

class Graphics {
    public:
        Graphics(const int width, const int height);
        ~Graphics();

        void clear(const unsigned short color);
        void fill_rect(const short left, const short top, const short width, const short height, const unsigned short color);
        void line(const unsigned short x1, const unsigned short y1, const unsigned short x2, const unsigned short y2, const unsigned short color);
        void draw_image(const Graphics& image, const short x, const short y);
        void draw_text(const std::string& text, const short x, const short y, const RgbColor color, const float point_size);
        void rotate_cw();
        void rotate_ccw();

        void render(const GraphicsDevice& device) const;

        unsigned short width() const;
        unsigned short height() const;

    private:
        unsigned short _height;
        unsigned short _width;

        unsigned short* _bitmap;
};


#endif