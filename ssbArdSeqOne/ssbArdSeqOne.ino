/* Program: ssbArdSeqOne
 * ============================================================================
 *
 * This sequencer uses set patterns of note shifts. These sequences are hard 
 * coded and are selected on sketch load/reload. There may be up to 8 sequences 
 * (A0 is 1-4 / 5-8 and A1 is Hi / Lo). The sequences are all 16 notes. 
 *
 * Note values may be one of the following:
 *    0  : Play the root note.
 *    #  : Play 1 to 12 notes above the root note (if 
 *         the specified note is higher than the allowed
 *         voltage, it will be dropped an octave.
 *    -# : Play 1 to 12 notes below the root note (if
 *         the specified note is lower than the allowed
 *         voltage, it will be raised an octave.
 *    512; A magic number to represent a rest.
 *    So a sequence could be something like:
 *    {0, 7, 7, 512, -4, 512, 5...}
 *
 * The Gates (D0 and D1) are set to divisions of the note clock trigger pulse. 
 * This means that the first time through, the sequencer can only guess at the 
 * tempo. Once it has two pulses it knows the speed and will then do (in 
 * clockwise direction) 1n, 1/2n, 1/4n, 1/8n, 1/16n.
 *
 * Note: A small padding is alwasy subtracted from the gate to allow it to have
 *     a falling / rising edge before the next gate. This pad is 10ms.
 *
 * A new clock pulse input will never inerrupt an existing gate, allowing for 
 * variable sequences using the expander.
 *
 * The expander will allow this to be tweaked in two ways.
 *     When set to > 512 (via CV) the gate will be expanded. 
 *         It will multiplied by *2, *4 and *8.
 *     When set to < 512 (via CV) the gate will be stuttered.
 *         It will be divided by /2, /4, and /8.
 *
 * Thus, if a 1/2 note gate is playing and the expander is set to *2, the gate
 * will be the full note. If it's * 8 it will be the length of 4 full pulses
 * of the input clock (minus the 5ms pad mentioned above). This means that
 * with CV one can play sequences like 1/2n, 1/2n, 1n, 2n, 1/2n, 4n...
 * On the other hand, with the same 1/2 note gate playing, if the expander is
 * set to /2, the half note will be divided in half, playing 1/4, 1/4 (with a
 * 5ms gap between).
 *
 * This then allows (with cv control) note gate widths like:
 *     1/2n, [1/4n,1/4n], 1n, 1n, [1/8n,1/8n,1/8n,1/8n], 2n....
 *
 * As gate width it's self can also be modulated, this allows for very 
 * expressive and / or randomistic sequences (with random or stepped lfos). 
 * The state of a given note is treated in a 'sample and hold' mannor. If both 
 * gates are closed (as the Quantized CV is Monophonic) and there is a clock
 * pulse, the next quantized note value, as well as all gate values are set
 * and the gate width and stutter count are set. These values are then used
 * till both gates close again.
 * 
 * As both gates can be timed and expanded / stepped differently, this allows
 * one to have normal note lengths for the vca envelope, but stepped for the
 * vcf envelope (for example).
 * 
 * ============================================================================
 *
 *  I/O Usage:
 *    Knob A0:         Root Note     (C-B#)
 *    Knob A1:         Root Octave   (C3-C8)
 *    Knob A2/Jack A2: Gate D0 Width
 *    Knob A3/Jack A3: Gate D1 Width
 *    Digital Out 1:   Gate D0 Out
 *    Digital Out 2:   Gate D1 Out
 *    Clock In:        Note Trigger
 *    Analog Out:      Quantized Note
 *  Input Expander: 
 *    Knob A4/Jack A4: Gate D0 Stutter/Adder
 *    Knob A5/Jack A5: Gate D0 Stutter/Adder
 *  Output Expander:
 *    Bits 0-7:        Not Used
 *
 *  Created:  Nov 22 2014 by Peter Fawcett (SoundSweepsBy)
 * ============================================================================
 *  License:
 *
 *  This software is licensed under the Creative Commons
 *  "Attribution-NonCommercial license. This license allows you
 *  to tweak and build upon the code for non-commercial purposes,
 *  without the requirement to license derivative works on the
 *  same terms. If you wish to use this (or derived) work for
 *  commercial work, please contact Peter Fawcett at our website
 *  (www.SoundSweepsBy.com).
 *  
 *  For more information on the Creative Commons CC BY-NC license,
 *  visit http://creativecommons.org/licenses///
 *===========================================================================*/

