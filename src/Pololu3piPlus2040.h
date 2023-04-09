// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus2040.h
///
/// \brief Main header file for the Pololu 3pi+ 2040 library.
///
/// This file includes all the other headers files provided by the library.

#pragma once

#ifndef ARDUINO_ARCH_RP2040
#error "This library only supports the RP2040.  Try selecting Raspberry Pi Pico in the Boards menu."
#endif

// Pull in Pololu's C SDK for the 3pi+ 2040 robot.
extern "C"
{
  #include "pololu-3pi-2040-robot/pololu_3pi_2040_robot.h"
}

#include "Pololu3piPlus2040BumpSensors.h"
#include "Pololu3piPlus2040Buttons.h"
#include "Pololu3piPlus2040Buzzer.h"
#include "Pololu3piPlus2040Encoders.h"
#include "Pololu3piPlus2040IMU.h"
#include "Pololu3piPlus2040LEDs.h"
#include "Pololu3piPlus2040LineSensors.h"
#include "Pololu3piPlus2040Motors.h"
#include "Pololu3piPlus2040OLED.h"

/// Top-level namespace for the Pololu3piPlus2040 library.
namespace Pololu3piPlus2040
{

/// \brief Turns the yellow user LED on pin 25 on or off.
///
/// \param on A value of 1 turns on the LED; 0 turns it off.
static inline void ledYellow(bool on)
{
  yellow_led(on);
}

/// \brief Reads the battery voltage and returns it in millivolts.
static inline uint16_t readBatteryMillivolts()
{
  return battery_get_level_millivolts();
}

}


// By default, the Pololu3piPlus2040 namespace will be used.
#ifndef DISABLE_POLOLU_3PI_PLUS_2040_NAMESPACE
    using namespace Pololu3piPlus2040;
#endif
