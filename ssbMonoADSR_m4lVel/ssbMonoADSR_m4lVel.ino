/* Program: ssbMonoADSR_EX
 *
 * This is an adaptation of the ADSR_ENV from the SnazzyFX libarary for use with the
 * Ardcore AND Max/MSP or Max4Live. The free runtime version of max/msp can be used.
 *
 * The peak level (attack peak) and sustain level are scaled by incoming serial data.
 * This data would most likely be sent by the ssbArdCoreVel.maxpat patch or the 
 * ssbArdCoreVel.amxd device. This allows scaling of notes via velocity data.
 * These patches are in the ssbArdLive folder.
 * 
 * Attack (A0), Decay (A1), Gate (A2) and Release (A3).
 * Gate and release should be CV controlable.
 *
 * Ccurrently the sustain value is fixed in the program, (modify variable 'sustainAmount' below.
 * Attack (A0), Decay (A1) and Release (A3) are mapped to knobs. 
 * Knob A2 can be used as a manual gate if nothing is plugged into A2.
 *
 * Note: Monophonic. Can handle only one note gate at a time. If it get's more than one, it WILL
 *       produce unpredictable results.
 *
 *  I/O Usage:
 *    Knob 1:          Attack
 *    Knob 2:          Decay
 *    Knob 3/Jack A2:  Gate (knob and/or CV)
 *    Knob 4/Jack A3:  Release (knob and/or CV)
 *    Digital Out 1:   Unused
 *    Digital Out 2:   Unused 
 *    Clock In:        Unused
 *    Analog Out:      ADSR OUT
 *  Input Expander: 
 *    Knob A4/Jack A4: Unused
 *    Knob A5/Jack A5: Unused
 *  Output Expander:
 *    Bits 0-7:        Each bit of the envelope
 *
 *  Created:  FEB 2012 by Dan Snazelle
 *  Adapted:  Nov 10 2013 by Peter Fawcett
 *                         - Modified from original to support sustain on A2 and gate in on A4
 *            Nov 16 2013  - Re-write envelope state support.
 *                         - Refactored loop into more supportable helper functions.
 *            Jan 01 2014  - Fixed A number of bugs.
 *                         - Updated the timing calculation code for attack, decay and release.
 *  ============================================================
 *
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
 */
 
// Envelope States:
const int     ATTACK       = 0;
const int     DECAY        = 1;
const int     SUSTAIN      = 2;
const int     RELEASE      = 3;

const int     ENVELOPE_MAX = 1023;
const boolean OFF          = 0;
const boolean ON           = 1;
const int     GATE_ON      = 650;  // Amount above which gate is considered on.
const int     GATE_OFF     = 600;  // Amount below which gate is considered off.
                                   // Set Gate off lower to help prevent some jitter.
const int     BAUD_RATE    = 9600;
//  constants related to the Arduino Nano pin use
const int     pinOffset    = 5;       // DAC     -> the first DAC pin (from 5-12)

// EDIT THIS VALUE TO CHANGE SUSTAIN AMOUNT!!!!
int           SUSTAIN_AMOUNT = 70;    // Percent value (int)  0 - 100.

// Envelope State:
// - 0 : attack phase     [gate on       -> max envelope  (or gate off)]
// - 1 : decay phase      [max envelope  -> sustain level (or gate off)]
// - 2 : sustain phase    [sustain level -> gate off                   ]
// - 3 : release phase    [gate off      -> 0             (or gate on) ]
int envelopeState = ATTACK;
// Current Gate State
boolean gateState = OFF;

// Current Envelope
float envelopeVal = 0.0;

// Envelope Values
float attackValue  = 0.0;
float decayValue   = 0.0;
float sustainValue = 0.0;
float releaseValue = 0.0;

String rawVelStr = "";
int currentVelocity = ENVELOPE_MAX;

/*  ==================== setup() START ======================
 *
 *  Setup patch. Enable state of pins as needed.
 *  This code will run once at start of patch, right after load.
 */
void setup()
{
    Serial.begin(BAUD_RATE);
    // set up the 8-bit DAC output pins
    for (int i = 0; i < 8; i++)
    {
        pinMode(pinOffset+i, OUTPUT);
        digitalWrite(pinOffset+i, LOW);
    }
}
//  ==================== setup() END =======================


/*  ==================== loop() START =======================
 *
 *  Master Loop.
 *  Loop will be called over and over with out pause.
 *  Main logic of patch.
 */
