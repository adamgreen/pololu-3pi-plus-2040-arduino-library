/* Copyright 2021 Adam Green (https://github.com/adamgreen/)

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
// Sample code that makes sure that PIO files were successfully assembled.
#include <stdio.h>
#include <hardware/pio.h>
#include <RP2040QTR.pio.h>
#include <RP2040Encoders.pio.h>


int main(void)
{
    pio_add_program(pio0, &RP2040QTR_program);
    pio_add_program(pio1, &RP2040Encoders_program);

    return 0;
}
