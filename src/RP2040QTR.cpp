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
#include "RP2040QTR.h"


namespace Pololu3piPlus2040
{
    static QTRSensors g_singletonQTR;

    QTRSensors* QTRSensors::getSharedQTR()
    {
        return &g_singletonQTR;
    }

} // namespace Pololu3piPlus2040
