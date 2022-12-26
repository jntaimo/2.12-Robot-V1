#include <Arduino.h>
#include <PS4Controller.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

#define DIR1_L 12
#define PWM1_L 27
#define DIR2_L 33
#define PWM2_L 15
#define DIR1_R A0
#define PWM1_R A1
#define DIR2_R A2
#define PWM2_R A3
#define MAX_DRIVE 255
#define DEADBANDPWM 10
int16_t backRightDrive = 0;
int16_t backLeftDrive = 0;
int16_t frontRightDrive = 0;
int16_t frontLeftDrive = 0;

void printPS4();
void drive(int16_t backRight = backRightDrive, int16_t backLeft = backLeftDrive, int16_t frontRight = frontRightDrive, int16_t frontLeft = frontLeftDrive);
void tankJoystickDrive(int16_t ch1, int16_t ch2, int16_t & backRight = backRightDrive, int16_t & backLeft = backLeftDrive, int16_t & frontRight = frontRightDrive, int16_t & frontLeft = frontLeftDrive);
void displaySetup();
void displayJoystick();
void clearSpace();

enum DriveMode {
  TANK_DRIVE,
  ONE_JOY_DRIVE
};
//default to tank drive
DriveMode driveMode = TANK_DRIVE;

void setup() {

  pinMode(DIR1_L, OUTPUT);
  pinMode(PWM1_L, OUTPUT);
  pinMode(DIR2_L, OUTPUT);
  pinMode(PWM2_L, OUTPUT);
  pinMode(DIR1_R, OUTPUT);
  pinMode(PWM1_R, OUTPUT);
  pinMode(DIR2_R, OUTPUT);
  pinMode(PWM2_R, OUTPUT);

  Serial.begin(115200);
  displaySetup();
  //address that PS4 controller is paired to
  PS4.begin("5c:f3:70:a6:b8:38");
  Serial.println("Ready.");
  while(!PS4.isConnected()){
    display.setCursor(0,0);
    display.getCursorX();
    display.println("Waiting to connect");
    display.display();
    delay(200);
    display.println("Waiting to connect.");
    display.display();
    delay(200);
    display.println("Waiting to connect..");
    display.display();
    delay(200);
    display.println("Waiting to connect...");
    display.display();
    delay(200);
    display.println("Waiting to connect..");
    display.display();
    delay(200);
    display.println("Waiting to connect..");
    display.display();
    delay(200);
    display.println("Waiting to connect.");
    display.display();
    delay(200);
    display.clearDisplay();
    display.display();
  }
}



unsigned long lastDriveTime = 0;//milliseconds
unsigned long driveDelay = 10;
unsigned long lastPrintTime = 0;
unsigned long lastPrintDelay = 100;

void loop() {
  
  // Below has all accessible outputs from the controller
  if (millis()-lastDriveTime > driveDelay){
    if(PS4.isConnected()){ 
      //use tank drive
      tankJoystickDrive(PS4.LStickY(), PS4.RStickY());
      PS4.setRumble(0,abs(frontLeftDrive + frontRightDrive)/2);
      PS4.setLed(abs(frontLeftDrive), 0, abs(frontRightDrive));
      PS4.sendToController();
      drive();
    } else {
      //turn off the motors if the controller disconnects
      drive(0, 0, 0, 0);
    }
    lastDriveTime = millis();
  }

  if(millis()-lastPrintTime > lastPrintDelay && PS4.isConnected()){
    displayJoystick();
    lastPrintTime = millis();
  }
}
//Configures the adafruit featherWing OLED display
void displaySetup(){
  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true); // Address 0x3C default
  display.display();
  delay(1000);
  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  delay(1000);
  display.clearDisplay();
  display.display();
}

void displayJoystick(){
  display.clearDisplay();
  display.setCursor(0,0);
  
  display.print("LJoy X: ");
  clearSpace();
  display.println(PS4.LStickX());
  
  display.print("LJoy Y: ");
  clearSpace();
  display.println(PS4.LStickY());
  
  display.print("RJoy X: ");
  clearSpace();
  display.println(PS4.RStickX());
  
  display.print("RJoy Y: ");
  clearSpace();
  display.println(PS4.RStickY());
  
  display.print("Left Drive: ");
  clearSpace();
  display.println(frontLeftDrive);

  display.print("Right Drive: ");
  clearSpace();
  display.println(frontRightDrive);

  display.display();
}

//clears 5 spaces on the display for text but returns cursor
//still need to run display() to send to screen
void clearSpace(){
  int x = display.getCursorX();
  int y = display.getCursorY();
  display.print("     ");
  display.setCursor(x, y);
}
//Function that uses PWM to drive the motors
//Values should be from -255 to 255
//Positive values always drive the robot forward
//defaults to the 4 drive variables as input
void drive(int16_t backRight, int16_t backLeft , int16_t frontRight, int16_t frontLeft){
  //constrain drive PWM to hardware limit
  backRight = constrain(backRight, -MAX_DRIVE, MAX_DRIVE);
  backLeft = constrain(backLeft, -MAX_DRIVE, MAX_DRIVE);
  frontRight = constrain(frontRight, -MAX_DRIVE, MAX_DRIVE);
  frontLeft = constrain(frontLeft, -MAX_DRIVE, MAX_DRIVE);

  //Deadband to ignore slight joystick drift
  if (abs(backRight < DEADBANDPWM)) backRight = 0;
  if (abs(backLeft < DEADBANDPWM)) backLeft = 0;
  if (abs(frontRight < DEADBANDPWM)) frontRight = 0;
  if (abs(frontLeft < DEADBANDPWM)) frontLeft = 0;

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

  analogWrite(PWM2_R,abs(backRight));
  analogWrite(PWM2_L,abs(backLeft));
  analogWrite(PWM1_R,abs(frontRight));
  analogWrite(PWM1_L,abs(frontLeft));  
}
//Takes joystick values from -127 to 127 of two joystick channels
//Typical channel is the left Y channel Ch1 for and the right Ychannel for channel 2
void tankJoystickDrive(int16_t ch1, int16_t ch2, int16_t & backRight, int16_t & backLeft, int16_t & frontRight, int16_t & frontLeft){
    //Double joystick to match joystick range to PWM Range
    ch1 = ch1*2 + 1;
    ch2 = ch2*2 + 1;
    //Map left drive to ch1 and right drive to ch 2
    backRight = ch2;
    backLeft = ch1;
    frontRight = ch2;
    frontLeft = ch1;
}

//Takes joystick values from -127 to 127 of a single joystick
//Outputs desired power for left and right drive train  (-255, 255) 
//changes the 4 drive power variables as inputs
//Based on formula here https://home.kendra.com/mauser/Joystick.html
void singleJoystickDrive(int16_t X, int16_t Y,int16_t & backRight, int16_t & backLeft, int16_t & frontRight, int16_t frontLeft){
  //Double joystick to match joystick range to PWM range
  X = X*2 + 1;
  Y = Y*2 + 1;
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

