#include "RotaryEncoder.h"

#define COUNTER_STEPSIZE 1

RotaryEncoder::RotaryEncoder(int clk, int dt, int sw, int maxVal)
    : rotCLK(clk), rotDT(dt), rotSW(sw), maxValue(maxVal),
      counter(0), CLKlastState(0), SWlastState(1) {}

void RotaryEncoder::begin() {
    pinMode(rotCLK, INPUT);
    pinMode(rotDT, INPUT);
    pinMode(rotSW, INPUT_PULLUP);

    CLKlastState = digitalRead(rotCLK);
    SWlastState = digitalRead(rotSW);
}

void RotaryEncoder::update() {
    int CLKcurrentState = digitalRead(rotCLK);

    if (CLKcurrentState != CLKlastState) {

        if (digitalRead(rotDT) != CLKcurrentState && counter < maxValue) {
            counter += COUNTER_STEPSIZE;
        } else if (digitalRead(rotDT) == CLKcurrentState && counter > 0) {
            counter -= COUNTER_STEPSIZE;
        }

        delay(5); // debounce

        Serial.println(counter);
    }

    CLKlastState = CLKcurrentState;

    int SWcurrentState = digitalRead(rotSW);
    if (SWcurrentState == LOW && SWlastState == HIGH) {
        delay(20);
        if (digitalRead(rotSW) == LOW) {
            Serial.println("Button pressed");
        }
    }

    SWlastState = SWcurrentState;
}

int RotaryEncoder::getValue() {
    return counter;
}