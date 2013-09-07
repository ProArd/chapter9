
#include "SecretKnock.h"
//#define giving me trouble
#define SERVO_PIN 1 

int initKnocks[MAX_KNOCKS]= { 50, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 

SecretKnock sKnock;

void setup() {

sKnock.begin(initKnocks);
  
}

void loop() {
  
  sKnock.checkKnock();

} 


