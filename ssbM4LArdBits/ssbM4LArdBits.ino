/*
Program: ssbM4LArdBits
Description:
    ssbM4LArdBits requires both an ArdCore and an ArdCore Expander modules as well
    as Ableton Live 9. This patch takes advantage of the ardcore expanders output for
    each bit of the data signal. By taking input from the serial connection, we can
    then toggle on or off specific bit outs each of which provides a 10v signal.
    For example, a signal of 4 is equal to bit 1, and a signal of 256 is bit 6 so
    a signal of 260 will toggle both bit 1 and bit 6 on.
    Value to bit map:
        4 -   7  => 0
        8 -  11  => 1
       16 -  19  => 2
       32 -  35  => 3
       64 -  67  => 4
      128 - 131  => 5
      256 - 259  => 6
      512 - 515  => 7
    
    Use this patch with the ssbArdBits Max4Live Device.

    I/O Usage:
        Knob A0:         Unused
        Knob A1:         Unused
        Knob A2/Jack A2: Unused
        Knob A3/Jack A3: Unused
        Digital Out 1:   Unused
        Digital Out 2:   Unused
        Clock In:        Unused
        Analog Out:      Signal (0 - 5v)
    Input Expander:
        Knob A4/Jack A4: Unused
        Knob A5/Jack A5: Unused
    Output Expander:
        Bits 0-7:        Each bit may be set separately. See m4l device ssbArdBits.
    Serial Input:        Input from Live (0 - 1023)

    Created:  Jan 3 2014 by Peter Fawcett (SoundSweepsBy).
        Version 1 - Based off of serial input template

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
const int     MIN_VAL      = -1024;
const int     MAX_VAL      = 1023;
const boolean FALSE        = 0;
const boolean TRUE         = 1;
const int     BAUD_RATE    = 9600;

//  constants related to the Arduino Nano pin use
const int     pinOffset    = 5;       // DAC     -> the first DAC pin (from 5-12)

String        sample       = "";
int           sigOut       = 0;
int           liveInput    = 0;
int           adjustVal    = 0;

// Debugging

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
    // Setup serial input. Max4Live will send data this way.
    Serial.begin(BAUD_RATE);
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
    if (Serial.available() > 0)
    {
        char c = (char)Serial.read();
        if(c == '[')
        {
            sample = "";
        }
        else if(c == ']')
        {
            sigOut = convertStringToInt(sample);
        }
        else
        {
            sample += c;
        }
    }
    dacOutput((sigOut >> 2));
}

//  =================== convenience routines ===================

int convertStringToInt(String str)
{
    char test[str.length() + 1];
    str.toCharArray(test, sizeof(test));
    return constrain(atoi(test), MIN_VAL, MAX_VAL);
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

