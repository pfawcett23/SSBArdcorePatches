/*
  ssbGate.h - A Gate Object for ArdCore patches.
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

#ifndef _ssb_ssb_gate_class_
#define _ssb_ssb_gate_class_

#include "Arduino.h"


class ssbGate
{
    public:
        // Constructors
        ssbGate();
        ssbGate(int in_dur);
        ssbGate(int in_dur, int in_delay);
        // Destructor
        ~ssbGate();
        // Gate Methods
        //  - Check state of gate. Gate is either active or not.
        //      Note that a gate may be active, but off 
        //     (ie it has a delay and has not started yet).
        bool isActive();
        // - Is the gate currently high (true if gate is active 
        //     AND has started).
        bool isOn();
        // - Update date with a new duration (or duration and delay 
        //     time). Will retart gate if currently active.
        void updateGate(int new_dur);
        void updateGate(int new_dur, int new_delay);
        // - Update the state of the gate. Call once per loop at either 
        //     start or end (end is better...).
        void updateState();
        // - Unset the gate, clear it and set it to off.
        void unsetGate();
        // - Turn the gate on or off. Use when using a gate that is not duration based.
        void setState(bool is_on);
        // - Write the on/off state (HIGH/LOW) to the specified pin.
        void render(int pin);
    private:
        int     _duration;   // Duration of the gate. For a stutter gate, it's the duration of a single fraction of a gate.
        long    _start;      // Start time for the gate. From millis(). In MS.
        int     _delay;      // Delay for start time
        bool    _on;         // Is the gate currently on (HIGH).
};

#endif // _ssb_ssb_gate_class_
