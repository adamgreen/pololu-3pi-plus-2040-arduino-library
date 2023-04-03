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
#include "Pololu3piPlus2040LEDs.h"
#include "Pololu3piPlus2040LineSensors.h"
#include "Pololu3piPlus2040Motors.h"
#include "Pololu3piPlus2040OLED.h"

#ifdef UNDONE
#include <Pololu3piPlus32U4Encoders.h>
#include <Pololu3piPlus32U4IMU_declaration.h>
#endif // UNDONE

/// Top-level namespace for the Pololu3piPlus2040 library.
namespace Pololu3piPlus2040
{

// TODO: servo support

#ifdef UNDONE
/// \brief Returns true if USB power is detected.
///
/// This function returns true if power is detected on the board's USB port and
/// returns false otherwise.  It uses the ATmega32U4's VBUS line, which is
/// directly connected to the power pin of the USB connector.
///
/// \sa A method for detecting whether the board's virtual COM port is open:
/// http://arduino.cc/en/Serial/IfSerial
inline bool usbPowerPresent()
{
    return USBSTA >> VBUS & 1;
}

/// Reads the battery voltage and returns it in millivolts.
inline uint16_t readBatteryMillivolts()
{
    const uint8_t sampleCount = 8;
    uint16_t sum = 0;
    for (uint8_t i = 0; i < sampleCount; i++)
    {
        sum += analogRead(A1);
    }

    // VBAT = 3 * millivolt reading = 3 * raw * 5000/1024
    //      = raw * 1875 / 128
    // The correction term below makes it so that we round to the
    // nearest whole number instead of always rounding down.
    const uint32_t correction = 64 * sampleCount - 1;
    return ((uint32_t)sum * 1875 + correction) / (128 * sampleCount);
}
#endif // UNDONE

}


// By default, the Pololu3piPlus2040 namespace will be used.
#ifndef DISABLE_POLOLU_3PI_PLUS_2040_NAMESPACE
    using namespace Pololu3piPlus2040;
#endif
