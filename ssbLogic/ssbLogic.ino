/*
Program: ssbLogic
Description:
    Simple logic device. A2 and A3 are used as input. Input >= 50% are treated as 1. Input < 50% is treated as 0.
    Input can be scaled with the A2 and A3 knobs. The analog out is the summed value (capped at min/max values).
    Digital Gate out is a logic gate. The type of output is set by the value of A0 (Digital 0) and A1 (Digital 1).
    The following logic types are supported on either D0 or D1; OR, AND, NOR, NAND, XOR, NXOR
            A0:   0    1    0    1
            A1:   0    0    1    1
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
// Max output.
const int     MAX_VAL      = 1023;
const int     MIN_VAL      = 0;
const boolean FALSE        = LOW;
const boolean TRUE         = HIGH;
const int     HALFWAY      = MAX_VAL / 2;
const int     STAGE_SEP    = MAX_VAL / 6;


//  constants related to the Arduino Nano pin use
const int     pinOffset    = 5;       // DAC     -> the first DAC pin (from 5-12)
const int     digPin[2]    = {3, 4};  // the digital output pins
     
const int     OR           = 0;
const int     AND          = 1;
const int     NOR          = 2;
const int     NAND         = 3;
const int     XOR          = 4;
const int     NXOR         = 5;

/* Debugging
boolean       debug        = TRUE;
int           tick         = 0;
int           tickOutCount = 6000;
 */

//  ==================== setup() START ======================
//
//  Setup patch. Enable state of pins as needed.
//  This code will run once at start of patch, right after load.
//
void setup()
{
    /* Debugging
    if (debug == TRUE)
    {
        Serial.begin(9600);
    } */
  
    // set up the digital outputs
    for (int i = 0; i < 2; i++)
    {
        pinMode(digPin[i], OUTPUT);
        digitalWrite(digPin[i], LOW);
    }

    // set up the 8-bit DAC output pins
    for (int i = 0; i < 8; i++)
    {
        pinMode(pinOffset+i, OUTPUT);
        digitalWrite(pinOffset+i, LOW);
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
    // Get the state of the first two controls. These set the 
    // desired types of logic for each Digital Out.
    int aZeroRaw = analogRead(0);
    int aOneRaw = analogRead(1);
    int aZeroType = LogicType(aZeroRaw);
    int aOneType = LogicType(aOneRaw);

    // Get the values for eadh analog inputs to test
    int aTwoInitState = analogRead(2);
    int aThrInitState = analogRead(3);

    // Get the logic state for each output based on the type
    // and the two init values.
    boolean dZeroState = doLogicState(aZeroType, aTwoInitState, aThrInitState);
    boolean dOneState = doLogicState(aOneType, aTwoInitState, aThrInitState);
    /* Debugging
    if (tick > tickOutCount)
    {
        Serial.print("A0 Input: ");
        Serial.print(aZeroRaw);
        Serial.print("  || Processed: ");
        Serial.println(aZeroType);
        Serial.print("A1 Input: ");
        Serial.print(aOneRaw);
        Serial.print("  || Processed: ");
        Serial.println(aOneType);
        Serial.print("A2 Input: ");
        Serial.println(aTwoInitState);
        Serial.print("A3 Input: ");
        Serial.println(aThrInitState);
        Serial.println("#########################");
        Serial.print("D0 Logic Out: ");
        Serial.println(dZeroState);
        Serial.print("D1 Logic Out: ");
        Serial.println(dOneState);
        Serial.println("#########################");
        tick = 0;
    }
    tick += 1; */
    int outState = sumStates(aTwoInitState, aThrInitState);
    dacOutput((outState >> 2));
    digiGateOut(0, dZeroState);
    digiGateOut(1, dOneState);
}

//  =================== convenience routines ===================

int LogicType(int input)
{
    int output = 0; // Default OR
    int stageMax = STAGE_SEP;
    while ((stageMax + 1) < input)
    {
        output++;
        stageMax += STAGE_SEP;
    }
    if (output > 5)
    {
        output = 5;
    }
    return output;
}

boolean doLogicState(int logicType, int initValA, int initValB)
{
    boolean boolA = boolState(initValA);
    boolean boolB = boolState(initValB);
    boolean output = FALSE;
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

int sumStates(int initValA, int initValB)
{
    return constrain((initValA + initValB), MIN_VAL, MAX_VAL);
}

boolean boolState(int initVal)
{
    if (initVal >= HALFWAY)
    {
        return TRUE;
    }
    return FALSE;
}

void digiGateOut(int gate, boolean state)
{
    if (state == TRUE)
    {
        digitalWrite(digPin[gate], HIGH);
    }
    else
    {
        digitalWrite(digPin[gate], LOW);
    }
}

//  dacOutput(long) - deal with the DAC output
//  ------------------------------------------
void dacOutput(long v)
{
    int tmpVal = v;
    bitWrite(PORTD, 5, tmpVal & 1);
    bitWrite(PORTD, 6, (tmpVal & 2) > 0);
    bitWrite(PORTD, 7, (tmpVal & 4) > 0);
    bitWrite(PORTB, 0, (tmpVal & 8) > 0);
    bitWrite(PORTB, 1, (tmpVal & 16) > 0);
    bitWrite(PORTB, 2, (tmpVal & 32) > 0);
    bitWrite(PORTB, 3, (tmpVal & 64) > 0);
    bitWrite(PORTB, 4, (tmpVal & 128) > 0);
}

