/*
This is a demo program for the 3pi+ 2040 OLED Robot.

It uses the buttons, display, and buzzer to provide a user
interface.  It presents a menu to the user that lets the user
select from several different demos.

To use this demo program, you will need to have the OLED
display connected to the 3pi+.
*/

#include <Wire.h>
#include <Pololu3piPlus2040.h>
#include <PololuMenu.h>

OLED display;
Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
LineSensors lineSensors;
BumpSensors bumpSensors;
IMU imu;
Motors motors;
Encoders encoders;
RGBLEDs leds;

PololuMenu mainMenu;

// declarations for splash screen
#include "splash.h"

bool launchSelfTest = false;

// A couple of simple tunes, stored in program space.
const char beepBrownout[] = "<c8";
const char beepWelcome[] = ">g32>>c32";
const char beepThankYou[] = ">>c32>g32";
const char beepFail[] = "<g-8r8<g-8r8<g-8";
const char beepPass[] = ">l32c>e>g>>c8";
const char beepReadySetGo[] = ">c16r2>c16r2>>c4";

// Custom characters for the LCD:

// This character is a back arrow.
const char backArrow[] = {
  0b00000,
  0b00010,
  0b00001,
  0b00101,
  0b01001,
  0b11110,
  0b01000,
  0b00100,
};

// This character is two chevrons pointing up.
const char forwardArrows[] = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
};

// This character is two chevrons pointing down.
const char reverseArrows[] = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b10001,
  0b01010,
  0b00100,
  0b00000,
};

// This character is two solid arrows pointing up.
const char forwardArrowsSolid[] = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b01110,
  0b11111,
  0b00000,
};

// This character is two solid arrows pointing down.
const char reverseArrowsSolid[] = {
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
};

void loadCustomCharacters()
{
  // The LCD supports up to 8 custom characters.  Each character
  // has a number between 0 and 7.  We assign #7 to be the back
  // arrow; other characters are loaded by individual demos as
  // needed.

  display.loadCustomCharacter(backArrow, 7);
}

// Assigns #0-6 to be bar graph characters.
void loadCustomCharactersBarGraph()
{
  static const char levels[] = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

// Assigns #0-4 to be arrow symbols.
void loadCustomCharactersMotorDirs()
{
  display.loadCustomCharacter(forwardArrows, 0);
  display.loadCustomCharacter(reverseArrows, 1);
  display.loadCustomCharacter(forwardArrowsSolid, 2);
  display.loadCustomCharacter(reverseArrowsSolid, 3);
}

// Clears the LCD and puts [back_arrow]B on the second line
// to indicate to the user that the B button goes back.
void displayBackArrow()
{
  display.clear();
  display.gotoXY(0,1);
  display.print(("\7B"));
  display.gotoXY(0,0);
}

void displaySplash(uint8_t *graphics, uint8_t offset = 0)
{
  memset(graphics, 0, sizeof(pololu3PiPlusSplash));
  for(uint16_t i = 0; i < sizeof(pololu3PiPlusSplash) - offset*128; i++)
  {
    graphics[i] = pololu3PiPlusSplash[(i%128)*8 + i/128 + offset];
  }
  display.display();
}

void showSplash()
{
  // We only need the graphics array within showSplash(); it's not
  // used elsewhere in the demo program, so we can make it a local
  // variable.
  uint8_t graphics[1024];

  display.setLayout21x8WithGraphics(graphics);
  displaySplash(graphics, 0);

  // keep LEDs on for 1s
  ledYellow(1);
  leds.setBrightness(4);
  uint16_t blinkStart = millis();
  while((uint16_t)(millis() - blinkStart) < 900)
  {
  }

  // scroll quickly up
  for(uint8_t offset = 1; offset < 5; offset ++)
  {
    delay(100);
    displaySplash(graphics, offset);
  }

  display.clear();
  display.gotoXY(0, 5);
  display.print("Push B to start demo!");
  display.gotoXY(0, 6);
  display.print("For more info, visit");
  display.gotoXY(0, 7);
  display.print(" www.pololu.com/3pi+");

  // Turn LEDs off for 1s
  ledYellow(0);
  leds.setBrightness(0);
  while((uint16_t)(millis() - blinkStart) < 2000)
  {
  }

  // Keep blinking the blue LED under the B button while waiting for the
  // user to press button B.
  blinkStart = millis();
  while (mainMenu.buttonMonitor() != 'B')
  {
    uint16_t blinkPhase = millis() - blinkStart;
    leds.setBrightness(BACK_CENTER_LED, blinkPhase < 1000 ? 4 : 0);
    if (blinkPhase >= 2000) { blinkStart += 2000; }
  }
  leds.setBrightness(BACK_CENTER_LED, 0);

  display.setLayout11x4WithGraphics(graphics);
  display.clear();
  display.gotoXY(0,3);
  display.noAutoDisplay();
  display.print("Thank you!!");
  display.display();

  buzzer.play(beepThankYou);
  delay(1000);
  display.clear();
  display.setLayout8x2();
}

// Blinks all of LEDs in sequence.
void ledDemo()
{
  static const char* noteArray[7] = {"a32", "b32", "c32", "d32", "e32", "f32", "g32"};
  static const char* nameArray[7] = {"Violet", "Blue  ", "Green ", "Yellow", "Orange", "Red   ", "yellow"};

  displayBackArrow();

  uint8_t state = 0;
  static uint16_t lastUpdateTime = millis() - 2000;
  while (mainMenu.buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastUpdateTime) >= 500)
    {
      lastUpdateTime = millis();
      state = state + 1;
      if (state >= 7) { state = 0; }

      leds.setBrightness(0);
      ledYellow(false);
      if (state < 6)
      {
        leds.setBrightness(state, 4);
      }
      else
      {
        ledYellow(true);
      }

      buzzer.play(noteArray[state]);
      display.gotoXY(0, 0);
      display.print(nameArray[state]);
    }
  }

  leds.setBrightness(0);
  ledYellow(false);
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  static const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};
  display.print(barChars[height]);
}

