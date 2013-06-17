/*
  Blink Test All IO Pins
 Turns on an LED on for one second, then off for one second, repeatedly.
 
 This example code is in the public domain.
 */


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  for (int ii = 0; ii < 5; ii++)
  {
    pinMode(ii, OUTPUT); 
  }

}

// the loop routine runs over and over again forever:
void loop() {

  for (int ii = 0; ii < 5; ii++)
  {
    digitalWrite(ii, HIGH);
    delay(250);
    digitalWrite(ii, LOW); 
    delay(250);
  }


}