/*=============================================================================
 * Globals and Constants
 *===========================================================================*/

const int     CLOCK_IN             = 2; 
const int     DIG_PINS[2]          = {3, 4};
const int     PIN_OFFSET           = 5;
const int     A0_INPUT             = 0;
const int     A1_INPUT             = 1;
const int     A2_INPUT             = 2;
const int     A3_INPUT             = 3;
const int     A4_INPUT             = 4;
const int     A5_INPUT             = 5;
const int     GATE_WIDTH_CTLS[2]   = {A2_INPUT, A3_INPUT};
const int     SUTTER_CTLS[2]       = {A4_INPUT, A5_INPUT};

const int     NOTE_COUNT           = 60;
// constant for actual 0-5V quantization (vs. >> 4) const int qArray[61] = { };
//    C   C#    D   D#    E    F   F#    G   G#    A   A#    B
const int     QNOTES[(NOTE_COUNT + 1)]   = {
      0,   9,  26,  43,  60,  77,  94, 111, 128, 145, 162, 180, // OCT3
    197, 214, 231, 248, 265, 282, 299, 316, 333, 350, 367, 384, // OCT4
    401, 418, 435, 452, 469, 486, 503, 521, 538, 555, 572, 589, // OCT5
    606, 623, 640, 657, 674, 691, 708, 725, 742, 759, 776, 793, // OCT6
    810, 827, 844, 862, 879, 896, 913, 930, 947, 964, 981, 998, // OCT7
   1015};

const int     MAX_STEP             = 15;
const int     MIN_STEP             = 0;
const int     REST_NOTE            = 512;

// values may be:
//  0       - play root note
//  1 - 24  - play note 1 - 24 semitones above root
// -1 - -24 - play note 1 - 24 semitones below root
//  512     - play a rest (no note, 'magic number')
//    S1,  S2,  S3,  S4,  S5,  S6,  S7,  S8,  S9, S10, S11, S12, S13, S14, S15, S16
const int     SONGPAT[8][(MAX_STEP + 1)] = {
    {  0,   5,   7,   9,   12,   512,   0,   -5,   -7,   -9,   -12,   512,   0,   5,   0,   -5},
    {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}};

const int     MAX_VAL              = 1023;
const int     MIN_VAL              = 0;
const int     HALFWAY              = MAX_VAL / 2;
const int     PAD_MS               = 10;              // ms
const int     SGATE_COUNTS[7]      = {8, 4, 2, 1, 2, 4, 8};

int           qNoteVal             = 0;

int           pattIndex            = 0;
int           stepIndex            = 0;

long          lastTrig             = 0;
long          trigTempo            = 500;            // ms trig to trig, default is 500 or 2 trig per sec or 120bpm...
long          gateStart[2]         = {0, 0};
int           gateDur[2]           = {0, 0};
boolean       gateOver[2]          = {true, true};
boolean       gateOn[2]            = {false, false};
int           stutterCount[2]      = {0, 0};
int           stutterGate[2]       = {1, 1};         // number of sequenced gates. default is 1, values 2, 4, 8 are for stutter steps.
boolean       noteOn               = false;

//  variables for interrupt handling of the clock input
volatile int  clkState             = LOW;

/*=============================================================================
 * setup() START 
 * Setup patch. Enable state of pins as needed.
 * This code will run once at start of patch, right after load.
 *===========================================================================*/

void setup()
{
    // Read the sketch index from A0 Input and the A1 Input.
    pattIndex = SetPattern(analogRead(A0_INPUT), analogRead(A1_INPUT));

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
        pinMode(PIN_OFFSET + i, OUTPUT);
        digitalWrite(PIN_OFFSET + i, LOW);
    }

    // Interrupt for clock input.
    attachInterrupt(0, isr, RISING);
}