void selfTestWaitShowingVBat()
{
  ledYellow(0);
  leds.setBrightness(0);
  while(!mainMenu.buttonMonitor())
  {
    display.gotoXY(0,0);
    display.print(' ');
    display.print(readBatteryMillivolts());
    display.print(" mV");
    delay(100);
  }
}

void selfTestFail()
{
  display.gotoXY(0, 1);
  display.print("FAIL");
  buzzer.play(beepFail);
  while(!mainMenu.buttonMonitor());
}

void selfTest()
{
  display.clear();
  display.print("3\xf7+ 2040");
  display.gotoXY(0, 1);
  display.print("SelfTest");
  delay(1000);

  bumpSensors.calibrate();

  display.clear();
  display.print("Press");
  display.gotoXY(0, 1);
  display.print("bumpers");
  do
  {
     bumpSensors.read();
  }
  while(!bumpSensors.leftIsPressed() || !bumpSensors.rightIsPressed());

  buzzer.play("!c32");
  display.gotoXY(0, 1);
  display.print("        ");

  // test some voltages and IMU presence
  display.gotoXY(0, 0);
  display.print("VBAT     ");
  int v = readBatteryMillivolts();
  display.gotoXY(4, 0);
  display.print(v);
  if(v < 4000 || v > 7000)
  {
    selfTestFail();
    return;
  }
  leds.setBrightness(BACK_LEFT_LED, 4);
  delay(500);

  display.gotoXY(0, 0);
  display.print("IMU     ");
  display.gotoXY(4, 0);
  if(!imu.init())
  {
    selfTestFail();
    return;
  }
  display.print("OK");
  leds.setBrightness(BACK_CENTER_LED, 4);
  delay(500);

  // test motor speed, direction, and encoders
  display.gotoXY(0, 0);
  display.print("Motors  ");
  leds.setBrightness(BACK_RIGHT_LED, 4);
  imu.configureForTurnSensing();

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  motors.setSpeeds(90, -90);
  delay(250);

  // check rotation speed
  imu.read();
  int16_t gyroReading = imu.g.z;

  motors.setSpeeds(0, 0);
  delay(100);
  int left = encoders.getCountsAndResetLeft();
  int right = encoders.getCountsAndResetRight();
  display.clear();
  if(gyroReading > -7000 && gyroReading < -5000 &&
    left > 212 && left < 288 && right > -288 && right < -212)
  {
    display.print("Standrd?");
  }
  else if(gyroReading > -1800 && gyroReading < -1200 &&
    left > 140 && left < 200 && right > -200 && right < -140)
  {
    display.print("Turtle?");
  }
  else if(gyroReading > 9500 && gyroReading < 17000 &&
    left > 130 && left < 370 && right > -370 && right < -130)
  {
    display.print("Hyper?");
  }
  else
  {
    display.clear();
    display.print(left);
    display.gotoXY(4, 0);
    display.print(right);

    display.gotoXY(4, 1);
    display.print(gyroReading/100);
    selfTestFail();
    return;
  }

  display.gotoXY(0,1);
  display.print("A=?  B=Y");
  while(true)
  {
    char button = mainMenu.buttonMonitor();
    if(button == 'A')
    {
      display.clear();
      display.print(left);
      display.gotoXY(4, 0);
      display.print(right);

      display.gotoXY(0, 1);
      display.print(gyroReading);
    }
    if(button == 'B')
    {
      break;
    }
    if(button == 'C')
    {
      selfTestFail();
      return;
    }
  }

  // Passed all tests!
  display.gotoXY(0, 1);
  display.print("PASS    ");
  delay(250); // finish the button beep
  buzzer.play(beepPass);
  selfTestWaitShowingVBat();
}

