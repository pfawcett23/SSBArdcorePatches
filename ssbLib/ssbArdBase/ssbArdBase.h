/*
  ssbArdBase.h - Header file for ssbArdBase.cp. 
  A collection of useful / common methods for use when working with the 
  ArdCore EuroRack module. These methods may also be of use for other Arduino 
  based sketches, but that is less clear at this time. Assumptions about pin 
  states and control voltages are based on the ArdCore structure.

  Created by Peter Fawcett, Jan 3. 2015.
    Version 0.1: Implemented / moved over from working code, the base
                 set of functionality.
    Version 0.2: Feb. 7, 2015
                    Added
                    - expanderGateBang
                    - mapCtlIndex
                    - getCtlIndex
                    - DAC_BITS and GATE_COUNT constants.

============================================================

License:

This software is licensed under the Creative Commons
"Attribution-NonCommercial license. This license allows you
to tweak and build upon the code for non-commercial purposes,
without the requirement to license derivative works on the
same terms. If you wish to use this (or derived) work for
commercial work, please contact Peter Fawcett at our website
(www.SoundSweepsBy.com).

For more information on the Creative Commons CC BY-NC license,
visit http://creativecommons.org/licenses/
*/

#ifndef _ssb_ard_base_
#define _ssb_ard_base_

#include <Arduino.h>

// ============================================================================
// ArdCore General Constants:
// ============================================================================
const int     MAX_VAL               = 1023;
const int     MIN_VAL               = 0;
const int     DAC_BITS              = 8;
const int     GATE_COUNT            = 2;

// ============================================================================
// ArdCore Pin Constants:
// ============================================================================
// IO, Clock and ArdCore Pin Constants
const int     A0_INPUT              = 0;
const int     A1_INPUT              = 1;
const int     A2_INPUT              = 2;
const int     A3_INPUT              = 3;
const int     A4_INPUT              = 4;
const int     A5_INPUT              = 5;
const int     CLOCK_IN              = 2;
const int     DIG_PINS[GATE_COUNT]  = {3, 4};
const int     PIN_OFFSET            = 5;

// ============================================================================
// DAC Out:
// ============================================================================
// - Write the current DAC value to the 8 bit registers.
void dacOutput(long out_value);

// - Write the on off states for each bit out in the expander.
void expanderGatesOut(bool bit0, 
                      bool bit1, 
                      bool bit2, 
                      bool bit3, 
                      bool bit4, 
                      bool bit5, 
                      bool bit6, 
                      bool bit7);

// - Turn on or off a single gate. Useful for step counters or
// step switches.
void expanderGateBang(int gate_index);


// ============================================================================
// Handle the ArdCore Clock:
// ============================================================================
// - Attach an interrupt to the ArdCore clock when a clock or gate trigger is 
//   received it will set a flag to high. To check the clock call 
//   getClockState.
void setClockInterrupt();

// - Check the state of the clock value with out changing it.
bool peekClockState();

// - Read the current state of the clock. If it is HIGH, it will be set back
//   to low so that the next read will be low. Each high represents a pulse 
//   from the clock.
bool readClockState();

// - quickly handle interrupts from the clock input. Set flag high and exit.
//   DO NOT CALL DIRECTLY
void isr();

// ============================================================================
// Utility Methods for working with the ArdCore:
// ============================================================================

// - is control state high or low (CV or Pot, < 50% is low, > 50% is high).
bool getCtlHighLow(int pin);

// - control index (CV or Pot)
int getCtlIndex(int pin, int max_index);
int getCtlIndex(int pin, int min_index, int max_index);

#endif /* _ssb_ard_base_ */
