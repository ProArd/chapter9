/*
* Simple manual servo library for ATtiny85
 * GPL
 */
#ifndef attinyservo_h
#define attinyservo_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#include "pins_arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

#ifndef SERVO_PIN
#define SERVO_PIN 1
#endif

void servoBegin();
void moveServo(int lenMicroSecondsOfPulse, int delayMillis);


#endif