// Display line sensor readings. Holding button C turns off
// the IR emitters.
void lineSensorDemo()
{
  loadCustomCharactersBarGraph();
  displayBackArrow();
  display.gotoXY(6, 1);
  display.print('C');

  LineSensorReadings lineSensorValues;

  while (mainMenu.buttonMonitor() != 'B')
  {
    bool emittersOff = buttonC.isPressed();

    lineSensors.read(lineSensorValues, emittersOff ? LineSensorsReadMode::Off :  LineSensorsReadMode::On);

    display.gotoXY(1, 0);
    for (uint8_t i = 0; i < 5; i++)
    {
      uint8_t barHeight = map(lineSensorValues.vals[i], 0, 1024, 0, 8);
      printBar(barHeight);
    }

    // Display an indicator of whether emitters are on or
    // off.
    display.gotoXY(7, 1);
    if (emittersOff)
    {
      display.print('\xa5');  // centered dot
    }
    else
    {
      display.print('*');
    }
  }
}

void bumpSensorDemo()
{
  bumpSensors.calibrate();
  displayBackArrow();

  while (mainMenu.buttonMonitor() != 'B')
  {
    bumpSensors.read();

    if (bumpSensors.leftChanged())
    {
      leds.setBrightness(FRONT_LEFT_LED, bumpSensors.leftIsPressed() ? 4 : 0);
      if (bumpSensors.leftIsPressed())
      {
        // Left bump sensor was just pressed.
        buzzer.play("a32");
        display.gotoXY(0, 0);
        display.print('L');
      }
      else
      {
        // Left bump sensor was just released.
        buzzer.play("b32");
        display.gotoXY(0, 0);
        display.print(' ');
      }
    }

    if (bumpSensors.rightChanged())
    {
      leds.setBrightness(FRONT_RIGHT_LED, bumpSensors.rightIsPressed() ? 4 : 0);
      if (bumpSensors.rightIsPressed())
      {
        // Right bump sensor was just pressed.
        buzzer.play("e32");
        display.gotoXY(7, 0);
        display.print('R');
      }
      else
      {
        // Right bump sensor was just released.
        buzzer.play("f32");
        display.gotoXY(7, 0);
        display.print(' ');
      }
    }
  }
}

// Starts I2C and initializes the inertial sensors.
void initInertialSensors()
{
  Wire.begin();
  imu.init();
  imu.enableDefault();
}

// Given 3 readings for axes x, y, and z, prints the sign
// and axis of the largest reading unless it is below the
// given threshold.
void printLargestAxis(int16_t x, int16_t y, int16_t z, uint16_t threshold)
{
  int16_t largest = x;
  char axis = 'X';

  if (abs(y) > abs(largest))
  {
    largest = y;
    axis = 'Y';
  }
  if (abs(z) > abs(largest))
  {
    largest = z;
    axis = 'Z';
  }

  if (abs(largest) < threshold)
  {
    display.print("  ");
  }
  else
  {
    bool positive = (largest > 0);
    display.print(positive ? '+' : '-');
    display.print(axis);
  }
}

// Print the direction of the largest rotation rate measured
// by the gyro and the up direction based on the
// accelerometer's measurement of gravitational acceleration
// (assuming gravity is the dominant force acting on the 3pi+).
void inertialDemo()
{
  displayBackArrow();

  display.gotoXY(3, 0);
  display.print("Rot");
  display.gotoXY(4, 1);
  display.print("Up");

  while (mainMenu.buttonMonitor() != 'B')
  {
    imu.read();

    display.gotoXY(6, 0);
    printLargestAxis(imu.g.x, imu.g.y, imu.g.z, 2000);
    display.gotoXY(6, 1);
    printLargestAxis(imu.a.x, imu.a.y, imu.a.z, 200);
  }
}

