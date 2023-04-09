// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus2040BumpSensors.h

#pragma once

#include <Arduino.h>
#include "RP2040SIO.h"
#include "RP2040QTR.h"

namespace Pololu3piPlus2040
{

/// Bump sensor sides.
enum BumpSide {
  /// Right bump sensor
  BumpRight = 0,

  /// Left bump sensor
  BumpLeft  = 1
};

/// \brief Gets readings from the two bump sensors on the front of the 3pi+
/// 2040.
class BumpSensors
{
  private:
    RP2040SIO::Pin<23> emitterPin;

  public:
    BumpSensors()
    {
        pQTR = QTRSensors::getSharedQTR();

        memset(&baseline, 0, sizeof(baseline));
        memset(&threshold, 0, sizeof(threshold));
        memset(&sensorValues, 0, sizeof(sensorValues));
        memset(pressed, 0, sizeof(pressed));
        memset(last, 0, sizeof(last));
    }

    /// \brief Calibrates the bump sensors.
    ///
    /// \param count The number of times to read the sensors during calibration.
    /// The default is 50.
    ///
    /// This method reads the bump sensors a number of times for calibration.
    /// You should call it while the bump sensors are not pressed and before
    /// using them in your program.
    ///
    /// Calling this function obtains a set of baseline readings that should
    /// should represent the raw sensor readings while the bump sensors are not
    /// pressed. The library uses these baseline values and an additional margin
    /// (#marginPercentage, defined as a percentage of the baselines) to derive
    /// thresholds that the sensors must exceed to register as pressed:
    ///
    /// \f[
    /// {\text{threshold} = \text{baseline} \times \frac{100 + \text{marginPercentage}}{100}}
    /// \f]
    void calibrate(uint8_t count = 50);

    /// \brief Reads both bump sensors.
    ///
    /// \return A bit field indicating whether each bump sensor is pressed. The
    /// bits representing each sensor are defined by the ::BumpSide enum.
    ///
    /// For example, a return value of 1 (0b01 in binary) indicates:
    /// * The right bump sensor is pressed, since bit 0 (BumpRight) is set.
    /// * The left bump sensor is not pressed, since bit 1 (BumpLeft) is
    ///   cleared.
    ///
    /// Instead of checking the return value of this method, you can instead
    /// call read() and then use the functions leftChanged(), rightChanged(),
    /// leftIsPressed(), and rightIsPressed() to get information about the bump
    /// sensors.
    uint8_t read();

    /// \brief Indicates whether the left bump sensor's state has changed.
    ///
    /// \return True if the left bump sensor's state has changed between the
    /// second-to-last and the last call to read(); false otherwise.
    bool leftChanged()    { return (pressed[BumpLeft] ^ last[BumpLeft]) != 0; }


    /// \brief Indicates whether the right bump sensor's state has changed.
    ///
    /// \return True if the right bump sensor's state has changed between the
    /// second-to-last and the last call to read(); false otherwise.
    bool rightChanged()   { return (pressed[BumpRight] ^ last[BumpRight]) != 0; }

    /// \brief Indicates whether the left bump sensor is pressed.
    ///
    /// \return True if the left bump sensor was pressed during the most recent
    /// call to read(); false otherwise.
    bool leftIsPressed()  { return pressed[BumpLeft]; }

    /// \brief Indicates whether the right bump sensor is pressed.
    ///
    /// \return True if the right bump sensor was pressed during the most recent
    /// call to read(); false otherwise.
    bool rightIsPressed() { return pressed[BumpRight]; }

  private:
    /// Pointer to the QTR sensor reading singleton shared with the line sensors.
    QTRSensors* pQTR;

  public:
    /// \brief The amount, as a percentage, that will be added to the measured
    /// baseline to get the threshold.
    ///
    /// You must calibrate after changing this.
    ///
    /// \sa calibrate()
    uint16_t marginPercentage = 50;

    /// Baseline readings obtained from calibration.
    BumperSensorReadings baseline;

    /// Thresholds for bump sensor press detection.
    BumperSensorReadings threshold;

    /// Raw reflectance sensor readings for latest measurement.
    BumperSensorReadings sensorValues;

  private:
    /// Current bumper pressed state. Used together with last[] to determine state
    /// changes.
    uint8_t pressed[2];

    /// Previous bumper pressed state. Used together with pressed[] to determine
    /// state changes.
    uint8_t last[2];

    void readRaw();
};

}
