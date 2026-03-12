#ifndef _GPIO_H_
#define _GPIO_H_

class Gpio {
    public:
        Gpio(unsigned char gpio);
        ~Gpio();

        void on() const;
        void off() const;

    private:
        unsigned char _gpio;
};

#endif