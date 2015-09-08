/*
  ssbScales.h - 
    
  Created by Peter Fawcett, Jan 23. 2015.
    Version 0.1: Implemented / moved over from working code, the base
                 set of functionality.

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

#ifndef _ssb_scales_
#define _ssb_scales_

#include "Arduino.h"

// ============================================================================
// Quantized Voltage Map:
// ============================================================================

const   int     NOTE_COUNT              = 61;
// constant for actual 0-5V quantization (vs. >> 4)
//    C   C#    D   D#    E    F   F#    G   G#    A   A#    B
const  int      QNOTES[(NOTE_COUNT)]    = {
      0,   9,  26,  43,  60,  77,  94, 111, 128, 145, 162, 180, // OCT3
    197, 214, 231, 248, 265, 282, 299, 316, 333, 350, 367, 384, // OCT4
    401, 418, 435, 452, 469, 486, 503, 521, 538, 555, 572, 589, // OCT5
    606, 623, 640, 657, 674, 691, 708, 725, 742, 759, 776, 793, // OCT6
    810, 827, 844, 862, 879, 896, 913, 930, 947, 964, 981, 998, // OCT7
   1015};

// ============================================================================
// Note Constants:
// These constants may be used to reference values in the QNOTES array from
// ssbArdBase. For example QNOTES[C_NOTE] is Oct 3 C.
// Or QNOTES[E_NOTE+12*2] is Oct 5 E. With the octive shifts below this is
// even easier.
// ============================================================================
const   int     C_NOTE                  = 0;
const   int     C_SHARP_NOTE            = 1;
const   int     D_NOTE                  = 2;
const   int     D_SHARP_NOTE            = 3;
const   int     E_NOTE                  = 4;
const   int     F_NOTE                  = 5;
const   int     F_SHARP_NOTE            = 6;
const   int     G_NOTE                  = 7;
const   int     G_SHARP_NOTE            = 8;
const   int     A_NOTE                  = 9;
const   int     A_SHARP_NOTE            = 10;
const   int     B_NOTE                  = 11;

// ============================================================================
// Octave Constants:
// Like the Notes above these may be used with the QNOTES array. There are 5
// Octaves avalible for the ArdCore, 3-7 corresponding to the range of the 
// 1v/o range over 5 volts.
// This allows for QNOTES[A_SHARP_NOTE+OCT_4]
// ============================================================================

const   int     OCT_3                   = 0;
const   int     OCT_4                   = 12;
const   int     OCT_5                   = 12 * 2;
const   int     OCT_6                   = 12 * 3;
const   int     OCT_7                   = 12 * 4;

// ============================================================================
// Major Scale:
// Define the intervals so that one can use offsets from the base note.
// For example to play the C Major scale:
// for (int note_index = 0; note_index < MAJOR_SCALE_LEN; note_index++)
// {
//     scale_note = C_NOTE + MAJOR_SCALE[note_index];
//  ...
// ============================================================================

const   int     MAJOR_SCALE_LEN         = 7;
const   int     MAJOR_SCALE[MAJOR_SCALE_LEN] = {0, 2, 4, 5, 7, 9, 11};

// ============================================================================
// Minor Scale:
// See above for usage...
// ============================================================================

const   int     MINOR_SCALE_LEN         = 7;
const   int     MINOR_SCALE[MINOR_SCALE_LEN] = {0, 2, 3, 5, 7, 8, 10};

// ============================================================================
// Pentatonic Major Scale:
// See above for usage...
// ============================================================================

const   int     PENTA_MAJOR_SCALE_LEN   = 5;
const   int     PENTA_MAJOR_SCALE[PENTA_MAJOR_SCALE_LEN] = {0, 2, 4, 7, 9};

// ============================================================================
// Pentatonic Minor Scale:
// See above for usage...
// ============================================================================

const   int     PENTA_MINOR_SCALE_LEN   = 5;
const   int     PENTA_MINOR_SCALE[PENTA_MINOR_SCALE_LEN] = {0, 3, 5, 7, 10};

#endif /* _ssb_scales_ */

