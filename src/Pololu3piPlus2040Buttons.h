// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus2040Buttons.h

#pragma once

#include <Pushbutton.h>

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
public:
    virtual bool isPressed()
    {
        return button_a_is_pressed();
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
    virtual bool isPressed()
    {
        return button_b_is_pressed();
    }
};

/// \brief Interfaces with button C on the 3pi+ 2040.
///
/// The pin used for button C is also used for the OLED D/C signal.
///
/// This class temporarily sets the pin to be an input with pull-up.
class ButtonC : public PushbuttonBase
{
public:
    virtual bool isPressed()
    {
        return button_c_is_pressed();
    }
};

}
