// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#include "Pololu3piPlus2040BumpSensors.h"
#include "RP2040SIO.h"

namespace Pololu3piPlus2040
{

void BumpSensors::readRaw()
{
  emitterPin.setOutputHigh();
  QTRSensorReadings readings = pQTR->read();
  emitterPin.setInput();

  sensorValues = readings.bumperReadings;
}

void BumpSensors::calibrate(uint8_t count)
{
  BumperSensorReadings sum = { 0, 0 };

  for (uint8_t i = 0; i < count; i++)
  {
    readRaw();
    sum.left  += sensorValues.left;
    sum.right += sensorValues.right;
  }

  for (uint8_t s = 0; s < sizeof(sum.vals)/sizeof(sum.vals[0]); s++)
  {
    baseline.vals[s] = (sum.vals[s] + count / 2) / count;

    // Calculate threshold to compare readings to by adding margin to baseline,
    // but make sure it is no larger than the QTR sensor timeout (i.e. if the
    // reading timed out, consider the bump sensor pressed).
    threshold.vals[s] = baseline.vals[s] + baseline.vals[s] * (uint32_t)marginPercentage / 100;
    if (threshold.vals[s] > QTRSensors::TIMEOUT)
    {
      threshold.vals[s] = QTRSensors::TIMEOUT;
    }
  }
}

uint8_t BumpSensors::read()
{
  readRaw();

  uint8_t bitField = 0;
  for (uint8_t s = 0; s < sizeof(pressed)/sizeof(pressed[0]); s++)
  {
    last[s] = pressed[s];
    pressed[s] = (sensorValues.vals[s] >= threshold.vals[s]);
    bitField |= pressed[s] << s;
  }
  return bitField;
}

}
