#include "st7735.h"

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <cstring>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <time.h>

using namespace std;

St7735::St7735(unsigned short transfer_speed_mhz) : _dc(DC_PIN), _rst(RST_PIN), _device(0), _color() {

    _device = open("/dev/spidev0.1", O_RDWR, 0);

    if(_device < 0) {
        cout << "Error opening SPI device" << endl;
        exit(-1);
    }

    const byte mode = 0;
    ioctl(_device, SPI_IOC_WR_MODE, &mode);
    _max_speed_hz = transfer_speed_mhz * 1000000;
    ioctl(_device, SPI_IOC_WR_MAX_SPEED_HZ, &_max_speed_hz);

    ioctl(_device, SPI_IOC_RD_MODE);
    ioctl(_device, SPI_IOC_RD_BITS_PER_WORD, &_bits_per_word);
    ioctl(_device, SPI_IOC_RD_MAX_SPEED_HZ, &_max_speed_hz);

    _rst.on();
    sleep_for_milliseconds(500);
    _rst.off();
    sleep_for_milliseconds(500);
    _rst.on();
    sleep_for_milliseconds(500);

    send_instruction(St7735Instruction::SWRESET);
    sleep_for_milliseconds(150);

	send_instruction(St7735Instruction::SLPOUT);
    sleep_for_milliseconds(500);

    send_instruction(St7735Instruction::FRMCTR1, 0x01, 0x2c, 0x2d);
	send_instruction(St7735Instruction::FRMCTR2, 0x01, 0x2c, 0x2d);
    byte frame_control_3_parameters[] = { 0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d };
	send_instruction(St7735Instruction::FRMCTR3, frame_control_3_parameters, 6);
    send_instruction(St7735Instruction::INVCTR, 0x07);
    send_instruction(St7735Instruction::PWCTR1, 0xa2, 0x02, 0x84);
    send_instruction(St7735Instruction::PWCTR2, 0x0a, 0x00);
    send_instruction(St7735Instruction::PWCTR4, 0xba, 0x2a);
    send_instruction(St7735Instruction::PWCTR5, 0x8a, 0xee);
    send_instruction(St7735Instruction::INVOFF);
    send_instruction(St7735Instruction::VMCTR1, 0x0e);
    send_instruction(St7735Instruction::MADCTL, 0xc8);
    send_instruction(St7735Instruction::COLMOD, 0x05);
    send_instruction(St7735Instruction::CASET, 0x00, 0x00, 0x00, 160);
    send_instruction(St7735Instruction::RASET, 0x00, 0x00, 0x00, 120);
    
	byte positive_gamma_parameters[16] = { 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10 };
	send_instruction(St7735Instruction::GMCTRP1, positive_gamma_parameters, 16);
	byte negative_gamma_parameters[16] = { 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10 };
	send_instruction(St7735Instruction::GMCTRN1, negative_gamma_parameters, 16);

    send_instruction(St7735Instruction::NORON);
    sleep_for_milliseconds(10);

    send_instruction(St7735Instruction::DISPON);
    sleep_for_milliseconds(100);

    clear(_color.Black);
}

St7735::~St7735() {

}

const Color& St7735::color() const {
    return _color;
}

void St7735::fill_rect(const unsigned short left, const unsigned short top, const unsigned short width, const unsigned short height, const unsigned short color) const {

    auto const adjusted_width = width;
    auto const adjusted_height = height;

    unsigned short buffer[adjusted_width * adjusted_height];

    for(int i = 0; i < adjusted_width * adjusted_height; ++i) {
        buffer[i] = color;
    }

    blit(left, top, adjusted_width, adjusted_height, buffer);
}

void St7735::blit(const unsigned short left, const unsigned short top, const unsigned short width, const unsigned short height, const unsigned short* pixels) const {
    send_instruction(St7735Instruction::CASET, static_cast<byte>(left >> 8), static_cast<byte>(left & 0xff), static_cast<byte>((left + width - 1) >> 8), static_cast<byte>((left + width - 1) & 0xff));
    send_instruction(St7735Instruction::RASET, static_cast<byte>(top >> 8), static_cast<byte>(top & 0xff), static_cast<byte>((top + height - 1) >> 8), static_cast<byte>((top + height - 1) & 0xff));
    send_instruction(St7735Instruction::RAMWR);

    send_data(reinterpret_cast<const byte*>(pixels), width * height * 2);
}

void St7735::clear(const unsigned short color) const {
    fill_rect(0, 0, 128, 160, color);
}

void St7735::send_byte(const byte data) const {
    send_bytes(&data, 1);
}

void St7735::send_bytes(const byte* data, const unsigned short data_length) const {
    spi_ioc_transfer transfer[1];
    memset(&transfer, 0, sizeof(transfer));

    int remaining_data_length = data_length;
    unsigned short offset = 0;

    while(remaining_data_length > 0) {

        transfer[0].tx_buf = reinterpret_cast<unsigned long>(data + offset);
        transfer[0].len = remaining_data_length > FRAME_SIZE ? FRAME_SIZE : remaining_data_length;
        transfer[0].speed_hz = _max_speed_hz;
        transfer[0].bits_per_word = _bits_per_word;

        ioctl(_device, SPI_IOC_MESSAGE(1), &transfer);

        remaining_data_length -= FRAME_SIZE;
        offset += FRAME_SIZE;
    }
}

void St7735::send_data(const byte* data, const unsigned short data_length) const {
    _dc.on();
    send_bytes(data, data_length);
}

void St7735::send_instruction(const byte instruction, const byte* parameters, const unsigned short parameter_count) const {
	_dc.off();

    send_byte(instruction);

	if(parameters != nullptr && parameter_count > 0) {
        send_data(parameters, parameter_count);
	}
}

void St7735::send_instruction(const byte instruction) const {
    send_instruction(instruction, nullptr, 0);
}

void St7735::send_instruction(const byte instruction, const byte parameter1) const {
    byte args[1] = { parameter1 };
    send_instruction(instruction, args, 1);
}

void St7735::send_instruction(const byte instruction, const byte parameter1, const byte parameter2) const {
    byte args[2] = { parameter1, parameter2 };
    send_instruction(instruction, args, 2);
}

void St7735::send_instruction(const byte instruction, const byte parameter1, const byte parameter2, const byte parameter3) const {
    byte args[3] = { parameter1, parameter2, parameter3 };
    send_instruction(instruction, args, 3);
}

void St7735::send_instruction(const byte instruction, const byte parameter1, const byte parameter2, const byte parameter3, const byte parameter4) const {
    byte args[4] = { parameter1, parameter2, parameter3, parameter4 };
    send_instruction(instruction, args, 4);
}

void St7735::sleep_for_milliseconds(unsigned int milliseconds) const {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
