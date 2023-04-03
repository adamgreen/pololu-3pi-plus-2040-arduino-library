// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus2040LineSensors.h

#pragma once

#include <Arduino.h>
#include "RP2040SIO.h"
#include "RP2040QTR.h"

namespace Pololu3piPlus2040
{

/// \brief Emitter behavior when taking readings.
enum class LineSensorsReadMode : uint8_t {
  /// Each reading is made without turning on the infrared (IR) emitters. The
  /// reading represents ambient light levels near the sensor.
  Off,

  /// Each reading is made with the emitters on. The reading is a measure of
  /// reflectance.
  On,

  /// Calling read() with this mode prevents it from automatically controlling
  /// the emitters: they are left in their existing states, which allows manual
  /// control of the emitters for testing and advanced use. Calibrating and
  /// obtaining calibrated readings are not supported with this mode.
  Manual
};

/// \brief Gets readings from the five reflectance sensors on the bottom of the
/// 3pi+ 2040.
///
/// The readLineBlack() and readLineWhite() methods will always return values
/// that increase from left to right, with 0 corresponding to the leftmost
/// sensor and 4000 corresponding to the rightmost sensor.
class LineSensors
{
  private:
    RP2040SIO::Pin<26> emitterPin;

public:
  /// The 3pi+ 2040 has 5 line sensors.
  static const uint8_t _sensorCount = LINE_SENSOR_COUNT;

  LineSensors()
  {
    pQTR = QTRSensors::getSharedQTR();
  }

  /// \brief Returns the timeout.
  ///
  /// \return The RC sensor timeout in microseconds.
  ///
  /// See also setTimeout().
  uint16_t getTimeout() { return QTRSensors::TIMEOUT; }

  /// \brief Reads the sensors for calibration.
  ///
  /// \param mode The emitter behavior during calibration, as a member of
  /// the ::LineSensorsReadMode enum. The default is LineSensorsReadMode::On.
  /// Manual emitter control with LineSensorsReadMode::Manual is not supported.
  ///
  /// This method reads the sensors 10 times and uses the results for
  /// calibration. The sensor values are not returned; instead, the maximum
  /// and minimum values found over time are stored in #calibrationOn and/or
  /// #calibrationOff for use by the readCalibrated() method.
  ///
  /// If the storage for the calibration values has not been initialized,
  /// this function will (re)allocate the arrays and initialize the maximum
  /// and minimum values to 0 and the maximum possible sensor reading,
  /// respectively, so that the very first calibration sensor reading will
  /// update both of them.
  ///
  /// Note that the `minimum` and `maximum` pointers in the CalibrationData
  /// structs will point to arrays of length five,
  /// and they will only be allocated when calibrate() is
  /// called. If you only calibrate with the emitters on, the calibration
  /// arrays that hold the off values will not be allocated (and vice versa).
  ///
  /// \if usage
  ///   See \ref md_usage for more information and example code.
  /// \endif
  void calibrate(LineSensorsReadMode mode = LineSensorsReadMode::On);

  /// \brief Resets all calibration that has been done.
  void resetCalibration();

  /// \brief Reads the raw sensor values into an array.
  ///
  /// \param[out] sensorValues A pointer to an array in which to store the
  /// raw sensor readings. There **MUST** be space in the array for five
  /// readings.
  ///
  /// \param mode The emitter behavior during the read, as a member of the
  /// ::LineSensorsReadMode enum. The default is LineSensorsReadMode::On.
  ///
  /// Example usage:
  /// ~~~{.cpp}
  /// uint16_t sensorValues[5];
  /// lineSensors.read(sensorValues);
  /// ~~~
  ///
  /// The values returned are a measure of the reflectance in abstract units,
  /// with higher values corresponding to lower reflectance (e.g. a black
  /// surface or a void).
  ///
  /// Analog sensors will return a raw value between 0 and 1023 (like
  /// Arduino's `analogRead()` function).
  ///
  /// RC sensors will return a raw value in microseconds between 0 and the
  /// timeout setting configured with setTimeout() (the default timeout is
  /// 2500 &micro;s).
  ///
  /// \if usage
  ///   See \ref md_usage for more information and example code.
  /// \endif
  void read(LineSensorReadings& sensorValues, LineSensorsReadMode mode = LineSensorsReadMode::On);

  /// \brief Reads the sensors and provides calibrated values between 0 and
  /// 1000.
  ///
  /// \param[out] sensorValues A pointer to an array in which to store the
  /// calibrated sensor readings.  There **MUST** be space in the array for
  /// all five values.
  ///
  /// \param mode The emitter behavior during the read, as a member of the
  /// ::LineSensorsReadMode enum. The default is LineSensorsReadMode::On. Manual
  /// emitter control with LineSensorsReadMode::Manual is not supported.
  ///
  /// 0 corresponds to the minimum value stored in #calibrationOn or
  /// #calibrationOff, depending on \p mode, and 1000 corresponds to the
  /// maximum value. Calibration values are typically obtained by calling
  /// calibrate(), and they are stored separately for each sensor, so that
  /// differences in the sensors are accounted for automatically.
  ///
  /// \if usage
  ///   See \ref md_usage for more information and example code.
  /// \endif
  void readCalibrated(LineSensorReadings& sensorValues, LineSensorsReadMode mode = LineSensorsReadMode::On);

