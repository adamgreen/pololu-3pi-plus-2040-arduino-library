// This program shows how to read the encoders on the 3pi+ 2040.
// The encoders can tell you how far, and in which direction each
// motor has turned.
//
// You can press button A on the 3pi+ to drive both motors
// forward at full speed.  You can press button C to drive both
// motors in reverse at full speed.
//
// Encoder counts are printed to the LCD/OLED screen and to the
// serial monitor.
//
// On the screen, the top line shows the counts from the left
// encoder, and the bottom line shows the counts from the right
// encoder.
//
// In the serial monitor, the first and second numbers represent
// counts from the left and right encoders, respectively.

#include <Pololu3piPlus2040.h>

Encoders encoders;
Motors motors;
ButtonA buttonA;
ButtonC buttonC;
OLED display;

const char encoderErrorLeft[] = "!<c2";
const char encoderErrorRight[] = "!<e2";

char report[80];

void setup()
{
  // Uncomment the following lines for the Hyper edition; its wheels
  // spin in the opposite direction relative to the encoders.
  // encoders.flipEncoders(true);
  // motors.flipLeftMotor(true);
  // motors.flipRightMotor(true);
}

void loop()
{
  static uint8_t lastDisplayTime = 0;

  if ((uint8_t)(millis() - lastDisplayTime) >= 100)
  {
    lastDisplayTime = millis();

    int16_t countsLeft = encoders.getCountsLeft();
    int16_t countsRight = encoders.getCountsRight();

    // Update the screen with encoder counts and error info.
    display.noAutoDisplay();
    display.clear();
    display.print(countsLeft);
    display.gotoXY(0, 1);
    display.print(countsRight);
    display.display();

    // Send the information to the serial monitor also.
    snprintf_P(report, sizeof(report),
        PSTR("%6d %6d"),
        countsLeft, countsRight);
    Serial.println(report);
  }

  if (buttonA.isPressed())
  {
    motors.setSpeeds(400, 400);
  }
  else if (buttonC.isPressed())
  {
    motors.setSpeeds(-400, -400);
  }
  else
  {
    motors.setSpeeds(0, 0);
  }
}
