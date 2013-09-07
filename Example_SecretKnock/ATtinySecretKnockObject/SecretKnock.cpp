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
 
 */
#include "SecretKnock.h"
#include "attinyservo.h"


SecretKnock::SecretKnock(void)
{
  // Pin definitions
  const int programSwitch = 0;       // If this is high we program a new code.
  //const int servoPin = 1;  //Teensy Pin 4 is PWM
  knockSensor = 1;         // Piezo sensor on pin ADC1.
  redLED = 3;              // Status LED
  greenLED = 4;            // Status LED
  //const int speakerPin = 2;

  // Tuning constants.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
  threshold = 500;           // Minimum signal from the piezo to register as a knock
  rejectValue = 25;        // If an individual knock is off by this percentage of a knock we don't unlock..
  averageRejectValue = 15; // If the average timing of the knocks is off by this percent we don't unlock.
  knockFadeTime = 200;     // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)
  lockTurnTime = 650;      // milliseconds that we run the motor to get it to go a half turn.
  lockMotor = 2; 
  knockComplete = 1200;     // Longest time to wait for a knock before we assume that it's finished.


  // Variables.
  // knockReadings[MAX_KNOCKS];   // When someone knocks this array fills with delays between knocks.
   knockSensorValue = 0;           // Last reading of the knock sensor.
   programButtonPressed = false;   // Flag so we remember the programming button setting at the end of the cycle.


}



void SecretKnock::begin(int  initKnocks[])
{

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(programSwitch, INPUT);
  
  digitalWrite(greenLED, HIGH);      // Green LED on, everything is go.
  delay(1000);
  digitalWrite(greenLED, LOW);      // Green LED on, everything is go.
  delay(1000);
  knockReadings = initKnocks;
  servoBegin();
  moveServo(1.0, 1000);
  delay(1000);

}

void SecretKnock::checkKnock()
{
  // Listen for any knock at all.
  knockSensorValue = analogRead(knockSensor);

  //Serial.println(knockSensorValue);

  if (digitalRead(programSwitch)==HIGH){  // is the program button pressed?
    programButtonPressed = true;          // Yes, so lets save that state
    digitalWrite(redLED, HIGH);           // and turn on the red light too so we know we're programming.
  } 
  else {
    programButtonPressed = false;
    digitalWrite(redLED, LOW);
  }

  if (knockSensorValue >=threshold)
  {
    listenToSecretKnock();
  }
}

// Records the timing of knocks.
void SecretKnock::listenToSecretKnock()
{

  int ii = 0;
  // First lets reset the listening array.
  for (ii=0; ii<MAX_KNOCKS; ii++){
    knockReadings[ii]=0;
  }

  int currentKnockNumber=0;                     // Incrementer for the array.
  int startTime=millis();                       // Reference for when this knock started.
  int now=millis();

  digitalWrite(greenLED, LOW);                  // we blink the LED for a bit as a visual indicator of the knock.
  if (programButtonPressed==true){
    digitalWrite(redLED, LOW);                         // and the red one too if we're programming a new knock.
  }
  delay(knockFadeTime);                                 // wait for this peak to fade before we listen to the next one.
  digitalWrite(greenLED, HIGH);  
  if (programButtonPressed==true){
    digitalWrite(redLED, HIGH);                        
  }
  do {
    //listen for the next knock or wait for it to timeout. 
    knockSensorValue = analogRead(knockSensor);
    if (knockSensorValue >=threshold){                   //got another knock...
      //record the delay time.

      now=millis();
      knockReadings[currentKnockNumber] = now-startTime;
      currentKnockNumber ++;                             //increment the counter
      startTime=now;          
      // and reset our timer for the next knock
      digitalWrite(greenLED, LOW);  
      if (programButtonPressed==true){
        digitalWrite(redLED, LOW);                       // and the red one too if we're programming a new knock.
      }
      delay(knockFadeTime);                              // again, a little delay to let the knock decay.
      digitalWrite(greenLED, HIGH);
      if (programButtonPressed==true){
        digitalWrite(redLED, HIGH);                         
      }
    }

    now=millis();

    //did we timeout or run out of knocks?
  } 
  while ((now-startTime < knockComplete) && (currentKnockNumber < MAX_KNOCKS));

  //we've got our knock recorded, lets see if it's valid
  if (programButtonPressed==false)
  {             // only if we're not in progrmaing mode.
    if (validateKnock() == true)
    {
      // triggerDoorUnlock(); 
      openDoor();
      //playSuccess();
      blinkSuccess();
    } 
    else 
    {
      digitalWrite(greenLED, LOW);          // We didn't unlock, so blink the red LED as visual feedback.
      moveServo(1.0, 100); // lock the door
      delay(1000);
      for (ii=0;ii<4;ii++){                    
        digitalWrite(redLED, HIGH);
        delay(100);
        digitalWrite(redLED, LOW);
        delay(100);
      }
      digitalWrite(greenLED, HIGH);
    }
  } 
  else { // if we're in programming mode we still validate the lock, we just don't do anything with the lock
    validateKnock();
    // and we blink the green and red alternately to show that program is complete.
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    for (ii=0;ii<3;ii++){
      delay(100);
      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
      delay(100);
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, HIGH);      
    }
  }
}


