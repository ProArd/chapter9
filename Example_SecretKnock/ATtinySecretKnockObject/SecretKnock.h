/*
* Secret Knock Project adapted into an Arduino 1.0 libraru for Attiny85
* GPL
 Detects patterns of knocks and triggers a motor to unlock
 it if the pattern is correct.
 
 By Steve Hoefer http://grathio.com
 Version 0.1.10.20.10
 Licensed under Creative Commons Attribution-Noncommercial-Share Alike 3.0
 http://creativecommons.org/licenses/by-nc-sa/3.0/us/
 (In short: Do what you want, just be sure to include this line and the four above it, and don't sell it or use it in anything you sell without contacting me.)
* Turned into an object by Rick Anderson http://github.com/ricklon/attinysecretknock
 */

#ifndef SecretKnock_h
#define SecretKnock_h

#if defined(ARDUINO) && ARDUINO  >= 100
#include "Arduino.h"
#include "pins_arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

#define MAX_KNOCKS 20


class SecretKnock 
{

private:
public:

  // Tuning constants.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
   int threshold;           // Minimum signal from the piezo to register as a knock
   int rejectValue;        // If an individual knock is off by this percentage of a knock we don't unlock..
   int averageRejectValue; // If the average timing of the knocks is off by this percent we don't unlock.
   int knockFadeTime;     // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)
   int lockTurnTime;      // milliseconds that we run the motor to get it to go a half turn.
   int lockMotor; 
   int knockComplete;     // Longest time to wait for a knock before we assume that it's finished.


  // Variables.
  int * knockReadings;   // When someone knocks this array fills with delays between knocks.
  int knockSensorValue;           // Last reading of the knock sensor.
  int programButtonPressed;   // Flag so we remember the programming button setting at the end of the cycle.


  int secretCode[MAX_KNOCKS];

  // Pin definitions
   int programSwitch;       // If this is high we program a new code.
  //const int servoPin = 1;  //Teensy Pin 4 is PWM
   int knockSensor;         // Piezo sensor on pin ADC1.
   int redLED;              // Status LED
   int greenLED;            // Status LED
  //const int speakerPin = 2;


  SecretKnock(void);

  void begin(int * initKnocks);

  void checkKnock();

  void listenToSecretKnock();

  void triggerDoorUnlock();

  boolean validateKnock();

  void openDoor();

  void blinkSuccess();

  void playSuccess();

};
#endif

