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
// Class to share the SPI peripheral between the OLED display addressable RGB
// LEDs on the Pololu 3π+ 2040 robot.
#include "RP2040SPI.h"

namespace Pololu3piPlus2040
{
    // The singleton SPI object used by both the OLED display and addressable RGB LEDs.
    static SharedSPI g_singletonSPI;

    SharedSPI* SharedSPI::getSharedSPI()
    {
        return &g_singletonSPI;
    }
} // namespace Pololu3piPlus2040
