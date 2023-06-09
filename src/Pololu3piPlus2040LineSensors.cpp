// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#include "Pololu3piPlus2040LineSensors.h"

namespace Pololu3piPlus2040
{

void LineSensors::resetCalibration()
{
  calibrationOn.init();
  calibrationOff.init();
}

void LineSensors::calibrate(LineSensorsReadMode mode)
{
  switch (mode)
  {
    case LineSensorsReadMode::On:
      return calibrateOnOrOff(calibrationOn, LineSensorsReadMode::On);
    case LineSensorsReadMode::Off:
      return calibrateOnOrOff(calibrationOff, LineSensorsReadMode::Off);
    default:
      // manual emitter control is not supported
      return;
  }
}

void LineSensors::calibrateOnOrOff(CalibrationData & calibration, LineSensorsReadMode mode)
{
  LineSensorReadings maxSensorValues;
  LineSensorReadings minSensorValues;

  for (uint8_t j = 0; j < 10; j++)
  {
    read(mode);

    for (uint8_t i = 0; i < _sensorCount; i++)
    {
      // set the max we found THIS time
      if (j == 0 || rawSensorValues[i] > maxSensorValues.vals[i])
      {
        maxSensorValues.vals[i] = rawSensorValues[i];
      }

      // set the min we found THIS time
      if (j == 0 || rawSensorValues[i] < minSensorValues.vals[i])
      {
        minSensorValues.vals[i] = rawSensorValues[i];
      }
    }
  }

  // record the min and max calibration values
  for (uint8_t i = 0; i < _sensorCount; i++)
  {
    // Update maximum only if the min of 10 readings was still higher than it
    // (we got 10 readings in a row higher than the existing maximum).
    if (minSensorValues.vals[i] > calibration.maximum.vals[i])
    {
      calibration.maximum.vals[i] = minSensorValues.vals[i];
    }

    // Update minimum only if the max of 10 readings was still lower than it
    // (we got 10 readings in a row lower than the existing minimum).
    if (maxSensorValues.vals[i] < calibration.minimum.vals[i])
    {
      calibration.minimum.vals[i] = maxSensorValues.vals[i];
    }
  }
  calibration.initialized = true;
}

void LineSensors::readCalibratedPrivate(LineSensorsReadMode mode)
{
  switch (mode)
  {
    case LineSensorsReadMode::On:
        return readCalibratedPrivate(calibrationOn, mode);
    case LineSensorsReadMode::Off:
        return readCalibratedPrivate(calibrationOff, mode);
    default:
        // manual emitter control is not supported
        return;
  }
}

void LineSensors::readCalibratedPrivate(CalibrationData& calibration, LineSensorsReadMode mode)
{
  // if not calibrated, do nothing
  if (!calibration.initialized)
  {
    return;
  }

  // read the needed values
  readPrivate(mode);

  for (uint8_t i = 0; i < _sensorCount; i++)
  {
    uint16_t calmin = calibration.minimum.vals[i];
    uint16_t calmax = calibration.maximum.vals[i];
    uint16_t denominator = calmax - calmin;
    int16_t  value = 0;

    if (denominator != 0)
    {
      value = (((int32_t)rawSensorValues[i]) - calmin) * 1000 / denominator;
    }

    if (value < 0)
    {
      value = 0;
    }
    else if (value > 1000)
    {
      value = 1000;
    }

    calibratedSensorValues[i] = value;
  }
}

uint16_t LineSensors::readLinePrivate(LineSensorsReadMode mode, bool invertReadings)
{
  bool onLine = false;
  uint32_t avg = 0; // this is for the weighted total
  uint16_t sum = 0; // this is for the denominator, which is <= 64000

  // manual emitter control is not supported
  if (mode == LineSensorsReadMode::Manual) { return 0; }

  readCalibratedPrivate(mode);

  for (uint8_t i = 0; i < _sensorCount; i++)
  {
    uint16_t value = calibratedSensorValues[i];
    if (invertReadings) { value = 1000 - value; }

    // keep track of whether we see the line at all
    if (value > 200) { onLine = true; }

    // only average in values that are above a noise threshold
    if (value > 50)
    {
      avg += (uint32_t)value * (i * 1000);
      sum += value;
    }
  }

  if (!onLine)
  {
    // If it last read to the left of center, return 0.
    if (_lastPosition < (_sensorCount - 1) * 1000 / 2)
    {
      return 0;
    }
    // If it last read to the right of center, return the max.
    else
    {
      return (_sensorCount - 1) * 1000;
    }
  }

  _lastPosition = avg / sum;
  return _lastPosition;
}

void LineSensors::readPrivate(LineSensorsReadMode mode)
{
  switch (mode)
  {
    case LineSensorsReadMode::Off:
      emittersOff();
      readPrivate();
      return;

    case LineSensorsReadMode::Manual:
      readPrivate();
      return;

    case LineSensorsReadMode::On:
      emittersOn();
      readPrivate();
      emittersOff();
      return;

    default: // invalid - do nothing
      return;
  }
}

void LineSensors::readPrivate()
{
  QTRSensorReadings readings = pQTR->read();

  // Reverse sensor reading order to match 32U4 version of the robot as it is copied into rawSensorValues array.
  rawSensorValues[0] = readings.lineReadings.vals[4];
  rawSensorValues[1] = readings.lineReadings.vals[3];
  rawSensorValues[2] = readings.lineReadings.vals[2];
  rawSensorValues[3] = readings.lineReadings.vals[1];
  rawSensorValues[4] = readings.lineReadings.vals[0];
}

}
