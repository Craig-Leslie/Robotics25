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
const int S0_PIN = 13;            // D13:
const int S1_PIN = A0;            // A0:
const int outputEnable = A1;      // A1:
const int S2_PIN = A2;            // A2:
const int S3_PIN = A3;            // A3:
const int OUT_PIN2 = A4;          // A4:
const int OUT_PIN1 = A5;          // A5: 
const int screwBotSwitch = A6;    // A6: Prevents forklift from over-traveling downward
// A7: Spare digital pin if needed

// Imports
include <Stepper.h>
const int stepsPerRevolution = 1600;
Stepper myStepper(stepsPerRevolution, leadScrewMot1, leadScrewMot2, leadScrewMot3, leadScrewMot4);

// Posssible States
enum RobotState{
  START,            // Setup phase, raises screw to correct height, turns on LEDs, starts motors
  FIND_WALL,        // Moves robot forward until the wall button is triggered
  WALL_CONTACT,     // Stops motors, turns off green LED, turn on red LED and buzzer, stays stationary for 8 seconds
  BACKWARDS,        //
  SKIP_TARGET2,
  FIND_TARGET1,
  JUMP_TO_TARGET1,
  WAIT_TARGET1,
  LOWER_PLUNGER,
  FIND_TARGET2,
  JUMP_TO_TARGET2,
  FIND_TARGET,       // Moves in reverse, uses function to detect targets, checks if target is the current target, if so stops motors, otherwise keep moving and searching#
  PITSTOP,           // Waits on the start target and adjusts screw height before moving off
  STOPPED,           // Stops completely on second target
  COLOUR_TEST,       // Testing state to test colour recognition
};

RobotState currentState = FIND_WALL;

// These values represent the first and second targets. In essence the value is the number of targets the robot should skip while searching for targets
int startTarget = 1;
int secondTarget = 0;

// curTarget is the target number the robot should be searching for
int curTarget = startTarget;

// targetsFound is the number of targets the robot has found
int targetsFound = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Setup Starting...");

  // Set LED and motor pins as OUTPUT
  pinMode(onLight, OUTPUT);
  pinMode(wallLight, OUTPUT);
  pinMode(DMDir, OUTPUT);
  pinMode(DMStep, OUTPUT);

  // Set color sensor control pins
  pinMode(S0_PIN, OUTPUT);
  pinMode(S1_PIN, OUTPUT);
  pinMode(S2_PIN, OUTPUT);
  pinMode(S3_PIN, OUTPUT);

  // Set color sensor read pins
  pinMode(OUT_PIN1, INPUT);
  pinMode(OUT_PIN2, INPUT);

  // Turn on status LED
  digitalWrite(onLight, HIGH);

  // Set wall button pins
  pinMode(wallBut, INPUT_PULLUP);
  digitalWrite(wallBut, HIGH);

  // Set pulse width scaling
  digitalWrite(S0_PIN, HIGH);
  digitalWrite(S1_PIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  String colour1 = detectcolour1();
  String colour2 = detectcolour2();

  Serial.print("Case: ");
  Serial.print(currentState);

  switch (currentState){
    case START:
      // Start stepper motor


      // For homing we have to move the screw up to the top switch
      // Should probably hardwire in screw raising duration instead of relying on switch.
      raiseScrew();

      // Turn on green LED
      digitalWrite(onLight, HIGH);
      currentState = FIND_WALL;
      break;
      
    case FIND_WALL:
      moveForwards();
      if (digitalRead(wallBut) == HIGH){
        stopMotor();
        currentState = WALL_CONTACT;
      }
      break; 

    case WALL_CONTACT:
      // Turn off green LED
      digitalWrite(onLight, LOW);

      // Turn on red LED
      digitalWrite(wallLight, HIGH);
      
      delay(8000);
      currentState = FIND_TARGET;

      // Turn off red LED
      digitalWrite(wallLight, LOW);
      break;

/*
      case BACKWARDS:
        
      break;
*/
      case FIND_TARGET:

      RobotState nextState;

      // Move in reverse for x seconds, then stop motor, for slower speed
      moveBackwardsIncrements(100,50);
      // Use the current target to create a target number
       int targetNum = curTarget;

      // Detect if a target is passed over
      if (colour1 == "black" || colour2 == "black"){
        // Determine if this is the correct target based on curTarget and targets found
        Serial.print("Target Detected.");
        if(curTarget == targetsFound){
          // Use the curTarget to determine the next state to use
          if (curTarget == startTarget){
            nextState = WALL_CONTACT;
          }
          else{
            nextState = STOPPED;
          }

          // If so, stop, nudge
          stopMotor();
          moveBackwardsIncrements(50,0);

          // Reset targetsFound
          targetsFound = 0;

          // Update the new curTarget
          curTarget = secondTarget;
          currentState = nextState;
        }

        else{
          targetsFound++;
        }
      }
      break;

      case PITSTOP:
        // Move screw down 15mm (needs testing to get number)
        lowerScrew();
        break; 

      case COLOUR_TEST:
        Serial.print("Colour 1: ");
        Serial.print(colour1);

        Serial.print("Colour 2: ");
        Serial.print(colour2);
        break;
    
  }

}