// Demonstrate the built-in magnetometer with a simple compass
// application.  This demo constantly calibrates by keeping
// track of min and max values in all axes, and it displays an
// estimated heading ("S", "NE", etc.) by comparing the current
// values to these values.
void compassDemo()
{
  IMU::vector<int16_t> magMax;
  IMU::vector<int16_t> magMin;
  IMU::vector<int16_t> m;
  bool showReadings = false;
  bool firstReading = true;

  while (true)
  {
    display.noAutoDisplay();
    displayBackArrow();
    display.gotoXY(7,1);
    display.print('C');
    imu.readMag();

    if (firstReading)
    {
      magMax = imu.m;
      magMin = imu.m;
      firstReading = false;
    }

    // Update min/max calibration values.
    magMax.x = max(magMax.x, imu.m.x);
    magMax.y = max(magMax.y, imu.m.y);
    magMax.z = max(magMax.z, imu.m.z);
    magMin.x = min(magMin.x, imu.m.x);
    magMin.y = min(magMin.y, imu.m.y);
    magMin.z = min(magMin.z, imu.m.z);

    // Subtract the average of min and max to get a vector
    // centered at an approximation of true zero.
    m.x = imu.m.x - (magMax.x+magMin.x)/2;
    m.y = imu.m.y - (magMax.y+magMin.y)/2;
    m.z = imu.m.z - (magMax.z+magMin.z)/2;

    if (showReadings)
    {
      display.gotoXY(0, 0);
      display.print(m.x/100);
      display.gotoXY(4, 0);
      display.print(m.y/100);

      display.gotoXY(3, 1);
      display.print(m.z/100);
    }
    else if (magMax.x - 1000 < magMin.x || magMax.y - 1000 < magMin.y)
    {
      display.gotoXY(0, 0);
      display.print("Turn me!");
    }
    else {
      // Estimate the direction by checking which vector is the closest
      // match.
      int n = m.x;
      int s = -n;
      int e = m.y;
      int w = -e;
      int ne = (n+e)*10/14;
      int nw = (n+w)*10/14;
      int se = (s+e)*10/14;
      int sw = (s+w)*10/14;

      const char* dir = "??";
      int max_value=0;
      if (n > max_value) { max_value = n; dir = "N"; }
      if (s > max_value) { max_value = s; dir = "S"; }
      if (e > max_value) { max_value = e; dir = "E"; }
      if (w > max_value) { max_value = w; dir = "W"; }
      if (ne > max_value) { max_value = ne; dir = "NE"; }
      if (se > max_value) { max_value = se; dir = "SE"; }
      if (nw > max_value) { max_value = nw; dir = "NW"; }
      if (sw > max_value) { max_value = sw; dir = "SW"; }

      display.gotoXY(3,0);
      display.print(dir);
    }
    display.display();

    switch (mainMenu.buttonMonitor())
    {
    case 'B':
      return;
    case 'C':
      showReadings = !showReadings;
      break;
    }

    delay(50);
  }

}

