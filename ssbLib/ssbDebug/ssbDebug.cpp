/*
  ssbDebug.cpp - An object to use for debugging ArdCore and Arduino
    patches. Uses Serial Output. Manages all the complexity to allow
    for clean patch code with all debugging uck hidden away.

  Created by Peter Fawcett, Dec 10. 2014.
    Version 0.1: Created basic ssbDebug Obect

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

#include "Arduino.h"
#include "ssbDebug.h"

// variables created by the build process when compiling the sketch
// used by getFreeMem call.
extern int __bss_end;
extern void *__brkval;

ssbDebug::ssbDebug()
{
    _debug_on = false;          // Start with debug off.
    _baud_rate = 9600;          // Default baud rate.
    _ms_freq = true;            // Default is to use time (it's easier to manage).
    _frequency = 500;           // default is every half a second.
    _last_out = millis();       // Current time. Will be reset each time the frequency passes.
    _tick_count = 0;            // Default to 0 as it is not not used in the default state.
    _current_tick = 0;          // Default to 0 as it is not not used in the default state.
    _do_print = false;
}

ssbDebug::ssbDebug(unsigned int frequency)
{
    _debug_on = false;          // Start with debug off.
    _baud_rate = 9600;          // Default baud rate.
    _ms_freq = true;            // Default is to use time (it's easier to manage).
    _frequency = frequency;     // Set to specified frequency.
    _last_out = millis();       // Current time. Will be reset each time the frequency passes.
    _tick_count = 0;            // Default to 0 as it is not not used in the default state.
    _current_tick = 0;          // Default to 0 as it is not not used in the default state.
    _do_print = false;
}

ssbDebug::ssbDebug(unsigned int frequency, bool use_ms)
{
    _debug_on = false;          // Start with debug off.
    _baud_rate = 9600;          // Default baud rate.
    _ms_freq = use_ms;          // Default is to use time (it's easier to manage).
    if (_ms_freq == true)
    {
        _frequency = frequency; // Set to specified frequency.
        _last_out = millis();   // Current time. Will be reset each time the frequency passes.
        _tick_count = 0;        // Default to 0 as it is not not used in the current state.
        _current_tick = 0;      // Default to 0 as it is not not used in the current state.
    }
    else
    {
        _frequency = 0;         // Set time to 0 as it's not used in current state.
        _last_out = 0;          // Set time to 0 as it's not used in current state.
        _tick_count = frequency; // Set tick count to frequency
        _current_tick = 0;      // Set current ticks to 0
    }
    _do_print = false;
}

ssbDebug::ssbDebug(unsigned int frequency, bool use_ms, unsigned int baud_rate)
{
    _debug_on = false;          // Start with debug off.
    _baud_rate = baud_rate;     // Default baud rate.
    _ms_freq = use_ms;          // Default is to use time (it's easier to manage).
    if (_ms_freq == true)
    {
        _frequency = frequency; // Set to specified frequency.
        _last_out = millis();   // Current time. Will be reset each time the frequency passes.
        _tick_count = 0;        // Default to 0 as it is not not used in the current state.
        _current_tick = 0;      // Default to 0 as it is not not used in the current state.
    }
    else
    {
        _frequency = 0;         // Set time to 0 as it's not used in current state.
        _last_out = 0;          // Set time to 0 as it's not used in current state.
        _tick_count = frequency; // Set tick count to frequency
        _current_tick = 0;      // Set current ticks to 0
    }
    _do_print = false;
}

ssbDebug::~ssbDebug()
{
    Serial.end();
}

void ssbDebug::enableSerial()
{
    Serial.begin(_baud_rate);   // Enable serial output to specified baud rate.
}

void ssbDebug::enableSerial(unsigned int baud_rate)
{
    _baud_rate = baud_rate;
    Serial.begin(_baud_rate);   // Enable serial output to specified baud rate.
}

bool ssbDebug::debugState()
{
    return _debug_on;
}

bool ssbDebug::debugState(bool state)
{
    _debug_on = state;
    return _debug_on;
}

void ssbDebug::resetFreqState()
{
    if (_ms_freq == true)
    {
        _last_out = millis();   // Current time. Will be reset each time the frequency passes.
    }
    else
    {
        _current_tick = 0;      // Set current ticks to 0
    }
}

void ssbDebug::resetFreqState(unsigned int frequency)
{
    if (_ms_freq == true)
    {
        _frequency = frequency; // Set to specified frequency.
        _last_out = millis();   // Current time. Will be reset each time the frequency passes.
    }
    else
    {
        _tick_count = frequency; // Set tick count to frequency
        _current_tick = 0;      // Set current ticks to 0
    }
}

void ssbDebug::resetFreqState(unsigned int frequency, bool use_ms)
{
    _ms_freq = use_ms;
    if (_ms_freq == true)
    {
        _frequency = frequency; // Set to specified frequency.
        _last_out = millis();   // Current time. Will be reset each time the frequency passes.
        _tick_count = 0;        // Default to 0 as it is not not used in the current state.
        _current_tick = 0;      // Default to 0 as it is not not used in the current state.
    }
    else
    {
        _frequency = 0;         // Set time to 0 as it's not used in current state.
        _last_out = 0;          // Set time to 0 as it's not used in current state.
        _tick_count = frequency; // Set tick count to frequency
        _current_tick = 0;      // Set current ticks to 0
    }
}


void ssbDebug::setFrequency(unsigned int frequency)
{
    if (_ms_freq == true)
    {
        _frequency = frequency; // Set to specified frequency.
    }
    else
    {
        _tick_count = frequency; // Set tick count to frequency
    }
}

void ssbDebug::updateTicks()
{
    //Serial.print("updateTicks::Do Print => ");
    //Serial.println(_do_print);
    //Serial.print("updateTicks::Last Out => ");
    //Serial.println(_last_out);
    //Serial.print("updateTicks::Frequency => ");
    //Serial.println(_frequency);
    //Serial.print("updateTicks::Current MS: => ");
    //Serial.println(millis());
    if (_do_print == false)
    {
        if (_ms_freq == true)
        {
            if (_last_out + _frequency <= millis())
            {
                _last_out = millis();
                _do_print = true;
            }
        }
        else
        {
            if (_current_tick <= _tick_count)
            {
                _current_tick += 1;
                _do_print = true;
            }
            else
            {
                _current_tick = 0;
                _do_print = true;
            }
        }
    }
    else
    {
        _do_print = false;
    }
}

void ssbDebug::updateTicks(unsigned int increment)
{
    if (_do_print == false)
    {
        if (_ms_freq == true)
        {
            if (_last_out + _frequency + increment <= millis())
            {
                _last_out = millis();
                _do_print = true;
            }
        }
        else
        {
            if (_current_tick <= _tick_count)
            {
                _current_tick += increment;
                _do_print = true;
            }
            else
            {
                _current_tick = 0;
                _do_print = true;
            }
        }
    }
    else
    {
        _do_print = false;
    }
}

unsigned int ssbDebug::getFreqValue()
{
    if (_ms_freq == false)
    {
        return _tick_count;
    }
    return _frequency;
}

unsigned int ssbDebug::getCurrentIndex()
{
    if (_ms_freq == false)
    {
        return _current_tick;
    }
    return _last_out + millis();
}

bool ssbDebug::getFreqType()
{
    return _ms_freq;
}

// function to return the amount of free RAM
int ssbDebug::getFreeMem()
{
    int freeValue;
    if ((int)__brkval == 0)
    {
        freeValue = ((int)&freeValue) - ((int)&__bss_end);
    }
    else
    {
        freeValue = ((int)&freeValue) - ((int)__brkval);
    }
    return freeValue;
}

void ssbDebug::debugValue(int value)
{
    if ((_debug_on == true) && (Serial.available()))
    {
        if (_output_time() == true)
        {
            Serial.print("Raw value (INT): ");
            Serial.println(value);
        }
    }
}

void ssbDebug::debugValue(long value)
{
    if ((_debug_on == true) && (Serial.available()))
    {
        if (_output_time() == true)
        {
            Serial.print("Raw value (LONG): ");
            Serial.println(value);
        }
    }
}

void ssbDebug::debugValue(float value)
{
    if ((_debug_on == true) && (Serial.available()))
    {
        if (_output_time() == true)
        {
            Serial.print("Raw value (FLOAT): ");
            Serial.println(value);
        }
    }
}

void ssbDebug::debugValue(char* label, int value)
{
    //Serial.println("void ssbDebug::debugValue(char* label, int value)");
    //Serial.print("\tDeubg On: ");
    //Serial.println(_debug_on);
    if (_debug_on == true)
    {
        //Serial.println("if ((_debug_on == true) && (Serial.available()))");
        if (_output_time() == true)
        {
            //Serial.println("if (_output_time() == true)");
            Serial.print(label);
            Serial.println(value);
        }
    }
}

void ssbDebug::debugValue(char* label, long value)
{
    //Serial.println("void ssbDebug::debugValue(char* label, long value)");
    if (_debug_on == true)
    {
        //Serial.println("if ((_debug_on == true) && (Serial.available()))");
        if (_output_time() == true)
        {
            //Serial.println("if (_output_time() == true)");
            Serial.print(label);
            Serial.println(value);
        }
    }
}

void ssbDebug::debugValue(char* label, float value)
{
    //Serial.println("void ssbDebug::debugValue(char* label, float value)");
    if (_debug_on == true)
    {
        //Serial.println("if ((_debug_on == true) && (Serial.available()))");
        if (_output_time() == true)
        {
            Serial.println("if (_output_time() == true)");
            Serial.print(label);
            Serial.println(value);
        }
    }
}

void ssbDebug::debugCtl(int pin)
{
    // Base string is 17 chars long (with null).
    // 25 chars should be big enough for any additional pin
    // index value which must be an int.
    const unsigned int BUFFER_SIZE = 25;
    char buffer[BUFFER_SIZE];
    int  buffer_size = 0;
    if (_debug_on == true)
    {
        if (_output_time() == true)
        {
            if (pin < 100)
            {
                // get ctl val and print
                buffer_size = snprintf(buffer, BUFFER_SIZE-1, "Control %d value: ", pin);
                Serial.print(buffer);
                Serial.println(analogRead(pin));
            }
        }
    }
}

void ssbDebug::debugCtl(char* pinName, int pin)
{
    // Base string is 17 chars long (with null).
    // 35 chars should be big enough for any additional pin label.
    const unsigned int BUFFER_SIZE = 35;
    char buffer[BUFFER_SIZE];
    int  buffer_size = 0;
    if (_debug_on == true)
    {
        if (_output_time() == true)
        {
            if (strlen(pinName) < 16)
            {
                // get ctl val and print
                buffer_size = snprintf(buffer, BUFFER_SIZE-1, "Control %s value: ", pinName);
                Serial.print(buffer);
                Serial.println(analogRead(pin));
            }
        }
    }
}

void ssbDebug::debugOutput(int outSignal, boolean d0Out, boolean d1Out)
{
    if ((_debug_on == true) && (Serial.available()))
    {
        if (_output_time() == true)
        {
            Serial.println("Output values:");
            Serial.println("-------------------------");
            Serial.print("D0: ");
            Serial.println(d0Out);
            Serial.print("D1: ");
            Serial.println(d1Out);
            Serial.print("DAC: ");
            Serial.println(outSignal);
            Serial.println("#########################");
        }
    }
}

bool ssbDebug::_output_time()
{
    //Serial.print("Output_Time::Do Print => ");
    //Serial.println(_do_print);
    return _do_print;
}