/*=============================================================================
 * setup() END
 *===========================================================================*/

/*=============================================================================
 * loop() START
 * Master Loop.
 * Loop will be called over and over with out pause.
 * Main logic of patch.
 * ----------------------------------------------------------------------------
 * Logic:
 *     - on clock input
 *         - get step index
 *         - get root note
 *         - get note index / magic number
 *         - if note index
 *             - get quantized note DAC out
 *             - set gate start for D0/D1
 *             - set gate D0/D1 data
 *                 - base length
 *                 - expand / stutter
 *                     - get start time
 *                     - get dur
 *                    - set stutter count
 *     - if no gate and start time
 *===========================================================================*/
 
void loop()
{
    int noteIndex = 0;
    int noteOffset = 0;
    int stutterIndex = 0;

    // if high, we have a note to generate...
    if ((clkState == HIGH) && (noteOn == false))
    {
        // Set Clock State and Note State
        // Get Root Note
        // Get Gate Start
        // Get Gate Sutter / Expand State
        // Get Gate Duration
        // Update Step Sequencer State
        // Update Trigger Tempo
        noteOn = true;
        clkState = LOW;
        
        // Process Note Data
        noteIndex = GetNoteIndex();
        noteOffset = GetNextNote(noteIndex, stepIndex, pattIndex);
        // If not a rest note
        if (noteOffset == REST_NOTE)
        {
            noteOn = false;
            for (int i = 0; i < 2; i++)
            {
                gateOn[i] = false;
                gateOver[i] = true;
                gateStart[i] = 0;
                stutterCount[i] = 0;
                gateDur[i] = 0;
                stutterGate[i] = 0;
            }
        }
        else
        {
            // Update Quantized CV Note Data
            qNoteVal = QNOTES[noteOffset];
            
            // Update Gate Information
            for (int i = 0; i < 2; i++)
            {
                gateStart[i] = millis();
                stutterCount[i] = 1;
                gateOn[i] = true;
                gateOver[i] = false;
                stutterIndex = map(analogRead(SUTTER_CTLS[i]), MIN_VAL, MAX_VAL, 0, 6);
                gateDur[i] = GetGateWidth(i, trigTempo, stutterIndex);
                stutterGate[i] = GetStutterCount(stutterIndex);
            }
        }
        // Update Step Sequencer State
        UpdateStepIndex();
        
        // Update Trigger Tempo Data
        UpdateTrigTempo();
    }
    for (int i = 0; i < 2; i++)
    {
        if (noteOn == true)
        {
            gateOn[i] = CalcGateState(i, gateOn[i]);
        }
        if (gateOn[i] == true)
        {
            digitalWrite(DIG_PINS[i], HIGH);
        }
        else
        {
            digitalWrite(DIG_PINS[i], LOW);
        }
    }
    noteOn = CalcNoteState(noteOn);
    dacOutput(qNoteVal);
    //debug_data();
    /* Debugging
    Comment out (move close) to disable debugging.
    if (debug == true)
    {
        Serial.println("===========================");
        Serial.print("Note ON 2: ");
        Serial.println(noteOn);
        Serial.println("===========================");
    }
    */
}

/*=============================================================================
 * loop() END
 *===========================================================================*/

/*=============================================================================
 * General Sketch Routines
 *===========================================================================*/

int SetPattern(int a0_state, int a1_state)
{
    // A0 Sets a value of 0 - 3
    // A1 Sets a value of 0 - 1
    // So if A1 is 0, output is 0 - 3
    // If A2 is 1,    output is 4 - 7
    // Giving access to a total of 8 patterns...
    int a0_base = map(analogRead(A0_INPUT), MIN_VAL, MAX_VAL, MIN_VAL, 3);
    int a1_shift = map(analogRead(A1_INPUT), MIN_VAL, MAX_VAL, MIN_VAL, 1);
    return (a0_base + a1_shift);
}