// Provides an interface to test the motors. Holding button A or C
// causes the left or right motor to accelerate; releasing the
// button causes the motor to decelerate. Tapping the button while
// the motor is not running reverses the direction it runs.
//
// If the showEncoders argument is true, encoder counts are
// displayed on the first line of the LCD; otherwise, an
// instructional message is shown.
void motorDemoHelper(bool showEncoders)
{
  loadCustomCharactersMotorDirs();
  display.clear();
  display.gotoXY(1, 1);
  display.print("A \7B C");

  int16_t leftSpeed = 0, rightSpeed = 0;
  int8_t leftDir = 1, rightDir = 1;
  uint16_t lastUpdateTime = millis() - 100;
  uint8_t btnCountA = 0, btnCountC = 0, instructCount = 0;

  int16_t encCountsLeft = 0, encCountsRight = 0;
  char buf[7];

  while (mainMenu.buttonMonitor() != 'B')
  {
    encCountsLeft += encoders.getCountsAndResetLeft();
    if (encCountsLeft < 0) { encCountsLeft += 1000; }
    if (encCountsLeft > 999) { encCountsLeft -= 1000; }

    encCountsRight += encoders.getCountsAndResetRight();
    if (encCountsRight < 0) { encCountsRight += 1000; }
    if (encCountsRight > 999) { encCountsRight -= 1000; }

    // Update the LCD and motors every 50 ms.
    if ((uint16_t)(millis() - lastUpdateTime) > 50)
    {
      lastUpdateTime = millis();

      display.gotoXY(0, 0);
      if (showEncoders)
      {
        sprintf(buf, "%03d", encCountsLeft);
        display.print(buf);
        display.gotoXY(5, 0);
        sprintf(buf, "%03d", encCountsRight);
        display.print(buf);
      }
      else
      {
        // Cycle the instructions every 2 seconds.
        if (instructCount == 0)
        {
          display.print("Hold=run");
        }
        else if (instructCount == 40)
        {
          display.print("Tap=flip");
        }
        if (++instructCount == 80) { instructCount = 0; }
      }

      if (buttonA.isPressed())
      {
        if (btnCountA < 4)
        {
          btnCountA++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          leftSpeed += 15;
        }
      }
      else
      {
        if (leftSpeed == 0 && btnCountA > 0 && btnCountA < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          leftDir = -leftDir;
        }
        btnCountA = 0;
        leftSpeed -= 30;
      }

      if (buttonC.isPressed())
      {
        if (btnCountC < 4)
        {
          btnCountC++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          rightSpeed += 15;
        }
      }
      else
      {
        if (rightSpeed == 0 && btnCountC > 0 && btnCountC < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          rightDir = -rightDir;
        }
        btnCountC = 0;
        rightSpeed -= 30;
      }

      leftSpeed = constrain(leftSpeed, 0, 400);
      rightSpeed = constrain(rightSpeed, 0, 400);

      motors.setSpeeds(leftSpeed * leftDir, rightSpeed * rightDir);

      // Display arrows pointing the appropriate direction
      // (solid if the motor is running, chevrons if not).
      display.gotoXY(0, 1);
      if (leftSpeed == 0)
      {
        display.print((leftDir > 0) ? '\0' : '\1');
      }
      else
      {
        display.print((leftDir > 0) ? '\2' : '\3');
      }
      display.gotoXY(7, 1);
      if (rightSpeed == 0)
      {
        display.print((rightDir > 0) ? '\0' : '\1');
      }
      else
      {
        display.print((rightDir > 0) ? '\2' : '\3');
      }
    }
  }
  motors.setSpeeds(0, 0);
}


// Motor demo with instructions.
void motorDemo()
{
  motorDemoHelper(false);
}

// Motor demo with encoder counts.
void encoderDemo()
{
  motorDemoHelper(true);
}

