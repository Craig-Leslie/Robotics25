// Arduino Pin Initialization
const int DMDir = 2;              // D2: For robot’s main movement; all 4 motors share this (parallel)
const int DMStep = 3;             // D3: Same motors as above; shared step signal
const int leadScrewMot4 = 4;      // D4:
const int leadScrewMot3 = 5;      // D5:
const int leadScrewMot2 = 6;      // D6:
const int leadScrewMot1 = 7;      // D7:
const int IRSens = 8;             // D8: For tracking movement of the rotary wheel
const int screwTopSwitch = 9;     // D9: For leadscrew homing calibration
const int wallBut = 10;           // D10: Flat surface should hit square-on to prevent veering
const int onLight = 11;           // D11: Turns on when robot powers up and starts moving		
const int wallLight = 12;         // D12: Activates when robot hits the wall, deactivates when it leaves
const int S0_PIN = 16;            // D13:
const int S1_PIN = 19;            // A0:
const int outputEnable = 20;      // A1:
const int S2_PIN = 21;            // A2:
const int S3_PIN = 22;            // A3:
const int OUT_PIN2 = 23;          // A4:
const int OUT_PIN1 = 24;          // A5: Prevents forklift from over-traveling downward
const int screwBotSwitch = 25;    // A6: 
// A7: Spare digital pin if needed

// Imports
#include <Stepper.h>
const int stepsPerRevolution = 2038;
Stepper myStepper = Stepper(stepsPerRevolution, 4, 5, 6, 7);

// Posssible States
enum RobotState{
  INITAL_SETUP
  HOMING
  HIT_WALL
  BACKWARDS
  SEARCH_TARGET2
  SKIP_TARGET2
  FIND_TARGET1
  JUMP_TO_TARGET1
  WAIT_TARGET1
  LOWER_PLUNGER
  FIND_TARGET2
  JUMP_TO_TARGET2
  STOPPED
}

RobotState currentState = INITIAL_SETUP


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Setup Starting...");

  // set the S0, S1, S2, S3 Pins as Output
  pinMode(S0_PIN, OUTPUT);
  pinMode(S1_PIN, OUTPUT);
  pinMode(S2_PIN, OUTPUT);
  pinMode(S3_PIN, OUTPUT);

  // set OUT_PINs as Input
  pinMode(OUT_PIN1, INPUT);
  pinMode(OUT_PIN2, INPUT);

  // set pulse width scaling to 20%
  digitalWrite(S0_PIN, HIGH);
  digitalWrite(S1_PIN, LOW);



}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print("Case: ")
  Serial.print(currentState)

  switch (currentState){
    case HOMING:
      raiseScrew();
      /*
      if(digitalRead(screwTopSwitch) == LOW){
        stopScrewMotor();
        currentState = HIT_WALL
        Serial.println("Screw Raised.");
      }
      */
      break;
      
  }
}

void raiseScrew() {
  myStepper.setSpeed(5);
	myStepper.step(stepsPerRevolution);
}

void 
