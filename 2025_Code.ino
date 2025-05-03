// Arduino Pin Initialization
const int S0_PIN = 5;          // D5
const int S1_PIN = 4;          // D4
const int S2_PIN = 7;          // D7
const int S3_PIN = 6;          // D6
const int OUT_PIN1 = 8;        // D8 : sensor 1 output
const int OUT_PIN2 = 9;        // D9 : sensor 2 output
const int microSwitch = 10;    // D10 : microswitch
const int onoffRelay = 11;     // D11 : relay turns motor on or off
const int directionRelay = 12; // D12 : relay decides forwards or backwards



// Posssible States
enum RobotState{
  INITAL_SETUP
  HIT_WALL
  BACKWARDS
  FIND_TARGET1
  HIT_WALL_LOWERING_PLUNGER
  FIND_TARGET_2
}

RobotState currentState = INITIAL_SETUP


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Test Message");
}

void loop() {
  // put your main code here, to run repeatedly:

}
