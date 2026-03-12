#ifndef _ST7735_H_
#define _ST7735_H_

#include "gpio.h"
#include "graphics.h"

#ifndef byte
#define byte unsigned char
#endif

#define DC_PIN		24
#define RST_PIN		25

class St7735Color565 : public Color {
	public:
		const unsigned short White = 0xffff;
		const unsigned short Black = 0x0000;
		const unsigned short Red = 0x1f00;
		const unsigned short Orange = 0xff03;
		const unsigned short Yellow = 0xff07;
		const unsigned short Lime = 0xef07;
		const unsigned short Green = 0xe007;
		const unsigned short Turquoise = 0xe07f;
		const unsigned short Cyan = 0xe0ff;
		const unsigned short SkyBlue = 0xe0fb;
		const unsigned short Blue = 0x00f8;
		const unsigned short Purple = 0x0ff8;
		const unsigned short Magenta = 0x1ff8;
		const unsigned short Violet = 0x1f78;
		const unsigned short DarkGray = 0xe739;
		const unsigned short Gray = 0xef7b;
		const unsigned short LightGray = 0xf7bd;

		unsigned short from_rgb(const byte red, const byte green, const byte blue) const override {
			auto const adjusted_blue = static_cast<unsigned short>(0x1f * (blue / 255.0)) << 11;
			auto const adjusted_green = static_cast<unsigned short>(0x3f * (green / 255.0)) << 5;
			auto const adjusted_red = static_cast<unsigned short>(0x1f * (red / 255.0));

			auto const color = adjusted_blue | adjusted_green | adjusted_red;
			auto const reordered_bytes = static_cast<unsigned short>(((color & 0xff) << 8) | (color >> 8));

			return reordered_bytes;
		}
};

class St7735 : public GraphicsDevice {
	public:
		St7735(unsigned short transfer_speed_mhz);
		~St7735();

		const Color& color() const override;

		void fill_rect(const unsigned short left, const unsigned short top, const unsigned short width, const unsigned short height, const unsigned short color) const override;
		void blit(const unsigned short left, const unsigned short top, const unsigned short width, const unsigned short height, const unsigned short* pixels) const override;
		void clear(const unsigned short color) const override;

		unsigned short width() const override { return 128; }
		unsigned short height() const override { return 160; }

	private:
		Gpio _dc;
		Gpio _rst;

		St7735Color565 _color;

		int _device;

		const unsigned short FRAME_SIZE = 4096;

		byte _bits_per_word;
		unsigned int _max_speed_hz;

		void send_byte(const byte data) const;
		void send_bytes(const byte* data, const unsigned short data_length) const;
		void send_data(const byte* data, const unsigned short data_length) const;
		void send_instruction(const byte instruction, const byte* parameters, const unsigned short parameter_count) const;
		void send_instruction(const byte instruction) const;
		void send_instruction(const byte instruction, const byte parameter1) const;
		void send_instruction(const byte instruction, const byte parameter1, const byte parameter2) const;
		void send_instruction(const byte instruction, const byte parameter1, const byte parameter2, const byte parameter3) const;
		void send_instruction(const byte instruction, const byte parameter1, const byte parameter2, const byte parameter3, byte parameter4) const;

		void sleep_for_milliseconds(const unsigned int milliseconds) const;
};

class St7735Instruction {
	public:
		static const byte NOP = 0x00;
		static const byte SWRESET = 0x01;
		static const byte RDDID = 0x04;
		static const byte RDDST = 0x09;
		static const byte SLPIN = 0x10;
		static const byte SLPOUT = 0x11;
		static const byte PTLON = 0x12;
		static const byte NORON = 0x13;
		static const byte INVOFF = 0x20;
		static const byte INVON = 0x21;
		static const byte DISPOFF = 0x28;
		static const byte DISPON = 0x29;
		static const byte CASET = 0x2A;
		static const byte RASET = 0x2B;
		static const byte RAMWR = 0x2C;
		static const byte RAMRD = 0x2E;
		static const byte PTLAR = 0x30;
		static const byte TEOFF = 0x34;
		static const byte TEON = 0x35;
		static const byte MADCTL = 0x36;
		static const byte COLMOD = 0x3A;
		static const byte MADCTL_MY = 0x80;
		static const byte MADCTL_MX = 0x40;
		static const byte MADCTL_MV = 0x20;
		static const byte MADCTL_ML = 0x10;
		static const byte MADCTL_RGB = 0x00;
		static const byte RDID1 = 0xDA;
		static const byte RDID2 = 0xDB;
		static const byte RDID3 = 0xDC;
		static const byte RDID4 = 0xDD;
		static const byte MADCTL_BGR = 0x08;
		static const byte MADCTL_MH = 0x04;
		static const byte FRMCTR1 = 0xB1;
		static const byte FRMCTR2 = 0xB2;
		static const byte FRMCTR3 = 0xB3;
		static const byte INVCTR = 0xB4;
		static const byte DISSET5 = 0xB6;
		static const byte PWCTR1 = 0xC0;
		static const byte PWCTR2 = 0xC1;
		static const byte PWCTR3 = 0xC2;
		static const byte PWCTR4 = 0xC3;
		static const byte PWCTR5 = 0xC4;
		static const byte VMCTR1 = 0xC5;
		static const byte PWCTR6 = 0xFC;
		static const byte GMCTRP1 = 0xE0;
		static const byte GMCTRN1 = 0xE1;
};

#endif