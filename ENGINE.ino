
#include <Bounce2.h>

//*PINS DECLARETION*//
//pin:
//relay
const int FIRST_STARTING_RELAY  = 14;   //A0
const int SECOND_STARTING_RELAY = 15;	  //A1
const int THIRD_STARTING_RELAY  = 16;	  //A2
const int FIRST_MAIN_RELAY      = 2;    //D2
const int SECOND_MAIN_RELAY     = 3;    //D3
const int THIRD_MAIN_RELAY      = 4;    //D4

//buttons
const int START_BUTTON          = 11;   //D11
//U sensor
const int U                     = A3;   //A3
//LED
const int GREEN_LED             = 12;   //?
const int RED_LED               = 13;   //?

//TERMOCUPLE 
const int TERMOCUPLE1           = 10;   //10
const int TERMOCUPLE2           = 7;    //7
const int TERMOCUPLE3           = 6;    //6
//* CONST*//
const int ENGINES_COUNT = 3;

//time
const int PERIOD                      = 5000;  // engin's sleep
const int DELAY_STOP_ENGINS           = 5000;  // working time after no U
const int GREEN_LED_PERIOD            = 1000;
const int PERIOD_WAIT_MAIN_RELAY      = 3200;
const int START_TURBO_MODE_TIME       = 2000;
const int SLEEP                       = 1500;
// U barrier
const int BARRIER                     = 2;    // Sensitivity of the sensor to 30А  ---  66mV / A   30/1023*66 = 0,0567590577996405
//arrais
const int startingRelayArr[3] = {FIRST_STARTING_RELAY, SECOND_STARTING_RELAY, THIRD_STARTING_RELAY};
const int mainRelayArr[3] = {FIRST_MAIN_RELAY, SECOND_MAIN_RELAY, THIRD_MAIN_RELAY};

//*VARIEBLES *//

int state                       = 0;
long priveousTime               = 0;
long greenLedTime               = 0;
long pressStartButtonTime       = 0;
boolean isTurbo                 = false;
boolean previousTurboState      = false;
boolean canStart                = false;
boolean greenLedState           = false;
boolean isShouldStop            = true;
boolean shouldChMainRelayState  = false;


// Instantiate a Bounce  for start button
Bounce startButtonDebouncer = Bounce();

void setup() {
  Serial.begin(9600);
  //OUTPUT
  pinMode(FIRST_STARTING_RELAY, OUTPUT);
  pinMode(SECOND_STARTING_RELAY, OUTPUT);
  pinMode(THIRD_STARTING_RELAY, OUTPUT);
  pinMode(FIRST_MAIN_RELAY, OUTPUT);
  pinMode(SECOND_MAIN_RELAY, OUTPUT);
  pinMode(THIRD_MAIN_RELAY, OUTPUT);
  turnOnOrOffAllEngins(HIGH);
  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  //INPUT
  pinMode(START_BUTTON, INPUT);
  pinMode(TERMOCUPLE1, INPUT);
  pinMode(TERMOCUPLE2, INPUT);
  pinMode(TERMOCUPLE3, INPUT);

  startButtonDebouncer.attach(START_BUTTON);
  startButtonDebouncer.interval(150); // interval in ms

  priveousTime = millis();
}

void loop() {
  checkStart();
  if (canStart || readU() > BARRIER) {
    startEngines();
  } else {
    isShouldStop = shouldStop();
    if (isShouldStop) {
      turnOnOrOffAllEngins(HIGH);
    }
  }

  // Delay a little bit to avoid bouncing
  delay(50);
}

void checkStart() {
  alerm();
  if (startButtonDebouncer.update()) {
    if (startButtonDebouncer.rose()) {
      if (millis() - pressStartButtonTime >= START_TURBO_MODE_TIME && canStart) {
        Serial.println("MODE WAS CHENGED");
        isTurbo = !isTurbo;
        previousTurboState = true;
      } else {
        canStart = !canStart;
      }
      if (!canStart) {
        turnOnOrOffAllEngins(HIGH);
      }
    }
  }
  showIndication();
}


void startEngines() {
  checkForTurbo();
  if (isTurbo) {
    startTurboMode();
  } else {
    waitNotify();
  }
}

void checkForTurbo() {
  // Update the Bounce instance :
  if (startButtonDebouncer.fell()) {
    pressStartButtonTime = millis();
    Serial.println("checkForTurbo");
    Serial.println("pressStartButtonTime");
  }
}

//change working mode one of  all engin's pin should be in HIGH case. One by one.
void startTurboMode() {
  if (previousTurboState) {
    turnOnOrOffAllEngins(LOW);
    previousTurboState = false;
    Serial.println("TURBO TURN ON");
  }
}

