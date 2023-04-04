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

/// Reads the battery voltage and returns it in millivolts.
uint16_t readBatteryMillivolts()
{
    // Pin 26 is shared with the down emitter. The code in the LineSensors class will reconfigure to SIO mode
    // every time it is used to enable or disable the emitter.
    const uint32_t batteryVoltagePin = 26;
    const uint32_t sampleCount = 10;
    uint32_t sum = 0;
    for (uint32_t i = 0; i < sampleCount; i++)
    {
        sum += analogRead(batteryVoltagePin);
    }

    // The voltage divider steps the voltage down by 1/11th of the actual battery voltage.
    // The analogRead readings fall in a 10-bit range.
    return (3300 * sum * 11 + sampleCount * 511) / (sampleCount * 1023);
}

}


// By default, the Pololu3piPlus2040 namespace will be used.
#ifndef DISABLE_POLOLU_3PI_PLUS_2040_NAMESPACE
    using namespace Pololu3piPlus2040;
#endif
