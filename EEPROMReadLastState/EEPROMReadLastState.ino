/*
 * EEPROMReadLastState
 *
 * Reads and prints (to Serial) most recent state value stored in EEPROM
 * where state is that recorded by the cricketNoDelay sketch in byte 1023
 * of EEPROM on an Arduino UNO
 * 
 * Christopher J. Cronin
 * Sternberg Lab
 * Caltech
 * Pasadena, CA  91125
 * cjc@caltech.edu
 * April 18, 2011
 * 
 * Based on example code in the public domain.
 *
 *
-------------------------------------------------
Copyright (c) 2011, California Institute of Technology.  Based on 
research work supported by a National Institutes of Health (NIH) United 
States Public Health Service Training Grant (5T32GM07616) to A.R.D. and by 
the Howard Hughes Medical Institute (with which P.W.S. is an investigator), 
by Christopher J. Cronin and Adler R. Dillman, 
in the laboratory of Paul W. Sternberg.
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice, 
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
- Neither the name of the California Institute of Technology (Caltech) nor 
  the names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

This software includes libraries licensed under the GNU General Public 
License (GPL) version 3.  For a copy of that license see
<http://www.gnu.org/licenses/>.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
-------------------------------------------------
 *
 */


#include <EEPROM.h>
//#include <avr/eeprom.h>
#include "WaveUtil.h"


uint8_t value;   
uint16_t lastByte                    = 1023;  // last byte address in EEPROM for Arduino UNO
// uint32_t resetValue                  = (uint32_t)0 - 1;     // maximum unsigned long value: take this as a power-reset flag
uint16_t lastStateByte               = 1023;              // EEPROM (byte) address (byte) for recording last state
uint16_t errorCodeByte               = 1022;              // EEPROM (byte) address for error source code
uint16_t errorDataByte               = 1021;              // EEPROM (byte) address for error data
uint16_t errorSourceByte             = 1020;              // EEPROM (byte) address for error source (see error message functions for details)


///////////////////////////////////////////////////////



void setup()
{
  Serial.begin(9600);
  //value = EEPROM.read(address);
  
  putstring("Last cricketNoDelay state = ");
  value = EEPROM.read(lastStateByte);
  Serial.println((int)value, DEC);
  
  putstring("\nError Source Code (if any) = ");
  value = EEPROM.read(errorSourceByte);
  Serial.println((int)value, DEC);

  putstring("Error Code (from sdReader) (if any) = ");
  value = EEPROM.read(errorCodeByte);
  Serial.println((int)value, HEX);

  putstring("Error Data (from sdReader) (if any) = ");
  value = EEPROM.read(errorDataByte);
  Serial.println((int)value, HEX);


  putstring_nl("\nDone!");
}



void loop()
{
}




