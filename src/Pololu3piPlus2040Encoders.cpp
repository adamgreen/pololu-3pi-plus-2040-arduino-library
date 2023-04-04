// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#include "Pololu3piPlus2040Encoders.h"
#include "RP2040Encoders.h"

namespace Pololu3piPlus2040
{

// Whether encoder count direction should be flipped.
static bool g_flip = false;

// This is used to reset the counters.
static int32_t g_leftReset = 0;
static int32_t g_rightReset = 0;

// PIO state machine indices used for each of the encoders.
static int32_t g_rightIndex = -1;
static int32_t g_leftIndex = -1;

static RP2040Encoders g_encoders;


void Encoders::init2()
{
    // Load the PIO program.
    bool result = g_encoders.init();
    assert ( result );

    // Enable pull-ups on the encoder pins.
    gpio_set_pulls(rightEncoderAPin, true, false);
    gpio_set_pulls(rightEncoderAPin+1, true, false);
    gpio_set_pulls(leftEncoderAPin, true, false);
    gpio_set_pulls(leftEncoderAPin+1, true, false);

    // Configure a PIO state machine for left and right encoders.
    g_rightIndex = g_encoders.addQuadratureEncoder(rightEncoderAPin);
    g_leftIndex = g_encoders.addQuadratureEncoder(leftEncoderAPin);
    assert ( g_rightIndex >= 0 && g_leftIndex >= 0 );
}

void Encoders::flipEncoders(bool f)
{
  g_flip = f;
}

int32_t Encoders::getCountsLeft()
{
  init();

  int32_t count = g_encoders.getCount(g_leftIndex) - g_leftReset;
  return g_flip ? -count : count;
}

int32_t Encoders::getCountsRight()
{
  init();

  int32_t count = g_encoders.getCount(g_rightIndex) - g_rightReset;
  return g_flip ? -count : count;
}

int32_t Encoders::getCountsAndResetLeft()
{
  init();

  int32_t curr = g_encoders.getCount(g_leftIndex);
  int32_t count = curr - g_leftReset;
  g_leftReset = curr;
  return g_flip ? -count : count;
}

int32_t Encoders::getCountsAndResetRight()
{
  init();

  int32_t curr = g_encoders.getCount(g_rightIndex);
  int32_t count = curr - g_rightReset;
  g_rightReset = curr;
  return g_flip ? -count : count;
}

}
