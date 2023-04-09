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
// Classes to control the 6 addressable LEDs and the single yellow LED
// on the Pololu 3π+ 2040 robot.
//
// This C++ code is based on Pololu's MicroPython implementation of the
// same functionality.
#pragma once
#include <mbed.h>
#include "RP2040SIO.h"
#include "RP2040SPI.h"

namespace Pololu3piPlus2040
{

    // Structure used to contain the RGB or HSV colour values for each LED.
    struct RGB
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        RGB(uint8_t r, uint8_t g, uint8_t b)
        {
            this->r = r;
            this->g = g;
            this->b = b;
        }
    };

    struct HSV
    {
        uint8_t h;
        uint8_t s;
        uint8_t v;

        HSV(uint8_t h, uint8_t s, uint8_t v)
        {
            this->h = h;
            this->s = s;
            this->v = v;
        }
    };

    // Some commonly used RGB colours.
    const RGB BLACK(0, 0, 0);
    const RGB WHITE(255, 255, 255);
    const RGB RED(255, 0, 0);
    const RGB ORANGE(255, 128, 0);
    const RGB YELLOW(255, 255, 0);
    const RGB GREEN(0, 255, 0);
    const RGB BLUE(0, 0, 255);
    const RGB VIOLET(255, 0, 255);

    // Indices for the 6 RGB LEDs installed on the 3π+ 2040 robot.
    const size_t BACK_LEFT_LED = 0;
    const size_t BACK_CENTER_LED = 1;
    const size_t BACK_RIGHT_LED = 2;
    const size_t FRONT_RIGHT_LED = 3;
    const size_t FRONT_CENTER_LED = 4;
    const size_t FRONT_LEFT_LED = 5;


    template <size_t ledCount>
    class TRGBLEDs
    {
        protected:
            static const PinName misoPin = p3;
            static const PinName sclkPin = p6;
            static const PinName mosiPin = NC;
            static const PinName csPin = NC;
            static const size_t  dataSize = 4 + ledCount * 4 + (ledCount + 14) / 16;

        public:
            TRGBLEDs()
            {
                m_pSPI = SharedSPI::getSharedSPI();
                memset(m_data, 0, sizeof(m_data));
                for (size_t led = 0 ; led < ledCount ; led++)
                {
                    setBrightnessInternal(led, 31);
                }
            }

            void setAutoShow(bool enableAutoShow)
            {
                m_autoShow = enableAutoShow;
            }

            void show()
            {
                m_pSPI->writeToLEDs(m_data, sizeof(m_data), m_dummy, sizeof(m_dummy));
            }

            void setBrightness(size_t led, uint8_t brightness)
            {
                assert ( led < ledCount );
                setBrightnessInternal(led, brightness);
                autoShow();
                return;
            }

            void setBrightness(uint8_t brightness)
            {
                for (size_t led = 0 ; led < ledCount ; led++)
                {
                    setBrightnessInternal(led, brightness);
                }
                autoShow();
            }

            uint8_t getBrightness(size_t led)
            {
                assert ( led < ledCount );

                return m_data[(led+1)*4] & 0x1F;
            }

            void set(size_t led, const RGB& rgb)
            {
                assert ( led < ledCount );

                m_data[4 + led*4 + 1] = rgb.b;
                m_data[4 + led*4 + 2] = rgb.g;
                m_data[4 + led*4 + 3] = rgb.r;
                autoShow();
            }

            void set(size_t led, const RGB& rgb, uint8_t brightness)
            {
                assert ( led < ledCount );

                setBrightnessInternal(led, brightness);
                set(led, rgb);
                autoShow();
            }

            RGB get(size_t led)
            {
                assert ( led < ledCount );

                return RGB(m_data[4 + led*4 + 3], m_data[4 + led*4 + 2], m_data[4 + led*4 + 1]);
            }

            void set(size_t led, const HSV& hsv, uint32_t hScale=360)
            {
                assert ( led < ledCount );

                RGB rgb = hsv2rgb(hsv.h, hsv.s, hsv.v, hScale);
                set(led, rgb);
            }

            RGB hsv2rgb(uint8_t h, uint8_t s, uint8_t v)
            {
                // Adapted from https://stackoverflow.com/a/14733008
                if (s == 0)
                {
                    return RGB(v, v, v);
                }

                uint32_t h32 = h;
                uint32_t s32 = s;
                uint32_t v32 = v;
                uint32_t region = h / 43;
                uint32_t remainder = (h - (region * 43)) * 6;
                uint8_t p = (v32 * (255 - s32)) >> 8;
                uint8_t q = (v32 * (255 - ((s32 * remainder) >> 8))) >> 8;
                uint8_t t = (v32 * (255 - ((s32 * (255 - remainder)) >> 8))) >> 8;

                switch (region)
                {
                    case 0:
                        return RGB(v, t, p);
                    case 1:
                        return RGB(q, v, p);
                    case 2:
                        return RGB(p, v, t);
                    case 3:
                        return RGB(p, q, v);
                    case 4:
                        return RGB(t, p, v);
                    default:
                        return RGB(v, p, q);
                }
            }

            void off()
            {
                for (size_t led = 0 ; led < ledCount ; led++)
                {
                    set(led, BLACK);
                }
                show();
            }

        protected:
            void setBrightnessInternal(size_t led, uint8_t brightness)
            {
                m_data[(led+1)*4] = 0xE0 | (brightness & 0x1F);
            }

            void autoShow()
            {
                if (!m_autoShow)
                {
                    return;
                }
                show();
            }

            SharedSPI* m_pSPI;
            bool       m_autoShow = true;
            char       m_data[dataSize];
            char       m_dummy[dataSize];
    };


    // Typedef for the 6 RGB LEDs actually the come stock on the 3π+ 2040 robot so that users don't have to understand
    // template parameters.
    typedef TRGBLEDs<6> RGBLEDs;

} // namespace Pololu3piPlus2040
