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
const boolean FALSE        = LOW;
const boolean TRUE         = HIGH;
const boolean SHIFT_UP     = HIGH;
const boolean SHIFT_DOWN   = LOW;
const boolean SHIFT_FOURTH = LOW;
const boolean SHIFT_FIFTH  = HIGH;
const int     FOURTH       = 5;
const int     FIFTH        = 7;


//  constants related to the Arduino Nano pin use
const int     pinOffset    = 5;       // DAC     -> the first DAC pin (from 5-12)

//  constant for actual 0-5V quantization (vs. >> 4)
const int     qArray[61] = {
  0,   9,   26,  43,  60,  77,  94,  111, 128, 145, 162, 180, 
  197, 214, 231, 248, 265, 282, 299, 316, 333, 350, 367, 384, 
  401, 418, 435, 452, 469, 486, 503, 521, 538, 555, 572, 589, 
  606, 623, 640, 657, 674, 691, 708, 725, 742, 759, 776, 793, 
  810, 827, 844, 862, 879, 896, 913, 930, 947, 964, 981, 998, 
  1015};

boolean       upDown       = SHIFT_DOWN;
boolean       noteShift    = SHIFT_FOURTH;
int           noteVolt     = 0;
long          outVolt      = 0;

/* Debugging
boolean       debug        = TRUE;
int           tick         = 0;
int           tickOutCount = 1000;
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
    upDown = HiLowState(analogRead(0));

    noteShift = HiLowState(analogRead(1));

    int tmpVolt = analogRead(2);

    if (tmpVolt != noteVolt)
    {
        noteVolt = tmpVolt;
        outVolt = doShift(noteVolt, upDown, noteShift);
    }

    /* Debugging
    if (tick > tickOutCount)
    {
        Serial.print("Note Volt: ");
        Serial.println(noteVolt);
        Serial.print("Out Volt: ");
        Serial.println(outVolt);
        tick = 0;
    }
    tick += 1; */
    dacOutput((outVolt >> 2));
}

//  =================== convenience routines ===================

boolean HiLowState(int input)
{
    if (input <= 511)
    {
        return LOW;
    }
    return HIGH;
}

int getShift(boolean type)
{
    if (type == SHIFT_FOURTH)
    {
        return FOURTH;
    }
    return FIFTH;
}

int shiftIndex(int index, int amt, boolean dir)
{
    if (dir == SHIFT_DOWN)
    {
        return (index - amt);
    }
    return (index + amt);
}

int noteIntex(int noteIn)
{
    int index = 0;
    for (int i = 0; i < 61; i++)
    {
        if (noteIn >= qArray[i])
        {
           index = i;
        }
    }
    return index;
}

int doShift(int noteIn, boolean shiftDir, boolean shiftType)
{
    int noteOut = 0;
    int noteIndex = 0;
    int shiftAmt = getShift(shiftType);
    //quantize
    noteIndex = noteIntex(noteIn);
    // shift by index
    noteIndex = shiftIndex(noteIndex, shiftAmt, shiftDir);
    return qArray[noteIndex];
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

