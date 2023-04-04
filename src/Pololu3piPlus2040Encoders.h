// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus2040Encoders.h

#pragma once

#include <stdint.h>

namespace Pololu3piPlus2040
{

/// \brief Reads counts from the encoders on the 3pi+ 2040.
///
/// This class allows you to read counts from the encoders on the 3pi+,
/// which lets you tell how much each motor has turned and in what direction.
///
/// The encoders are monitored in the background using the RP2040's PIO and
/// DMA peripherals, so your code can perform other tasks without missing
/// encoder counts.
class Encoders
{
private:
    // Each pair of encoder pins must be consecutive pin numbers to work in the PIO.
    static const uint32_t rightEncoderAPin = 8;
    static const uint32_t leftEncoderAPin = 12;

public:

    /// \brief Flips the direction of the encoders.
    ///
    /// This is useful if you have to flip the direction of the motors
    /// due to a non-standard gearbox.
    ///
    /// \param flip If true, the direction of counting will be
    /// reversed relative to the standard 3pi+ 32U4.
    static void flipEncoders(bool flip);

    /// \brief Initializes the encoders (called automatically).
    ///
    /// This function initializes the encoders if they have not been initialized
    /// already and starts listening for counts.  This function is called
    /// automatically whenever you call any other function in this class, so you
    /// should not normally need to call it in your code.
    static void init()
    {
        static bool initialized = 0;
        if (!initialized)
        {
            initialized = true;
            init2();
        }
    }

    /// \brief Returns the number of counts that have been detected from the
    /// left-side encoder.
    ///
    /// The count starts at 0.  Positive counts correspond to forward movement
    /// of the left side of the 3pi+, while negative counts correspond to
    /// backwards movement.
    ///
    /// The count is returned as a signed 32-bit integer.  When the count goes
    /// over 2147483647, it will overflow down to -2147483648.  When the count
    //  goes below -2147483648, it will overflow up to 2147483647.
    static int32_t getCountsLeft();

    /// \brief Returns the number of counts that have been detected from the
    /// right-side encoder.
    ///
    /// \sa getCountsLeft()
    static int32_t getCountsRight();

    /// \brief Returns the number of counts that have been detected from the
    /// left-side encoder and clears the counts.
    ///
    /// This function is just like getCountsLeft() except it also clears the
    /// counts before returning.  If you call this frequently enough, you will
    /// not have to worry about the count overflowing.
    static int32_t getCountsAndResetLeft();

    /// \brief Returns the number of counts that have been detected from the
    /// left-side encoder and clears the counts.
    ///
    /// \sa getCountsAndResetLeft()
    static int32_t getCountsAndResetRight();

private:

    static void init2();
};

}
