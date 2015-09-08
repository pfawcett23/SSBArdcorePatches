/*
  ssbArdBase.cpp - Source File
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

#include "ssbArdBase.h"

/*
Track the clock state from CLOCK_IN
NOTE: Be sure to call setClockInterrupt to set the interrupt or this will
ALWAYS be LOW....
*/
volatile int ssb_clock_state = LOW;

// ============================================================================
// DAC Out:
// ============================================================================

/* dacOutput
- output valute to the DAC output quickly.
*/
void dacOutput(long v)
{
    v = v >> 2;
    PORTB = (PORTB & B11100000) | (v >> 3);
    PORTD = (PORTD & B00011111) | ((v & B00000111) << 5);
}

/* expanderGatesOut
- Use the expander as a set of on off gates.
Will also produce various stepped voltages on the DAC outputs.
*/
void expanderGatesOut(bool bit0, 
                      bool bit1, 
                      bool bit2, 
                      bool bit3, 
                      bool bit4, 
                      bool bit5, 
                      bool bit6, 
                      bool bit7)
{
    long signal = 0;
    if (bit0 == true)
    {
        signal += 4;
    }
    if (bit1 == true)
    {
        signal += 8;
    }
    if (bit2 == true)
    {
        signal += 16;
    }
    if (bit3 == true)
    {
        signal += 32;
    }
    if (bit4 == true)
    {
        signal += 64;
    }
    if (bit5 == true)
    {
        signal += 128;
    }
    if (bit6 == true)
    {
        signal += 256;
    }
    if (bit7 == true)
    {
        signal += 512;
    }
    PORTB = (PORTB & B11100000) | (signal >> 3);
    PORTD = (PORTD & B00011111) | ((signal & B00000111) << 5);
}

/* expanderGateBang
- Use the expander as a sequential gate for sequences, steps and switches.
Will also produce various stepped voltages on the DAC outputs.
*/
void expanderGateBang(int gate_index)
{
    long signal = 0;
    if (gate_index >= 0 and gate_index < DAC_BITS)
    {
        signal = (4 * (gate_index + 1));
    }
    PORTB = (PORTB & B11100000) | (signal >> 3);
    PORTD = (PORTD & B00011111) | ((signal & B00000111) << 5);
}

// ============================================================================
// Handle the ArdCore Clock:
// ============================================================================

/* setClockInterrupt
- Attach an interrupt to the ArdCore clock when a clock or gate trigger is 
received it will set a flag to high. To check the clock call getClockState.
*/
void setClockInterrupt()
{
    // set up the digital (clock) input
    pinMode(CLOCK_IN, INPUT);
    attachInterrupt(0, isr, RISING);
}

/* peekClockState
- Check the state of the clock value with out changing it.
*/
bool peekClockState()
{
    return ssb_clock_state;
}

/* readClockState
- Read the current state of the clock. If it is HIGH, it will be
set back to low so that the next read will be low. Each high represents
a pulse from the clock.
*/
bool readClockState()
{
    if (ssb_clock_state == true)
    {
        ssb_clock_state = false;
        return true;
    }
    return false;
}

/* isr
- quickly handle interrupts from the clock input. Set flag high and exit.
*/
void isr()
{
    ssb_clock_state = HIGH;
}

// ============================================================================
// Utility Methods for working with the ArdCore:
// ============================================================================

/* getCtlHighLow
- treat an Analog Input as a True / False input.
Greater than (or equal) 50% return true otherwise return false.
*/
bool getCtlHighLow(int pin)
{
    if (analogRead(pin) >= (MAX_VAL/2))
    {
        return true;
    }
    return false;
}

/* getCtlIndex
- map Analog Input to 0 - max value range.
*/
int getCtlIndex(int pin, int max_index)
{
    int tmp_val = 0;
    tmp_val = map(analogRead(pin), MIN_VAL, MAX_VAL, 0, max_index+1);
    if (tmp_val > max_index)
    {
        // handle case where input == MAX_VAL and tmp_val is max_index + 1
        tmp_val = max_index;
    }
    return tmp_val;
}

/* getCtlIndex
- map Analog Input to min - max value range.
*/
int getCtlIndex(int pin, int min_index, int max_index)
{
    int tmp_val = 0;
    tmp_val = map(analogRead(pin), MIN_VAL, MAX_VAL, min_index, max_index+1);
    if (tmp_val > max_index)
    {
        // handle case where input == MAX_VAL and tmp_val is max_index + 1
        tmp_val = max_index;
    }
    return tmp_val;
}
