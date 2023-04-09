// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus2040Motors.h

#pragma once

#include <stdint.h>

namespace Pololu3piPlus2040
{

/// \brief Controls motor speed and direction on the 3pi+ 2040.
class Motors
{
  public:
    /// \brief Flips the direction of the left motor.
    ///
    /// You can call this function with an argument of \c true if the left motor
    /// of your 3pi+ was not wired in the standard way and you want a
    /// positive speed argument to correspond to forward movement.
    ///
    /// \param flip If true, then positive motor speeds will correspond to the
    /// direction pin being high.  If false, then positive motor speeds will
    /// correspond to the direction pin being low.
    ///
    static inline void flipLeftMotor(bool flip)
    {
        motors_flip_left(flip);
    }

    /// \brief Flips the direction of the right motor.
    ///
    /// You can call this function with an argument of \c true if the right
    /// motor of your 3pi+ was not wired in the standard way and you want a
    /// positive speed argument to correspond to forward movement.
    ///
    /// \param flip If true, then positive motor speeds will correspond to the
    /// direction pin being high.  If false, then positive motor speeds will
    /// correspond to the direction pin being low.
    static inline void flipRightMotor(bool flip)
    {
        motors_flip_right(flip);
    }

    /// \brief Sets the speed for the left motor.
    ///
    /// \param speed A number from -400 to 400 representing the speed and
    /// direction of the left motor.  Values of -400 or less result in full
    /// speed reverse, and values of 400 or more result in full speed forward.
    static inline void setLeftSpeed(int16_t speed)
    {
        init();
        motors_set_left_speed(mapSpeed(speed));
    }

    /// \brief Sets the speed for the right motor.
    ///
    /// \param speed A number from -400 to 400 representing the speed and
    /// direction of the right motor. Values of -400 or less result in full
    /// speed reverse, and values of 400 or more result in full speed forward.
    static inline void setRightSpeed(int16_t speed)
    {
        init();
        motors_set_right_speed(mapSpeed(speed));
    }

    /// \brief Sets the speeds for both motors.
    ///
    /// \param leftSpeed A number from -400 to 400 representing the speed and
    /// direction of the right motor. Values of -400 or less result in full
    /// speed reverse, and values of 400 or more result in full speed forward.
    /// \param rightSpeed A number from -400 to 400 representing the speed and
    /// direction of the right motor. Values of -400 or less result in full
    /// speed reverse, and values of 400 or more result in full speed forward.
    static inline void setSpeeds(int16_t leftSpeed, int16_t rightSpeed)
    {
        init();
        motors_set_speeds(mapSpeed(leftSpeed), mapSpeed(rightSpeed));
    }

  private:

    // Maps the existing Arduino library speed limits of -400 and 400 to the C
    // SDK limits of -6000 and 6000.
    static inline int32_t mapSpeed(int16_t speed)
    {
      return (int32_t)map(speed, -400, 400, -6000, 6000);
    }

    static inline void init()
    {
        static bool initialized = false;

        if (!initialized)
        {
            initialized = true;
            motors_init();
        }
    }
};

}