// Runs the motor (or whatever) to unlock the door.
void SecretKnock::triggerDoorUnlock(){
  int ii=0;

  // turn the motor on for a bit.
  digitalWrite(lockMotor, HIGH);
  digitalWrite(greenLED, HIGH);            // And the green LED too.

  delay (lockTurnTime);                    // Wait a bit.

  digitalWrite(lockMotor, LOW);            // Turn the motor off.

  // Blink the green LED a few times for more visual feedback.
  for (ii=0; ii < 5; ii++){   
    digitalWrite(greenLED, LOW);
    delay(100);
    digitalWrite(greenLED, HIGH);
    delay(100);
  }

}


// Sees if our knock matches the secret.
// returns true if it's a good knock, false if it's not.
// todo: break it into smaller functions for readability.
boolean SecretKnock::validateKnock()
{
  int ii=0;

  // simplest check first: Did we get the right number of knocks?
  int currentKnockCount = 0;
  int secretKnockCount = 0;
  int maxKnockInterval = 0;                     // We use this later to normalize the times.

  for (ii=0;ii<MAX_KNOCKS;ii++){
    if (knockReadings[ii] > 0){
      currentKnockCount++;
    }
    if (secretCode[ii] > 0){                     //todo: precalculate this.
      secretKnockCount++;
    }

    if (knockReadings[ii] > maxKnockInterval){   // collect normalization data while we're looping.
      maxKnockInterval = knockReadings[ii];
    }
  }

  // If we're recording a new knock, save the info and get out of here.
  if (programButtonPressed==true){
    for (ii=0;ii<MAX_KNOCKS;ii++){ // normalize the times
      secretCode[ii]= map(knockReadings[ii],0, maxKnockInterval, 0, 100); 
    }
    // And flash the lights in the recorded pattern to let us know it's been programmed.
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    delay(1000);
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, HIGH);
    delay(50);
    for (ii = 0; ii < MAX_KNOCKS ; ii++){
      digitalWrite(greenLED, LOW);
      digitalWrite(redLED, LOW);  
      // only turn it on if there's a delay
      if (secretCode[ii] > 0){                                   
        delay( map(secretCode[ii],0, 100, 0, maxKnockInterval)); // Expand the time back out to what it was.  Roughly. 
        digitalWrite(greenLED, HIGH);
        digitalWrite(redLED, HIGH);
      }
      delay(50);
    }
    return false;     // We don't unlock the door when we are recording a new knock.
  }

  if (currentKnockCount != secretKnockCount){
    return false; 
  }

  /*  Now we compare the relative intervals of our knocks, not the absolute time between them.
   (ie: if you do the same pattern slow or fast it should still open the door.)
   This makes it less picky, which while making it less secure can also make it
   less of a pain to use if you're tempo is a little slow or fast. 
   */
  int totaltimeDifferences=0;
  int timeDiff=0;
  for (ii=0;ii<MAX_KNOCKS;ii++){ // Normalize the times
    knockReadings[ii]= map(knockReadings[ii],0, maxKnockInterval, 0, 100);      
    timeDiff = abs(knockReadings[ii]-secretCode[ii]);
    if (timeDiff > rejectValue){ // Individual value too far out of whack
      return false;
    }
    totaltimeDifferences += timeDiff;
  }
  // It can also fail if the whole thing is too inaccurate.
  if (totaltimeDifferences/secretKnockCount>averageRejectValue){
    return false; 
  }

  return true;

}

void SecretKnock::openDoor()
{

  moveServo(2.0, 100);
  delay(1000);
}


void SecretKnock::blinkSuccess()
{
  for (int ii = 0; ii < 50; ii++)
  {
    digitalWrite(greenLED, HIGH);
    delay(25);
    digitalWrite(greenLED, LOW);
    delay(25);
  }
}
