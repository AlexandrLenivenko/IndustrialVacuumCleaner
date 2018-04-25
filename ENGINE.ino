
#include <Bounce2.h>
//pins
const int FIRST_ENGINE  = 14;   //A0
const int SECOND_ENGINE = 15;	  //A1
const int THIRD_ENGINE  = 16;	  //A2
const int TURBO_BUTTON  = 10;   //10

//variable const
const int PERIOD = 1500;
const int ENGINES_COUNT = 3;
const int engines[3] = {FIRST_ENGINE, SECOND_ENGINE, THIRD_ENGINE};

int state = 0;
long priveousTime = 0;
boolean isTurbo = false;
boolean previousTurboState = false;

// Instantiate a Bounce object
Bounce debouncer = Bounce(); 

void setup() {
  Serial.begin(9600);
  pinMode(FIRST_ENGINE, OUTPUT);
  pinMode(SECOND_ENGINE, OUTPUT);
  pinMode(THIRD_ENGINE, OUTPUT);
  pinMode(TURBO_BUTTON, INPUT);
  priveousTime = millis();

  debouncer.attach(TURBO_BUTTON);
  debouncer.interval(150); // interval in ms

  changeEngineState();
}

void loop() {
checkForTurbo();
if(isTurbo) {
    startTurboMode();
  }else{
    waitNotify();
  }

  // Delay a little bit to avoid bouncing
    delay(50);
}

void checkForTurbo() {
    // Update the Bounce instance :
  if(debouncer.update()){        
    if(debouncer.rose()) {
      Serial.println("HIGH");
      isTurbo = !isTurbo;
      previousTurboState = true;
    }
   }
}

//change working mode one of  all engin's pin should be in HIGH case. One by one.
void startTurboMode() {
  if(previousTurboState){
        for(int i =0; i<ENGINES_COUNT; i++){        
          digitalWrite(engines[i], HIGH);
        }
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

