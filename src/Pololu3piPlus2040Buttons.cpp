// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus2040Buttons.cpp

#include <Arduino.h>
#include "hardware/structs/ioqspi.h"
#include <hardware/sync.h>
#include "Pololu3piPlus2040Buttons.h"


namespace Pololu3piPlus2040
{

    // Copied copied from TinyUSB samples in PicoSDK.
    //
    // The MIT License (MIT)
    //
    // Copyright (c) 2018, hathach (tinyusb.org)
    //
    // Permission is hereby granted, free of charge, to any person obtaining a copy
    // of this software and associated documentation files (the "Software"), to deal
    // in the Software without restriction, including without limitation the rights
    // to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    // copies of the Software, and to permit persons to whom the Software is
    // furnished to do so, subject to the following conditions:
    //
    // The above copyright notice and this permission notice shall be included in
    // all copies or substantial portions of the Software.
    //
    // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    // AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    // OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    // THE SOFTWARE.
    //
    // Picoboard has a button attached to the flash CS pin, which the bootrom
    // checks, and jumps straight to the USB bootcode if the button is pressed
    // (pulling flash CS low). We can check this pin in by jumping to some code in
    // SRAM (so that the XIP interface is not required), floating the flash CS
    // pin, and observing whether it is pulled low.
    //
    // This doesn't work if others are trying to access flash at the same time,
    // e.g. XIP streamer, or the other core.
    static bool __no_inline_not_in_flash_func(get_bootsel_button)(void)
    {
        const uint CS_PIN_INDEX = 1;

        // Must disable interrupts, as interrupt handlers may be in flash, and we
        // are about to temporarily disable flash access!
        uint32_t flags = save_and_disable_interrupts();

        // Set chip select to Hi-Z
        hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                        GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                        IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

        // Note we can't call into any sleep functions in flash right now
        for (volatile int i = 0; i < 1000; ++i);

        // The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
        // Note the button pulls the pin *low* when pressed.
        bool button_state = (sio_hw->gpio_hi_in & (1u << CS_PIN_INDEX));

        // Need to restore the state of chip select, else we are going to have a
        // bad time when we return to code in flash!
        hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                        GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                        IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

        restore_interrupts(flags);

        return button_state;
    }

    bool ButtonB::isPressed()
    {
        return !get_bootsel_button();
    }
}