void loop()
{
    /*
       - if A2 connected to note on/off gate
           Will toggle between 0 and the max for the gate (941 in my case).
           A2 Knob will adjust gate amount.
           Note if it is too low, the gate will not trigger.
       - If A2 is not connected to a gate signal.
           Knob will adjust gate amount between 0 and 1023.
    */
    gateState = gate_state(gateState, analogRead(2));

    // Check the serial input to see if there in new velocity data to process.
    process_velocity();
    /*
        get current state of controls.
        - Note that attack, decay and release are the ammounts to add to/subtract from
          the current envelope value.
          Larger values on the knobs, become smaller add/subtract increments and thus
          longer attack/decay/releases...
        - But sustain is not an increment. it's a fixed value to sustain at. Thus
          use the sustainBase (for now, may use gateValue in future)
    */
    attackValue = map_float(analogRead(0), 0, 1023, 204.6, .5);
    decayValue = map_float(analogRead(1), 0, 1023, 204.6, .5);
    sustainValue = (int)(currentVelocity * (float)(SUSTAIN_AMOUNT / 100.0));
    releaseValue = map_float(analogRead(3), 0, 1023, 204.6, .5);
    
    int envLoopState = ATTACK;

    if (gateState == ON)
    {
        if (envelopeState == RELEASE)
        {
            envLoopState = ATTACK;
        }
        if (envelopeState == ATTACK)     // Attack
        {
            envLoopState = ATTACK;
            envelopeVal = do_attack_inc(envelopeVal, attackValue);
        }
        else if (envelopeState == DECAY) // Decay
        {
            envLoopState = DECAY;
            envelopeVal = do_decay_dec(envelopeVal, decayValue, sustainValue);
        }
        else                             // Sustain
        {
            envLoopState = SUSTAIN;
            envelopeVal = sustainValue;
        }
    }
    else                                 // Release
    {
        envLoopState = RELEASE;
        envelopeVal = do_release_dec(envelopeVal, releaseValue);
    }

    // Keep track of state based on envelope, current state and gate state.
    envelopeState = envelope_state(envelopeState, envLoopState, envelopeVal, sustainValue);

    // Write envelope to DAC.
    dacOutput(((long)envelopeVal >> 2));
}

//  ==================== loop() END =======================

float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void process_velocity()
{
    if (Serial.available() > 0)
    {
        char c = (char)Serial.read();
        if(c == '[')
        {
            rawVelStr = "";
        }
        else if(c == ']')
        {
            currentVelocity = convert_string_to_int(rawVelStr);
        }
        else
        {
            rawVelStr += c;
        }
    }
}

int convert_string_to_int(String str)
{
    char test[str.length() + 1];
    str.toCharArray(test, sizeof(test));
    return constrain(atoi(test), 0, ENVELOPE_MAX);
}

boolean gate_state(boolean gate, int currentGate)
{
    if (currentGate > GATE_ON)
    {
        gate = ON;
    }
    else if (currentGate < GATE_OFF)
    {
        gate = OFF;
    }
    return gate;
}

int envelope_state(int state, int currentState, float envelopeVal, float sustainVal)
{
    if (envelopeVal >= currentVelocity)
    {
        state = DECAY;
    }
    else if ((state == DECAY) && (envelopeVal <= sustainVal))
    {
        state = SUSTAIN;
    }
    else if (envelopeVal <= 0)
    {
        state = ATTACK;
    }
    else if (state != currentState)
    {
        state = currentState;
    }
    return state;
}

float do_attack_inc(float envelopeVal, float attackValue)
{
    envelopeVal = (envelopeVal + attackValue); //step up to full level
    if (envelopeVal >= currentVelocity)
    {
        envelopeVal = currentVelocity;
    }
    return envelopeVal;
}

float do_decay_dec(float envelopeVal, float decayValue, float sustainValue)
{
    if (envelopeVal > sustainValue) //stop at sustain level
    {
        // Decriment by decay value.
        envelopeVal = (envelopeVal - decayValue);
    }
    // if envelope is < 0 set to 0. Shouldn't get here unless sustain is 0.
    if (envelopeVal <= 0)
    {
        envelopeVal = 0;
    }
    else if (envelopeVal < sustainValue)
    {
        // if current envelope got set lower than sustain, set it to sustain.
        envelopeVal = sustainValue;
    }
    return envelopeVal;
}

float do_release_dec(float envelopeVal, float releaseValue)
{
    if (envelopeVal > 0)
    {
        // calculate new value of releasse envelope.
        envelopeVal = envelopeVal - releaseValue;
    }
    if (envelopeVal < 0)
    {
        envelopeVal = 0; // if below 0, set to 0.
    }
    return envelopeVal;
}
    

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

