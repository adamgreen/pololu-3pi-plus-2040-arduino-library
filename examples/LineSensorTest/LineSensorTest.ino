// This example shows how to read the raw values from the five line sensors on
// the 3pi+ 2040.  This example is useful if you are having trouble with the
// sensors or just want to characterize their behavior.
//
// The raw (uncalibrated) values are reported to the serial monitor.  You can
// press the "C" button to toggle whether the IR emitters are on during the
// reading.

#include <Pololu3piPlus2040.h>

ButtonC buttonC;
LineSensors lineSensors;
LineSensorReadings sensorValues;

bool useEmitters = true;

void setup()
{

}

// Prints a line with all the sensor readings to the serial
// monitor.
void printReadingsToSerial()
{
  char buffer[80];
  sprintf(buffer, "%4d %4d %4d %4d %4d %c\n",
    sensorValues.vals[0],
    sensorValues.vals[1],
    sensorValues.vals[2],
    sensorValues.vals[3],
    sensorValues.vals[4],
    useEmitters ? 'E' : 'e'
  );
  Serial.print(buffer);
}

void loop()
{
  static uint16_t lastSampleTime = 0;

  if ((uint16_t)(millis() - lastSampleTime) >= 100)
  {
    lastSampleTime = millis();

    // Read the line sensors.
    lineSensors.read(sensorValues, useEmitters ? LineSensorsReadMode::On : LineSensorsReadMode::Off);

    // Send the results to the LCD and to the serial monitor.
    printReadingsToSerial();
  }

  // If button C is pressed, toggle the state of the emitters.
  if (buttonC.getSingleDebouncedPress())
  {
    useEmitters = !useEmitters;
  }
}
