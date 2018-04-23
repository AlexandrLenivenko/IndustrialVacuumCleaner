int FIRST_ENGINE = 14;   //A0
int SECOND_ENGINE = 15;	 //A1
int THIRD_ENGINE = 16;	 //A2
int TURBO_BUTTON = 10;   //10

int engines[3] = {FIRST_ENGINE, SECOND_ENGINE, THIRD_ENGINE};
int enginesState[3] = {HIGH, HIGH, LOW};

int state = 1;

long priveousTime = 0;

const int PERIOD = 1000;
boolean isTurbo = false;

void setup() {
  Serial.begin(9600);
  pinMode(FIRST_ENGINE, OUTPUT);
  pinMode(SECOND_ENGINE, OUTPUT);
  pinMode(THIRD_ENGINE, OUTPUT);
  pinMode(TURBO_BUTTON, INPUT);
  priveousTime = millis();
}

void loop() {
checkForTurbo();
if(isTurbo) {
    startTurboMode();
  }else{
    waitNotify();
  }
}

void checkForTurbo() {
    int temp = digitalRead(TURBO_BUTTON);
    if(temp == HIGH) {
        Serial.println("HIGH");
        
        isTurbo = !isTurbo;
        delay(150);
      }
  }
  
void startTurboMode() {
    digitalWrite(engines[0], HIGH);
    digitalWrite(engines[1], HIGH);
    digitalWrite(engines[2], HIGH);
  }

void waitNotify() {
      if(millis() - priveousTime >= PERIOD) {
        changeEngineState();
        priveousTime = millis();
       }
  }


void changeEngineState() {
    switch(state) {
      case 0: digitalWrite(engines[0], LOW);
              digitalWrite(engines[1], HIGH);
              digitalWrite(engines[2], HIGH);
              state = 1;
              break;
      case 1: digitalWrite(engines[0], HIGH);
              digitalWrite(engines[1], LOW);
              digitalWrite(engines[2], HIGH);
              state = 2;
              break;
      case 2: digitalWrite(engines[0], HIGH);
              digitalWrite(engines[1], HIGH);
              digitalWrite(engines[2], LOW);
              state = 0;
              break;  
      }
  }
