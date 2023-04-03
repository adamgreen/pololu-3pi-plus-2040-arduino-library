// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file RP2040SIO.h

RP2040SIO is a C++ header library for efficient RP2040 I/O. It makes use of the
SIO peripheral to gain fast access to individual pins on the device. It doesn't
use static functions like the AVR FastGPIO library since each pin needs to be
configured for use with the SIO peripheral and the best place to do this is
from an object constructor. The pin bitmask is still a static constant so the
code generated can contain the necessary bitmask directly in the code and not
need to be calculated at runtime.

The RP2040SIO::Pin class provides inline methods for manipulating pins.  See
its class reference for more information.

\class RP2040SIO::Pin

@tparam pin The pin number

The RP2040SIO::Pin class provides inline methods for manipulating pins.  This
class can only be used if the pin number is known at compile time, which means
it does not come from a variable that might change and it does not come from the
result of a complicated calculation.

Here is some example code showing how to use this class to blink an LED:

~~~{.cpp}
#include <RP2040SIO.h>

#define LED_PIN 25

void setup() {
}

void loop() {
  RP2040SIO::Pin<LED_PIN> led;

  led.setOutput(0);
  delay(500);
  led.setOutput(1);
  delay(500);
}
~~~

*/

#pragma once
#include <Arduino.h>
#include <stdint.h>

#ifndef ARDUINO_ARCH_RP2040
#error "This library only supports the RP2040.  Try selecting Raspberry Pi Pico in the Boards menu."
#endif

// UNDONE: I have only implemented the methods which have actually been used and tested during the port of the
// Pololu3piPlus2040 Arduino library.
namespace RP2040SIO
{
    template<uint32_t pin> class Pin
    {
    public:
        /*! \brief Constructs the pin to be used by SIO quickly in future.
         *
         * @param isOutput If true set output enable for this GPIO
         * @param initOutput Sets GPIO initial state if isOutput is true
         * @param pullUp   If true set a pull up on the GPIO
         * @param pullDown If true set a pull down on the GPIO
         */
        inline Pin(bool isOutput, bool initOutput, bool pullUp, bool pullDown) __attribute__((always_inline))
        {
            init(isOutput, initOutput, pullUp, pullDown);
        }

        /*! \brief Constructs the pin to be used by SIO quickly in future.
         *
         * This is equivalent to calling init() with all 4 arguments set to false.
         */
        inline Pin() __attribute__((always_inline))
        {
            init(false, false, false, false);
        }

        /*! \brief Configures the pin to be used by SIO in the future.
         *
         * @param isOutput If true set output enable for this GPIO
         * @param initOutput Sets GPIO initial state if isOutput is true
         * @param pullUp   If true set a pull up on the GPIO
         * @param pullDown If true set a pull down on the GPIO
         */
        static inline void init(bool isOutput, bool initOutput, bool pullUp, bool pullDown) __attribute__((always_inline))
        {
            gpio_set_function(pin, GPIO_FUNC_SIO);
            gpio_set_pulls(pin, pullUp, pullDown);

            if (isOutput)
            {
                sio_hw->gpio_oe_set = m_mask;
                setOutput(initOutput);
            }
            else
            {
                sio_hw->gpio_oe_clr = m_mask;
            }
        }

        /*! \brief Configures the pin to be an output driving low.
         *
         * This is equivalent to calling setOutput with an argument of 0,
         * but it has a simpler implementation which means it is more
         * likely to be compiled down to just 2 assembly instructions.
         */
        static inline void setOutputLow() __attribute__((always_inline))
        {
            sio_hw->gpio_clr = m_mask;
            sio_hw->gpio_oe_set = m_mask;
        }

        /*! \brief Configures the pin to be an output driving high.
         *
         * This is equivalent to calling setOutput with an argument of 1,
         * but it has a simpler implementation which means it is more
         * likely to be compiled down to just 2 assembly instructions.
         */
        static inline void setOutputHigh() __attribute__((always_inline))
        {
            sio_hw->gpio_set = m_mask;
            sio_hw->gpio_oe_set = m_mask;
        }

        /*! \brief Sets the pin as an output.
         *
         * @param value Should be 0, LOW, or false to drive the pin low.  Should
         * be 1, HIGH, or true to drive the pin high.
         *
         * The PORT bit is set before the DDR bit to ensure that the output is
         * not accidentally driven to the wrong value during the transition.
         */
        static inline void setOutput(bool value) __attribute__((always_inline))
        {
            if (value)
            {
                sio_hw->gpio_set = m_mask;
            }
            else
            {
                sio_hw->gpio_clr = m_mask;
            }
            sio_hw->gpio_oe_set = m_mask;
        }

        /*! \brief Sets the output value of the pin to 0.
         *
         * This is mainly intended to be used on pins that have already been
         * configured as an output in order to make the output drive low.
         *
         * If this is used on an input pin, it has the effect of disabling the
         * input pin's pull-up resistor.
         */
        static inline void setOutputValueLow() __attribute__((always_inline))
        {
            sio_hw->gpio_clr = m_mask;
        }

        /*! \brief Sets the output value of the pin to 1.
         *
         * This is mainly intended to be used on pins that have already been
         * configured as an output in order to make the output drive low.
         *
         * If this is used on an input pin, it has the effect of enabling the
         * input pin's pull-up resistor.
         */
        static inline void setOutputValueHigh() __attribute__((always_inline))
        {
            sio_hw->gpio_set = m_mask;
        }

        /*! \brief Sets the output value of the pin.
         *
         * @param value Should be 0, LOW, or false to drive the pin low.  Should
         * be 1, HIGH, or true to drive the pin high.
         *
         * This is mainly intended to be used on pins that have already been
         * configured as an output.
         *
         * If this function is used on an input pin, it has the effect of
         * toggling setting the state of the input pin's pull-up resistor.
         */
        static inline void setOutputValue(bool value) __attribute__((always_inline))
        {
            if (value)
            {
                sio_hw->gpio_set = m_mask;
            }
            else
            {
                sio_hw->gpio_clr = m_mask;
            }
        }

        /*! \brief Sets a pin to be a digital input with the internal pull-up
         *  resistor disabled.
         */
        static inline void setInput() __attribute__((always_inline))
        {
            sio_hw->gpio_oe_clr = m_mask;
        }

        /*! \brief Reads the input value of the pin.
         *
         * @return 0 if the pin is low, 1 if the pin is high.
         */
        static inline bool isInputHigh() __attribute__((always_inline))
        {
            return !!(sio_hw->gpio_in & m_mask);
        }

#ifdef UNDONE
        /*! \brief Configures the pin to be an output and toggles it.
         */
        static inline void setOutputToggle() __attribute__((always_inline))
        {
            setOutputValueToggle();
            _FG_SBI(pinStructs[pin].ddrAddr, pinStructs[pin].bit);
        }

        /*! \brief Toggles the output value of the pin.
         *
         * This is mainly intended to be used on pins that have already been
         * configured as an output.  If the pin was driving low, this function
         * changes it to drive high.  If the pin was driving high, this function
         * changes it to drive low.
         *
         * If this function is used on an input pin, it has the effect of
         * toggling the state of the input pin's pull-up resistor.
         */
        static inline void setOutputValueToggle() __attribute__((always_inline))
        {
            _FG_SBI(pinStructs[pin].pinAddr, pinStructs[pin].bit);
        }

        /*! \brief Sets a pin to be a digital input with the internal pull-up
         *  resistor enabled.
         */
        static inline void setInputPulledUp() __attribute__((always_inline))
        {
            _FG_CBI(pinStructs[pin].ddrAddr, pinStructs[pin].bit);
            _FG_SBI(pinStructs[pin].portAddr, pinStructs[pin].bit);
        }
#endif // UNDONE

        /*! \brief Returns 1 if the pin is configured as an output.
         *
         * @return 1 if the pin is an output, 0 if it is an input.
         */
        static inline bool isOutput() __attribute__((always_inline))
        {
            return !!(sio_hw->gpio_oe & m_mask);
        }

        /*! \brief Returns the output value of the pin.
         *
         * This is mainly intended to be called on pins that have been
         * configured an an output.  If it is called on an input pin, the return
         * value indicates whether the pull-up resistor is enabled or not.
         */
        static inline bool isOutputValueHigh() __attribute__((always_inline))
        {
            return !!(sio_hw->gpio_out & m_mask);
        }

        /*! \brief Returns the full 2-bit state of the pin.
         *
         * Bit 0 of this function's return value is the pin's output value.
         * Bit 1 of the return value is the pin direction; a value of 1
         * means output.  All the other bits are zero.
         */
        static uint8_t getState()
        {
              return (isOutput() << 1) | isOutputValueHigh();
        }

        /*! \brief Sets the full 2-bit state of the pin.
         *
         * @param state The state of the pin, as returns from getState.
         * All bits other than bits 0 and 1 are ignored.
         */
        static void setState(uint8_t state)
        {
            if (state & 2)
            {
                setOutput(state & 1);
            }
            else
            {
                setInput();
                setOutputValue(state & 1);
            }
        }

      private:
        static const uint32_t m_mask = 1 << pin;
    };

