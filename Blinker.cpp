#include <Mouse.h>

#include "Arduino.h"
#include "Blinker.h"

  Blinker::Blinker() : pin(1), timeOn(500), timeOff(500), previousMillis(0), ledState(LOW) {
    pinMode(pin,OUTPUT);
  }
  Blinker::Blinker(int aSensorPin) : pin(aSensorPin), timeOn(500), timeOff(500), previousMillis(0), ledState(LOW) {
    pinMode(pin,OUTPUT);
  }
  Blinker::Blinker(int aSensorPin, long atimeOn, long atimeOff) : 
   pin(aSensorPin), timeOn(atimeOn), timeOff(atimeOff), previousMillis(0), ledState(LOW) {
    pinMode(pin,OUTPUT);
  }

  void Blinker::blink() {
    currentMillis = millis();
    if((ledState == HIGH) && (currentMillis - previousMillis >= timeOn))
    {
      ledState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(pin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= timeOff))
    {
      ledState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(pin, ledState);    // Update the actual LED
    }
  }
  void Blinker::blink(long atimeOn, long atimeOff) {
    currentMillis = millis();
    if((ledState == HIGH) && (currentMillis - previousMillis >= atimeOn))
    {
      ledState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(pin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= atimeOff))
    {
      ledState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(pin, ledState);    // Update the actual LED
    }
  }

  void Blinker::setPin(int aPin) {
    pin = aPin;
  }
  void Blinker::setTimeOff(long aTimeOff) {
    timeOff = aTimeOff;
  }
  void Blinker::setTimeOn(long aTimeOn) {
    timeOn = aTimeOn;
  }

  void Blinker::turnOn() {
    digitalWrite(pin, HIGH);
  }
  void Blinker::turnOff() {
    digitalWrite(pin, LOW);
  }

  Fader::Fader() : Blinker(), fadeTime(10), fadeAmount(5), brightness(0) {
    pinMode(pin,OUTPUT);
  }
  
  Fader::Fader(int aSensorPin) : Blinker(aSensorPin), fadeTime(10), fadeAmount(5), brightness(0) {
    pinMode(pin,OUTPUT);
  }
  
  Fader::Fader(int aSensorPin, long atimeOn, long atimeOff, long aFadeAmount) :
   Blinker(aSensorPin, atimeOn, atimeOff), fadeTime(10), fadeAmount(5), brightness(0) {
    pinMode(pin,OUTPUT);
  }

  void Fader::fade() {
    currentMillis = millis();

    if (currentMillis - previousMillis >= fadeTime) {
      brightness += fadeAmount;
      analogWrite(pin,brightness);
    }

    if (brightness <= 0 || brightness >= 128) {
      fadeAmount = -fadeAmount;
    }
    previousMillis = currentMillis; 
  }

  void Fader::setBrightness(int aBrightness){
    analogWrite(pin,aBrightness);
  }
 
