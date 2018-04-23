int FIRST_ENGINE = 14;   //A0
int SECOND_ENGINE = 15;	 //A1
int THIRD_ENGINE = 16;	 //A2

int engines[3] = {FIRST_ENGINE, SECOND_ENGINE, THIRD_ENGINE};
int enginesState[3] = {HIGH, HIGH, LOW};

int state = 0;

int priveousTime = 0;

const int PERIOD = 1000;

void setup() {
  pinMode(FIRST_ENGINE, OUTPUT);
  pinMode(SECOND_ENGINE, OUTPUT);
  pinMode(THIRD_ENGINE, OUTPUT);

  priveousTime = millis();
}

void loop() {

  if(millis() - priveousTime >= PERIOD) {
      changeEngineState();
      priveousTime = millis();
    }

  for(int i=0; i<3; i++) {
        digitalWrite(engines[i], enginesState[i]);
    }

}

void changeEngineState() {
    switch(state) {
      case 0: enginesState[0] = LOW;
              enginesState[1] = HIGH;
              enginesState[2] = HIGH;
              state = 1;
              break;
      case 1: enginesState[0] = HIGH;
              enginesState[1] = LOW;
              enginesState[2] = HIGH;
              state = 2;
              break;
      case 2: enginesState[0] = HIGH;
              enginesState[1] = HIGH;
              enginesState[2] = LOW;
              state = 0;
              break;  
      }
  }