void raiseScrew() {
	myStepper.step(stepsPerRevolution);
  return;
}

void lowerScrew() {
	myStepper.step(-stepsPerRevolution);
  return;
}

void moveForwards(){
  for (int i = 0; i < stepsPerRevolution; i++) {
    digitalWrite(DMStep, HIGH);
    delayMicroseconds(2000);
    digitalWrite(DMStep, LOW);
    delayMicroseconds(2000);
  }

}

void moveForwardsIncrements(int movementTime, int delayTime){
  moveForwards();
  delay(movementTime);
  stopMotor();
  delay(delayTime);
  return;
}

void moveBackwards(){
  for (int i = 0; i < stepsPerRevolution; i++) {
    digitalWrite(DMStep, LOW);
    delayMicroseconds(2000);
    digitalWrite(DMStep, LOW);
    delayMicroseconds(2000);
  }
}

void moveBackwardsIncrements(int movementTime, int delayTime){
  moveBackwards();
  delay(movementTime);
  stopMotor();
  delay(delayTime);
  return;
}


void stopMotor(){
  digitalWrite(DMStep, HIGH);
  return;
}

// Code below is colour detection taken from last year
int process_red_value1() {
  digitalWrite(S2_PIN, LOW);
  digitalWrite(S3_PIN, LOW);
  int pulse_length = pulseIn(OUT_PIN1, LOW);
  return pulse_length;
}
 
int process_green_value1() {
  digitalWrite(S2_PIN, HIGH);
  digitalWrite(S3_PIN, HIGH);
  int pulse_length = pulseIn(OUT_PIN1, LOW);
  return pulse_length;
}
 
int process_blue_value1() {
  digitalWrite(S2_PIN, LOW);
  digitalWrite(S3_PIN, HIGH);
  int pulse_length = pulseIn(OUT_PIN1, LOW);
  return pulse_length;
}
 
int process_red_value2() {
  digitalWrite(S2_PIN, LOW);
  digitalWrite(S3_PIN, LOW);
  int pulse_length = pulseIn(OUT_PIN2, LOW);
  return pulse_length;
}
 
int process_green_value2() {
  digitalWrite(S2_PIN, HIGH);
  digitalWrite(S3_PIN, HIGH);
  int pulse_length = pulseIn(OUT_PIN2, LOW);
  return pulse_length;
}
 
int process_blue_value2() {
  digitalWrite(S2_PIN, LOW);
  digitalWrite(S3_PIN, HIGH);
  int pulse_length = pulseIn(OUT_PIN2, LOW);
  return pulse_length;
}
String detectcolour1() {
  int r1 = process_red_value1();
  int g1 = process_green_value1();
  int b1 = process_blue_value1();
  int colourVal = 0;
  String colour = findcolour(colourVal);
  return colour;
}
 
String detectcolour2() {
  int r2 = process_red_value2();
  int g2 = process_green_value2();
  int b2 = process_blue_value2();
  int colourVal = 0;
  colourVal = setcolour(r2, g2, b2);
  String colour = findcolour(colourVal);
  return colour;
}
 
int setcolour(int r, int g, int b){
  int colour = 0;
  if(b<50){
    colour += 1;
  }
  if(g<50){
    colour += 2;
  }  
  if(r<50){
    colour += 4;
  }
  if (r>60 && g>60 && b>60){
    colour = 8;
  }
  if (r<25 && g<25 && b<25){
    colour = 9;
  }
  return colour;
}

String findcolour(int c){
  if(c == 1){
    return "blue";
  } else if(c == 2){
    return "green";
  } else if(c == 4){
    return "red";
  } else if(c == 7 || c == 5){
    return "board";
  } else if(c == 9){
    return "white";
  } else if(c == 8){
    return "black";
  }
  return "unknown";
}