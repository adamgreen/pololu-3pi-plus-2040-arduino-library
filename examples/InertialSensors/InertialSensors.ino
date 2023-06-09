/* This example reads the raw values from the accelerometer,
magnetometer, and gyro on the 3pi+ 2040, and prints those raw
values to the serial monitor.

The accelerometer readings can be converted to units of g using
the sensitivity specified in the LSM6DO datasheet.
The default full-scale (FS) setting is +/- 2 g, so the conversion
factor is 0.061 mg/LSB (least-significant bit).  A raw reading of
16384 would correspond to 1 g.

The gyro readings can be converted to degrees per second (dps)
using the sensitivity specified in the LSM6DO
datasheet.  The default sensitivity is 8.75 mdps/LSB (least-
significant bit).  A raw reading of 10285 would correspond to
90 dps.

The magnetometer readings are more difficult to interpret and
will usually require calibration. */

#include <Wire.h>
#include <Pololu3piPlus2040.h>

IMU imu;

char report[120];

void setup()
{
  Wire.begin();

  if (!imu.init())
  {
    // Failed to detect the compass.
    ledYellow(1);
    while(1)
    {
      Serial.println("Failed to initialize IMU sensors.");
      delay(100);
    }
  }

  imu.enableDefault();
}

void loop()
{
  imu.read();

  snprintf_P(report, sizeof(report),
    PSTR("A: %6d %6d %6d    M: %6d %6d %6d    G: %6d %6d %6d"),
    imu.a.x, imu.a.y, imu.a.z,
    imu.m.x, imu.m.y, imu.m.z,
    imu.g.x, imu.g.y, imu.g.z);
  Serial.println(report);

  delay(100);
}
