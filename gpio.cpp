#include "gpio.h"

#include <pigpio.h>

Gpio::Gpio(unsigned char gpio) : _gpio(gpio) {
    gpioInitialise();
    gpioSetMode(gpio, PI_OUTPUT);
}

Gpio::~Gpio() {

}

void Gpio::on() const {
    gpioWrite(_gpio, 1);
}

void Gpio::off() const {
    gpioWrite(_gpio, 0);
}