  /// \brief Reads the sensors, provides calibrated values, and returns an
  /// estimated black line position.
  ///
  /// \param[out] sensorValues A pointer to an array in which to store the
  /// calibrated sensor readings.  There **MUST** be space in the array for
  /// five values.
  ///
  /// \param mode The emitter behavior during the read, as a member of the
  /// ::LineSensorsReadMode enum. The default is LineSensorsReadMode::On. Manual
  /// emitter control with LineSensorsReadMode::Manual is not supported.
  ///
  /// \return An estimate of the position of a black line under the sensors.
  ///
  /// The estimate is made using a weighted average of the sensor indices
  /// multiplied by 1000, so that a return value of 0 indicates that the line
  /// is directly below sensor 0, a return value of 1000 indicates that the
  /// line is directly below sensor 1, 2000 indicates that it's below sensor
  /// 2000, etc. Intermediate values indicate that the line is between two
  /// sensors. The formula is (where \f$v_0\f$ represents the value from the
  /// first sensor):
  ///
  /// \f[
  /// {(0 \times v_0) + (1000 \times v_1) + (2000 \times v_2) + \cdots
  /// \over
  /// v_0 + v_1 + v_2 + \cdots}
  /// \f]
  ///
  /// As long as your sensors aren’t spaced too far apart relative to the
  /// line, this returned value is designed to be monotonic, which makes it
  /// great for use in closed-loop PID control. Additionally, this method
  /// remembers where it last saw the line, so if you ever lose the line to
  /// the left or the right, its line position will continue to indicate the
  /// direction you need to go to reacquire the line. For example, if sensor
  /// 4 is your rightmost sensor and you end up completely off the line to
  /// the left, this function will continue to return 4000.
  ///
  /// This function is intended to detect a black (or dark-colored) line on a
  /// white (or light-colored) background. For a white line, see
  /// readLineWhite().
  ///
  /// \if usage
  ///   See \ref md_usage for more information and example code.
  /// \endif
  uint16_t readLineBlack(LineSensorReadings& sensorValues, LineSensorsReadMode mode = LineSensorsReadMode::On)
  {
    return readLinePrivate(sensorValues, mode, false);
  }

  /// \brief Reads the sensors, provides calibrated values, and returns an
  /// estimated white line position.
  ///
  /// \param[out] sensorValues A pointer to an array in which to store the
  /// calibrated sensor readings.  There **MUST** be space in the array for
  /// five values.
  ///
  /// \param mode The emitter behavior during the read, as a member of the
  /// ::LineSensorsReadMode enum. The default is LineSensorsReadMode::On. Manual
  /// emitter control with LineSensorsReadMode::Manual is not supported.
  ///
  /// \return An estimate of the position of a white line under the sensors.
  ///
  /// This function is intended to detect a white (or light-colored) line on
  /// a black (or dark-colored) background. For a black line, see
  /// readLineBlack().
  ///
  /// \if usage
  ///   See \ref md_usage for more information and example code.
  /// \endif
  uint16_t readLineWhite(LineSensorReadings& sensorValues, LineSensorsReadMode mode = LineSensorsReadMode::On)
  {
    return readLinePrivate(sensorValues, mode, true);
  }


  /// \brief Stores sensor calibration data.
  ///
  /// See calibrate() and readCalibrated() for details.
  struct CalibrationData
  {
    /// Lowest readings seen during calibration.
    LineSensorReadings minimum;
    /// Highest readings seen during calibration.
    LineSensorReadings maximum;
    /// Has calibration been run to intialize this structure.
    bool initialized;

    CalibrationData()
    {
        init();
    }

    void init()
    {
      for (uint8_t i = 0; i < _sensorCount; i++)
      {
        maximum.vals[i] = 0;
        minimum.vals[i] = QTRSensors::TIMEOUT;
        initialized = false;
      }
    }
  };

  /// \name Calibration data
  ///
  /// See calibrate() and readCalibrated() for details.
  ///
  /// These variables are made public so that you can use them for your own
  /// calculations and do things like saving the values to EEPROM, performing
  /// sanity checking, etc.
  /// \{

  /// Data from calibrating with emitters on.
  CalibrationData calibrationOn;

  /// Data from calibrating with emitters off.
  CalibrationData calibrationOff;

  /// \}

  /// \brief Turns the IR LEDs on.
  void emittersOn()
  {
    emitterPin.setOutputHigh();
  }

  /// \brief Turns the IR LEDs off.
  void emittersOff()
  {
    emitterPin.setInput();
  }

private:

  // Handles the actual calibration, including (re)allocating and
  // initializing the storage for the calibration values if necessary.
  void calibrateOnOrOff(CalibrationData & calibration, LineSensorsReadMode mode);

  void readCalibratedPrivate(CalibrationData& calibration, LineSensorReadings& sensorValues, LineSensorsReadMode mode);

  void readPrivate(LineSensorReadings& sensorValues);

  uint16_t readLinePrivate(LineSensorReadings& sensorValues, LineSensorsReadMode mode, bool invertReadings);

  void swapUINT16(uint16_t* p1, uint16_t* p2)
  {
    uint16_t temp = *p1;
    *p1 = *p2;
    *p2 = temp;
  }
  /// Pointer to the QTR sensor reading singleton shared with the bumper sensors.
  QTRSensors* pQTR;

  uint16_t _timeout = QTRSensors::TIMEOUT;
  uint16_t _maxValue = QTRSensors::TIMEOUT; // the maximum value returned by readPrivate()
  uint16_t _lastPosition = 0;
};

}
