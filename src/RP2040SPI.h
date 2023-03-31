/* Copyright 2023 Adam Green (https://github.com/adamgreen/)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
// Class to share the SPI0 peripheral between the OLED display addressable RGB
// LEDs on the Pololu 3π+ 2040 robot.
#pragma once
#include <mbed.h>
#include "RP2040SIO.h"

#ifndef ARDUINO_ARCH_RP2040
#error "This library only supports the RP2040.  Try selecting Raspberry Pi Pico in the Boards menu."
#endif


namespace Pololu3piPlus2040
{

    class SharedSPI : public mbed::SPI
    {
        protected:
            static const uint32_t sckForDisplayPin = 2;
            static const uint32_t sckForLEDsPin = 6;
            static const PinName mosiPin = p3;
            static const PinName misoPin = NC;
            static const PinName csPin = NC;

            bool m_configuredForDisplay = false;
            RP2040SIO::Pin<sckForDisplayPin> m_displaySckPin;
            RP2040SIO::Pin<sckForLEDsPin>    m_ledsSckPin;

        public:
            SharedSPI() :
                mbed::SPI(mosiPin, misoPin, p2, csPin)
            {
                format(8, 0);
                switchToDisplay();
            }

            void writeToDisplay(const void* pTx, size_t txSize, void* pRx, size_t rxSize)
            {
                switchToDisplay();
                write((const char*)pTx, txSize, (char*)pRx, rxSize);
            }

            void writeToLEDs(const void* pTx, size_t txSize, void* pRx, size_t rxSize)
            {
                switchToLEDs();
                write((const char*)pTx, txSize, (char*)pRx, rxSize);
            }

            static SharedSPI* getSharedSPI();

        protected:
            void switchToDisplay()
            {
                if (m_configuredForDisplay)
                    return;
                frequency(4000000);
                // Switch sck pins from LED to Display.
                // UNDONE: I think the setOutputLow() stuff can just be done once in constructor.
                m_ledsSckPin.setOutputLow();
                gpio_set_function(sckForLEDsPin, GPIO_FUNC_SIO);
                gpio_set_function(sckForDisplayPin, GPIO_FUNC_SPI);
                m_configuredForDisplay = true;
            }

            void switchToLEDs()
            {
                if (!m_configuredForDisplay)
                    return;
                frequency(20000000);
                // Switch sck pins from Display to LED.
                m_displaySckPin.setOutputLow();
                gpio_set_function(sckForDisplayPin, GPIO_FUNC_SIO);
                gpio_set_function(sckForLEDsPin, GPIO_FUNC_SPI);
                m_configuredForDisplay = false;
            }
    };

} // namespace Pololu3piPlus2040
