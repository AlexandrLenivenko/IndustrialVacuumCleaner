int FIRST_ENGINE = 14;   //A0
int SECOND_ENGINE = 15;	 //A1
int THIRD_ENGINE = 16;	 //A2

int PERIOD = 5000;

void setup() {
  pinMode(FIRST_ENGINE, OUTPUT);
  pinMode(SECOND_ENGINE, OUTPUT);
  pinMode(THIRD_ENGINE, OUTPUT);
}

void loop() {
  digitalWrite(FIRST_ENGINE, LOW); 		//2 3
  digitalWrite(SECOND_ENGINE, HIGH); 
  digitalWrite(THIRD_ENGINE, HIGH); 
  delay(PERIOD);
  
  digitalWrite(SECOND_ENGINE, LOW); 	// 1 3
  digitalWrite(FIRST_ENGINE, HIGH); 
  delay(PERIOD);

  digitalWrite(THIRD_ENGINE, LOW); 		// 1 2
  digitalWrite(SECOND_ENGINE, HIGH); 
  delay(PERIOD);
}
