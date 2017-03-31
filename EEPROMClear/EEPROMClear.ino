/*
 * EEPROM Clear
 *
 * Sets all of the bytes of the EEPROM to 0.
 * Based on the eeprom_clear.pde example sketch provided with the Arduino IDE
 * but modified to clear all 1024 bytes of EEPROM in the Arduino Uno 
 * to provide serial output for the user's reference.
 *
 * Christopher J. Cronin
 * Sternberg Lab
 * Caltech
 * Pasadena, CA  91125
 * cjc@caltech.edu
 * March 30, 2011
 * Expanded introductory comments January 4, 2013
 * 
 * Based on example code in the public domain.
 *
 *
-------------------------------------------------
Copyright (c) 2011, 2013, California Institute of Technology.  Based on 
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

void setup()
{
  pinMode(13, OUTPUT);
  
  Serial.begin(9600);
  
  delay(5000);
  
  // write a 0 to all 1024 bytes of the EEPROM
  for (int i = 0; i < 1024; i++){
    EEPROM.write(i, 0);
    Serial.println(i, DEC);
  }
  
  Serial.println("Done!");
    
  // turn the LED on when we're done
  digitalWrite(13, HIGH);
}

void loop()
{
}
