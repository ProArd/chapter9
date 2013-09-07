/*
* Fubarino Object Detection
 * Determine with one sensor which where an object is 
 * 2 - 4 IR leds
 * 1 IR Sensor
 */

//#define pinSrc 12 // used as external interrupt source
//PIN_INT1 for the FubarinoSD is Pin 0
#define pinInt PIN_INT1

#define SENSOR1_PIN 0
#define EMITTER1_PIN 12
#define EMITTER2_PIN 13
#define BEATS 6

int emmiter1_id;
int emmiter2_id;


unsigned long blink1_var;
unsigned long blink2_var;

volatile boolean emitter1State = LOW;
volatile boolean emitter2State = LOW;
volatile boolean prevEmitter1State = LOW;
volatile boolean prevEmitter2State = LOW;

volatile boolean detected = LOW;
volatile boolean e1detected = LOW;
volatile boolean e2detected = LOW;

volatile unsigned long emit1Count = 0;
volatile unsigned long emit2Count = 0;
volatile unsigned long detectCount = 0;

volatile int phaseA = 0;
volatile int phaseB = 0;
volatile int prevPhaseA = -1;
volatile int prevPhaseB = -1;
volatile int measureA = 0;
volatile int measureB = 0;
volatile int prevMeasureA = -1;
volatile int prevMeasureB = -1;


//Prototypes
void blink_emitter1(int id, void * tptr);
void blink_emitter2(int id, void * tptr);
void readIRSensor();
void blink(int);

void setup() {
  Serial.begin(115200);
  delay(2000);
  // initialize the digital pin as an output.
  // Pin PIN_LED1 has an LED connected on most Arduino boards:
  pinMode(pinInt, INPUT);
  //debugging LED, shows when pulse found
  pinMode(PIN_LED1, OUTPUT);
  digitalWrite(PIN_LED1, HIGH);
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(EMITTER1_PIN, OUTPUT);
  pinMode(EMITTER2_PIN, OUTPUT);

  digitalWrite(EMITTER1_PIN, LOW);
  digitalWrite(EMITTER2_PIN, LOW);    
  //blink before timers and interrups
  blinkAll(6);

  attachInterrupt(1, readIRSensor, RISING);
  emmiter1_id = createTask(blink_emitter1, 13, TASK_ENABLE, &blink1_var);
  emmiter2_id = createTask(blink_emitter2, 13, TASK_ENABLE, &blink2_var);

}

void loop() {
  digitalWrite(PIN_LED1, LOW);
  if (detected) {      
    Serial.print("{ \"IRDetect\": ");
    Serial.print(detectCount);
    Serial.print(" ,\"measureA\": ");
    Serial.print(measureA);
    Serial.print(" ,\"measureB\": ");
    Serial.print(measureB);
    Serial.print(" ,\"phaseA\": ");
    Serial.print(phaseA);
    Serial.print(" ,\"phaseB\": ");
    Serial.print(phaseB);
    Serial.print(" ,\"Emmit1\": ");
    Serial.print((int)emitter1State);
    Serial.print(" ,\"prevEmmit1\": ");
    Serial.print((int)prevEmitter1State);
    Serial.print(" ,\"count\": ");
    Serial.print(emit1Count);
    Serial.print(" ,\"Emmit2\": ");
    Serial.print((int)emitter2State);
    Serial.print(" ,\"prevEmmit2\": ");
    Serial.print((int)prevEmitter2State);
    Serial.print(" ,\"count\": ");
    Serial.print(emit2Count);


    if(emitter1State) {
      prevEmitter1State = emitter1State;
      Serial.print(" ,\"Obj\": \"Right\"");
    } 
    if (prevMeasureA == measureA) {
      if (e1detected && e2detected)
      {
        Serial.print(" ,\"Obj\": \"Front\"");
      }
    } 
   if(emitter2State) {
      prevEmitter2State = emitter2State;
      Serial.print(" ,\"Obj\": \"Left\"");
    } 
    Serial.println("}");
    prevMeasureA = measureA;
    prevMeasureB = measureB;
    prevPhaseA = phaseA;
    detected = false;
  }

}

void readIRSensor() {
  digitalWrite(PIN_LED1, HIGH);
  if(emitter1State) {
    emit1Count++;
    detectCount++;
    detected = true;
    e1detected = true;
  }  
  else if (emitter2State) {
    emit2Count++;
    detectCount++;
    detected = true;
    e2detected = true;
  }
}

void blink_emitter1(int id, void * tptr) {
  if(phaseA >= BEATS) {
    phaseA = 0;
    measureA++;
    e1detected = false;
  }

  if (phaseA== 1) {
    emitter1State = true;
    phaseA++;
    digitalWrite(EMITTER1_PIN, emitter1State);   
  }
  else  {
    emitter1State = false;
    phaseA++;
    digitalWrite(EMITTER1_PIN, emitter1State);   

  }
}

void blink_emitter2(int id, void * tptr) {
  if(phaseB >= BEATS) {
    phaseB = 0;
    measureB++;
    e2detected = false;
  }
  if (phaseB == 3) {
    emitter2State = true;
    phaseB++;
    digitalWrite(EMITTER2_PIN, emitter2State);  
  }
  else  
  {
    emitter2State = false;
    phaseB++;
    digitalWrite(EMITTER2_PIN, emitter2State);  
  }

}

void blinkAll(int loops)
{
  for (int ii = 0; ii < loops; ii++)
  {
    digitalWrite(PIN_LED1, HIGH);
    digitalWrite(EMITTER1_PIN, HIGH);
    digitalWrite(EMITTER2_PIN, HIGH);
    delay(250);
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(EMITTER1_PIN, LOW);
    digitalWrite(EMITTER2_PIN, LOW);     
    delay(250);
  }
}


