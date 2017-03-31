/*
 * EEPROMReadTime
 *
 * Reads and prints (to Serial) formatted time values stored in EEPROM
 * 
 * Christopher J. Cronin
 * Sternberg Lab
 * Caltech
 * Pasadena, CA  91125
 * cjc@caltech.edu
 * March 29, 2011
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

 
//#include <EEPROM.h>
#include <avr/eeprom.h>
#include "WaveUtil.h"

/********************************************************/
/* Uncomment the next line to print time in long format */
// #define LONGFORM      // Print verbose time values
/********************************************************/

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
uint32_t value;   // uint32_t == unsigned long ???
uint8_t  blockSize                   = sizeof(uint32_t);
uint16_t dwordsInEEPROM              = 256;  // for Arduino UNO
uint32_t resetValue                  = (uint32_t)0 - 1;     // maximum unsigned long value: take this as a power-reset flag

// Function definitions (we define them here, but the code is below)
void printTime(unsigned long time);


///////////////////////////////////////////////////////



void setup()
{
  Serial.begin(9600);
  //value = EEPROM.read(address);

  delay(5000);

  for (address = 0; address < dwordsInEEPROM; address++){
    // read a word from the current address of the EEPROM
    value = eeprom_read_dword((uint32_t*)(address * sizeof(uint32_t)));

    if (value > 0){
      Serial.print(address);
      Serial.print(":\t");
      if (value == resetValue)
        putstring_nl("  RESET");
      else
      printTime(value);
    }
  }
  putstring_nl("Done!");
}



void loop()
{
}



////////////////////////////////////////////////////////

/*
 * Print out formatted version of time
 */
void printTime(unsigned long time)
{
  uint32_t remainingTime = time;

  uint32_t days;
  uint32_t hours;
  uint32_t minutes;
  uint32_t seconds;

  uint32_t milisecondsPerSecond = 1000;
  uint32_t milisecondsPerMinute = milisecondsPerSecond * 60;
  uint32_t milisecondsPerHour = milisecondsPerMinute * 60;
  uint32_t milisecondsPerDay = milisecondsPerHour * 24;

  days = remainingTime/milisecondsPerDay;
  remainingTime = remainingTime - (days * milisecondsPerDay);

  hours = remainingTime/milisecondsPerHour;
  remainingTime = remainingTime - (hours * milisecondsPerHour);

  minutes = remainingTime/milisecondsPerMinute;
  remainingTime = remainingTime - (minutes * milisecondsPerMinute);

  seconds = remainingTime/milisecondsPerSecond;
  remainingTime = remainingTime - (seconds * milisecondsPerSecond);

  /* The actual print-out */
#ifdef LONGFORM
  Serial.print(days);
  putstring(" Days  ");

  Serial.print(hours);
  putstring(" hours : ");

  if (minutes < 10)
    Serial.print(0, DEC);
  Serial.print(minutes);
  putstring(" minutes : ");

  if (seconds < 10)
    Serial.print(0, DEC);
  Serial.print(seconds);
  putstring_nl(" seconds");

#else

  Serial.print(days);
  if (days == 1)
    putstring(" Day   ");
  else
    putstring(" Days  ");

  Serial.print(hours);
  putstring(":");

  if (minutes < 10)
    Serial.print(0, DEC);
  Serial.print(minutes);
  putstring(":");

  if (seconds < 10)
    Serial.print(0, DEC);
  Serial.println(seconds);

#endif
}



