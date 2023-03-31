// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#include <Pololu3piPlus2040BumpSensors.h>
#include <RP2040SIO.h>

namespace Pololu3piPlus2040
{

void BumpSensors::readRaw()
{
  BumperSensorReadings readings = pQTR->read();

  // UNDONE: Switch sensorValues to BumperSensorReadings.
  sensorValues[BumpLeft] = readings.left;
  sensorValues[BumpRight] = readings.right;
}

void BumpSensors::calibrate(uint8_t count)
{
  uint32_t sum[2] = {0, 0};

  for (uint8_t i = 0; i < count; i++)
  {
    readRaw();
    sum[BumpLeft]  += sensorValues[BumpLeft];
    sum[BumpRight] += sensorValues[BumpRight];
  }

  for (uint8_t s = BumpLeft; s <= BumpRight; s++)
  {
    baseline[s] = (sum[s] + count / 2) / count;

    // Calculate threshold to compare readings to by adding margin to baseline,
    // but make sure it is no larger than the QTR sensor timeout (i.e. if the
    // reading timed out, consider the bump sensor pressed).
    threshold[s] = baseline[s] + baseline[s] * (uint32_t)marginPercentage / 100;
    if (threshold[s] > QTRSensors::TIMEOUT) { threshold[s] = QTRSensors::TIMEOUT; }
  }
}

uint8_t BumpSensors::read()
{
  readRaw();

  uint8_t bitField = 0;
  for (uint8_t s = BumpLeft; s <= BumpRight; s++)
  {
    last[s] = pressed[s];
    pressed[s] = (sensorValues[s] >= threshold[s]);
    bitField |= pressed[s] << s;
  }
  return bitField;
}

}
