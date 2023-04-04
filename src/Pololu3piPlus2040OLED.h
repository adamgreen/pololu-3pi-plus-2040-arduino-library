// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#pragma once

#include <Arduino.h>
#include <PololuSH1106Main.h>
#include "RP2040SIO.h"


namespace Pololu3piPlus2040
{

/// @brief Low-level functions for writing data to the SH1106 OLED on the
/// Pololu 3pi+ 2040 robot.
class OLEDCore
{
private:
  // Pin assignments
  static const PinName mosiPin = p3;
  static const PinName sclkPin = p2;
  static const PinName misoPin = NC;
  static const PinName csPin = NC;
  RP2040SIO::Pin<0> dcPin;
  RP2040SIO::Pin<1> resetPin;

  SharedSPI* m_pSPI;

  char   buffer[132];
  size_t bufferIndex = 0;

public:
  OLEDCore() :
    // DC pin is shared with switch C which requires pull-up so might as well turn it on here as well.
    dcPin(false, false, true, false)
  {
    m_pSPI = SharedSPI::getSharedSPI();
  }

  void initPins()
  {
  }

  void reset()
  {
    resetPin.setOutputLow();
    delayMicroseconds(10);
    resetPin.setOutputHigh();
    delayMicroseconds(10);
    bufferIndex = 0;
  }

  void sh1106TransferStart()
  {
    assert ( bufferIndex == 0 );
  }

  void sh1106TransferEnd()
  {
    flushData();
  }

  void sh1106CommandMode()
  {
    flushData();
    dcPin.setOutputLow();
  }

  void sh1106DataMode()
  {
    flushData();
    dcPin.setOutputHigh();
  }

  void sh1106Write(uint8_t d)
  {
    assert ( bufferIndex < sizeof(buffer) );
    buffer[bufferIndex++] = d;
  }

private:
  void flushData()
  {
    if (bufferIndex == 0)
    {
        return;
    }
    m_pSPI->writeToDisplay(buffer, bufferIndex, buffer, bufferIndex);
    bufferIndex = 0;
  }
};

/// @brief Makes it easy to show text and graphics on the SH1106 OLED of
/// the Pololu 3pi+ 2040 robot.
///
/// This class inherits from the PololuSH1106Main class in the PololuOLED
/// library, which provides almost all of its functionality.  See the
/// [PololuOLED library documentation](https://pololu.github.io/pololu-oled-arduino/)
/// for more information about how to use this class.
///
/// This class also inherits from the Arduino Print class
/// (via PololuSH1106Main), so you can call the `print()` function on it with a
/// variety of arguments.  See the
/// [Arduino print() documentation](http://arduino.cc/en/Serial/Print) for
/// more information.
class OLED : public PololuSH1106Main<OLEDCore>
{
};

}
