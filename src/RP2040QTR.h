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
// Use the RP2040's PIO state machines to read the 7 QTR light sensors on the
// 3π+ 2040 robot. It is a C/C++ port of Pololu's MicroPython code.
#pragma once
#include <Arduino.h>
#include <hardware/clocks.h>
#include "RP2040SIO.h"
#include "RP2040QTR.pio.h"


#ifndef ARDUINO_ARCH_RP2040
#error "This library only supports the RP2040.  Try selecting Raspberry Pi Pico in the Boards menu."
#endif


namespace Pololu3piPlus2040
{
    #define BUMP_SENSOR_COUNT 2
    #define LINE_SENSOR_COUNT 5
    #define LIGHT_SENSOR_COUNT (BUMP_SENSOR_COUNT + LINE_SENSOR_COUNT)

    struct BumperSensorReadings
    {
        union
        {
            struct
            {
                uint16_t right;
                uint16_t left;
            };
            uint16_t vals[2];
        };
    };

    struct LineSensorReadings
    {
        uint16_t vals[LINE_SENSOR_COUNT];
    };

    struct QTRSensorReadings
    {
        union
        {
            struct
            {
                BumperSensorReadings bumperReadings;
                LineSensorReadings lineReadings;
            };
            uint16_t raw[LIGHT_SENSOR_COUNT];
        };
    };

    class QTRSensors
    {
        protected:
            // The 7 analog light sensors start at this pin. Must be continuous to work with the PIO.
            static const uint32_t lightSensorPinBase = 16;
            static const uint32_t lightSensorPinCount = LIGHT_SENSOR_COUNT;

            PIO      m_pio = pio0;
            int32_t  m_stateMachine = -1;
            uint32_t m_codeOffset = 0;
            enum {
                IDLE,
                READING,
            }  m_state = IDLE;

        public:
            static const uint32_t TIMEOUT = 1024;

            QTRSensors()
            {
                // Make sure that there is enough room to load this program into one of the PIO instances.
                m_pio = pio0;
                if (!pio_can_add_program(m_pio, &RP2040QTR_program) )
                {
                    m_pio = pio1;
                    if (!pio_can_add_program(m_pio, &RP2040QTR_program) )
                    {
                        assert ( !"No free PIO" );
                        return;
                    }
                }
                m_codeOffset = pio_add_program(m_pio, &RP2040QTR_program);

                // Find an unused state machine in the PIO to run the code for counting this encoder.
                m_stateMachine = pio_claim_unused_sm(m_pio, false);
                if (m_stateMachine < 0)
                {
                    // No free state machines so return a failure code.
                    assert ( !"No free state machines" );
                    return;
                }

                // Connect the 7 sensor pins to the PIO peripheral for output.
                for (uint32_t pin = lightSensorPinBase ; pin < lightSensorPinBase + lightSensorPinCount ; pin++)
                {
                    pio_gpio_init(m_pio, pin);
                    gpio_set_pulls(pin, false, false);
                }

                // Use the state machine to set all of the pins high by default.
                // When PIO code sets pindirs to enable output, it will pull the pins high.
                uint32_t pinMask = ((1 << lightSensorPinCount) - 1) << lightSensorPinBase;
                pio_sm_set_pins_with_mask(m_pio, m_stateMachine, pinMask, pinMask);

                // Configure the state machine to run the QTR sampling program.
                pio_sm_config smConfig = RP2040QTR_program_get_default_config(m_codeOffset);
                const bool shiftLeft = false;
                const bool autoPush = true;
                const uint pushThreshold = 23;
                sm_config_set_in_shift(&smConfig, shiftLeft, autoPush, pushThreshold);
                sm_config_set_in_pins(&smConfig, lightSensorPinBase);
                sm_config_set_out_pins(&smConfig, lightSensorPinBase, lightSensorPinCount);
                // Use the TX FIFO entries for RX since we don't use the TX path. This makes for an 8 element RX FIFO.
                sm_config_set_fifo_join(&smConfig, PIO_FIFO_JOIN_RX);
                // Configure the state machine to run at 8MHz.
                float div = (float)clock_get_hz(clk_sys) / 8000000.0f;
                sm_config_set_clkdiv(&smConfig, div);
                pio_sm_init(m_pio, m_stateMachine, m_codeOffset, &smConfig);
            }

            void startRead()
            {
                if (m_state == READING)
                {
                    return;
                }

                // Restart the state machine to see how long the capacitor takes to discharge through the QTR.
                pio_sm_set_enabled(m_pio, m_stateMachine, false);
                // Initialize the registers in the state machine so that I don't need to waste precious PIO code space.
                // Set OSR to 32 bits of 1s for future shifting out to initialize y, x, pindirs, and y again. This
                // requires 7 + 8 + 10 + 7 = 32 bits.
                pio_sm_exec(m_pio, m_stateMachine, pio_encode_mov_not(pio_osr, pio_null));
                // Set Y counter to 255 by pulling 8 high bits from OSR. At 8MHz this results in ~32us of charge time.
                pio_sm_exec(m_pio, m_stateMachine, pio_encode_out(pio_y, 8));
                // Initialize X (last pin state) to 7 bits of 1s.
                pio_sm_exec(m_pio, m_stateMachine, pio_encode_out(pio_x, 7));
                // Reset the program counter back to the beginning of the program.
                pio_sm_exec(m_pio, m_stateMachine, pio_encode_jmp(m_codeOffset));
                // Start the state machine up again.
                pio_sm_set_enabled(m_pio, m_stateMachine, true);
                m_state = READING;
            }

            QTRSensorReadings read()
            {
                startRead();

                QTRSensorReadings readings;

                for (size_t i = 0 ; i < sizeof(readings.raw)/sizeof(readings.raw[0]) ; i++)
                {
                    readings.raw[i] = TIMEOUT;
                }
                uint32_t lastPinStates = (1 << lightSensorPinCount) - 1;

                while (true)
                {
                    uint32_t val = pio_sm_get_blocking(m_pio, m_stateMachine);
                    if (val == 0xFFFFFFFF)
                    {
                        // The PIO code returns -1 when it stops.
                        break;
                    }
                    uint32_t currPinStates = (val >> 16) & 0x7F;
                    uint32_t currTime = val & 0xFFFF;
                    uint32_t newZeros = lastPinStates ^ currPinStates;
                    for (size_t i = 0 ; i < sizeof(readings.raw)/sizeof(readings.raw[0]) ; i++)
                    {
                        uint32_t bitmask = 1 << i;
                        if ((newZeros & bitmask) && readings.raw[i] == TIMEOUT)
                        {
                            readings.raw[i] = TIMEOUT - currTime;
                        }
                    }
                    lastPinStates = currPinStates;
                }
                m_state = IDLE;

                return readings;
            }

            static QTRSensors* getSharedQTR();
    };

} // namespace Pololu3piPlus2040
