// Program: ssbMonoADSR
// Ccurrently the sustain value is fixed in the program, (modify variable 'sustainAmount' below.
// Attack (A0), Decay (A1) and Release (A3) are mapped to knobs. 
// Knob A2 can be used as a manual gate if nothing is plugged into A2.
//
// Note: Monophonic. Can handle only one note gate at a time. If it get's more than one, it WILL
//       produce unpredictable results.
//
//  I/O Usage:
//    Knob 1: Attack
//    Knob 2: Decay
//    Knob 3/Jack A2: GATE INPUT
//    Knob 4/Jack A3: RELEASE KNOB AND/OR RELEASE CV
//    Digital Out 1:
//    Digital Out 2: 
//    Clock In:
//    Analog Out:ADSR OUT
//  Input Expander: unused
//  Output Expander:unused
//
//  Created:  FEB 2012 by Dan Snazelle
//  Adapted:  Nov 2013 by Peter Fawcett
//  
//  ============================================================
//
//  License:
//
//  This software is licensed under the Creative Commons
//  "Attribution-NonCommercial license. This license allows you
//  to tweak and build upon the code for non-commercial purposes,
//  without the requirement to license derivative works on the
//  same terms. If you wish to use this (or derived) work for
//  commercial work, please contact 20 Objects LLC at our website
//  (www.20objects.com).
//
//  For more information on the Creative Commons CC BY-NC license,
//  visit http://creativecommons.org/licenses/
//
//

// Envelope States:
const int     ATTACK       = 0;
const int     DECAY        = 1;
const int     SUSTAIN      = 2;
const int     RELEASE      = 3;

const int     ENVELOPE_MAX = 1023;

//  constants related to the Arduino Nano pin use
const int     pinOffset    = 5;       // DAC     -> the first DAC pin (from 5-12)

// Adjust for taste
int gateValueoldOn  = 120;
int gateValueoldOff = 100;
int sustainAmount = 70;        // Precent int value 0 - 100.

// Envelope State:
// - 0 : attack phase     [gate on       -> max envelope  (or gate off)]
// - 1 : decay phase      [max envelope  -> sustain level (or gate off)]
// - 2 : sustain phase    [sustain level -> gate off                   ]
// - 3 : release phase    [gate off      -> 0             (or gate on) ]
int envelopeState = ATTACK;

/* Un-Comment For Debugging
// Debugging
const boolean FALSE        = 0;
const boolean TRUE         = 1;
boolean debug = FALSE;
int tick = 0;
int tickOutCount = 50;
*/

// Current Envelope
int envelopeVal = 0;
// Current Gate Value
int gateValue = 0;

// Envelope Values
float attackValue  = 0.0;
float decayValue   = 0.0;
float sustainValue = 0.0;
float releaseValue = 0.0;


//  ==================== setup() START ======================
//
//  Setup patch. Enable state of pins as needed.
//  This code will run once at start of patch, right after load.
//
void setup()
{
    /* Un-Comment For Debugging.
    if (debug)
    {
        Serial.begin(9600);
    }
    */
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
    /* gateValue:
           - if A2 connected to note on/off gate
               Will toggle between 0 and the max for the gate (941 in my case).
               A2 Knob will adjust gate amount.
               Note if it is too low, the gate will not trigger.
           - If A2 is not connected to a gate signal.
               Knob will adjust gate amount between 0 and 1023.
    */
    gateValue = analogRead(2);

    /*
        get current state of controls.
        - Note that attack, decay and release are the ammounts to add to/subtract from
          the current envelope value.
          Larger values on the knobs, become smaller add/subtract increments and thus
          longer attack/decay/releases...
        - But sustain is not an increment. it's a fixed value to sustain at. Thus
          use the sustainBase (for now, may use gateValue in future)
    */
    attackValue = 255.0 / (analogRead(0) + 5);
    decayValue = 255.0 / (analogRead(1) + 5);
    sustainValue = ENVELOPE_MAX * (float)(sustainAmount / 100.0);
    releaseValue = 255.0 / (analogRead(3) + 5);
  
    if (gateValue > gateValueoldOn)
    {
        if (envelopeState == RELEASE)
        {
            envelopeState == ATTACK;  // We were re-triggered before the end of the release.
        }
        if (envelopeState == ATTACK) // Attack
        {
            envelopeVal = (envelopeVal + attackValue); //step up to full level
            if (envelopeVal >= ENVELOPE_MAX)
            {
                envelopeState = DECAY; // Attack Over, State Set to Decay.
                envelopeVal = ENVELOPE_MAX;
            }
        }    //switch to decay
        else if (envelopeState == DECAY) // Decay
        {
            if (envelopeVal > sustainValue) //stop at sustain level
            {
                envelopeVal = (envelopeVal - decayValue);
            }
            else
            {
                if (envelopeVal <= 0)  // Handle sustain set to 0.
                {
                    envelopeVal = 0;
                    envelopeState = ATTACK; // reset for next gate on.
                }
                else
                {
                    envelopeState = SUSTAIN;
                    envelopeVal = sustainValue;
                }
            }
        }
        else
        {
            envelopeVal = sustainValue;
        }
    }
    else if (gateValue < gateValueoldOff)
    {
        if (envelopeState != RELEASE)
        {
            envelopeState = RELEASE;
        }
        if (envelopeVal > 0)
        {
            envelopeVal = envelopeVal - releaseValue; //gate is done, do the release
        }
        else
        {
            envelopeVal = 0;
            envelopeState = ATTACK; // reset for next gate on.
        }
    }

    dacOutput((envelopeVal >> 2));

    /* Un-Comment For Debugging
    if (debug == TRUE)
    {
        if (tick == tickOutCount)
        {
            Serial.print("Envelope Value      => ");
            Serial.println(envelopeVal);
            Serial.println("");
            tick = 0;
        }
        tick++;
    }
    */
}


//  ==================== loop() END =======================

//  =================== convenience routines ===================

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

