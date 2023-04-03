// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus2040Buttons.h

#pragma once

#include <Pushbutton.h>
#include <hardware/gpio.h>
#include <hardware/structs/iobank0.h>
#include "RP2040SIO.h"

namespace Pololu3piPlus2040
{

/// \brief Interfaces with button A on the 3pi+ 2040.
///
/// The pin used for button A is also used for the Yellow LED.
///
/// This class temporarily sets the pin to be an input with no pull-up/down
/// as there is an external pull-up on the PCB.
class ButtonA : public PushbuttonBase
{
private:
    /// The pin number for the pin connected to button C on the 3pi+ 2040.
    static const uint32_t buttonAPin = 25;

public:

    virtual bool isPressed()
    {
        // Remember current pad output enable override setting and then force
        // the pad to disconnect any peripheral output.
        uint32_t originalPadCTRL = iobank0_hw->io[buttonAPin].ctrl;
        gpio_set_oeover(buttonAPin, GPIO_OVERRIDE_LOW);

        // Wait for pin to stabilize.
        delayMicroseconds(1);

        // Read the state of the pin, bypassing whatever peripheral is using it.
        bool isPressed = !((iobank0_hw->io[buttonAPin].status & IO_BANK0_GPIO0_STATUS_INFROMPAD_BITS)
                            >> IO_BANK0_GPIO0_STATUS_INFROMPAD_LSB);

        // Restore the output enable override bits.
        iobank0_hw->io[buttonAPin].ctrl = originalPadCTRL;
        return isPressed;
    }
};

/// \brief Interfaces with button B on the 3pi+ 2040.
///
/// The pin used for button B is also used for the FLASH chip select and BOOTSEL.
///
/// This class temporarily sets the pin to be an input with pull-up.
class ButtonB : public PushbuttonBase
{
public:
    virtual bool isPressed();
};

/// \brief Interfaces with button C on the 3pi+ 2040.
///
/// The pin used for button C is also used for the OLED D/C signal.
///
/// This class temporarily sets the pin to be an input with pull-up.
class ButtonC : public PushbuttonBase
{
private:
    /// The pin number for the pin connected to button C on the 3pi+ 2040.
    static const uint32_t buttonCPin = 0;

public:

    virtual bool isPressed()
    {
        bool isOutputNo = false;
        bool outputValOff = false;
        bool pullUpYes = true;
        bool pullDownNo = false;
        RP2040SIO::PinLoan<buttonCPin> loan;

        RP2040SIO::Pin<buttonCPin>::init(isOutputNo, outputValOff, pullUpYes, pullDownNo);
        delayMicroseconds(1);
        return !RP2040SIO::Pin<buttonCPin>::isInputHigh();
    }
};

}
