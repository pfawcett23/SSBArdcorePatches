Program: ssbSkipper

Description:
    Input Clock or Gate Signals on the clock input.
    Depending upon the state of A2/A3, Either output the same gate signal on
    D0/D1 or skip it. At the lowest setting it plays all gates. At higher settings
    it will skip every other, third, fourth, fifth, sixth or seventh. At the highest
    setting it will skip all. If A0/A1 is high (>50%) the skip will be random, ie 
    it may or may not occur, (50% chance). The expander A4/A5 may be used to 
    shift the randomness from 50% chance to weigighted either toward 10% or
    90% (low or high).
    The skip none and skip all settings are best with random as they you get
    a 50% don't play or a 50% do play for all gates. With the expander this 
    can then be maniuplated further. Note that the skip and the random 
    percents may be modulated.
    I/O Usage:
        Knob A0:         Enable random for D0
        Knob A1:         Enable random for D1
        Knob/Jack A2:    Skip beat (None, 2nd, 3rd, 4th, 5th, 6th, 7th, All) for D0
        Knob/Jack A3:    Skip beat (None, 2nd, 3rd, 4th, 5th, 6th, 7th, All) for D1
        Digital Out 1:   Gate / Clock out 1
        Digital Out 2:   Gate / Clock out 2
        Clock In:        Gate / Clock In
        Analog Out:      Stepped Voltage
    Input Expander:
        Knob A4/Jack A4: Shift randomness weight 10%-90% for D0
        Knob A5/Jack A5: Shift randomness weight 10%-90% for D1
    Output Expander:
        Bits 0-7:        Step Counter. On each clcok pulse, will advance from 0 to 7 and reset.
        Analog Out 11:   Unused
        Digital Out 13:  Unused
    Serial:              Unused (debug)
    Created:  Feb 7 2015 by Peter Fawcett (SoundSweepsBy).
        Version 1 - Original patch developement.
        Version 2 - Sept. 7 2015:
          Updated to use ssbLib code.
          Fixed bugs.
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