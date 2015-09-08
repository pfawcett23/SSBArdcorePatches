/*
Program: ssbSkipper
Description:
    Input Clock or Gate Signals on the clock input.
    Depending upon the state of A2/A3, Either output the same gate signal on
    D0/D1 or skip it. At the lowest setting it plays all gates. At higher settings
    it will skip every other, third, fourth, fifth, sixth or seventh. At the highest
    setting it will skip all. If A0/A1 is high (>50%) the skip will be random, ie 
    it may or may not occur, (50% chance). The expander A4/A5 may be used to 
    shift the randomness from 50% chance to weigighted either toward 10% or
    90% (low or high).
    The skip none and skip all settings are best with random as they you get
    a 50% don't play or a 50% do play for all gates. With the expander this 
    can then be maniuplated further. Note that the skip and the random 
    percents may be modulated.

    I/O Usage:
        Knob A0:         Enable random for D0
        Knob A1:         Enable random for D1
        Knob/Jack A2:    Skip beat (None, 2nd, 3rd, 4th, 5th, 6th, 7th, All) for D0
        Knob/Jack A3:    Skip beat (None, 2nd, 3rd, 4th, 5th, 6th, 7th, All) for D1
        Digital Out 1:   Gate / Clock out 1
        Digital Out 2:   Gate / Clock out 2
        Clock In:        Gate / Clock In
        Analog Out:      Stepped Voltage
    Input Expander:
        Knob A4/Jack A4: Shift randomness weight 10%-90% for D0
        Knob A5/Jack A5: Shift randomness weight 10%-90% for D1
    Output Expander:
        Bits 0-7:        Step Counter. On each clcok pulse, will advance from 0 to 7 and reset.
        Analog Out 11:   Unused
        Digital Out 13:  Unused
    Serial:              Unused (debug)

    Created:  Feb 7 2015 by Peter Fawcett (SoundSweepsBy).
        Version 1 - Original patch developement.
        Version 2 - Sept. 7 2015:
          Updated to use ssbLib code.
          Fixed bugs.

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

#include <ssbArdBase.h>
#include <ssbGate.h>
/* // DEBUGGING
#include <ssbDebug.h>
*/

const int   SKIP_TYPES                      = 8;
const int   SKIP_NONE                       = 0;
const int   SKIP_2ND                        = 1;
const int   SKIP_3RD                        = 2;
const int   SKIP_4TH                        = 3;
const int   SKIP_5TH                        = 4;
const int   SKIP_6TH                        = 5;
const int   SKIP_7TH                        = 6;
const int   SKIP_ALL                        = 7;

// Ever increasing counter. Will be modded by the skip step to determine if it's a 
// 'second step' or a 'third step' and by 8 to determine the expander 0-7 gate.
long        step_counter                    = -1;
bool        clock_state                     = false;

int         skip_rand_on_ctl[GATE_COUNT]    = {A0_INPUT, A1_INPUT};
int         skip_step_ctl[GATE_COUNT]       = {A2_INPUT, A3_INPUT};
int         skip_rand_amt_ctl[GATE_COUNT]   = {A4_INPUT, A5_INPUT};
bool        skip_step_rand_on[GATE_COUNT]   = {false, false};
int         skip_step_rand_amt[GATE_COUNT]  = {0, 0};
int         skip_step_index[GATE_COUNT]     = {0, 0};
ssbGate     d_gates[GATE_COUNT]             = {ssbGate(), ssbGate()};

/* //DEBUGGING:
ssbDebug    DEBUG                           = ssbDebug();
*/

//  ==================== setup() START ======================
//
//  Setup patch. Enable state of pins as needed.
//  This code will run once at start of patch, right after load.
//
void setup()
{
    randomSeed(millis());
    // set up the digital outputs
    for (int i = 0; i < 2; i++)
    {
        pinMode(DIG_PINS[i], OUTPUT);
        digitalWrite(DIG_PINS[i], LOW);
    }
    // set up the 8-bit DAC output pins
    for (int i = 0; i < 8; i++)
    {
        pinMode(PIN_OFFSET + i, OUTPUT);
        digitalWrite(PIN_OFFSET + i, LOW);
    }
    setClockInterrupt();
    /* // Debugging
    DEBUG.enableSerial();
    DEBUG.debugState(true);
    */
}
//  ==================== setup() END =======================

//  ==================== loop() START =======================
//
//  Master Loop.
//  Loop will be called over and over with out pause.
//  Main logic of patch.
//
void loop()
{
    clock_state = readClockState();
    if (clock_state)
    {
        // We have a leading edge of a clock pulse.
        step_counter++;
        for (int i = 0; i < GATE_COUNT; i++)
        {
            skip_step_index[i] = getCtlIndex(skip_step_ctl[i], SKIP_ALL);
            skip_step_rand_on[i] = getCtlHighLow(skip_rand_on_ctl[i]);
            skip_step_rand_amt[i] = getCtlIndex(skip_rand_amt_ctl[i], 10, 90);
            if (!doSkipStep(step_counter, skip_step_index[i], skip_step_rand_on[i], skip_step_rand_amt[i]))
            {
                d_gates[i].setState(true);
            }
            /* // Debugging
            DEBUG.debugValue("Gate Index:", i);
            DEBUG.debugValue("Skip Step Index:", skip_step_index[i]);
            DEBUG.debugValue("Skip Step Rand On:", skip_step_rand_on[i]);
            DEBUG.debugValue("Skip Step Rand Amt:", skip_step_rand_amt[i]);
            DEBUG.debugValue("Gate Is On:", d_gates[i].isOn());
            DEBUG.debugValue("Gate Is Active:", d_gates[i].isActive());
            */
        }
        /* // Debugging
        DEBUG.updateTicks();
        */
    }
    else if (digitalRead(CLOCK_IN) == false)
    {
        clock_state = false;
        for (int i = 0; i < GATE_COUNT; i++)
        {
            d_gates[i].setState(false);
        }
    }
    for (int i = 0; i < GATE_COUNT; i++)
    {
        d_gates[i].render(DIG_PINS[i]);
    }
    expanderGateBang((step_counter % 8));
}

bool doSkipStep(int current_step, int skip_step_i, bool rand_enabled, int rand_amt)
{
    bool do_skip = false;
    int step_mod = 0;
    if (skip_step_i > SKIP_NONE)
    {
        if (skip_step_i == SKIP_ALL)
        {
            do_skip = true;
            if ((rand_enabled == true) and (random(0, 100) < rand_amt))
            {
                do_skip = !do_skip;
            }
        }
        else
        {
            step_mod = (current_step + 1) % (skip_step_i + 1);
            if (step_mod == 0)
            {
                do_skip = true;
            }
            if ((rand_enabled == true) and (random(0, 100) < rand_amt))
            {
                do_skip = !do_skip;
            }
        }
    }
    else
    {
        if ((rand_enabled == true) and (random(0, 100) < rand_amt))
        {
            do_skip = !do_skip;
        }
    }
    return do_skip;
}
