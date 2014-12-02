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
        Knob/Jack A2:    Input voltage to be quantized. (Knob at Max).
        Knob/Jack A3:    Gate Frequency (Divisions of Clock)
        Digital Out 1:   Gate 0
        Digital Out 2:   Gate 1
        Clock In:        Clock in (Clock or tempo pulse)
        Analog Out:      Signal (0 - 5v)
    Input Expander:
        Knob A4/Jack A4: Width of Gate D0
        Knob A5/Jack A5: Width of Gate D1
    Output Expander:
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

// Logic Constants:
const int     MAX_VAL      = 1023;
const boolean FALSE        = LOW;
const boolean TRUE         = HIGH;
const boolean SHIFT_UP     = HIGH;
const boolean SHIFT_DOWN   = LOW;
const boolean SHIFT_FOURTH = LOW;
const boolean SHIFT_FIFTH  = HIGH;
const int     FOURTH       = 5;
const int     FIFTH        = 7;
const int     WIDTH_SHIFT  = 512;

// IO, Clock and Arduino Constants
const int     SHIFT_KNOB   = 0;
const int     AMT_KNOB     = 1;
const int     QUANT_KNOB   = 2;
const int     FREQ_KNOB    = 3;
const int     GATE_KNOB1   = 4;
const int     GATE_KNOB2   = 5;
const int     CLOCK_IN     = 2;
const int     MAX_GATE     = 2;
const int     DIG_PINS[2]  = {3,4};
const int     PIN_OFFSET   = 5;           // DAC  -> the first DAC pin (from 5-12)

//  constant for actual 0-5V quantization (vs. >> 4)
const int     QARRAY[61]   = {
  0,   9,   26,  43,  60,  77,  94,  111, 128, 145, 162, 180, 
  197, 214, 231, 248, 265, 282, 299, 316, 333, 350, 367, 384, 
  401, 418, 435, 452, 469, 486, 503, 521, 538, 555, 572, 589, 
  606, 623, 640, 657, 674, 691, 708, 725, 742, 759, 776, 793, 
  810, 827, 844, 862, 879, 896, 913, 930, 947, 964, 981, 998, 
  1015};

// Init values for application data
boolean       upDown       = SHIFT_DOWN;
boolean       noteShift    = SHIFT_FOURTH;
int           noteVolt     = 0;
long          outVolt      = 0;

//  variables for interrupt handling of the clock input
volatile int  clkState     = LOW;
int           clockTick[2] = {1, 1};
float         gateWidth[2] = {.5, .5};

//  variables used to control the current Digital IO output states
int           digState[2]  = {LOW, LOW};  // start with both set low
unsigned long prevMilli[2] = {0, 0};      // the last time of a loop
unsigned long lastTick = 0;               // the time of the last clock pulse
int           triggerTime = 10;           // start at 10ms, but will be set by first/each clock pulse.
/* Debugging
   Comment out (move close) to disable debugging.
 */
boolean       debug        = TRUE;
int           tick         = 0;
int           tickOutCount = 1000;

//  ==================== setup() START ======================
//
//  Setup patch. Enable state of pins as needed.
//  This code will run once at start of patch, right after load.
//
void setup()
{
    /* Debugging
    Comment out (move close) to disable debugging.
    */
    if (debug == TRUE)
    {
        Serial.begin(9600);
    }

    // set up the digital (clock) input
    pinMode(CLOCK_IN, INPUT);

    // set up the digital outputs
    for (int i = 0; i < 2; i++)
    {
        pinMode(DIG_PINS[i], OUTPUT);
        digitalWrite(DIG_PINS[i], LOW);
    }

    // set up the 8-bit DAC output pins
    for (int i = 0; i < 8; i++)
    {
        pinMode(PIN_OFFSET+i, OUTPUT);
        digitalWrite(PIN_OFFSET+i, LOW);
    }

    // Interrupt for clock input.
    attachInterrupt(0, isr, RISING);
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
    upDown = HiLowState(analogRead(SHIFT_KNOB));
    noteShift = HiLowState(analogRead(AMT_KNOB));
    int tmpVolt = analogRead(QUANT_KNOB);
    int gateFreq = analogRead(FREQ_KNOB);
    gateWidth[0] = GetGateTime(analogRead(GATE_KNOB1));
    gateWidth[1] = GetGateTime(analogRead(GATE_KNOB2));
    
    // Handle Clock Trigger
    if (clkState == HIGH)
    {
        clkState = LOW;
        if (lastTick > 0)
        {
            triggerTime = millis() - lastTick;
        }
        lastTick = millis();
        handleTicks(gateFreq);
    }

    // Handle Trigger Turnoff
    handleTrigOff(gateFreq, triggerTime);
    
    if (tmpVolt != noteVolt)
    {
        noteVolt = tmpVolt;
        outVolt = doShift(noteVolt, upDown, noteShift);
    }

    /* Debugging
    Comment out (move close) to disable debugging.
    */
    if (tick > tickOutCount)
    {
        Serial.print("Gate Width 0: ");
        Serial.println(gateWidth[0]);
        Serial.print("Gate Width 1: ");
        Serial.println(gateWidth[1]);
        tick = 0;
    }
    tick += 1;
    dacOutput((outVolt >> 2));
}

//  =================== convenience routines ===================

float GetGateTime(int input)
{
    float temp = (float(input)/1023.0);
    if (temp < .2)
    {
        temp = .2;
    }
    return temp;
}

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
        if (noteIn >= QARRAY[i])
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
    return QARRAY[noteIndex];
}

// Handle Clock

void handleTicks(int gateFreq)
{
    for (int index = 0; index < MAX_GATE; index++)
    {
        clockTick[index]--;
        if (clockTick[index] < 1)
        {
            digState[index] = HIGH;
            prevMilli[index] = millis();
            digitalWrite(DIG_PINS[index], HIGH);
            clockTick[index] = ((gateFreq >> 6) + 1);
        }
    }
}

void handleTrigOff(int gateFreq, int trigger_gate_width)
{
    for (int index = 0; index < MAX_GATE; index++)
    {
        int gate_time = int(float(trigger_gate_width * (gateFreq >> 6) * gateWidth[index]));
        if ((digState[index] == HIGH) && ((millis() - prevMilli[index]) > gate_time))
        {
            digState[index] = LOW;
            digitalWrite(DIG_PINS[index], LOW);
        }
    }
}

//  isr() - quickly handle interrupts from the clock input
//  ------------------------------------------------------
void isr()
{
  // Note: you don't want to spend a lot of time here, because
  // it interrupts the activity of the rest of your program.
  // In most cases, you just want to set a variable and get
  // out.
  clkState = HIGH;
}

//  dacOutput(long) - deal with the DAC output
//  ------------------------------------------
void dacOutput(byte v)
{
    PORTB = (PORTB & B11100000) | (v >> 3);
    PORTD = (PORTD & B00011111) | ((v & B00000111) << 5);
}