    /*! This class saves the state of the specified pin in its constructor when
     * it is created, and restores the pin to that state in its destructor.
     * This can be very useful if a pin is being used for multiple purposes.
     * It allows you to write code that temporarily changes the state of the
     * pin and is guaranteed to restore the state later.
     *
     * For example, if you were controlling both a button and an LED using a
     * single pin and you wanted to see if the button was pressed without
     * affecting the LED, you could write:
     *
     * ~~~{.cpp}
     * bool buttonIsPressed()
     * {
     *     RP2040SIO::PinLoan<IO_D5> loan;
     *     RP2040SIO::Pin<IO_D5>::setInputPulledUp();
     *     _delay_us(10);
     *     return !RP2040SIO::Pin<IO_D5>::isInputHigh();
     * }
     * ~~~
     *
     * This is equivalent to:
     *
     * ~~~{.cpp}
     * bool buttonIsPressed()
     * {
     *     uint8_t state = RP2040SIO::Pin<IO_D5>::getState();
     *     RP2040SIO::Pin<IO_D5>::setInputPulledUp();
     *     _delay_us(10);
     *     bool value = !RP2040SIO::Pin<IO_D5>::isInputHigh();
     *     RP2040SIO::Pin<IO_D5>::setState(state);
     *     return value;
     * }
     * ~~~
     */
    template<uint32_t pin> class PinLoan
    {
    public:
        PinLoan()
        {
            m_state = RP2040SIO::Pin<pin>::getState();
        }

        ~PinLoan()
        {
            RP2040SIO::Pin<pin>::setState(m_state);
        }
    protected:
        /*! \brief The state of the pin as returned from RP2040SIO::Pin::getState. */
        uint32_t m_state;
    };
};
