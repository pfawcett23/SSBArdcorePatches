Program: ssbArdSeqOne
============================================================================

This sequencer uses set patterns of note shifts. These sequences are hard 
coded and are selected on sketch load/reload. There may be up to 8 sequences 
(A0 is 1-4 / 5-8 and A1 is Hi / Lo). The sequences are all 16 notes. 

Note values may be one of the following:
   0  : Play the root note.
   #  : Play 1 to 12 notes above the root note (if 
        the specified note is higher than the allowed
        voltage, it will be dropped an octave.
   -# : Play 1 to 12 notes below the root note (if
        the specified note is lower than the allowed
        voltage, it will be raised an octave.
   512; A magic number to represent a rest.
   So a sequence could be something like:
   {0, 7, 7, 512, -4, 512, 5...}

The Gates (D0 and D1) are set to divisions of the note clock trigger pulse. 
This means that the first time through, the sequencer can only guess at the 
tempo. Once it has two pulses it knows the speed and will then do (in 
clockwise direction) 1n, 1/2n, 1/4n, 1/8n, 1/16n.

Note: A small padding is alwasy subtracted from the gate to allow it to have
    a falling / rising edge before the next gate. This pad is 10ms.

A new clock pulse input will never inerrupt an existing gate, allowing for 
variable sequences using the expander.

The expander will allow this to be tweaked in two ways.
    When set to > 512 (via CV) the gate will be expanded. 
        It will multiplied by *2, *4 and *8.
    When set to < 512 (via CV) the gate will be stuttered.
        It will be divided by /2, /4, and /8.

Thus, if a 1/2 note gate is playing and the expander is set to *2, the gate
will be the full note. If it's8 it will be the length of 4 full pulses
of the input clock (minus the 5ms pad mentioned above). This means that
with CV one can play sequences like 1/2n, 1/2n, 1n, 2n, 1/2n, 4n...
On the other hand, with the same 1/2 note gate playing, if the expander is
set to /2, the half note will be divided in half, playing 1/4, 1/4 (with a
5ms gap between).

This then allows (with cv control) note gate widths like:
    1/2n, [1/4n,1/4n], 1n, 1n, [1/8n,1/8n,1/8n,1/8n], 2n....

As gate width it's self can also be modulated, this allows for very 
expressive and / or randomistic sequences (with random or stepped lfos). 
The state of a given note is treated in a 'sample and hold' mannor. If both 
gates are closed (as the Quantized CV is Monophonic) and there is a clock
pulse, the next quantized note value, as well as all gate values are set
and the gate width and stutter count are set. These values are then used
till both gates close again.

As both gates can be timed and expanded / stepped differently, this allows
one to have normal note lengths for the vca envelope, but stepped for the
vcf envelope (for example).

============================================================================

I/O Usage:
    Knob A0:         Root Note   (C-B#)
    Knob A1:         Root Octave (C3-C8)
    Knob A2/Jack A2: Gate D0 Width
    Knob A3/Jack A3: Gate D1 Width
    Digital Out 1:   Gate D0 Out
    Digital Out 2:   Gate D1 Out
    Clock In:        Note Trigger
    Analog Out:      Quantized Note
Input Expander: 
    Knob A4/Jack A4: Gate D0 Stutter/Adder
    Knob A5/Jack A5: Gate D0 Stutter/Adder
Output Expander:
    Bits 0-7:        Not Used

============================================================================

Created:  Nov 22 2014 by Peter Fawcett (SoundSweepsBy)