/* Load this example to make the 3pi+ 2040 drive forward until it hits
a wall, detect the collision with its bumpers, then reverse, turn, and
keep driving.
*/

#include <Pololu3piPlus2040.h>
#include <PololuMenu.h>

OLED display;
BumpSensors bumpSensors;
Buzzer buzzer;
Motors motors;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
RGBLEDs leds;

/* Configuration for specific 3pi+ editions: the Standard, Turtle, and
Hyper versions of 3pi+ have different motor configurations, requiring
the demo to be configured with different parameters for proper
operation.  The following functions set up these parameters using a
menu that runs at the beginning of the program.  To bypass the menu,
you can replace the call to selectEdition() in setup() with one of the
specific functions.
*/

// This is the maximum speed the motors will be allowed to turn.
// A maxSpeed of 400 lets the motors go at top speed.  Decrease
// this value to impose a speed limit.
int16_t maxSpeed;

int16_t turnTime;

void selectHyper()
{
  motors.flipLeftMotor(true);
  motors.flipRightMotor(true);
  // Encoders are not used in this example.
  // encoders.flipEncoders(true);
  maxSpeed = 75;
  turnTime = 150;
}

void selectStandard()
{
  maxSpeed = 100;
  turnTime = 250;
}

void selectTurtle()
{
  maxSpeed = 200;
  turnTime = 500;
}

PololuMenu menu;

void selectEdition()
{
  display.clear();
  display.print("Select");
  display.gotoXY(0,1);
  display.print("edition");
  delay(1000);

  static const PololuMenuItem items[] = {
    { "Standard", selectStandard },
    { "Turtle", selectTurtle },
    { "Hyper", selectHyper },
  };

  menu.setItems(items, 3);
  menu.setDisplay(display);
  menu.setBuzzer(buzzer);
  menu.setButtons(buttonA, buttonB, buttonC);

  while(!menu.select());

  display.gotoXY(0,1);
  display.print("OK!  ...");
}

void setup()
{
  leds.set(FRONT_LEFT_LED, RED, 0);
  leds.set(FRONT_RIGHT_LED, RED, 0);

  // To bypass the menu, replace this function with
  // selectHyper(), selectStandard(), or selectTurtle().
  selectEdition();

  bumpSensors.calibrate();
  delay(1000);
  display.clear();
}

void loop()
{
  motors.setSpeeds(maxSpeed, maxSpeed);
  bumpSensors.read();

  if (bumpSensors.leftIsPressed())
  {
    // Left bump sensor is pressed.
    leds.setBrightness(FRONT_LEFT_LED, 31);
    motors.setSpeeds(0, 0);
    buzzer.play("a32");
    display.gotoXY(0, 0);
    display.print('L');

    motors.setSpeeds(maxSpeed, -maxSpeed);
    delay(turnTime);

    motors.setSpeeds(0, 0);
    buzzer.play("b32");
    leds.setBrightness(FRONT_LEFT_LED, 0);
    display.gotoXY(0, 0);
    display.print(' ');
  }
  else if (bumpSensors.rightIsPressed())
  {
    // Right bump sensor is pressed.
    leds.setBrightness(FRONT_RIGHT_LED, 31);
    motors.setSpeeds(0, 0);
    buzzer.play("e32");
    display.gotoXY(7, 0);
    display.print('R');

    motors.setSpeeds(-maxSpeed, maxSpeed);
    delay(turnTime);

    motors.setSpeeds(0, 0);
    buzzer.play("f32");
    leds.setBrightness(FRONT_RIGHT_LED, 0);
    display.gotoXY(7, 0);
    display.print(' ');
  }
}
