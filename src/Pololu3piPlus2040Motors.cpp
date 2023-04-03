// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#include <mbed.h>
#include "Pololu3piPlus2040Motors.h"
#include "RP2040SIO.h"

namespace Pololu3piPlus2040
{
// Configure the direction pins to be output with a default value of 0.
static RP2040SIO::Pin<10> rightDirectionPin(true, false, false, false);
static RP2040SIO::Pin<11> leftDirectionPin(true, false, false, false);
static mbed::PwmOut rightPWM(p14);
static mbed::PwmOut leftPWM(p15);
static bool flipLeft = false;
static bool flipRight = false;

// PWM frequency of 20kHz.
const uint32_t pwmFrequency = 20000;
const uint32_t pwmPeriod_us = 10000000 / pwmFrequency;

// initialize timer1 to generate the proper PWM outputs to the motor drivers
void Motors::init2()
{
    rightPWM.period_us(pwmPeriod_us);
    leftPWM.period_us(pwmPeriod_us);
    rightPWM.pulsewidth_us(0);
    leftPWM.pulsewidth_us(0);
}

void Motors::flipLeftMotor(bool flip)
{
    flipLeft = flip;
}

void Motors::flipRightMotor(bool flip)
{
    flipRight = flip;
}

void Motors::setLeftSpeed(int16_t speed)
{
    init();

    bool reverse = 0;

    if (speed < 0)
    {
        speed = -speed; // Make speed a positive quantity.
        reverse = 1;    // Preserve the direction.
    }
    if (speed > 400)
    {
        speed = 400;
    }

    leftPWM.pulsewidth_us(map(speed, 0, 400, 0, pwmPeriod_us/2));
    leftDirectionPin.setOutput(reverse ^ flipLeft);
}

void Motors::setRightSpeed(int16_t speed)
{
    init();

    bool reverse = 0;

    if (speed < 0)
    {
        speed = -speed;  // Make speed a positive quantity.
        reverse = 1;     // Preserve the direction.
    }
    if (speed > 400)
    {
        speed = 400;
    }

    rightPWM.pulsewidth_us(map(speed, 0, 400, 0, pwmPeriod_us/2));
    rightDirectionPin.setOutput(reverse ^ flipRight);
}

void Motors::setSpeeds(int16_t leftSpeed, int16_t rightSpeed)
{
    setLeftSpeed(leftSpeed);
    setRightSpeed(rightSpeed);
}

}
