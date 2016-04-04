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

#import "ssbTrigTempo.h"

/*
Constructor, takes no args.
*/
ssbTrigTempo::ssbTrigTempo()
{
    _last_check_ms = millis();
    _running_avg = 0;
    _count = 0;
    _tempo = 0;
}

/*
Destructor
*/
ssbTrigTempo::~ssbTrigTempo()
{
    /*nothing to destruct*/
}

int ssbTrigTempo::getTrigTempo()
{
    return _tempo;
}

int ssbTrigTempo::updateTrigTempo(bool clock_state)
{
    /* 
    if clock state is HIGH / true update tempo
    */
    if (clock_state == true)
    {
        long now = millis();
        int slice = now - _last_check_ms;
        _last_check_ms = now;
        _tempo = calcRunningTempo(slice)
    }
    return _tempo;
}

int ssbTrigTempo::getTempoDivision(int division)
{
    /*
    really just divide tempo by division, but use more acurate 
    running avg (float) before casting to int
    */
    return int(_running_avg / division);
}

// Private methods

int ssbTrigTempo::calcRunningTempo(int current_division)
{
    // simple running avg
    // avg n+1 = (current + n * avg n) / n + 1
    _running_avg = (current_division + _count * _running_avg) / (_count + 1)
    _count += 1;
    return int(_running_avg);
}

