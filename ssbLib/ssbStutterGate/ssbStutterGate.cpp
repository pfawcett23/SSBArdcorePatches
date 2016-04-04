/*
  ssbStutterGate.cpp - A Gate Object for ArdCore patches.
  Created by Peter Fawcett, Dec 10. 2014.
    Version 0.1: Created basic ssbStutterGate Obect

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

#include "ssbStutterGate.h"

const int DEFAULT_MS_PAD = 10;

// Constructors

ssbStutterGate::ssbStutterGate()
{
    _duration = 0;
    _stutter_count = 0;
    _stutter_index = 0;
    _start = 0;
    _ms_pad = DEFAULT_MS_PAD;
    _on = false;
}

ssbStutterGate::ssbStutterGate(int in_dur)
{
    _duration = in_dur;
    _stutter_count = 1;
    _stutter_index = 1;
    _start = millis();
    _ms_pad = DEFAULT_MS_PAD;
    _on = true;
}

ssbStutterGate::ssbStutterGate(int in_dur, int in_count)
{
    _duration = in_dur;
    _stutter_count = in_count;
    _stutter_index = 1;
    _start = millis();
    _ms_pad = DEFAULT_MS_PAD;
    _on = true;
}


// Destructor

ssbStutterGate::~ssbStutterGate(){/*nothing to destruct*/}

// Stutter Gate Methods

/* isActive
 - Check state of gate. Gate is either active or not.
   Note that a gate may be active, but off (ie it has a delay and has not
   started yet).
*/
bool ssbStutterGate::isActive()
{
    if (_on == true)
    {
        return true;
    }
    else if (_stutter_index < _stutter_count)
    {
        // gate is not on, but we are between stutters
        return true;
    }
    return false;
}

/* isOn
 - Is the gate currently high (true if gate is active AND has started).
*/
bool ssbStutterGate::isOn()
{
    return _on;
}

/* updateState
 - Update the state of the gate. Call once per loop at either start or end
   (end is better...).
*/
void ssbStutterGate::updateState()
{
    bool tmp_on = false;
    if (_on == true)
    {
        // is gate still active..?
        if ((_start + _duration - _ms_pad) > millis())
        {
            // if gate duration is still active 
            // (start plus dur is greater than now)
            tmp_on = true;
        }
        if (tmp_on == false)
        {
            // gate is active, but over...
            // will be set to false at end of function...
            // are we a stutter gate? if yes, do we have
            // another stutter to render...
            if (_stutter_index < _stutter_count)
            {
                _start = _start + _duration;
            }
        }
    }
    else if ((_stutter_index < _stutter_count) && (_start <= millis()))
    {
        tmp_on = true;
        _stutter_index += 1;
    }
    _on = tmp_on;
}

/* updateGate
 - Update date with a new duration (or duration and stutter count). 
   Will retart gate if currently active.
*/

void ssbStutterGate::updateGate(int new_dur)
{
    _duration = new_dur;
    _stutter_count = 1;
    _stutter_index = 1;
    _start = millis();
    _on = true;
}

void ssbStutterGate::updateGate(int new_dur, int new_count)
{
    _duration = new_dur;
    _stutter_count = new_count;
    _stutter_index = 1;
    _start = millis();
    _on = true;
}

/* unsetGate
 - Unset the gate, clear it and set it to off.
 */
void ssbStutterGate::unsetGate()
{
    _duration = 0;
    _stutter_count = 0;
    _stutter_index = 0;
    _start = 0;
    _on = false;
}

/* setStutterGapMS
 - Set the time (in ms) for the gap between stutters.
*/
void ssbStutterGate::setStutterGapMS(int ms_pad)
{
    _ms_pad = ms_pad;
}

/* render
 - Write the on/off state (HIGH/LOW) to the specified pin.
*/
void ssbStutterGate::render(int pin)
{
    if (_on == true)
    {
        digitalWrite(pin, HIGH);
    }
    else
    {
        digitalWrite(pin, LOW);
    }
}
