// Copyright (C) Pololu Corporation.  See LICENSE.txt for details.

#include "pololu_3pi_2040_robot.h"
//#include <pico/stdlib.h>
#include <hardware/gpio.h>
#define gpio_init _gpio_init

void yellow_led(bool b)
{
  gpio_init(25);
  gpio_put(25, !b);
  gpio_set_dir(25, GPIO_OUT);
}
