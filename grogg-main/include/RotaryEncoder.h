#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>

class RotaryEncoder {
private:
    int rotCLK, rotDT, rotSW;
    int maxValue;

    int counter;
    int CLKlastState;
    int SWlastState;

public:
    RotaryEncoder(int clk, int dt, int sw, int maxVal);

    void begin();
    void update();

    int getValue();
};

#endif