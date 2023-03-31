// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include <stdint.h>
#include "Pololu3piPlus2040Buzzer.h"

namespace Pololu3piPlus2040
{

// Set up the timer to play the desired frequency (in Hz or .1 Hz) for the
//   the desired duration (in ms). Allowed frequencies are 40 Hz to 10 kHz.
//   volume controls buzzer volume, with 15 being loudest and 0 being quietest.
// Note: frequency*duration/1000 must be less than 0xFFFF (65535).  This
//   means that you can't use a max duration of 65535 ms for frequencies
//   greater than 1 kHz.  For example, the max duration you can use for a
//   frequency of 10 kHz is 6553 ms.  If you use a duration longer than this,
//   you will cause an integer overflow that produces unexpected behavior.
void Buzzer::playFrequency(unsigned int freq, unsigned int dur,
                     unsigned char volume)
{
  buzzerFinished = false;

  uint32_t multiplier = 1;

  if (freq & DIV_BY_10) // if frequency's DIV_BY_10 bit is set
  {                     //  then the true frequency is freq/10
    multiplier = 10;    //  (gives higher resolution for small freqs)
    freq &= ~DIV_BY_10; // clear DIV_BY_10 bit
  }

  uint32_t period_us = 1000000 * multiplier / freq;
  uint32_t pulse_width_us = period_us >> (16-volume);
  pwm.period_us(period_us);
  pwm.pulsewidth_us(pulse_width_us);
  timeout.attach(mbed::callback(this, &Buzzer::timerDone), std::chrono::milliseconds(dur));
}


void Buzzer::timerDone()
{
    buzzerFinished = true;
    pwm.pulsewidth_us(0);
    if (buzzerSequence && (play_mode_setting == PLAY_AUTOMATIC))
      nextNote();
}


// Determine the frequency for the specified note, then play that note
//  for the desired duration (in ms).  This is done without using floats
//  and without having to loop.  volume controls buzzer volume, with 15 being
//  loudest and 0 being quietest.
// Note: frequency*duration/1000 must be less than 0xFFFF (65535).  This
//  means that you can't use a max duration of 65535 ms for frequencies
//  greater than 1 kHz.  For example, the max duration you can use for a
//  frequency of 10 kHz is 6553 ms.  If you use a duration longer than this,
//  you will cause an integer overflow that produces unexpected behavior.
void Buzzer::playNote(unsigned char note, unsigned int dur,
                 unsigned char volume)
{
  // note = key + octave * 12, where 0 <= key < 12
  // example: A4 = A + 4 * 12, where A = 9 (so A4 = 57)
  // A note is converted to a frequency by the formula:
  //   Freq(n) = Freq(0) * a^n
  // where
  //   Freq(0) is chosen as A4, which is 440 Hz
  // and
  //   a = 2 ^ (1/12)
  // n is the number of notes you are away from A4.
  // One can see that the frequency will double every 12 notes.
  // This function exploits this property by defining the frequencies of the
  // 12 lowest notes allowed and then doubling the appropriate frequency
  // the appropriate number of times to get the frequency for the specified
  // note.

  // if note = 16, freq = 41.2 Hz (E1 - lower limit as freq must be >40 Hz)
  // if note = 57, freq = 440 Hz (A4 - central value of ET Scale)
  // if note = 111, freq = 9.96 kHz (D#9 - upper limit, freq must be <10 kHz)
  // if note = 255, freq = 1 kHz and buzzer is silent (silent note)

  // The most significant bit of freq is the "divide by 10" bit.  If set,
  // the units for frequency are .1 Hz, not Hz, and freq must be divided
  // by 10 to get the true frequency in Hz.  This allows for an extra digit
  // of resolution for low frequencies without the need for using floats.

  unsigned int freq = 0;
  unsigned char offset_note = note - 16;

  if (note == SILENT_NOTE || volume == 0)
  {
    freq = 1000;  // silent notes => use 1kHz freq (for cycle counter)
    playFrequency(freq, dur, 0);
    return;
  }

  if (note <= 16)
    offset_note = 0;
  else if (offset_note > 95)
    offset_note = 95;

  unsigned char exponent = offset_note / 12;

  // frequency table for the lowest 12 allowed notes
  //   frequencies are specified in tenths of a Hertz for added resolution
  switch (offset_note - exponent * 12)  // equivalent to (offset_note % 12)
  {
    case 0:        // note E1 = 41.2 Hz
      freq = 412;
      break;
    case 1:        // note F1 = 43.7 Hz
      freq = 437;
      break;
    case 2:        // note F#1 = 46.3 Hz
      freq = 463;
      break;
    case 3:        // note G1 = 49.0 Hz
      freq = 490;
      break;
    case 4:        // note G#1 = 51.9 Hz
      freq = 519;
      break;
    case 5:        // note A1 = 55.0 Hz
      freq = 550;
      break;
    case 6:        // note A#1 = 58.3 Hz
      freq = 583;
      break;
    case 7:        // note B1 = 61.7 Hz
      freq = 617;
      break;
    case 8:        // note C2 = 65.4 Hz
      freq = 654;
      break;
    case 9:        // note C#2 = 69.3 Hz
      freq = 693;
      break;
    case 10:      // note D2 = 73.4 Hz
      freq = 734;
      break;
    case 11:      // note D#2 = 77.8 Hz
      freq = 778;
      break;
  }

  if (exponent < 7)
  {
    freq = freq << exponent;  // frequency *= 2 ^ exponent
    if (exponent > 1)      // if the frequency is greater than 160 Hz
      freq = (freq + 5) / 10;  //   we don't need the extra resolution
    else
      freq += DIV_BY_10;    // else keep the added digit of resolution
  }
  else
    freq = (freq * 64 + 2) / 5;  // == freq * 2^7 / 10 without int overflow

  if (volume > 15)
    volume = 15;
  playFrequency(freq, dur, volume);  // set buzzer this freq/duration
}


// Returns 1 if the buzzer is currently playing, otherwise it returns 0
unsigned char Buzzer::isPlaying()
{
  return !buzzerFinished || buzzerSequence != 0;
}


// Plays the specified sequence of notes.  If the play mode is
// PLAY_AUTOMATIC, the sequence of notes will play with no further
// action required by the user.  If the play mode is PLAY_CHECK,
// the user will need to call playCheck() in the main loop to initiate
// the playing of each new note in the sequence.  The play mode can
// be changed while the sequence is playing.
// This is modeled after the PLAY commands in GW-BASIC, with just a
// few differences.
//
// The notes are specified by the characters C, D, E, F, G, A, and
// B, and they are played by default as "quarter notes" with a
// length of 500 ms.  This corresponds to a tempo of 120
// beats/min.  Other durations can be specified by putting a number
// immediately after the note.  For example, C8 specifies C played as an
// eighth note, with half the duration of a quarter note. The special
// note R plays a rest (no sound).
//
// Various control characters alter the sound:
//   '>' plays the next note one octave higher
//
//   '<' plays the next note one octave lower
//
//   '+' or '#' after a note raises any note one half-step
//
//   '-' after a note lowers any note one half-step
//
//   '.' after a note "dots" it, increasing the length by
//       50%.  Each additional dot adds half as much as the
//       previous dot, so that "A.." is 1.75 times the length of
//       "A".
//
//   'O' followed by a number sets the octave (default: O4).
//
//   'T' followed by a number sets the tempo (default: T120).
//
//   'L' followed by a number sets the default note duration to
//       the type specified by the number: 4 for quarter notes, 8
//       for eighth notes, 16 for sixteenth notes, etc.
//       (default: L4)
//
//   'V' followed by a number from 0-15 sets the music volume.
//       (default: V15)
//
//   'MS' sets all subsequent notes to play staccato - each note is played
//       for 1/2 of its allotted time, followed by an equal period
//       of silence.
//
//   'ML' sets all subsequent notes to play legato - each note is played
//       for its full length.  This is the default setting.
//
//   '!' resets all persistent settings to their defaults.
//
// The following plays a c major scale up and back down:
//   play("L16 V8 cdefgab>cbagfedc");
//
// Here is an example from Bach:
//   play("T240 L8 a gafaeada c+adaeafa <aa<bac#ada c#adaeaf4");
void Buzzer::play(const char *notes)
{
  buzzerSequence = notes;
  staccato_rest_duration = 0;
  nextNote();          // this re-enables the timer interrupt
}


// stop all sound playback immediately
void Buzzer::stopPlaying()
{
  pwm.period_ms(1);
  pwm.pulsewidth_us(0);
  timeout.detach();

  buzzerFinished = true;
  buzzerSequence = 0;
}


// Gets the current character, converting to lower-case and skipping
// spaces.  For any spaces, this automatically increments sequence!
char Buzzer::currentCharacter()
{
  char c = 0;
  do
  {
    c = *buzzerSequence;
    if(c >= 'A' && c <= 'Z')
      c += 'a'-'A';
  } while(c == ' ' && (buzzerSequence ++));

  return c;
}


// Returns the numerical argument specified at buzzerSequence[0] and
// increments sequence to point to the character immediately after the
// argument.
unsigned int Buzzer::getNumber()
{
  unsigned int arg = 0;

  // read all digits, one at a time
  char c = currentCharacter();
  while(c >= '0' && c <= '9')
  {
    arg *= 10;
    arg += c-'0';
    buzzerSequence ++;
    c = currentCharacter();
  }

  return arg;
}


void Buzzer::nextNote()
{
  unsigned char note = 0;
  unsigned char rest = 0;
  unsigned char tmp_octave = octave; // the octave for this note
  unsigned int tmp_duration; // the duration of this note
  unsigned int dot_add;

  char c; // temporary variable

  // if we are playing staccato, after every note we play a rest
  if(staccato && staccato_rest_duration)
  {
    playNote(SILENT_NOTE, staccato_rest_duration, 0);
    staccato_rest_duration = 0;
    return;
  }

 parse_character:

  // Get current character
  c = currentCharacter();
  buzzerSequence ++;

  // Interpret the character.
  switch(c)
  {
  case '>':
    // shift the octave temporarily up
    tmp_octave ++;
    goto parse_character;
  case '<':
    // shift the octave temporarily down
    tmp_octave --;
    goto parse_character;
  case 'a':
    note = NOTE_A(0);
    break;
  case 'b':
    note = NOTE_B(0);
    break;
  case 'c':
    note = NOTE_C(0);
    break;
  case 'd':
    note = NOTE_D(0);
    break;
  case 'e':
    note = NOTE_E(0);
    break;
  case 'f':
    note = NOTE_F(0);
    break;
  case 'g':
    note = NOTE_G(0);
    break;
  case 'l':
    // set the default note duration
    note_type = getNumber();
    duration = whole_note_duration/note_type;
    goto parse_character;
  case 'm':
    // set music staccato or legato
    if(currentCharacter() == 'l')
      staccato = false;
    else
    {
      staccato = true;
      staccato_rest_duration = 0;
    }
    buzzerSequence ++;
    goto parse_character;
  case 'o':
    // set the octave permanently
    octave = tmp_octave = getNumber();
    goto parse_character;
  case 'r':
    // Rest - the note value doesn't matter.
    rest = 1;
    break;
  case 't':
    // set the tempo
    whole_note_duration = 60*400/getNumber()*10;
    duration = whole_note_duration/note_type;
    goto parse_character;
  case 'v':
    // set the volume
    volume = getNumber();
    goto parse_character;
  case '!':
    // reset to defaults
    octave = 4;
    whole_note_duration = 2000;
    note_type = 4;
    duration = 500;
    volume = 15;
    staccato = 0;
    // reset temp variables that depend on the defaults
    tmp_octave = octave;
    tmp_duration = duration;
    goto parse_character;
  default:
    buzzerSequence = 0;
    return;
  }

  note += tmp_octave*12;

  // handle sharps and flats
  c = currentCharacter();
  while(c == '+' || c == '#')
  {
    buzzerSequence ++;
    note ++;
    c = currentCharacter();
  }
  while(c == '-')
  {
    buzzerSequence ++;
    note --;
    c = currentCharacter();
  }

  // set the duration of just this note
  tmp_duration = duration;

  // If the input is 'c16', make it a 16th note, etc.
  if(c > '0' && c < '9')
    tmp_duration = whole_note_duration/getNumber();

  // Handle dotted notes - the first dot adds 50%, and each
  // additional dot adds 50% of the previous dot.
  dot_add = tmp_duration/2;
  while(currentCharacter() == '.')
  {
    buzzerSequence ++;
    tmp_duration += dot_add;
    dot_add /= 2;
  }

  if(staccato)
  {
    staccato_rest_duration = tmp_duration / 2;
    tmp_duration -= staccato_rest_duration;
  }

  // this will re-enable the timer overflow interrupt
  playNote(rest ? SILENT_NOTE : note, tmp_duration, volume);
}


// This puts play() into a mode where instead of advancing to the
// next note in the sequence automatically, it waits until the
// function playCheck() is called. The idea is that you can
// put playCheck() in your main loop and avoid potential
// delays due to the note sequence being checked in the middle of
// a time sensitive calculation.  It is recommended that you use
// this function if you are doing anything that can't tolerate
// being interrupted for more than a few microseconds.
// Note that the play mode can be changed while a sequence is being
// played.
//
// Usage: playMode(PLAY_AUTOMATIC) makes it automatic (the
// default), playMode(PLAY_CHECK) sets it to a mode where you have
// to call playCheck().
void Buzzer::playMode(PlayMode mode)
{
  play_mode_setting = mode;

  // We want to check to make sure that we didn't miss a note if we
  // are going out of play-check mode.
  if(mode == PLAY_AUTOMATIC)
    playCheck();
}


// Checks whether it is time to start another note, and starts
// it if so.  If it is not yet time to start the next note, this method
// returns without doing anything.  Call this as often as possible
// in your main loop to avoid delays between notes in the sequence.
//
// Returns true if it is still playing.
unsigned char Buzzer::playCheck()
{
  if(buzzerFinished && buzzerSequence != 0)
    nextNote();
  return buzzerSequence != 0;
}

} // namespace
