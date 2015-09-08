SSBArdcorePatches
=================

Modified and Original patches for the Ardcore created by SoundSweepsBy.

Current patch list:
- ssbArdLive
    An example patch for sending data from Max4Live devices and Max/MSP 
    patches to the ardcore via the serial usb connection. The example
    included here sends velocity data from the midi input. This 
    device/patch is used by both the ssbMonoADSR_m4lVel and the
    ssbMonoADSR_m4lVelEX.

- ssbM4LArdBits
    This patch and associated Max4Live device / Max/MSP patch allows
    the toggling of specific bits on the ArdCore expander. Triggering
    bits will also set the voltage of the DAC output to specific 
    amounts. This is a side effect, but could be useful as well.
    As the bits may be automated from live, this may be used as a
    drum machine trigger for 8 drums or to enable/disable specific
    effects via on/off gates.
    Each bit sets a known value:
    bit 0: 4
    bit 1: 8
    bit 2: 16
    bit 3: 32
    bit 4: 64
    bit 5: 128
    bit 6: 256
    bit 7: 512
    
    If all bits are set, the DAC output will be 1020 or just under 5v.

- ssbMonoADSR
    A simple ADSR envelope for the ArdCore. The sustain level is fixed
    (hard coded in the patch, of course you can edit the level by editing
    the hard coded value). The attack, decay and release are adjustable.
    The gate can either be manual or triggered via CV. When using CV to
    trigger the gate, the A2 knob should be at maximum value (usually)
    as it attenuates the input of A2.
    A0: Attack time
    A1: Decay time
    A2: Gate
    A3: Release time
    DAC: envelope signal

- ssbMonoADSR_m4lVel
    This patch works the same as the above patch, except that the maximum
    value is scaled by input from the Max4Live ssbArdCoreVel device (or 
    any Max/MSP patch). If the midi velocity is 127 (max), then the max 
    amount reached by the attack will be the full amount. Any midi
    velocity less than 127 will scale the envelope max amount accordingly.
    The sustain (still hard coded), will be applied to the scaled max
    amount. This allows for some velocity control of the envelope.

- ssbMonoADSR_EX
    A simple ADSR envelope for the ArdCore and the ArdCore Expander.
    The code is basicly the same as for the ssbMonoADSR device above
    how ever it uses the Expander for the gate input which allows
    the A2 knob/CV to be used to set the sustain. Note that the 
    A4 knob is bipolar, so the gate behavior is a tad odd for manual
    trigger, but it works the same with CV once this is taken into 
    account.

- ssbMonoADSR_m4lVelEX
    Like the last three patches, this is an ADSR envelope.
    This patch is a combination of the m4lVel patch and the
    EX patch. Velocity data is sent via Serial USB to the 
    ArdCore and the A2 knob/CV controls the sustain amount
    while the A4 knob/CV controls the envelope gate.

- ssbLogic
    Two simple logic gates. A0 and A1 allow the user to select between OR,
    AND, NOR, NAND, XOR and NXOR. A2 and A3 are used as inputs. If the value
    of A2 or A3 is less than half, it is FALSE, if equal or greater than 
    half, it is TRUE. The two values are compared with the selected logic
    types. DO outputs the logic type set by A0 and D1 outputs the logic 
    type set by A1. The DAC out outputs the sum of A2 and A3 (constrained
    between 0v and 5v).

- ssb4thsAnd5ths
    A simple quantized pitch shifter. The patch shfits the input on A2 either
    up or down (as set by A0) a 4th or a 5th (as set by A1). The note output
    is constrained to 0-5volts (C3 to C7). Notes outside these values can not 
    be produced by the ArdCore.

- ssb4thsAnd5thsGate
    A simple quantized pitch shifter. The patch shfits the input on A2 either
    up or down (as set by A0) a 4th or a 5th (as set by A1). The note output
    is constrained to 0-5volts (C3 to C7). Notes outside these values can not 
    be produced by the ArdCore. Different gates trigger outputs may be set for
    the D0 and D1 outputs.

- ssb4thsAnd5thsGateEX
    A simple quantized pitch shifter. The patch shfits the input on A2 either
    up or down (as set by A0) a 4th or a 5th (as set by A1). The note output
    is constrained to 0-5volts (C3 to C7). Notes outside these values can not 
    be produced by the ArdCore. Different gates trigger outputs may be set for
    the D0 and D1 outputs. The gate width may be varied with the expander.

- ssbArdSeqOne
    A step sequencer that uses the expander as well as the ardcore. It plays 
    a sequence of semitone offsets from a root note (set by A0 and A1).
    Each Step has two independant gates (D0 and D1). The gate widths may be
    set and the gates may be expanded or stuttered with use of the expander.
    See the comments in the patch / read me in the patch directory for more
    details.

- ssbSkipper
    Input Clock or Gate Signals on the clock input.
    Depending upon the state of A2/A3, Either output the same gate signal on
    D0/D1 or skip it. At the lowest setting it plays all gates. At higher
    settings it will skip every other, third, fourth, fifth, sixth or seventh.
    At the highest setting it will skip all. If A0/A1 is high (>50%) the skip
    will be random, ie it may or may not occur, (50% chance). The expander 
    A4/A5 may be used to shift the randomness from 50% chance to weigighted 
    either toward 10% or 90% (low or high).
    The skip none and skip all settings are best with random as they you get
    a 50% don't play or a 50% do play for all gates. With the expander this 
    can then be maniuplated further. Note that the skip and the random 
    percents may be modulated.
