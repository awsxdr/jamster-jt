#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <tuple>

#ifndef byte
#define byte unsigned char
#endif

class Color {
    public:
        virtual unsigned short from_rgb(const byte red, const byte green, const byte blue) const = 0;
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

struct Location {
    short x;
    short y;
};

class Graphics {
    public:
        Graphics(const GraphicsDevice& device);
        ~Graphics();

        unsigned short from_rgb(byte red, byte green, byte blue) const;

        void clear(const unsigned short color);
        void fill_rect(const short left, const short top, const short width, const short height, const unsigned short color);
        void line(const unsigned short x1, const unsigned short y1, const unsigned short x2, const unsigned short y2, const unsigned short color);

        void render() const;

        unsigned short width() const;
        unsigned short height() const;

    private:
        const GraphicsDevice& _device;
        const unsigned short _height;
        const unsigned short _width;

        Location translate_point(const Location& point) const;

        unsigned short* _bitmap;
};


#endif