int GetNoteIndex()
{
    int rootNote = map(analogRead(A0_INPUT), MIN_VAL, MAX_VAL, MIN_VAL, 11);
    int octaveIndex = map(analogRead(A1_INPUT), MIN_VAL, MAX_VAL, MIN_VAL, 4);
    int rootIndex = (rootNote + (octaveIndex * 12));
    if (rootIndex > NOTE_COUNT)
    {
        // if outside upper bound, drop an octave...
        // cant go negative as input can only be > 0.
        rootIndex =- 12;
    }
    return rootIndex;
}

int GetNextNote(int iRoot, int iStep, int iPatt)
{
    int semiToneOffset = SONGPAT[iPatt][iStep];
    int noteIndex = REST_NOTE;
    if (semiToneOffset != REST_NOTE)
    {
        noteIndex = (iRoot + semiToneOffset);
        if (noteIndex > NOTE_COUNT)
        {
            while (noteIndex > NOTE_COUNT)
            {
                noteIndex -= 12;
            }
        }
        else if (noteIndex < MIN_VAL)
        {
            while (noteIndex < MIN_VAL)
            {
                noteIndex += 12;
            }
        }
    }
    return noteIndex;
}

int GetStutterCount(int index)
{
    /* Stutter Index Values:
     *  - 0 : 1/8
     *  - 1 : 1/4
     *  - 2 : 1/2
     *  - 3 : 1
     *  - 4 : 2
     *  - 5 : 4
     *  - 6 : 8
     */
     int sCount = 1;
     if (index < 3)
     {
         sCount = SGATE_COUNTS[index];
     }
     return sCount;
}

int GetGateWidth(int index, long tempo, int stutterIndex)
{
    int gDivision = map(analogRead(GATE_WIDTH_CTLS[index]), MIN_VAL, MAX_VAL, 4, 1);
    int gTime = int(tempo / gDivision);
    if (stutterIndex > 4)
    {
        // Extend gate time.
        gTime = gTime * SGATE_COUNTS[stutterIndex];
    }
    else if (stutterIndex < 4)
    {
        // Divide gate time.
        gTime = int(gTime / SGATE_COUNTS[stutterIndex]);
    }
    return gTime;
}

boolean CalcGateState(int gateIndex, boolean gateState)
{
    boolean gOn = false;
    // if gate on
    //     gate off
    //     if start + (width-pad) > current time
    //         gate on
    //     if gate off
    //         if stutter count < stutter gate count
    //             update start to be (start + width)
    //         else
    //             gate over
    // else if gate off and stutter count < stutter gate count and start <= current time
    //     stutter count = stutter count + 1
    //     gate on
    // else
    //     gate off
    // gateStart[i], gateWidth[i], stutterCount[i], stutterGate[i]
    //            gateOn[i] = true;
    //            gateOver[i] = false;
    if (gateState == true)
    {
        if ((gateStart[gateIndex] + gateDur[gateIndex] - PAD_MS) > millis())
        {
            gOn = true;
        }
        if (gOn == false)
        {
            if (stutterCount[gateIndex] < stutterGate[gateIndex])
            {
                gateStart[gateIndex] = gateStart[gateIndex] + gateDur[gateIndex];
            }
            else
            {
                gateOver[gateIndex] = true;
            }
        }
    }
    else if ((stutterCount[gateIndex] < stutterGate[gateIndex]) && (gateStart[gateIndex] <= millis()))
    {
        gOn = true;
        stutterCount[gateIndex]++;
    }
    return gOn;
}

boolean CalcNoteState(boolean curNoteOn)
{
    boolean localNoteOn = false;
    if (curNoteOn == true)
    {
        if ((gateOver[0] == false) || (gateOver[1] == false))
        {
            localNoteOn = true;
        }
    }
    return localNoteOn;
}

void UpdateStepIndex()
{
    stepIndex++;
    if (stepIndex > MAX_STEP)
    {
        stepIndex = 0;
    }
}

void UpdateTrigTempo()
{
    long curMS = millis();
    if (lastTrig > 0)
    {
        trigTempo = curMS - lastTrig;
    }
    lastTrig = curMS;
}

/*=============================================================================
 * Utility Routines
 *===========================================================================*/

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