// Spin in place
void spinDemo()
{
  display.setLayout21x8();
  display.clear();
  display.print("Warning: this will"); display.gotoXY(0,1);
  display.print("run both motors up to"); display.gotoXY(0,2);
  display.print("full speed for a few"); display.gotoXY(0,3);
  display.print("seconds."); display.gotoXY(0,5);

  display.print("Press A to continue."); display.gotoXY(0,7);
  display.print("\7 Press B to cancel.");

  bool waiting = true;
  while(waiting)
  {
    char button = mainMenu.buttonMonitor();
    switch(button)
    {
    case 'B':
      display.setLayout8x2();
      return;
    case 'A':
      waiting = false;
    }
  }

  display.setLayout8x2();
  displayBackArrow();
  display.print("Spinning");
  display.gotoXY(5,1);
  display.print("...");
  delay(200);
  buzzer.play(beepReadySetGo);
  while(buzzer.isPlaying())
  {
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  leds.setBrightness(4);
  spinDemoInternal();
  motors.setSpeeds(0, 0);
  leds.setBrightness(0);
}

void spinDemoInternal()
{
  // Spin right
  for(int i = 0; i < 40; i++)
  {
    motors.setSpeeds(i * 10, -i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  for(int i = 40; i >= 0; i--)
  {
    motors.setSpeeds(i * 10, -i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }

  // Spin left
  for(int i = 0; i < 40; i++)
  {
    motors.setSpeeds(-i * 10, i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  for(int i = 40; i >= 0; i--)
  {
    motors.setSpeeds(-i * 10, i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
}

const char fugue[] =
  "! T120O5L16agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8MS<b-d<b-d MLe-<ge-<g MSc<ac<a MLd<fd<f O5MSb-gb-g"
  "ML>c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe fO6dc#dfdc#<b c#4";

const char fugueTitle[] =
  "       Fugue in D Minor - by J.S. Bach       ";

// Play a song on the buzzer and display its title.
void musicDemo()
{
  displayBackArrow();

  size_t fugueTitlePos = 0;
  uint16_t lastShiftTime = millis() - 2000;

  while (mainMenu.buttonMonitor() != 'B')
  {
    // Shift the song title to the left every 250 ms.
    if ((uint16_t)(millis() - lastShiftTime) > 250)
    {
      lastShiftTime = millis();

      display.gotoXY(0, 0);
      for (uint8_t i = 0; i < 8; i++)
      {
        char c = fugueTitle[fugueTitlePos + i];
        display.print(c);
      }
      fugueTitlePos++;

      if (fugueTitlePos + 8 >= strlen(fugueTitle))
      {
        fugueTitlePos = 0;
      }
    }

    if (!buzzer.isPlaying())
    {
      buzzer.play(fugue);
    }
  }
}

// Display the the battery (VIN) voltage.
void powerDemo()
{
  displayBackArrow();

  uint16_t lastDisplayTime = millis() - 2000;
  char buf[6];

  while (mainMenu.buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastDisplayTime) > 250)
    {
      uint16_t batteryLevel = readBatteryMillivolts();

      lastDisplayTime = millis();
      display.gotoXY(0, 0);
      sprintf(buf, "%5d", batteryLevel);
      display.print(buf);
      display.print(F(" mV"));
    }
  }
}

// This demo shows all characters that the OLED can display, 128 at a
// time.  Press any button to advance to the the next page of 8
// characters.  Note that the first eight are the custom characters.
// Most of these are initially blank, but if you run other demos that
// set custom characters then return here, you will see what they
// loaded.
void displayDemo() {
  display.setLayout21x8();
  display.noAutoDisplay();

  for(int y=0; y<8; y++)
  {
    display.gotoXY(0,y);
    display.print(y, HEX);
    display.print(": ");
    for(int x=0; x<16; x++)
    {
      display.print((char)(y*16+x));
    }
    display.print(" :");
  }
  display.display();

  // wait for a button press
  while(mainMenu.buttonMonitor() == 0);

  display.clear();
  display.noAutoDisplay();

  for(int y=0; y<8; y++)
  {
    display.gotoXY(0,y);
    display.print(y+8, HEX);
    display.print(": ");
    for(int x=0; x<16; x++)
    {
      display.print((char)(128+y*16+x));
    }
    display.print(" :");
  }
  display.display();

  // wait for a button press
  while(mainMenu.buttonMonitor() == 0);

  display.setLayout8x2();
}

void setup()
{
  leds.set(FRONT_LEFT_LED, RED, 0);
  leds.set(FRONT_CENTER_LED, ORANGE, 0);
  leds.set(FRONT_RIGHT_LED, YELLOW, 0);
  leds.set(BACK_RIGHT_LED, GREEN, 0);
  leds.set(BACK_CENTER_LED, BLUE, 0);
  leds.set(BACK_LEFT_LED, VIOLET, 0);

  static const PololuMenuItem mainMenuItems[] = {
    { F("Power"), powerDemo },
    { F("LineSens"), lineSensorDemo },
    { F("BumpSens"), bumpSensorDemo },
    { F("Inertial"), inertialDemo },
    { F("Compass"), compassDemo },
    { F("Motors"), motorDemo },
    { F("Encoders"), encoderDemo },
    { F("Spin"), spinDemo },
    { F("LEDs"), ledDemo },
    { F("OLED"), displayDemo },
    { F("Music"), musicDemo },
  };
  mainMenu.setItems(mainMenuItems, sizeof(mainMenuItems)/sizeof(mainMenuItems[0]));
  mainMenu.setDisplay(display);
  mainMenu.setBuzzer(buzzer);
  mainMenu.setButtons(buttonA, buttonB, buttonC);
  mainMenu.setSecondLine(F("\x7f" "A \xa5" "B C\x7e"));

  initInertialSensors();

  loadCustomCharacters();

  buzzer.play(beepWelcome);

  // allow skipping quickly to the menu by holding button C
  if (buttonC.isPressed())
  {
    selfTest();
    return;
  }

  showSplash();

  mainMenuWelcome();
}

// Clear LEDs and show a message about the main menu.
void mainMenuWelcome()
{
  ledYellow(false);
  leds.setBrightness(0);
  display.clear();
  display.print("  Main");
  display.gotoXY(0, 1);
  display.print("  Menu");
  delay(1000);
}

void loop()
{
  if(mainMenu.select())
  {
    // a menu item ran; show "Main Menu" again and repeat
    mainMenuWelcome();
  }
}
