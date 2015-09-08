/*
Program: ssbLogic
Description:
    Simple logic device. A2 and A3 are used as input. Input >= 50% are treated as 1. Input < 50% is treated as 0.
    Input can be scaled with the A2 and A3 knobs. The analog out is the summed value (capped at min/max values).
    Digital Gate out is a logic gate. The type of output is set by the value of A0 (Digital 0) and A1 (Digital 1).
    The following logic types are supported on either D0 or D1; OR, AND, NOR, NAND, XOR, NXOR
            A2:   0    1    0    1
            A3:   0    0    1    1
            ------------------------------
            OR:   0    1    1    1
            AND:  0    0    0    1
            NOR:  1    0    0    0
            NAND: 1    1    1    0
            XOR:  0    1    1    0
            NXOR: 1    0    0    1

    I/O Usage:
        Knob A0:         Logic type for D0; OR, AND, NOR, NAND, XOR, NXOR
        Knob A1:         Logic type for D1; OR, AND, NOR, NAND, XOR, NXOR
        Knob/Jack A2:    Input X: < 512 = 0 || >= 512 = 1 
        Knob/Jack A3:    Input Y: < 512 = 0 || >= 512 = 1 
        Digital Out 1:   Logic Ouput 1
        Digital Out 2:   Logic Ouput 2
        Clock In:        Unused
        Analog Out:      Sum of A2 and A3 Constrained to Max/Min (0 - 5v)
    Input Expander:
        Knob A4/Jack A4: Unused
        Knob A5/Jack A5: Unused
    Output Expander:
        Knob
        Bits 0-7:        Unused
        Analog Out 11:   Unused
        Digital Out 13:  Unused
    Serial:              Unused

    Created:  May 11 2014 by Peter Fawcett (SoundSweepsBy).
        Version 1 - Original patch developement.
        Version 2 - Jan 24. 2015
                    Update to use ssbArdBase lib. Code tightened up.

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
                                
const   int     OR              = 0;
const   int     AND             = 1;
const   int     NOR             = 2;
const   int     NAND            = 3;
const   int     XOR             = 4;
const   int     NXOR            = 5;

int             logic_types[2]  = {0, 0};
int             input_values[2] = {0, 0};
bool            logic_states[2] = {false, false};
bool            gate_states[2]  = {false, false};
int             logic_ctls[2]   = {A0_INPUT, A1_INPUT};
int             input_ctls[2]   = {A2_INPUT, A3_INPUT};


//  ==================== setup() START ======================
//
//  Setup patch. Enable state of pins as needed.
//  This code will run once at start of patch, right after load.
//
void setup()
{
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
    for (int i = 0; i < GATE_COUNT; i++ )
    {
        // Get the state of the first two controls. These set the 
        // desired types of logic for each Digital Out.
        logic_types[i] = getCtlIndex(logic_ctls[i], NXOR);
        input_values[i] = analogRead(input_ctls[i]);
        logic_states[i] = inputToBool(input_values[i]);
    }
    for (int i = 0; i < GATE_COUNT; i++ )
    {
        // Get the logic state for each output based on the type
        // and the two init values.
        gate_states[i] = doLogicState(logic_types[i], logic_states[0], logic_states[1]);
        digitalWrite(DIG_PINS[i], gate_states[i]);
    }
    int outState = constrain((input_values[0] + input_values[1]), MIN_VAL, MAX_VAL);
    dacOutput(outState);
}

//  =================== convenience routines ===================

bool doLogicState(int logicType, bool boolA, bool boolB)
{
    bool output = false;
    switch (logicType)
    {
        case OR:
            output = (boolA | boolB);
            break;
        case AND:
            output = (boolA & boolB);
            break;
        case NOR:
            output = (!(boolA | boolB));
            break;
        case NAND:
            output = (!(boolA & boolB));
            break;
        case XOR:
            output = (boolA ^ boolB);
            break;
        case NXOR:
            output = (!(boolA ^ boolB));
            break;
    }
    return output;
}

bool inputToBool(int input)
{
    if (input > (MAX_VAL/2))
    {
        return true;
    }
    return false;
}

