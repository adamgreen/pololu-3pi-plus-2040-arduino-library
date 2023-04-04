// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus2040IMU.h

#pragma once
#include <Wire.h>

/// \anchor device_addresses
///
/// \name Device Addresses
/// \{
#define LSM6DSO_ADDR 0b1101011
#define LIS3MDL_ADDR 0b0011110
/// \}

///  \anchor register_addresses
///
/// \name Register Addresses
/// \{
#define LSM6DSO_REG_WHO_AM_I   0x0F
#define LSM6DSO_REG_CTRL1_XL   0x10
#define LSM6DSO_REG_CTRL2_G    0x11
#define LSM6DSO_REG_CTRL3_C    0x12
#define LSM6DSO_REG_STATUS_REG 0x1E
#define LSM6DSO_REG_OUTX_L_G   0x22
#define LSM6DSO_REG_OUTX_L_XL  0x28

#define LIS3MDL_REG_WHO_AM_I   0x0F
#define LIS3MDL_REG_CTRL_REG1  0x20
#define LIS3MDL_REG_CTRL_REG2  0x21
#define LIS3MDL_REG_CTRL_REG3  0x22
#define LIS3MDL_REG_CTRL_REG4  0x23
#define LIS3MDL_REG_STATUS_REG 0x27
#define LIS3MDL_REG_OUT_X_L    0x28
/// \}

namespace Pololu3piPlus2040
{

/// \brief The type of the inertial sensors.
enum class IMUType : uint8_t {
  /// Unknown or unrecognized
  Unknown,
  /// LSM6DSO gyro + accelerometer, LIS3MDL magnetometer
  LSM6DSO_LIS3MDL
};

/// \brief Interfaces with the inertial sensors on the 3pi+ 2040.
///
/// This class allows you to configure and get readings from the I2C sensors
/// that make up the 3pi+ 2040's inertial measurement unit (IMU): gyro,
/// accelerometer, and magnetometer.
///
/// You must call `Wire.start()` before using any of this library's functions
/// that access the sensors.
class IMU
{
public:

  /// Represents a 3-dimensional vector with x, y, and z components.
  template <typename T> struct vector
  {
    T x, y, z;
  };

  /// Raw accelerometer readings.
  vector<int16_t> a = {0, 0, 0};

  /// Raw gyro readings.
  vector<int16_t> g = {0, 0, 0};

  /// Raw magnetometer readings.
  vector<int16_t> m = {0, 0, 0};

  /// \brief Returns 0 if the last I2C communication with the IMU was
  /// successful, or a non-zero status code if there was an error.
  uint8_t getLastError() { return lastError; }

  /// \brief Initializes the inertial sensors and detects their type.
  ///
  /// \return True if the sensor type was detected successfully; false otherwise.
  bool init();

  /// \brief Returns the type of the inertial sensors on the 3pi+ 2040.
  ///
  /// \return The sensor type as a member of the IMUType enum. If the
  /// type is not known (e.g. if init() has not been called yet), this will be
  /// IMUType::Unknown.
  IMUType getType() { return type; }

  /// \brief Enables all of the inertial sensors with a default configuration.
  void enableDefault();

  /// \brief Configures the sensors with settings optimized for turn sensing.
  void configureForTurnSensing();

  /// \brief Configures the sensors with settings optimized for the FaceUphill
  /// example program.
  void configureForFaceUphill();

  /// \brief Configures the sensors with settings optimized for determining a
  /// compass heading with the magnetometer.
  void configureForCompassHeading();

  /// \brief Writes an 8-bit sensor register.
  ///
  /// \param addr Device address.
  /// \param reg Register address.
  /// \param value The 8-bit register value to be written.
  void writeReg(uint8_t addr, uint8_t reg, uint8_t value)
  {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(value);
    lastError = Wire.endTransmission();
  }

  /// \brief Reads an 8-bit sensor register.
  ///
  /// \param addr Device address.
  /// \param reg Register address.
  ///
  /// \return The 8-bit register value read from the device.
  uint8_t readReg(uint8_t addr, uint8_t reg)
  {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    lastError = Wire.endTransmission();
    if (lastError) { return 0; }

    uint8_t byteCount = Wire.requestFrom(addr, (uint8_t)1);
    if (byteCount != 1)
    {
      lastError = 50;
      return 0;
    }
    return Wire.read();
  }

  /// \brief Takes a reading from the accelerometer and makes the measurements
  /// available in #a.
  void readAcc();

  /// \brief Takes a reading from the gyro and makes the measurements available
  /// in #g.
  void readGyro();

  /// \brief Takes a reading from the magnetometer and makes the measurements
  /// available in #m.
  void readMag();

  /// \brief Takes a reading from all three sensors (accelerometer, gyro, and
  /// magnetometer) and makes their measurements available in the respective
  /// vectors.
  void read();

  /// \brief Indicates whether the accelerometer has new measurement data ready.
  ///
  /// \return True if there is new accelerometer data available; false
  /// otherwise.
  bool accDataReady();

  /// \brief Indicates whether the gyro has new measurement data ready.
  ///
  /// \return True if there is new gyro data available; false otherwise.
  bool gyroDataReady();

  /// \brief Indicates whether the magnetometer has new measurement data ready.
  ///
  /// \return True if there is new magnetometer data available; false otherwise.
  bool magDataReady();

private:

  uint8_t lastError = 0;
  IMUType type = IMUType::Unknown;

  int16_t testReg(uint8_t addr, uint8_t reg)
  {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0)
    {
      return -1;
    }

    uint8_t byteCount = Wire.requestFrom(addr, (uint8_t)1);
    if (byteCount != 1)
    {
      return -1;
    }
    return Wire.read();
  }

  void readAxes16Bit(uint8_t addr, uint8_t firstReg, vector<int16_t> & v)
  {
    Wire.beginTransmission(addr);
    Wire.write(firstReg);
    lastError = Wire.endTransmission();
    if (lastError) { return; }

    uint8_t byteCount = (Wire.requestFrom(addr, (uint8_t)6));
    if (byteCount != 6)
    {
      lastError = 50;
      return;
    }
    uint8_t xl = Wire.read();
    uint8_t xh = Wire.read();
    uint8_t yl = Wire.read();
    uint8_t yh = Wire.read();
    uint8_t zl = Wire.read();
    uint8_t zh = Wire.read();

    // combine high and low bytes
    v.x = (int16_t)(xh << 8 | xl);
    v.y = (int16_t)(yh << 8 | yl);
    v.z = (int16_t)(zh << 8 | zl);
  }
};

}
