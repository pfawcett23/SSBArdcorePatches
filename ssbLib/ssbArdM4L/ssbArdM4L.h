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

#ifndef _ssb_max4live_class_
#define _ssb_max4live_class_

#import <Arduino.h>

class ssbArdM4L
{
    private:
        unsigned int    _baud_rate;     // Baud rate set by constructor, currently not changeable after construction
        char            _open_marker;   // Opening char for i/o string. Default '['
        char            _close_marker;  // Closing char for i/o string. Default ']'
        String          _input_buffer;  // String to contian buffer of char objects
        bool            _buffer_full;   // Is the i/o buffer fully updated.
    public:
        // Constructors
        ssbArdM4L();
        ssbArdM4L(char open_marker, char close_marker);
        // Destructor
        ~ssbArdM4L();

        // enable the serial output in setup.
        void enableSerial();
        void enableSerial(unsigned int baud_rate);

        // handle input
        // NOTE: Data MUST be read from buffer when doRead returns true, 
        //       or the data will be lost!
        bool doRead();
        int getBufferAsInt();
        float getBufferAsFloat();
        void getBufferAsStr(String *buffer_str);
        void getBufferAsIntArray(int* data, int list_len, char sep_char);
        void getBufferAsCharArray(char* data, int list_len, char sep_char);
};

#endif // _ssb_max4live_class_
