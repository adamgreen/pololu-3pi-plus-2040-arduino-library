# Pololu3piPlus2040 library

**Note:** *This is not an official library from Pololu. It is a community created port of the [Pololu3piPlus32U4 Library for Arduino](https://github.com/pololu/pololu-3pi-plus-32u4-arduino-library) to the RP2040 version of the same robot kit.*

[www.pololu.com](https://www.pololu.com/category/300/3pi-plus-2040-robot)

## Summary

<img align="right" src="https://a.pololu-files.com/picture/0J12003.300.jpg">

This is a C++ library for the Arduino IDE that helps access the on-board hardware of the [Pololu 3pi+ 2040 Robot](https://www.pololu.com/category/300/3pi-plus-2040-robot) .

The 3pi+ 2040 robot is a complete, high-performance mobile platform based on the Raspberry Pi RP2040 microcontroller.  It has integrated motor drivers, encoders, a graphical OLED display, a buzzer, buttons, line sensors, front bump sensors, an LSM6DSO accelerometer and gyro, and an LIS3MDL compass. See the [3pi+ 2040 user's guide](https://www.pololu.com/docs/0J86) for more information.

## Installing the library

* You can download this GitHub repository by clicking the `Code` button towards the top of this very page and selecting the `Download ZIP` option.<br><br>
![Copy button](pio/CopyButton.png)<br><br>
![Download ZIP](pio/DownloadZip.png)<br><br>
* Once downloaded you can follow the [Importing a .zip Library](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries#importing-a-zip-library) steps from the official Arduino documentation.

~~Use the Library Manager in version 1.8.10 or later of the Arduino software (IDE) to install this library:~~

1. ~~In the Arduino IDE, open the "Tools" menu and select "Manage Libraries...".~~
2. ~~Search for "3pi+".~~
3. ~~Click the Pololu3piPlus2040 entry in the list.~~
4. ~~Click "Install".~~
5. ~~If you see a prompt asking to install missing dependencies, click "Install all".~~

## Usage

To access most of features of this library, you just need a single include statement.  For convenience, we recommend declaring all of the objects you want to use as global variables, as shown below:

```cpp
#include <Pololu3piPlus2040.h>

OLED display;
Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
LineSensors lineSensors;
BumpSensors bumpSensors;
Motors motors;
Encoders encoders;
RGBLEDs leds;
IMU imu;
```

## Examples

Several example sketches are available that show how to use the library.  You can access them from the Arduino IDE by opening the "File" menu, selecting "Examples", and then selecting "Pololu3piPlus2040".  If you cannot find these examples, the library was probably installed incorrectly and you should retry the installation instructions above.

## Classes and functions

The contents of the library are contained in the `Pololu3piPlus2040` namespace. The main classes and functions provided by the library are listed below:

* Pololu3piPlus2040::ButtonA
* Pololu3piPlus2040::ButtonB
* Pololu3piPlus2040::ButtonC
* Pololu3piPlus2040::Buzzer
* Pololu3piPlus2040::Encoders
* Pololu3piPlus2040::OLED
* Pololu3piPlus2040::Motors
* Pololu3piPlus2040::LineSensors
* Pololu3piPlus2040::BumpSensors
* Pololu3piPlus2040::IMU
* Pololu3piPlus2040::RGBLEDs
* Pololu3piPlus2040::ledYellow()
* Pololu3piPlus2040::readBatteryMillivolts()

## Dependencies

This library also references several other Arduino libraries which are used to help implement the classes and functions above.

* [PololuOLED](https://github.com/pololu/pololu-oled-arduino)
* [Pushbutton](https://github.com/pololu/pushbutton-arduino)

## Version history
* 0.1.0 (2023-04-4): Initial release from community.