//mesuring time according cost PERIOD
void waitNotify() {
  Serial.println("waitNotify");
  if (millis() - priveousTime >= PERIOD) {
    changeStartingRelayState();
    priveousTime = millis();
    shouldChMainRelayState = true;
  }
  if (shouldChMainRelayState && millis() - priveousTime >= PERIOD_WAIT_MAIN_RELAY)  {
    changeMainRelayState();
    shouldChMainRelayState = false;
  }
}


//change working mode one of pins should be in LOW case. One by one.
void changeStartingRelayState() {
  for (int i = 0; i < ENGINES_COUNT; i++) {
    if (state == i) {
      digitalWrite(startingRelayArr[i], HIGH);
    } else {
      digitalWrite(startingRelayArr[i], LOW);
    }
  }

  state = ++state % ENGINES_COUNT;
  digitalWrite(mainRelayArr[state], HIGH);
}

//change working mode one of pins should be in LOW case. One by one.
void changeMainRelayState() {
  for (int i = 0; i < ENGINES_COUNT; i++) {
    if (state == i) {
      digitalWrite(mainRelayArr[i], HIGH);
    } else {
      digitalWrite(mainRelayArr[i], LOW);
    }
  }

  state = ++state % ENGINES_COUNT;
}

void turnOnOrOffAllEngins(int state) {
  for (int i = 0; i < ENGINES_COUNT; i++) {
    digitalWrite(startingRelayArr[i], state);
    digitalWrite(mainRelayArr[i], state);
  }
  if (state == HIGH) {
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

  while (millis() - delayToStopTime <= DELAY_STOP_ENGINS) {
    int read = readU();
    checkStart();
    if (read > BARRIER || canStart) {
      shouldStop = false;
      break;
    }
  }
  return shouldStop;
}
int readU() {
  int read = analogRead(U);
  //Serial.print("U=");

  read = read - 512;
  if (read < 0) {
    read = read * (-1);
  }

  //Serial.println(read);
  return read;
}

void showIndication() {
  int green_led_level = greenLedState;
  if (canStart || !isShouldStop) {
    green_led_level = 1;
  } else {
    if (millis() - greenLedTime > GREEN_LED_PERIOD) {
      greenLedState = !greenLedState;
      green_led_level = greenLedState;
      greenLedTime = millis();
    }
  }
  if(isTurbo) {
    green_led_level = 0;
  }
  digitalWrite(GREEN_LED, green_led_level);
  digitalWrite(RED_LED, isTurbo);
}

void alerm() {
  //Serial.println("alerm");
  boolean dangerWork = false;
  if(digitalRead(TERMOCUPLE1) == HIGH) {
      stopAndShowProblem(TERMOCUPLE1);
  }
  if(digitalRead(TERMOCUPLE2) == HIGH) {
      stopAndShowProblem(TERMOCUPLE2);
  }

  if(digitalRead(TERMOCUPLE3) == HIGH) {
     stopAndShowProblem(TERMOCUPLE3);
  }
}

void stopAndShowProblem(int TERMOCUPLE) {
  turnOnOrOffAllEngins(HIGH);
  isTurbo  = false;
  int green_led_level = 0;
  
  while(digitalRead(TERMOCUPLE) == HIGH) {
   digitalWrite(GREEN_LED, LOW);
    if(TERMOCUPLE == TERMOCUPLE1) {
      showIndicationFirstTurmcuple();
    }

    if(TERMOCUPLE == TERMOCUPLE2) {
      showIndicationSecondtTurmcuple();
    }

    if(TERMOCUPLE == TERMOCUPLE3) {
      showIndicationTheardTurmcuple();
    }
  }   
 }

void showIndicationFirstTurmcuple() {
    digitalWrite(RED_LED, HIGH);
    delay(SLEEP/2);
    digitalWrite(RED_LED, LOW); 
    delay(SLEEP);
}

void showIndicationSecondtTurmcuple() {
    digitalWrite(RED_LED, HIGH);
    delay(SLEEP/3);
    digitalWrite(RED_LED, LOW);
    delay(SLEEP/3); 
    digitalWrite(RED_LED, HIGH);
    delay(SLEEP/3);
    digitalWrite(RED_LED, LOW); 
    delay(SLEEP);
}

void showIndicationTheardTurmcuple() {
    digitalWrite(RED_LED, HIGH);
    delay(SLEEP/4);
    digitalWrite(RED_LED, LOW);
    delay(SLEEP/4); 
    digitalWrite(RED_LED, HIGH);
    delay(SLEEP/4);
    digitalWrite(RED_LED, LOW);
    delay(SLEEP/4); 
    digitalWrite(RED_LED, HIGH);
    delay(SLEEP/4);
    digitalWrite(RED_LED, LOW);  
    delay(SLEEP);
}
