/*
  ssbTrigTempo.h - An object to use in ArdCore patches to manage the current
  temp. The object gets updated with the current state of the clock each loop
  (in future, change this to use a callback to the clock object...). It keeps
  track of the pulse count and updates the current tempo acordingly. The object
  uses a running average for the tempo (rounded to an int on output, the
  internal value is a float). This means that the tempo is less volitile, but 
  may be a bit slugish with a changing tempo. (In the future this may be a 
  setable state, ie, use running avg or not, or even use different sized 
  windows / do a window reset...) but that is all for the future...
    
  Created by Peter Fawcett, Jan 3. 2015.
    Version 0.1: Implemented base object code.

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


#ifndef _ssb_trig_tempo_class_
#define _ssb_trig_tempo_class_

#include <Arduino.h>

class ssbTrigTempo
{
    private:
        long    _last_check_ms;
        float   _running_avg;
        int     _count;
        int     _tempo;
    public:
        ssbTrigTempo();
        ~ssbTrigTempo();
        int updateTrigTempo(bool clock_state);
        int getTrigTempo();
        int getTempoDivision(int division);
    private:
        int calcRunningTempo(int current_division);
}

#endif // _ssb_trig_tempo_class_
