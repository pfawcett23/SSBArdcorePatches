/*
Program: ssbQuantShift
Description:
    Shift incoming 1v/o value up or down by either a fifth (7 semitones) or a 4th (5 semitones).
    The shift may be either up or down. Note that due to the 0-5 volt limitation of the Ardcore.
    This gives us a range of C3 - C7. The patch can not shift above C7 or below C3. One can use 
    a the Beasts Chalkboard to shift the result down one or two octives (negative volts).

    I/O Usage:
        Knob A0:         Shift Up or down (LOW is down, HIGH is up).
        Knob A1:         Shifth by 4th or 5th (LOW is 4th, HIGH is 5th).
        Knob/Jack A2: Input voltage to be quantized. (Knob at Max).
        Knob/Jack A3: Unused
        Digital Out 1:   Unused
        Digital Out 2:   Unused
        Clock In:        Unused
        Analog Out:      Signal (0 - 5v)
    Input Expander:
        Knob A4/Jack A4: Unused
        Knob A5/Jack A5: Unused
    Output Expander:
        Knob
        Bits 0-7:        Each bit may be set separately. See m4l device ssbArdBits.
        Analog Out 11:   Unused
        Digital Out 13:  Unused
    Serial:              Unused

    Created:  Jan 11 2014 by Peter Fawcett (SoundSweepsBy).
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
#include "ssbScales.h"

const   bool    SHIFT_UP        = HIGH;
const   bool    SHIFT_DOWN      = LOW;
const   bool    SHIFT_FOURTH    = LOW;
const   bool    SHIFT_FIFTH     = HIGH;
const   int     FOURTH          = 5;
const   int     FIFTH           = 7;

boolean         upDown          = SHIFT_DOWN;
boolean         noteShift       = SHIFT_FOURTH;
int             noteVolt        = 0;
long            outVolt         = 0;

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
    upDown = getCtlHighLow(A0_INPUT);
    noteShift = getCtlHighLow(A1_INPUT);
    int tmpVolt = analogRead(A2_INPUT);
    if (tmpVolt != noteVolt)
    {
        noteVolt = tmpVolt;
        outVolt = doShift(noteVolt, upDown, noteShift);
    }
    dacOutput(outVolt);
}

//  =================== convenience routines ===================

int doShift(int noteIn, boolean shiftDir, boolean shiftType)
{
    int noteOut = 0;
    int noteIndex = 0;
    int shiftAmt = getShift(shiftType);
    //quantize
    noteIndex = noteIntex(noteIn);
    // shift by index
    noteIndex = shiftIndex(noteIndex, shiftAmt, shiftDir);
    return QNOTES[noteIndex];
}

int noteIntex(int noteIn)
{
    int index = 0;
    for (int i = 0; i < NOTE_COUNT; i++)
    {
        if (noteIn >= QNOTES[i])
        {
           index = i;
            if ((i + 1 < NOTE_COUNT) && (noteIn < QNOTES[i+1]))
            {
               break;
            }
        }
    }
    return index;
}

int shiftIndex(int index, int amt, boolean dir)
{
    if (dir == SHIFT_DOWN)
    {
        return (index - amt);
    }
    return (index + amt);
}

int getShift(boolean type)
{
    if (type == SHIFT_FOURTH)
    {
        return FOURTH;
    }
    return FIFTH;
}

