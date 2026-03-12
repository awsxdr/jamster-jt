build: ; g++ main.cpp st7735.cpp gpio.cpp graphics.cpp -std=c++14 -lpigpio -o stopwatch
run: build ; sudo ./stopwatch
