/*
Program: ssbArdLive
Description:
    ssbArdLive is a template for creating ardcore patches that support serial input
    from Max4Live ssbArdLive Devices.
    Data is sent via the Serial input to the Ardcore. In this case the input is simply
    converted to an output value which is then written to the DAC output.
    The input data is constrained to int values between 0 and 1023.
    It should be sent in the format [0] where '[' is the start marker and ']' is the
    end marker.
    The A0 Knob scales the value from the serial input. It is treated as a 0-100%
    control.
    Output is sent via both DAC outputs.
    This is a simple patch currently. It may be used as a template for more comples
    patches.
    
    See the ssbArdCoreVel Max4Live patch for an example.

    I/O Usage:
        Knob A0:         Scale Input (0 - 100%)
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
        Bits 0-7:        Each bit of the envelope
    Serial Input:        Input from Live (0 - 1023)

    Created:  Nov 16 2013 by Peter Fawcett (SoundSweepsBy).
        Version 1 - Serial input. Parser. Scaling by A0, Output to DAC.

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
const int     MIN_VAL      = 0;
const int     MAX_VAL      = 1023;
const boolean FALSE        = 0;
const boolean TRUE         = 1;

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
    Serial.begin(9600);
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
            liveInput = convertStringToInt(sample);
            // showSample(liveInput);              // Debug, uncomment if needed.
        }
        else
        {
            sample += c;
        }
    }
    sigOut = scaleSig(liveInput, analogRead(0));
    // showSample(sigOut);                         // Debug, uncomment if needed.
    // showADials();                               // Debug, uncomment if needed.
    dacOutput((sigOut >> 2));
}

//  =================== convenience routines ===================

int convertStringToInt(String str)
{
    char test[str.length() + 1];
    str.toCharArray(test, sizeof(test));
    return constrain(atoi(test), MIN_VAL, MAX_VAL);
}

int scaleSig(int signal, int scale)
{
    adjustVal = map(scale, 0, 1023, 0, 100);
    return int((float)signal * (float)(adjustVal/100.0));
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

//  =================== debug routines =========================
/*
void showSample(int var)
{
    Serial.print("Sample => ");
    Serial.println(var);
    Serial.println("");
}

void showFloat(float var)
{
    Serial.print("Sample => ");
    Serial.println(var);
    Serial.println("");
}

void showADials(void)
{
    Serial.println("");
    Serial.print("A0     => ");
    Serial.println(analogRead(0));
    Serial.println("");
    Serial.print("A1     => ");
    Serial.println(analogRead(1));
    Serial.println("");
    Serial.print("A2     => ");
    Serial.println(analogRead(2));
    Serial.println("");
    Serial.print("A3     => ");
    Serial.println(analogRead(3));
    Serial.println("");
    Serial.print("A4     => ");
    Serial.println(analogRead(4));
    Serial.println("");
    Serial.print("A5     => ");
    Serial.println(analogRead(5));
    Serial.println("");
}
*/
