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

#ifndef _ssb_debug_class_
#define _ssb_debug_class_

#include <Arduino.h>

class ssbDebug
{
    private:
        bool            _debug_on;      // Debug On/Off
        unsigned int    _baud_rate;     // Baud rate set by constructor, currently not changeable after construction
        bool            _ms_freq;       //
        unsigned int    _frequency;     // Number of MS between output (if ms_freq is true).
        unsigned long   _last_out;      // Start time, or last time we had output (if ms_freq is true).
        unsigned int    _tick_count;    // Number of ticks between output (if ms_freq is false).
        unsigned int    _current_tick;  // The current tick count (if ms_freq is false).
        bool            _do_print;      // State. Will be set at end of loop on update and will print next run though.
    public:
        // Constructors
        ssbDebug();
        ssbDebug(unsigned int frequency);
        ssbDebug(unsigned int frequency, bool use_ms);
        ssbDebug(unsigned int frequency, bool use_ms, unsigned int baud_rate);
        // Destructor
        ~ssbDebug();

        // enable the serial output in setup.
        void enableSerial();
        void enableSerial(unsigned int baud_rate);

        // Get/Set current debug state
        bool debugState();
        bool debugState(bool state);

        // Reset the frequency of debug output. All calls reset counter to 0/now.
        // Other calls allow updating of frequency (either ms or ticks) as well as
        // updating of counter type
        void resetFreqState();
        void resetFreqState(unsigned int frequency);
        void resetFreqState(unsigned int frequency, bool use_ms);

        // Set the frequency of debug output. Current state is not reset.
        void setFrequency(unsigned int frequency);

        // When doing tick counts, call this as the end of the loop that is
        // being debugged to update the tick count.
        void updateTicks();
        void updateTicks(unsigned int increment);

        // Get the current frequency value.
        unsigned int getFreqValue();

        // Get current Tick/MS value.
        unsigned int getCurrentIndex();

        // Get the current type of frequency updates.
        // true  => ms count
        // false => tick count
        bool getFreqType();

        // Get the current amount of free memory in RAM
        int getFreeMem();

        // Various debug outputs for general values
        void debugValue(int value);
        void debugValue(long value);
        void debugValue(float value);
        void debugValue(char* label, int value);
        void debugValue(char* label, long value);
        void debugValue(char* label, float value);

        // Various debug outputs specific for controls
        void debugCtl(int pin);
        void debugCtl(char* label, int pin);

        // Debug output for ArdOutputs.
        void debugOutput(int dac_out, boolean d0_out, boolean d1_out);
    private:
        bool _output_time();
};

#endif // _ssb_debug_class_
