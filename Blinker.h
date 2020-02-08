#ifndef Blinker_h
#define Blinker_h

#include "Arduino.h"

class Blinker {
public:
    Blinker();
    Blinker(int aSensorPin);
    Blinker(int aSensorPin, long atimeOn, long atimeOff);
    void blink();
    void blink(long atimeOn, long atimeOff);
    void setPin(int aPin);
    void setTimeOff(long aTimeOff);
    void setTimeOn(long aTimeOn);
    void turnOn();
    void turnOff();
protected:
    int ledState;
    int analogState;
    int pin;
    long timeOn;
    long timeOff;
    unsigned long previousMillis;
    unsigned long currentMillis;
};

class Fader : public Blinker {
public:
    Fader();
    Fader(int aSensorPin);
    Fader(int aSensorPin, long atimeOn, long atimeOff, long aFadeAmount);
    void fade();
    void setBrightness(int aBrightness);
protected:
    long fadeTime;
    int fadeAmount;
    int brightness; 
};


#endif
