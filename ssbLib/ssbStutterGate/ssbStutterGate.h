/*
  ssbStutterGate.h - A Stutter Gate Object for ArdCore patches.
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

#ifndef _ssb_stutter_gate_class_
#define _ssb_stutter_gate_class_

#include <Arduino.h>


class ssbStutterGate
{
    private:
        int     _duration;      // Duration of the gate. For a stutter gate, it's the duration of a single fraction of a gate.
        long    _start;         // Start time for the gate. From millis(). In MS.
        int     _stutter_count; // Number of stutters. default is 1
        int     _stutter_index; // Current stutter index
        int     _ms_pad;        // Default amount of of ms to pad between stutters.
        bool    _on;            // Is the gate currently on (HIGH).
    public:
        // Constructors
        ssbStutterGate();
        ssbStutterGate(int in_dur);
        ssbStutterGate(int in_dur, int in_count);
        // Destructor
        ~ssbStutterGate();
        // Gate Methods
        //  - Check state of gate. Gate is either active or not.
        //      Note that a gate may be active, but off 
        //     (ie between a stutter).
        bool isActive();
        // - Is the gate currently high (true if gate is active 
        //     AND is not between stutters).
        bool isOn();
        // - Update the state of the gate. Call once per loop at either 
        //     start or end (end is better...).
        void updateState();
        // - Update date with a new duration (or duration and stutter 
        //     count). Will retart gate if currently active.
        void updateGate(int new_dur);
        void updateGate(int new_dur, int new_count);
        // - Set the time (in ms) for the gap between stutters.
        void setStutterGapMS(int ms_pad);
        // - Unset the gate, clear it and set it to off.
        void unsetGate();
        // - Write the on/off state (HIGH/LOW) to the specified pin.
        void render(int pin);
};

#endif  // _ssb_stutter_gate_class_
