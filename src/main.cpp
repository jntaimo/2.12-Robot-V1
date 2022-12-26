#include <Arduino.h>
#include <PS4Controller.h>

#define DIR1_L 13
#define PWM1_L 12
#define DIR2_L 27
#define PWM2_L 33
#define DIR1_R 15
#define PWM1_R 32
#define DIR2_R 14
#define PWM2_R 21
#define MAX_DRIVE 255
void printPS4();
void drive(int16_t backRight, int16_t backLeft, int16_t frontRight, int16_t frontLeft);
void setup() {
  Serial.begin(115200);
  PS4.begin("5c:f3:70:a6:b8:38");
  Serial.println("Ready.");
}

int16_t backRightDrive = 0;
int16_t backLeftDrive = 0;
int16_t frontRightDrive = 0;
int16_t frontLeftDrive = 0;

void loop() {
  // Below has all accessible outputs from the controller

}

//Function that uses PWM to drive the motors
//Values should be from -255 to 255
//Positive values always drive the robot forward
void drive(int16_t backRight, int16_t backLeft, int16_t frontRight, int16_t frontLeft){
  //constrain drive PWM to hardware limit
  backRight = constrain(backRight, -MAX_DRIVE, MAX_DRIVE);
  backLeft = constrain(backLeft, -MAX_DRIVE, MAX_DRIVE);
  frontRight = constrain(frontRight, -MAX_DRIVE, MAX_DRIVE);
  frontLeft = constrain(frontLeft, -MAX_DRIVE, MAX_DRIVE);

  //store wheel directions based on if input is negative
  bool dirBR = backRight < 0;
  bool dirBL = backLeft < 0;
  bool dirFR = frontRight > 0;
  bool dirFL = frontLeft > 0;

  //drive motor
  digitalWrite(DIR2_R,dirBR);
  digitalWrite(DIR2_L,dirBL);
  digitalWrite(DIR1_R,dirFR);
  digitalWrite(DIR1_L,dirFL);

  analogWrite(PWM2_R,backRight);
  analogWrite(PWM2_L,backLeft);
  analogWrite(PWM1_R,frontRight);
  analogWrite(PWM1_L,frontLeft);  
}
//Takes joystick values from -127 to 127 of two joystick channels
//Typical channel is the left Y channel Ch1 for and the right Ychannel for channel 2
void tankJoystickDrive(int8_t ch1, int8_t ch2, int16_t & backRight, int16_t & backLeft, int16_t & frontRight, int16_t frontLeft){
    //Double joystick to match joystick range to PWM Range
    ch1 *= 2;
    ch2 *= 2;
    //Map left drive to ch1 and right drive to ch 2
    backRight = ch2;
    backLeft = ch1;
    frontRight = ch2;
    frontLeft;
}

//Takes joystick values from -127 to 127 of a single joystick
//Outputs desired power for left and right drive train  (-255, 255) 
//changes the 4 drive power variables as inputs
//Based on formula here https://home.kendra.com/mauser/Joystick.html
void singleJoystickDrive(int8_t X, int8_t Y,int16_t & backRight, int16_t & backLeft, int16_t & frontRight, int16_t frontLeft){
  //Double joystick to match joystick range to PWM range
  X *= 2;
  Y *= 2;
  //flip X
  X = -X;
  //intermediate calculations
  int V =  (MAX_DRIVE-abs(X))*(Y/MAX_DRIVE) + Y;
  int W = (MAX_DRIVE-abs(Y))*(X/MAX_DRIVE) + X;
  //output values
  int rightDrive = (V+W)/2;
  int leftDrive = (V-W)/2;

  backRight = rightDrive;
  backLeft = leftDrive;
  frontRight = rightDrive;
  frontLeft = leftDrive;
}
void printPS4(){
  if (PS4.isConnected()) {
    if (PS4.Right()) Serial.println("Right Button");
    if (PS4.Down()) Serial.println("Down Button");
    if (PS4.Up()) Serial.println("Up Button");
    if (PS4.Left()) Serial.println("Left Button");

    if (PS4.Square()) Serial.println("Square Button");
    if (PS4.Cross()) Serial.println("Cross Button");
    if (PS4.Circle()) Serial.println("Circle Button");
    if (PS4.Triangle()) Serial.println("Triangle Button");

    if (PS4.UpRight()) Serial.println("Up Right");
    if (PS4.DownRight()) Serial.println("Down Right");
    if (PS4.UpLeft()) Serial.println("Up Left");
    if (PS4.DownLeft()) Serial.println("Down Left");

    if (PS4.L1()) Serial.println("L1 Button");
    if (PS4.R1()) Serial.println("R1 Button");

    if (PS4.Share()) Serial.println("Share Button");
    if (PS4.Options()) Serial.println("Options Button");
    if (PS4.L3()) Serial.println("L3 Button");
    if (PS4.R3()) Serial.println("R3 Button");

    if (PS4.PSButton()) Serial.println("PS Button");
    if (PS4.Touchpad()) Serial.println("Touch Pad Button");

    if (PS4.L2()) {
      Serial.printf("L2 button at %d\n", PS4.L2Value());
    }
    if (PS4.R2()) {
      Serial.printf("R2 button at %d\n", PS4.R2Value());
    }

    if (PS4.LStickX()) {
      Serial.printf("Left Stick x at %d\n", PS4.LStickX());
    }
    if (PS4.LStickY()) {
      Serial.printf("Left Stick y at %d\n", PS4.LStickY());
    }
    if (PS4.RStickX()) {
      Serial.printf("Right Stick x at %d\n", PS4.RStickX());
    }
    if (PS4.RStickY()) {
      Serial.printf("Right Stick y at %d\n", PS4.RStickY());
    }

    if (PS4.Charging()) Serial.println("The controller is charging");
    if (PS4.Audio()) Serial.println("The controller has headphones attached");
    if (PS4.Mic()) Serial.println("The controller has a mic attached");

    Serial.printf("Battery Level : %d\n", PS4.Battery());

    Serial.println();
    // This delay is to make the output more human readable
    // Remove it when you're not trying to see the output
    delay(1000);
  }
}

