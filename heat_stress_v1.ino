/*
 * Heat Stress Monitor
 * Description: Utilizes the body temperature, galvanic skin response (GSR), and
 * pulse oximetry sensors to get temperature, skin conductance, and oxygen levels
 * respectively. Warns user of heat stress if these readings reach dangerous
 * levels by blinking LEDs.
 * 
 */



#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "Blinker.h"
#include <LiquidCrystal.h>


LiquidCrystal lcd(8,9,10,11,12,13);
MAX30105 particleSensor;

// Variables to read sensor
int gsrSensor, beatAvg;
float tempSensor, tempC, gsrResistance, gsrConductance, beatsPerMinute;
int buttonState = 0;
int displayMode = 0;

// For Pulse Ox
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long irValue = 0;
long lastBeat = 0;

//Time Values to Refresh LCD Display
unsigned long lcdCurrentTime;
unsigned long lcdInitialTime = 0;
unsigned long lcdRefreshPeriod = 500;
int blinkStressWarning = 0;


//TODO: Setup Threshold Values
float tempThreshold = 39;
float gsrThreshold = 35;
float bpmThreshold = 190;
float pulseOxThreshold = 96;
int tempWarning = 0;
int gsrWarning = 0;
int bpmWarning = 0;

Blinker LED1(3,100,100);
Blinker LED2(4,200,200);
Blinker LED3(5,300,300);
Fader Buzzer(6,100,100,10);

template <typename T>
T runningAverage(int sensorPin, int samples=10) {
  T sum = 0;
  T avg = 0;
  for (int i = 0; i<samples; i++) {
    sum += analogRead(sensorPin);
    delay(1);
  }
  avg = sum/(T)samples;
  return avg;
}

void getBPM() {
  if (checkForBeat(irValue) == true) {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60.0 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
}


void setup() {
  Serial.begin(115200);
  lcd.begin(16,2);
  lcd.print("Initializing...");
  
  LED1.turnOn();
  Buzzer.setBrightness(50);
  delay(500);
  LED2.turnOn();
  Buzzer.setBrightness(75);
  delay(500);
  LED3.turnOn();
  Buzzer.setBrightness(100);
  delay(1000);
  LED1.turnOff();
  LED2.turnOff();
  LED3.turnOff();
  Buzzer.setBrightness(0);
  delay(500);
  LED1.turnOn();
  LED2.turnOn();
  LED3.turnOn();
  Buzzer.setBrightness(100);
  delay(1000);
  LED1.turnOff();
  LED2.turnOff();
  LED3.turnOff();
  Buzzer.setBrightness(0);
  
  // Setup Sensors
  pinMode(7,INPUT); //Button
  pinMode(A0,INPUT); // Body Temperature Sensor
  pinMode(A1,INPUT); // GSR 
  Serial.println("Temperature and GSR sensors are setup.");
   // Pulse Ox Sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop() {
  // Body Temperature
  tempSensor = runningAverage<float>(A0,10);
  tempC = tempSensor*.48828125;
  // GSR Sensor
  gsrSensor = analogRead(A1);
  gsrResistance = ((1024.0+2.0*gsrSensor)*10000.0)/(512.0-gsrSensor);
  gsrConductance = (1.0/gsrResistance)*1000000.0; //microSiemens
  // Pulse Ox Sensor
  irValue = particleSensor.getIR();
  getBPM();

  // Heat Stress Thresholds
  // Blink LEDs and/or Enable Buzzer if threshold is reached
  if (tempC >= tempThreshold) {
    LED1.blink();
    tempWarning = 1;
  }
  else {
    LED1.turnOff();
    tempWarning = 0;
  }
  if (gsrConductance >= gsrThreshold) {
    LED2.blink();
    Buzzer.blink();
    gsrWarning = 1;
  }
  else {
    LED2.turnOff();
    Buzzer.turnOff();
    gsrWarning = 0;
  }

  if (beatsPerMinute >= bpmThreshold) {
    LED3.blink();
    bpmWarning = 1;
    //Buzzer.blink();
  }
  else {
    LED3.turnOff();
    bpmWarning = 0;
  }

  //Print Info to Serial
  Serial.print("Temperature (C): ");
  Serial.print(tempC);
  Serial.print("\t");
  Serial.print("Skin Conductance (uS): ");
  Serial.print(gsrConductance);
  Serial.print("\t");
  Serial.print(gsrSensor);
  Serial.print("\t");
  Serial.print("IR: ");
  Serial.print(irValue);
  Serial.print("\t");
  Serial.print("BPM: ");
  Serial.print(beatsPerMinute);
  Serial.print("\n");
  Serial.print(displayMode);

  // Show Info on LCD Display
  lcdCurrentTime = millis();
  if (lcdCurrentTime - lcdInitialTime >= lcdRefreshPeriod) {
    if (displayMode == 0) {
      lcd.clear(); 
      lcd.setCursor(0,0);
      lcd.print("Temp.(C): ");
      lcd.print(tempC);
    }
    else if (displayMode == 1) {
      lcd.clear(); 
      lcd.setCursor(0,0);
      lcd.print("GSR(uS):");
      lcd.print(gsrConductance);
    }
    else if (displayMode == 2) {
      lcd.clear(); 
      lcd.setCursor(0,0);
      lcd.print("BPM: ");
      lcd.print(beatsPerMinute);
    }
    lcdInitialTime = lcdCurrentTime;

    //Display Heat Stress Warning
    if (tempWarning || gsrWarning || bpmWarning) {
      if (blinkStressWarning == 1) {
        lcd.setCursor(0,1);
        lcd.print("STRESS WARNING!");
        blinkStressWarning = 0;
      }
      else {
        blinkStressWarning = 1;
      }
    }
  }

  //Button to Change What Info to Display
  buttonState = digitalRead(7);
  //Button
  if (buttonState == HIGH) {
    displayMode += 1;
    Buzzer.blink();
    lcdCurrentTime += 500;
    delay(200);
  }
  if (displayMode > 2) {
    displayMode = 0;
  }
  
  //delay(100);
}
