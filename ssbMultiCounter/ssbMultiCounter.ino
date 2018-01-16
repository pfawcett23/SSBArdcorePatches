/*
Program: ssbMultiCounter
Description:
    ssbMultiCounter includes 4 patches related to processing and modifying clocks or 
    streams of gates. To select a patch, set A0 to the desired state and turn the module
    on or hit the reset button on the expander. Note: the expander is required for the
    full functionality of most of these patches.

    The Analog Out / Bit Outputs on the expander have 4 types of output as well. All are
    based on updated values for each clock input pulse. The four types are set by the
    position of the A1 knob on power on / when the reset button on the expander is hit.
    The four output types are:
        - Sequential Switch: each gate advances the pulse on to the next Bit Output. The
                              Analog output is stepped accordingly.
        - Byte Counter: count bytes (0, 1, 10, 11, 100, 101, 110, 111...) on the Bit
                              Output.
                              The Analog output is stepped accordingly.
        - Pattern Cycle: cycle through a sequence of 8 patterns on the Bit Output. The
                              Analog output is stepped accordingly.
                              Patterns:
                                  1: x . x . x . x .
                                  2: . x . x . x . x
                                  3: x . . . x . . .
                                  4: x x . . x x . .
                                  5: x x x . x x x .
                                  6: . . x . . . x .
                                  7: . . . x . . . x
                                  8: . . x x . . x x
        - Pulse Divider: Bit 1 is every gate, Bit 2 is every other gate, gate 3 is every 
                              third gate and so on.Analog output is stepped accordingly
                                
    
    Patch 1: ssbSkipper
        Description:
            Input Clock or Gate Signals on the clock input.
            Depending upon the state of A2/A3, Either output the same gate signal on
            D0/D1 or skip it. At the lowest setting it plays all gates. At higher settings
            it will skip every other, third, fourth, fifth, sixth or seventh. At the
            highest setting it will skip all. If A0/A1 is high (>50%) the skip will be
            random, ie it may or may not occur, (50% chance). The expander A4/A5 may be
            used to shift the randomness from 50% chance to weighted either toward 10%
            or 90% (low or high).
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
            Bits 0-7:        Step Counter. On each clcok pulse, will advance from 0 to 7 
                             and reset.
            Analog Out 11:   Unused
            Digital Out 13:  Unused
        Serial:              Unused (debug)

    Patch 2: ssbPrimeCounter
        Description:
            Input Clock or Gate Signals on the clock input.
            Count the pulses of gates or clock pulses on the clock input.
            For every #Nth clock pulse, output a pulse on D0 or D1.
            Counts may be set to 3, 5, 7, 11, 13, 17, 19, 23.

        I/O Usage:
            Knob A0:         Count for D0: 3, 5, 7, 11, 13, 17, 19, 23.
            Knob A1:         Count for D1: 3, 5, 7, 11, 13, 17, 19, 23.
            Knob/Jack A2:    Unused
            Knob/Jack A3:    Unused
            Digital Out 1:   Gate / Counter Output 1
            Digital Out 2:   Gate / Counter Output 2
            Clock In:        Gate / Clock In
            Analog Out:      Stepped Voltage
        Input Expander:
            Knob A4/Jack A4: Unused
            Knob A5/Jack A5: Unused
        Output Expander:
            Bits 0-7:        Step Counter. On each clcok pulse, will advance from 0 to 7 
                             and reset.
            Analog Out 11:   Unused
            Digital Out 13:  Unused
        Serial:              Unused (debug)

        
    Patch 3: ssbPatter
        Description:
            Input Clock or Gate Signals on the clock input.
            For each pulse, advance and output a pulse or not, depending upon
            the currently selected pattern. Each pattern has a different sequence
            of on / off gate patterns. Pattern lengths are set by the A2/3 knob 
            and may be any where from 4 pulses up to 16.
            Patterns:
                1: x . x . x . x .
                2: . x . x . x . x
                3: x . . . x . . .
                4: x x . . x x . .
                5: x x x . x x x .
                6: . . x . . . x .
                7: . . . x . . . x
                8: . . x x . . x x

        I/O Usage:
            Knob A0:         Pattern to use for D0 out.
            Knob A1:         Pattern to use for D1 out.
            Knob/Jack A2:    Length of D0 Pattern. 4 - 16.
            Knob/Jack A3:    Length of D1 Pattern. 4 - 16.
            Digital Out 1:   Gate Output 1
            Digital Out 2:   Gate Output 2
            Clock In:        Gate / Clock In
            Analog Out:      Stepped Voltage
        Input Expander:
            Knob A4/Jack A4: Direction (Backward < 40% Forward < 80% Pendulum)
            Knob A5/Jack A5: Direction (Backward < 40% Forward < 80% Pendulum)
        Output Expander:
            Bits 0-7:        Step Counter. On each clcok pulse, will advance from 0 to 7 
                             and reset.
            Analog Out 11:   Unused
            Digital Out 13:  Unused
        Serial:              Unused (debug)
        Count pulses in the clock input and for every #n pulses output a pulse on D0/D1.

    Patch 4: To Be Determined
        
        
    Created:  Oct 30 2014 by Peter Fawcett (SoundSweepsBy).
        Version 1 - Original patch developement.

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
// DEBUGGING
//#include <ssbDebug.h>



// Sketch Constants:
// ============================================================================
const int     STATE_KNOB                    = A0_INPUT;
const int     SSB_SKIPPER                   = 0;
const int     SSB_PRIME                     = 1;
const int     SSB_PATT                      = 2;
const int     SSB_TBD                       = 3;

const int     DAC_KNOB                      = A1_INPUT;
const int     SSB_DAC_SEQ_SWITCH            = 0;
const int     SSB_DAC_BYTE_CNT              = 1;
const int     SSB_DAC_PAT_CYCLE             = 2;
const int     SSB_DAC_PULSE_DIV             = 3;

// Sketch Variables:
// ============================================================================
// Sketch index set on load / reset.
int         sketch_index                    = SSB_SKIPPER;
int         dac_index                       = SSB_DAC_SEQ_SWITCH;
// Ever increasing counter. Will be modded to determine correct Byte Output state.
long        step_counter                    = -1;
// Clock State
bool        clock_state                     = false;
// Gates for D0 and D1
ssbGate     d_gates[GATE_COUNT]             = {ssbGate(), ssbGate()};
// Analog input controls
int         row_one_ctl[GATE_COUNT]         = {A0_INPUT, A1_INPUT};
int         row_two_ctl[GATE_COUNT]         = {A2_INPUT, A3_INPUT};
int         expander_ctl[GATE_COUNT]        = {A4_INPUT, A5_INPUT};

// ============================================================================

// ============================================================================
//ssbSkipper:
// Constants:
// ============================================================================
const int   NO_SKIP                         = 0;
const int   ALL_SKIP                        = 7;
const int   SKIP_MIN_RAND                   = 10;
const int   SKIP_MAX_RAND                   = 90;
// Variables:
// ============================================================================
bool        skip_step_rand_on[GATE_COUNT]   = {false, false};
int         skip_step_rand_amt[GATE_COUNT]  = {0, 0};
int         skip_step_index[GATE_COUNT]     = {0, 0};
// ============================================================================

// ============================================================================
//ssbPrimeCounter:
// Constants:
const int   PRIME_TYPES                     = 8;
const int   PRIME_23                        = 7;
const int   PRIMES[PRIME_TYPES]             = {3, 5, 7, 11, 13, 17, 19, 23};
// Variables:
// ============================================================================
int         prime_step_index[GATE_COUNT]    = {0, 0};
bool        prime_step_invert[GATE_COUNT]   = {false, false};
// ============================================================================

// ============================================================================
//ssbPatter:
// Constants:
const int   PAT_COUNT                       = 8;
const int   PAT_STEPS                       = 8;
const int   PAT_SHIFT_MAX                   = 3;
const int   PATS[PAT_COUNT][PAT_STEPS]      = {{1, 0, 1, 0, 1, 0, 1, 0},
                                               {0, 1, 0, 1, 0, 1, 0, 1},
                                               {1, 0, 0, 0, 1, 0, 0, 0},
                                               {1, 1, 0, 0, 1, 1, 0, 0},
                                               {1, 1, 1, 0, 1, 1, 1, 0},
                                               {0, 0, 1, 0, 0, 0, 1, 0},
                                               {0, 0, 0, 1, 0, 0, 0, 1},
                                               {0, 0, 1, 1, 0, 0, 1, 1}};
const int   PAT_MIN_LEN                     = 4;            
const int   PAT_MAX_LEN                     = 8;
// Variables:
// ============================================================================
int         patt_index[GATE_COUNT]          = {0, 0};
int         patt_length[GATE_COUNT]         = {8, 8};
bool        patt_start[GATE_COUNT]          = {false, false};
bool        byte_states[PAT_STEPS]          = {false, false, false, false, 
                                               false, false, false, false};
// ============================================================================

//DEBUGGING:
//ssbDebug    DEBUG                           = ssbDebug();

//  ==================== setup() START ========================================
//
//  Setup patch. Enable state of pins as needed.
//  This code will run once at start of patch, right after load.
//
void setup()
{
    randomSeed(millis());

    // Read the sketch index from A0 Input.
    sketch_index = analogRead(STATE_KNOB) >> 8;
    
    // Read the sketch index from A1 Input.
    dac_index = analogRead(DAC_KNOB) >> 8;
    
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
    // Debugging
    //DEBUG.enableSerial();
    //DEBUG.debugState(true);
}

//  ==================== setup() END ==========================================


//  ==================== loop() START =========================================
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
        // trigger on the interrupt.
        step_counter++;
        switch (sketch_index)
        {
            case SSB_SKIPPER:
                for (int i = 0; i < GATE_COUNT; i++)
                {
                    skip_step_rand_on[i] = getCtlHighLow(row_one_ctl[i]);
                    skip_step_index[i] = getCtlIndex(row_two_ctl[i], ALL_SKIP);
                    skip_step_rand_amt[i] = getCtlIndex(expander_ctl[i], SKIP_MIN_RAND, SKIP_MAX_RAND);
                    if (!doSkipStep(step_counter, skip_step_index[i], skip_step_rand_on[i], skip_step_rand_amt[i]))
                    {
                        d_gates[i].setState(true);
                    }
                }
                break;
            case SSB_PRIME:
                for (int i = 0; i < GATE_COUNT; i++)
                {
                    prime_step_index[i] = getCtlIndex(row_one_ctl[i], PRIME_23);
                    prime_step_invert[i] = getCtlHighLow(row_two_ctl[i]);
                    // expander_ctl unused.
                    if (doPrimeStep(step_counter, prime_step_index[i], prime_step_invert[i]))
                    {
                        d_gates[i].setState(true);
                    }
                }
                break;
            case SSB_PATT:
                for (int i = 0; i < GATE_COUNT; i++)
                {
                    patt_index[i] = getCtlIndex(row_one_ctl[i], (PAT_COUNT - 1));
                    patt_start[i] = getCtlIndex(row_two_ctl[i], PAT_SHIFT_MAX);
                    patt_length[i] = getCtlIndex(expander_ctl[i], PAT_MIN_LEN, PAT_MAX_LEN);
                    if (doPattStep(step_counter, patt_index[i], patt_start[i], patt_length[i]))
                    {
                        d_gates[i].setState(true);
                    }
                }
                break;
            case SSB_TBD:
                break;
        }
    }
    else if (digitalRead(CLOCK_IN) == false)
    {
        // else go low when clock/gate goes low, note falling edge is 'lazy'
        // and will only be detected on next loop, not an interrupt.
        clock_state = false;
        switch (sketch_index)
        {
            case SSB_SKIPPER:
            case SSB_PRIME:
            case SSB_PATT:
                for (int i = 0; i < GATE_COUNT; i++)
                {
                    d_gates[i].setState(false);
                }
                break;
            case SSB_TBD:
                break;
        } 
    }
    for (int i = 0; i < GATE_COUNT; i++)
    {
        d_gates[i].render(DIG_PINS[i]);
    }
    render_dac_bytes(step_counter, dac_index);
}

//  ==================== loop() END ===========================================


//  ==================== ssbSkipper Methods START =============================

boolean doSkipStep(int current_step, int skip_step_i, bool rand_enabled, int rand_amt)
{
    bool do_skip = false;
    int step_mod = 0;
    if (skip_step_i > NO_SKIP)
    {
        if (skip_step_i == ALL_SKIP)
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

//  ==================== ssbSkipper Methods END ===============================


//  ==================== ssbPrimeCounter Methods START ========================

boolean doPrimeStep(int current_step, int prime_index, boolean step_invert)
{
    boolean do_skip = false;
    int prime_value = PRIMES[prime_index];
    int step_mod = (current_step + 1) % (prime_value);
    if (step_mod == 0)
    {
        do_skip = true;
    }
    if (step_invert == true)
    {
      do_skip = !do_skip;
    }
    return do_skip;
}

//  ==================== ssbPrimeCounter Methods END ==========================


//  ==================== ssbPatter Methods START ==============================

boolean doPattStep(int current_step, int patt_index, int patt_start, int patt_len)
{
    boolean is_on = false;
    int patt_max = PAT_STEPS;
    int actual_step = 0;
    if (patt_len < patt_max)
    {
        patt_max = patt_len;
    }
    actual_step = (current_step % patt_max) - 1;
    if (actual_step < 0)
    {
        actual_step = (patt_max-1);
    }
    actual_step += (patt_start - 1);
    if (actual_step >= patt_max)
    {
        actual_step = actual_step - patt_max;
    }
    if (PATS[patt_index][actual_step] == 1)
    {
        is_on = true;
    }
    return is_on;
}

//  ==================== ssbPatter Methods END ================================


//  ==================== render_dac_bytes START ===============================

void render_dac_bytes(int step_count, int render_type)
{
    switch (render_type)
    {
        case SSB_DAC_SEQ_SWITCH:
            expanderGateBang((step_count % 8));
            break;
        case SSB_DAC_BYTE_CNT:
            dacOutput((step_count % 1023));
            break;
        case SSB_DAC_PAT_CYCLE:
            // Note we are using the same patterns as ssbPatter but as rows rather
            // than cells.
            step_count = step_count % 8;
            for (int i = 0; i < PAT_STEPS; i++)
            {
                if (PATS[step_count][i] == 1)
                {
                    byte_states[i] = true;
                }
                else
                {
                    byte_states[i] = false;
                }
            }
            expanderGatesOut(byte_states[0], 
                             byte_states[1], 
                             byte_states[2], 
                             byte_states[3], 
                             byte_states[4], 
                             byte_states[5], 
                             byte_states[6], 
                             byte_states[7]);
            break;
        case SSB_DAC_PULSE_DIV:
            int mod_index = 0;
            for (int i = 0; i < PAT_STEPS; i++)
            {
                // only care about on bits, all are off now.
                //
                mod_index = step_count % i;
                if (mod_index == 0)
                {
                    byte_states[i] = true;
                }
                else
                {
                    byte_states[i] = false;
                }
            }
            expanderGatesOut(byte_states[0], 
                             byte_states[1], 
                             byte_states[2], 
                             byte_states[3], 
                             byte_states[4], 
                             byte_states[5], 
                             byte_states[6], 
                             byte_states[7]);
            break;
    }
}
