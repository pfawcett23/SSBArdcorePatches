/*
  ssbArdM4L.cpp - An object to use for serial communication between 
    the Ardcore and Max4Live. Uses Serial Output. Can not be used
    at same time as ssbDebug as it uses the same serial connection.
    how ever the M4L object provides some debugging ability and may
    be used in testing with the serial console of the Arduino Serial
    console.
    
  Created by Peter Fawcett, Sept 28. 2015.
    Version 0.1: Created basic ssbArdM4L Obect

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

#include "ssbArdM4L.h"

ssbArdM4L::ssbArdM4L()
{
    _baud_rate = 9600;          // Default baud rate.
    _open_marker = '[';         // Default char stream opener.
    _close_marker = ']';        // Default char stream closer.
    _input_buffer = "";         // String object to contain input buffer.
    _buffer_full = false;       // Is the i/o buffer fully updated.
}

ssbArdM4L::ssbArdM4L(char open_marker, char close_marker)
{
    _baud_rate = 9600;              // Default baud rate.
    _open_marker = open_marker;     // Default char stream opener.
    _close_marker = close_marker;   // Default char stream closer.
    _input_buffer = "";             // String object to contain input buffer.
    _buffer_full = false;           // Is the i/o buffer fully updated.
}

ssbArdM4L::~ssbArdM4L()
{
    Serial.end();
}

void ssbArdM4L::enableSerial()
{
    Serial.begin(_baud_rate);   // Enable serial output to specified baud rate.    
}

void ssbArdM4L::enableSerial(unsigned int baud_rate)
{
    _baud_rate = baud_rate;
    Serial.begin(_baud_rate);   // Enable serial output to specified baud rate.    
}


bool ssbArdM4L::doRead()
{
    if (Serial.available() > 0)
    {
        // NOTE: Data MUST be read from buffer when doRead returns true, 
        //       or the data will be lost!
        char tmp_c = (char)Serial.read();
        if (tmp_c == _open_marker)
        {
            _input_buffer = "";
            _buffer_full = false;
        }
        else if (tmp_c == _close_marker)
        {
            _buffer_full = true;                                
        }
        else
        {
            _input_buffer += tmp_c;
        }
    }
    return _buffer_full;
}

int ssbArdM4L::getBufferAsInt()
{
    int tmp_val = _input_buffer.toInt();
    return tmp_val;
}

float ssbArdM4L::getBufferAsFloat()
{
    float tmp_val = _input_buffer.toFloat();
    return tmp_val;
}

void ssbArdM4L::getBufferAsIntArray(int* data, int list_len, char sep_char)
{
    int tmp_int = 0;
    String tmp_str = "";
    int current_comma_index = -1;
    int next_comma_index = -1;
    int index = 0;
    while (index < list_len)
    {
        if (current_comma_index == -1)
        {
            current_comma_index = 0;
            next_comma_index = _input_buffer.indexOf(',');
        }
        else
        {
            current_comma_index = next_comma_index + 1;
            next_comma_index = _input_buffer.indexOf(',', current_comma_index + 1);
        }
        if ((current_comma_index > -1) && (next_comma_index > -1))
        {
            tmp_str = _input_buffer.substring(current_comma_index, next_comma_index);
        }
        else if ((current_comma_index > -1) && (next_comma_index == -1))
        {
            tmp_str = _input_buffer.substring(current_comma_index);
        }
        if (!(tmp_str == ""))
        {
            tmp_int = tmp_str.toInt();
        }
        data[index] = tmp_int;
        index++;
    }
}

void ssbArdM4L::getBufferAsCharArray(char* data, int list_len, char sep_char)
{
    int tmp_int = 0;
    String tmp_str = "";
    int current_comma_index = -1;
    int next_comma_index = -1;
    int index = 0;
    while (index < list_len)
    {
        if (current_comma_index == -1)
        {
            current_comma_index = 0;
            next_comma_index = _input_buffer.indexOf(',');
        }
        else
        {
            current_comma_index = next_comma_index + 1;
            next_comma_index = _input_buffer.indexOf(',', current_comma_index + 1);
        }
        if ((current_comma_index > -1) && (next_comma_index > -1))
        {
            tmp_str = _input_buffer.substring(current_comma_index, next_comma_index);
        }
        else if ((current_comma_index > -1) && (next_comma_index == -1))
        {
            tmp_str = _input_buffer.substring(current_comma_index);
        }
        char tmp_char[tmp_str.length()];
        tmp_str.toCharArray(tmp_char, tmp_str.length());
        data[index] = *tmp_char;
        index++;
    }
}
