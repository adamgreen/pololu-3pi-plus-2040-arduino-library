#include <Pololu3piPlus2040.h>

using namespace Pololu3piPlus2040;
BumpSensors bumpSensors;
Buzzer buzzer;
OLED display;
RGBLEDs leds;


void setup()
{
  bumpSensors.calibrate();
  display.clear();
  display.gotoXY(0, 1);
  display.print("Bump me!");
  leds.set(FRONT_LEFT_LED, &GREEN, 0);
  leds.set(FRONT_RIGHT_LED, &RED, 0);
  leds.show();
}

void loop()
{
  bumpSensors.read();

  if (bumpSensors.leftChanged())
  {
    leds.setBrightness(FRONT_LEFT_LED, bumpSensors.leftIsPressed() ? 31 : 0);
    leds.show();

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
    leds.setBrightness(FRONT_RIGHT_LED, bumpSensors.rightIsPressed() ? 31 : 0);
    leds.show();

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
