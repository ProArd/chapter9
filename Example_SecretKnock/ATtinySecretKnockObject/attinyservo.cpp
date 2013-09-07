/*
* Simple manual servo library for ATtiny85
*  Manual servo control 
* Copyright 2012 by Zipwhip. 
* You are free to use and modify this code in your own software.
* http://blog.zipwhip.com/2012/03/28/manual-control-of-a-servo-on-the-arduino-for-the-zipwhip-textspresso-machine/
* Converted to an object by Rick Anderson http://github.com/ricklon/attinysecretknock
*/

#include "attinyservo.h"


int lenMicroSecondsOfPeriod = 20 * 1000; // 20 milliseconds (ms)
int lenMicroSecondsOfPulse = 1.8 * 1000; // 1.0 ms is 0 degrees

void servoBegin()
{
   pinMode(SERVO_PIN, OUTPUT); 
}

void  moveServo(int lenMicroSecondsOfPulse, int delayMillis)
{

  int currentMillis = millis();
  int previousMillis = 0;

  lenMicroSecondsOfPulse = lenMicroSecondsOfPulse*1000;
  while(currentMillis - previousMillis > delayMillis)
  {
    previousMillis = currentMillis;
    // Servos work by sending a 20 ms pulse.  
    // 1.0 ms at the start of the pulse will turn the servo to the 0 degree position
    // 1.5 ms at the start of the pulse will turn the servo to the 90 degree position 
    // 2.0 ms at the start of the pulse will turn the servo to the 180 degree position 
    // Turn voltage high to start the period and pulse
    digitalWrite(SERVO_PIN, HIGH);


    // Delay for the length of the pulse
    delayMicroseconds(lenMicroSecondsOfPulse);

    // Turn the voltage low for the remainder of the pulse
    digitalWrite(SERVO_PIN, LOW);

    // Delay this loop for the remainder of the period so we don't
    // send the next signal too soon or too late
    delayMicroseconds(lenMicroSecondsOfPeriod - lenMicroSecondsOfPulse);  
  }
}

