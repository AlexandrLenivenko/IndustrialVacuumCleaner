
#include <Bounce2.h>
                                //pin:
const int FIRST_ENGINE  = 14;   //A0
const int SECOND_ENGINE = 15;	  //A1
const int THIRD_ENGINE  = 16;	  //A2
const int TURBO_BUTTON  = 10;   //10
const int START_BUTTON  = 11;   //11
const int U             = A6;   //A6
const int GREEN_LED     = 13;   //?
const int RED_LED       = A7;   //?

//variable const
//time const 
const int PERIOD            = 1500; // engin's sleep
const int DELAY_STOP_ENGINS = 5000; // working time after no U

const int ENGINES_COUNT = 3;

// Чуствительность датчика на 30А  ---  66mV / A   30/1023*66 = 0,0567590577996405
const int BARRIER = 2;              // 2 * 
const int engines[3] = {FIRST_ENGINE, SECOND_ENGINE, THIRD_ENGINE};

int GREEN_LED_PERIOD = 1000;
int state = 0;
long priveousTime = 0;
long greenLedTime = 0;
boolean isTurbo = false;
boolean previousTurboState = false;
boolean canStart = false;
boolean greenLedState = false;
boolean isShouldStop = true;

// Instantiate a Bounce  for turbo button
Bounce turboModeDebouncer = Bounce(); 

// Instantiate a Bounce  for start button
Bounce startButtonDebouncer = Bounce(); 

void setup() {
  Serial.begin(9600);
  //OUTPUT
  pinMode(FIRST_ENGINE, OUTPUT);
  pinMode(SECOND_ENGINE, OUTPUT);
  pinMode(THIRD_ENGINE, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  //INPUT
  pinMode(TURBO_BUTTON, INPUT);
  pinMode(START_BUTTON, INPUT);
  
  turboModeDebouncer.attach(TURBO_BUTTON);
  turboModeDebouncer.interval(150); // interval in ms
  startButtonDebouncer.attach(START_BUTTON);
  startButtonDebouncer.interval(150); // interval in ms

  priveousTime = millis();
}

void loop() {
  checkStart();
  if(canStart || readU() > BARRIER){
     startEngines();
  }else{
    isShouldStop = shouldStop();
    if(isShouldStop) {
      turnOnOrOffAllEngins(LOW);
    }
   }

  // Delay a little bit to avoid bouncing
    delay(50);
}

void checkStart(){
  if(startButtonDebouncer.update()){
    if(startButtonDebouncer.rose()){
      canStart = !canStart;
      if(!canStart) {
        turnOnOrOffAllEngins(LOW);
      }
    }
  }
  showIndication();
}


void startEngines(){
  checkForTurbo();
  if(isTurbo) {
    startTurboMode();
  }else{
    waitNotify();
  }
}

void checkForTurbo() {
    // Update the Bounce instance :
  if(turboModeDebouncer.update()){        
    if(turboModeDebouncer.rose()) {
      Serial.println("MODE WAS CHENGED");
      isTurbo = !isTurbo;
      previousTurboState = true;
    }
   }
}

//change working mode one of  all engin's pin should be in HIGH case. One by one.
void startTurboMode() {
  if(previousTurboState){
        turnOnOrOffAllEngins(HIGH);
        previousTurboState = false;
        Serial.println("TURBO TURN ON");
  }
}

//mesuring time according cost PERIOD
void waitNotify() {
      if(millis() - priveousTime >= PERIOD) {
        changeEngineState();
        priveousTime = millis();
       }
  }


//change working mode one of pins should be in LOW case. One by one.
void changeEngineState() {
      for(int i =0; i<ENGINES_COUNT; i++){
          if(state == i) {
            digitalWrite(engines[i], LOW);
            }else{
              digitalWrite(engines[i], HIGH);
            }
    }

  state = ++state % ENGINES_COUNT;
}

void turnOnOrOffAllEngins(int state){
  for(int i =0; i<ENGINES_COUNT; i++){
        digitalWrite(engines[i], state);
  }
  if(state == LOW) {
      previousTurboState = false;
      isTurbo = false;
      isShouldStop = true;
  }
  Serial.print("ALL ENGINES WERE TURNED ");  
  Serial.println(state);
}

boolean shouldStop() {
  long delayToStopTime = millis();
  boolean shouldStop = true;
  
  while(millis() - delayToStopTime <= DELAY_STOP_ENGINS) {
     int read = readU();
     checkStart();
     if(read > BARRIER || canStart) {
       shouldStop = false;
       break;
     }
  }
  return shouldStop;
}
int readU(){        
  int read = analogRead(U);
  Serial.print("U=");
  
  read = read - 512;
  if(read < 0) {
     read = read * (-1);
  }
  
  Serial.println(read);
  return read;
}

void showIndication(){
  int green_led_level = greenLedState;
  if(canStart || !isShouldStop){
      green_led_level = 1;
    }else{
      if(millis() - greenLedTime > GREEN_LED_PERIOD){
        greenLedState = !greenLedState;
        green_led_level = greenLedState;
        greenLedTime = millis();
      }
    }
  digitalWrite(GREEN_LED, green_led_level);  
  digitalWrite(RED_LED, isTurbo);  
}
