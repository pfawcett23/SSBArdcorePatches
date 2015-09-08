/*
  ssbGate.cpp - A Gate Object for ArdCore patches.
  Created by Peter Fawcett, Dec 10. 2014.
    Version 0.1: Created basic ssbGate Obect
            0.2: Jan 3. 2015
                 Updated to render, generic constructor 
                 and isOn vs isActive distinction.

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

#include "ssbGate.h"

// Constructors

ssbGate::ssbGate()
{
    _duration = 0;
    _delay = 0;
    _start = 0;
    _on = false;
}

ssbGate::ssbGate(int in_dur)
{
    _duration = in_dur;
    _delay = 0;
    _start = millis();
    _on = true;
}

ssbGate::ssbGate(int in_dur, int in_delay)
{
    _duration = in_dur;
    _delay = in_delay;
    _start = millis() + in_delay;
    _on = false;
}

// Destructor

ssbGate::~ssbGate(){/*nothing to destruct*/}

// Gate Methods

/* isActive
 - Check state of gate. Gate is either active or not.
   Note that a gate may be active, but off (ie it has a delay and has not
   started yet).
*/
bool ssbGate::isActive()
{
    // if gate is active OR if delay is greater than 0. Delay is set to 0 when
    // it expires.
    if ((_on == true) || (_delay > 0))
    {
        return true;
    }
    return false;
}

/* isOn
 - Is the gate currently high (true if gate is active AND has started).
*/
bool ssbGate::isOn()
{
    return _on;
}

/* updateGate
 - Update date with a new duration (or duration and delay time). 
   Will retart gate if currently active.
*/

void ssbGate::updateGate(int new_dur)
{
    _duration = new_dur;
    _delay = 0;
    _start = millis();
    _on = true;
}

void ssbGate::updateGate(int new_dur, int new_delay)
{
    _duration = new_dur;
    _delay = new_delay;
    _start = millis() + new_delay;
    _on = false;
}

/* updateState
 - Update the state of the gate. Call once per loop at either start or end
   (end is better...).
*/
void ssbGate::updateState()
{
    if (_on == true)
    {
        if ((_start + _duration) <= millis())
        {
            _on = false;
        }
    }
    else if ((_delay > 0) && (_start <= millis()))
    {
        _on = true;
        _delay = 0;
    }
}

/* unsetGate
 - Unset the gate, clear it and set it to off.
 */
void ssbGate::unsetGate()
{
    _duration = 0;
    _delay = 0;
    _start = 0;
    _on = false;
}

/* setState
 - Set the current state of the gate for manual gates.
 */
void ssbGate::setState(bool is_on)
{
    _on = is_on;
}

/* render
 - Write the on/off state (HIGH/LOW) to the specified pin.
*/
void ssbGate::render(int pin)
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
