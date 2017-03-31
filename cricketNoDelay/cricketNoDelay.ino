/* 
 * cricketNoDelay
 * 
 * Automates operation of a semi-permanently installed mole cricket trap.
 * 
 * 
 * This is the Caltech Cricket Lure sketch 'cricketNoDelay,' developed in
 * Spring 2011 in the Sternberg Lab at Caltech (California Institute of
 * Technology, Pasadena, California, USA).  Our objective is the automated
 * operation of an audio lure for a semi-permanently installed mole cricket
 * trap.  cricketNoDelay automatically plays a male mole cricket mating call
 * audio recording every day, beginning after dusk, while controlling
 * amplifier power switching (to conserve battery power) and recording event
 * information in non-volatile memory.
 * 
 * ==ATTACHED HARDWARE==
 * Hardware-wise, the sketch expects to be run on an Arduino microcontroller
 * board 
 *   www.arduino.cc
 * such as the Arduino Uno, with an attached Wave Shield from Adafruit
 * Industries 
 *   www.adafruit.com/products/94 and 
 *   www.ladyada.net/make/waveshield/
 *
 * The sketch also expects connections to: 
 * - an LED circuit (digital output pin 7) to blink information about
 *   program state and errors.
 * - an ambient light-level sensor circuit (analog input pin 0) to sense
 *   ambient light level with a photocell/potentiometer voltage divider.
 * - a relay control circuit (digital output pin 8) to turn an attached
 *   audio amplifier on and off.  
 * Note: 
 * - The schematics and details of these circuits are better described in
 *   our accompanying journal article.  
 * - These pin assignments are made in the 'Constants' section of the sketch.
 *
 * ==ADDITIONAL SOFTWARE==
 * This sketch depends on an additional library, beyond those delivered with
 * the Arduino IDE:
 * - waveHC -- Handles audio playback and SD card interaction.  Download from:
 *     http://code.google.com/p/wavehc/
 *  linked from:
 *     http://www.ladyada.net/make/waveshield/libraryhc.html
 *
 *
 * ==PROGRAM STRUCTURE==
 * The basic program structure of the sketch is to repeatedly loop (using
 * the Arduino loop function) while setting and monitoring state flags to
 * track progress through the program.  As certain milestones are sensed by
 * the program (e.g. elapsed time or sensed light level) the program state
 * flag is changed to initiate or continue the actions corresponding to the
 * program state.  We have established seven states:
 *  Program state flag  (zero-based)
 *   0 = Waiting for the light-level sensor to indicate appropriate darkness.
 *   1 = Sensed appropriate darkness, but watching and waiting to ensure 
 *       that the darkness is constant for a first period.
 *   2 = Still dark after first waiting period; continuing to monitor
 *       constant darkness for second (final) period.
 *   3 = Been dark long enough...  Begin playing audio recording.
 *   4 = Done playing audio for the night.  'Sleeping' state; waiting for
 *       timer to indicate what should be the next afternoon.
 *   5 = Done with sleeping state.  Waiting for light-level sensor to
 *       indicate daylight.  If sleeping period is set appropriately light
 *       is sensed immediately.
 *   6 = Sensed daylight, but watching and waiting to ensure daylight is
 *       constant through a waiting period
 * The program progressively steps through the seven states.  If a test
 * during one of the 'waiting' steps fails the sketch returns to the prior
 * watching state.  Specifically, inconsistent darkness in states 1 or 2
 * will throw the sketch back into state 0; inconsistent light in state 6
 * will put the sketch back into state 5.  These are intended to avoid
 * accidentally advancing the program state in response to, for example,
 * passing car headlights or dark cloud shadows.
 *
 * While the Arduino does not have a clock per se it does keep track of time
 * with respect to when the microcontroller was last reset/powered.  We use
 * onboard EEPROM memory to record when, with respect to the prior reset,
 * the sketch played its audio program.  We also use the EEPROM memory to
 * record the current program state (for debugging).  We developed three
 * additional sketches to handle work with data we store in EEPROM memory: 
 *  EEPROMReadTime      - Reads and prints (to Serial) event start times
 *  EEPROMReadLastState - Reads and prints (to Serial) most recent state
 *                       value stored in EEPROM
 *  EEPROMClear         - sets all of the bytes of the EEPROM to 0.
 *
 * Because our semi-permanent cricket trap was designed to be installed in a
 * golf course maintenance yard that was locked at night while the cricket
 * audio was to be playing, we built in several mechanisms to let us know
 * what was happening within the program without the benefit of having the
 * Arduino attached to a computer.  The program blinks the attached LED in a
 * pattern defined in the VARIABLES section (just before the 'setup'
 * function below) to let us know the program state remotely (e.g. 20m away,
 * peering through the maintenance yard fence).  Also, we stored ten audio
 * announcement .WAV files on the Wave Shield's SD card to have the sketch
 * audibly announce program state changes.  For example, when the program
 * notices darkness, when when the light sensor has been in darkness long
 * enough, when the program wakes up again the following afternoon,...  The
 * audio announcements are optional and are enabled by setting the 'SPEAK'
 * flag in 'USER SETTINGS' (at the top of the code below) to 'true.'  The
 * user can generate or record their own announcements in numerous ways; we
 * used an online text-to-speech engine to generate the audio for use in our
 * non-commercial purpose.  In the code below these ten announcement files
 * are expected to be named:
 * ready.wav    -  saying "Caltech cricket lure is ready!"
 * state0.wav   -  saying "Daylight proved.  Waiting for darkness."
 * state1.wav   -  saying "Sensed darkness.  Begin proving constant."
 * state2.wav   -  saying "Halfway through proof of darkness."
 * state3.wav   -  saying "Darkness proved.  Beginning audio."
 * state4.wav   -  saying "Audio program complete.  Going to sleep. Goodnight!"
 * state5.wav   -  saying "Now awake and looking for daylight."
 * state6.wav   -  saying "Sensed light.  Begin proving constant."
 * back_to0.wav -  saying "Darkness inconsistent.  Waiting for darkness again."
 * 6_to_5.wav   -  saying "Light level dropped.  Waiting for light again."
 *
 * The purpose of this sketch ultimately is to play a male mole cricket
 * mating call recording at the appropriate time.  As such, we have two .WAV
 * files for the audio program saved on the Wave Shield's SD card.  For
 * field use our two-hour male mole cricket mating call recording is saved
 * as 'cricket.wav', while for in-lab debugging we used a short audio file
 * (a short song) called 'test.wav' in the code below.  Note that, as of
 * this writing, the Wave Shield depends on files being named with the 8.3
 * (short filename) file-naming convention.
 *
 *
 *
 * This sketch based upon example sketch daphc.pde, retrieved from 
 * http://code.google.com/p/wavehc/
 * as part of file wavehc20101009.zip and described in
 * http://www.ladyada.net/make/waveshield/libraryhc.html
 *
 *
 * While this program is coded for our specific application (with an eye
 * toward ease of expansion and customization) we've drawn inspiration and
 * code sections from numerous sources.  Notably, the code for handling
 * audio playback from the SD card on the Wave Shield was derived from the
 * tremendously helpful Wave Shield examples generously provided by
 * ladyada.net at http://www.ladyada.net/make/waveshield/examples.html
 *
 *
 * Christopher J. Cronin
 * cjc@caltech.edu
 * Sternberg Lab
 * California Institute of Technology
 * Pasadena  CA  91125
 * April 19, 2011
 * Expanded introductory comments January 11, 2013
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

 
 

/**************************************************************/
/**************************************************************/
/******* USER SETTINGS (set to 'true' or 'false') *************/
/**************************************************************/
/**************************************************************/
/* Use In-Lab settings?  (Useful for debugging in-lab...)     */
const boolean LAB_SETTINGS = true;
/**************************************************************/
/* Print time in long format when printing time values?       *
 * - Long form has text labels and looks like:                *
 *      "2 Days  1 hours : 23 minutes : 45 seconds"           *
 * - whereas short (or non-long) format looks like:           *
 *      "2 Days  1:23:45"                                     */
const boolean LONGFORM = false;
/**************************************************************/
/* Print descriptive info and snarky comments to Serial port? *
 * (Should be no downside to leaving enabled.) */
const boolean SERIAL_OUT = true;
/**************************************************************/
/* Speak (out loud) state change alerts?                      *
 * (Rather handy for debugging, particularly in the field,    *
 * but requires the ten announcement .WAV files to be saved   *
 * onto the SD card along with the cricket.wav file.)         */
const boolean SPEAK = true;
/**************************************************************/



// INCLUDES
#include <FatReader.h>
#include <SdReader.h>
// #include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"
#include <avr/eeprom.h>   // For reading/writing EEPROM memory
#include <EEPROM.h>       // For simple read/write from EEPROM



// **********************************************************************
// CONSTANTS
//   Arduino pin assignments
const uint8_t ledPin    = 7;    // pin for the LED (13 == built-in led)
const uint8_t sensorPin = A0;    // input pin from the photocell/potentiometer voltage divider
const uint8_t relayPin  = 8;     // pin to control relay

//   Conversions
const uint16_t minutesPerHour   = 60;
const uint16_t secondsPerMinute = 60;
const uint16_t Second_inMilliseconds = (uint16_t) 1000;
const uint32_t Minute_inMilliseconds = (uint32_t) 60 * Second_inMilliseconds;
const uint32_t Hour_inMilliseconds   = (uint32_t) 60 * Minute_inMilliseconds;
const uint32_t Day_inMilliseconds    = (uint32_t) 24 * Hour_inMilliseconds;


// **********************************************************************
// VARIABLES

//   for field vs in-lab settings
unsigned int  DARKNESS_THRESHOLD;  // analog value read at sensor pin
unsigned int  DAYLIGHT_THRESHOLD;  // analog value read at sensor pin
unsigned int  LIGHT_HYSTERESIS;    // hysteresis for light values to help prevent bouncing between states
unsigned long darkProveOutMS;      // Ensure dark out for this long before triggering sound (milliseconds)
unsigned long sleepPeriodMS;       // Sleepy time: Delay before looking for light again (milliseconds)
unsigned long lightProveOutMS;     // Ensure light out for this long before re-arming (milliseconds)
unsigned long ampWarmUpMS;         // Let amplifier warm up this long (milliseconds)
unsigned long lightPrintPeriodMS;  // Period between printing light levels to Serial


//   for Wave Shield
SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader f;      // This holds the information for the file we're play
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

//   for cricket
int lightValue = 0;    // variable to store the value coming from the photocell

/*   Program state flag  (zero-based)
 *    0 = Armed, waiting for first darkness
 *    1 = Sensed first darkness, proving constant darkness for first period
 *    2 = Still dark after first period, proving constant darkness for second (final) period
 *    3 = Been dark long enough, now PLAYING
 *    4 = Sleeping
 *    5 = Looking for first light
 *    6 = Sensed first light, proving constant light
 */
int state = 0;         // State of cricket program

//   for indicator LED
int ledState = LOW;    // ledState used to set the LED
int blinkDelay[][2] = { // {off_msec, on_msec}
                               {2000, 2000},   // for state 0
                               { 250,  250},   // for state 1
                               { 125,  125},   // for state 2
                               {   0, 1000},   // for state 3
                               {4900,  100},   // for state 4
                               {4000, 1000},   // for state 5
                               { 900,  100} }; // for state 6


//   for time
uint32_t currentTime = 0;
uint32_t firstDark   = 0;
uint32_t secondDark  = 0;
uint32_t lastTrigger = 0;
uint32_t firstLight  = 0;
uint32_t lastToggle  = 0;
uint32_t lastLightPrint = 0;

//   for EEPROM memory
//      -- Arduino UNO has 1024 bytes EEPROM memory
//      ---- Use first 1000 bytes (250 dwords) for recording trigger times & resets
//      ---- Reserve last 24 bytes (6 dwords) to record last state, error source and error code
uint16_t nextEmptyEEPROMAddressBlock = 0;
uint8_t  blockSize                   = sizeof(uint32_t);
uint16_t dwordsInEEPROM              = 250;               // dwords available for recording trigger times & resets
uint32_t resetValue                  = (uint32_t)0 - 1;   // maximum unsigned long value: take this as a power-reset flag
uint16_t lastStateByte               = 1023;              // EEPROM (byte) address (byte) for recording last state
uint16_t errorCodeByte               = 1022;              // EEPROM (byte) address for error code
uint16_t errorDataByte               = 1021;              // EEPROM (byte) address for error data
uint16_t errorSourceByte             = 1020;              // EEPROM (byte) address for error source (see error message functions for details)


// **********************************************************************
/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))



// **********************************************************************
// FUNCTION DEFINITIONS (we define them here, but the code is below)
void blinkHelper();
void printTime(unsigned long time);
void playcomplete(char *name);
void playfile(char *name);
int  freeRam(void);
void error_P(const char *str);
void sdErrorCheck(void);
void twoBits();
void doubleClick();
void printLightValue();







// **********************************************************************
// *****  Arduino 'setup' Function  *************************************
// **********************************************************************
void setup() {
  
  /* Indicator LED */
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);     // Turn on LED to show program running
  
  /* Relay set-up */
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);   // Ensure relay is off at start

  // ALWAYS SET UP FOR SERIAL PRINTING, EVEN IF WE DON'T USE IT  if (SERIAL_OUT){
  // Set up for Serial printing
  Serial.begin(9600);
  //  }
  
  
  if (LAB_SETTINGS) {
    // LAB
    DARKNESS_THRESHOLD  = 500;  // analog value read at sensor pin
    DAYLIGHT_THRESHOLD  = 700;  // analog value read at sensor pin
    LIGHT_HYSTERESIS    = 10;   // hysteresis for light values to help prevent bouncing between states
    darkProveOutMS      = (unsigned long) 10 * Second_inMilliseconds;  // Ensure dark out for this long before triggering sound (milliseconds)
    sleepPeriodMS       = (unsigned long) 90 * Second_inMilliseconds;  // Wait this long before starting to look for light (milliseconds)
    lightProveOutMS     = (unsigned long) 20 * Second_inMilliseconds;  // Ensure light out for this long before re-arming (milliseconds)
    ampWarmUpMS         = (unsigned long)  1 * Second_inMilliseconds;  // Let amplifier warm up this long (milliseconds)
    lightPrintPeriodMS  = (unsigned long)  1 * Second_inMilliseconds;  // Period between printing light levels to Serial
  } else {
    // FIELD     //  <=== DEFAULT TO FIELD SETTINGS
    DARKNESS_THRESHOLD  = 75;   // analog value read at sensor pin
    DAYLIGHT_THRESHOLD  = 150;  // analog value read at sensor pin
    LIGHT_HYSTERESIS    = 10;   // hysteresis for light values to help prevent bouncing between states
    darkProveOutMS      = (unsigned long) 10 * Minute_inMilliseconds;  // Ensure dark out for this long before triggering sound (milliseconds)
    sleepPeriodMS       = (unsigned long) 19 * Hour_inMilliseconds;    // Wait this long before starting to look for light (milliseconds)
    lightProveOutMS     = (unsigned long) 10 * Minute_inMilliseconds;  // Ensure light out for this long before re-arming (milliseconds)
    ampWarmUpMS         = (unsigned long)  1 * Second_inMilliseconds;  // Let amplifier warm up this long (milliseconds)
    lightPrintPeriodMS  = (unsigned long)  5 * Second_inMilliseconds;  // Period between printing light levels to Serial
  }


  // ===================================================
  // ========= SET UP FOR WAVE SHIELD ==================
  // ===================================================
  // Annotated code derived from example sketches (linked from www.ladyada.net/make/waveshield/examples.html)

  // === Step 1: Alert user that sketch is running (Optional) === //
  // Say hello to the nice people
  putstring_nl("XXXXXXXXX  === cricketNoDelay ===  XXXXXXXXXX");
  
  // === Step 2: Check and report free RAM (Optional) === //
  putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!
  
  // === Step 3: Set the pin modes for the DAC control === //
  // Set the output pins for the DAC control. This pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  // pin13 LED
  pinMode(13, OUTPUT); 		// Sets up on-board LED for output, but unused in this sketch
  
  // === Step 4: Initialize the SD card === //
  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
    sdErrorCheck();
    while(1);                            // then 'halt' - do nothing!
  }
  
  // === Step 5: Enable partial-block reads (Optional) === //
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
  
  // === Step 6: Try to fine a FAT partition in the first 5 slots === //
  // Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                             // we found one, lets bail
  }
  if (part == 5) {                       // if we ended up not finding one  :(
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();      // Something went wrong, lets print out why
    while(1);                            // then 'halt' - do nothing!
  }
  
  // === Step 7: Report type of FAT partition found (Optional) === //
  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?
  
  // === Step 8: Try to open the root directory === //
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); // Something went wrong,
    while(1);                             // then 'halt' - do nothing!
  }
  
  // === Step 9: Whew! We got past the tough parts. === //
  putstring_nl("Ready!");
  // ===================================================
  // ========= END OF WAVE SHIELD SET UP ===============
  // ===================================================

  /****************************************************************/
  /****************************************************************/
  /* Print out Previous Trigger Event times (since last power-up) *
   * and record next available EEPROM memory block address        */
  putstring_nl("\nPrevious trigger times (since their respective reset):");
  uint32_t triggerTime;
  while (nextEmptyEEPROMAddressBlock < dwordsInEEPROM){
    triggerTime = eeprom_read_dword((uint32_t*)nextEmptyEEPROMAddressBlock);

    if (triggerTime == 0){
      /* Write the 'resetValue' (the maximum unsigned long value into the address block */
      eeprom_write_dword((uint32_t*)nextEmptyEEPROMAddressBlock, (uint32_t)resetValue);

      /* If there's nothing in EEPROM memory, say so */
      if (nextEmptyEEPROMAddressBlock == 0)
        putstring_nl("   ==> No trigger events recorded yet...");

      /* Increment to next open memory block */
      nextEmptyEEPROMAddressBlock += blockSize;  // to get us to the next open spot

      break;     // out of while loop
    }

    /* Print appropriately */
    if (triggerTime == resetValue){
      putstring_nl("  == ADRDUINO RESET ==");
    }
    else {
      printTime(triggerTime);
    }

    nextEmptyEEPROMAddressBlock += blockSize;  // to get us to the next open spot
  }

  /* Handle full memory */
  if (nextEmptyEEPROMAddressBlock >= dwordsInEEPROM){
    putstring_nl("\n------------------------------------------------------------");
    putstring_nl("WARNING: No more empty address blocks in EEPROM!!!");
    putstring_nl("         ==> Trigger event times WILL NO LONGER BE RECORDED");
    putstring_nl("         UNTIL MEMORY IS CLEARED!");
    putstring_nl("------------------------------------------------------------");
  }

  /****************************************************************/
  /****************************************************************/


  /* Click out audio acknowledgement */
  twoBits();
  delay(3000);
  doubleClick();
  
  // Play audio acknowledgement
  if (SPEAK) playcomplete("ready.wav");
  
  // Set last byte in EEPROM to 0 for start of sketch
  EEPROM.write(lastStateByte, state);


}







// **********************************************************************
// *****  Arduino 'loop' Function  **************************************
// **********************************************************************
void loop() {
  // Note current time
  currentTime = millis();
  
  // Read light value (since it is  needed for 5 of the 7 states)
  lightValue = analogRead(sensorPin);
  
  switch (state){
    // ------------------------------------------------
    case 0:
      // case 0 = Armed, waiting for first darkness
      if (lightValue <= DARKNESS_THRESHOLD) {
        firstDark = currentTime;
        if (SPEAK) playcomplete("state1.wav");
        state = 1;
        if (SERIAL_OUT) {
          putstring_nl("Sensed darkness.  Prove constant. (--> state 1)");
        }
        EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
      }   // if (lightValue <= DARKNESS_THRESHOLD)
      break;
      
    // ------------------------------------------------
    case 1:
      // case 1 = Sensed first darkness, proving constant darkness for first period
      if (lightValue > (unsigned int)(DARKNESS_THRESHOLD + LIGHT_HYSTERESIS)) {
        if (SPEAK) playcomplete("back_to0.wav");
        state = 0;
        if (SERIAL_OUT) {
          putstring_nl("Didn't stay dark.  Looking for darkness again. (--> state 0)");
        }
        EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
      } else {
        if ((currentTime - firstDark) >= darkProveOutMS) {
          secondDark = currentTime;
          if (SPEAK) playcomplete("state2.wav");
          state = 2;
          if (SERIAL_OUT) {
            putstring("Been dark for ");
            Serial.print(darkProveOutMS, DEC);
            putstring_nl(" msec.  Keep proving darkness. (--> state 2)");
          }
          EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
        }   // if ((currentTime - firstDark) >= darkProveOutMS)
        
      }   // if (lightValue > (unsigned int)(DARKNESS_THRESHOLD + LIGHT_HYSTERESIS)) ... else
      break;
      
    // ------------------------------------------------
    case 2:
      // case 2 = Still dark after first period, proving constant darkness for second (final) period
      if (lightValue > (unsigned int)(DARKNESS_THRESHOLD + LIGHT_HYSTERESIS)) {
        if (SPEAK) playcomplete("back_to0.wav");
        state = 0;
        if (SERIAL_OUT) {
          putstring_nl("Didn't stay dark.  Looking for darkness again. (--> state 0)");
        }
        EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
      } else {
        if ((currentTime - secondDark) >= darkProveOutMS) {
          // Go to Playing state!
          if (SPEAK) playcomplete("state3.wav");
          state = 3;
          if (SERIAL_OUT) {
            putstring("Been dark for another ");
            Serial.println(darkProveOutMS, DEC);
            putstring_nl(" msec.  Go on to play WAV file! (--> state 3)");
          }
          EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
        }   // if ((currentTime - secondDark) >= darkProveOutMS)
        
      }   // if (lightValue > (unsigned int)(DARKNESS_THRESHOLD + LIGHT_HYSTERESIS)) ... else
     break;

    // ------------------------------------------------
    case 3:
      // case 3 = Been dark long enough, now PLAY
      
      /* Note current time */
      lastTrigger = currentTime;
      if (SERIAL_OUT) {
        putstring("\nStarting playback at:");
        printTime(lastTrigger);
      }

      /* Record trigger time in EEPROM */
      if (nextEmptyEEPROMAddressBlock < dwordsInEEPROM){    // don't try writing if memory is full
        eeprom_write_dword((uint32_t*)nextEmptyEEPROMAddressBlock, (uint32_t)lastTrigger);
        nextEmptyEEPROMAddressBlock += blockSize;  // to get us to the next open spot
      }


      // ***************************
      // PLAY FILE
      if (LAB_SETTINGS)
        playcomplete("test.wav");
      else
        playcomplete("cricket.wav");
      // ***************************

      
      if (SPEAK) playcomplete("state4.wav");
      state = 4;
      if (SERIAL_OUT) {
        putstring_nl("Done playing WAV.  Go to sleep. (--> state 4)");
      }
      EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
      break;
      
    // ------------------------------------------------
    case 4:
      // case 4 = Sleeping
      if ((currentTime - lastTrigger) >= sleepPeriodMS) {
        if (SPEAK) playcomplete("state5.wav");
        state = 5;
        if (SERIAL_OUT) {
          putstring("(yawn...) Been sleeping long enough (");
          Serial.print(sleepPeriodMS, DEC);
          putstring_nl(" msec).  Starting looking for light. (--> state 5)");
        }
        EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
      }
      break;
      
    // ------------------------------------------------
    case 5:
      // case 5 = Looking for first light
      if (lightValue > DAYLIGHT_THRESHOLD) {
        firstLight = currentTime;
        if (SPEAK) playcomplete("state6.wav");
        state = 6;
        if (SERIAL_OUT) {
          putstring_nl("It's light out!  Prove that it stays light. (--> state 6)");
        }
        EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
      }
      break;

    // ------------------------------------------------
    case 6:
      // case 6 = Sensed first light, proving constant light
      if (lightValue < (unsigned int)(DAYLIGHT_THRESHOLD - LIGHT_HYSTERESIS)) {   // Build in hysteresis
        if (SPEAK) playcomplete("6_to_5.wav");
        state = 5;     // Go back to state 5 to wait for light
        if (SERIAL_OUT) {
          putstring_nl("Didn't stay light.  Looking for light again. (--> state 5)");
        }
        EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
      } else {
        if ((currentTime - firstLight) >= lightProveOutMS) {
          if (SPEAK) playcomplete("state0.wav");
          state = 0;
          if (SERIAL_OUT) {
            putstring("Been light out for ");
            Serial.println(lightProveOutMS, DEC);
            putstring_nl(" msec.  Looking for darkness again (--> state 0)");
          }
          EEPROM.write(lastStateByte, state);   // Record new state value in EEPROM
        }
      }   // if (lightValue < (unsigned int)(DAYLIGHT_THRESHOLD - 10)) ... else
      break;
      
    // ------------------------------------------------
    default:
      // We should Never get here; error!
      /* FIXME ==> error("''switch'' statement reached default (which should never happen)."); */
      errorBlink(2);
      break;
  }
  
  // Blink
  blinkHelper();
  
  // Print lightValue periodically, but only when value is relevant
  if ( (SERIAL_OUT) &&  
     ( (state == 0) ||    // Armed, waiting for first darkness
       (state == 1) ||    // Sensed first darkness, proving constant darkness for first period
       (state == 2) ||   // Still dark after first period, proving constant darkness for second (final) period
       (state == 5) ||    // Looking for first light
       (state == 6)   ) )  // Sensed first light, proving constant light
    printLightValue();

}







// **********************************************************************
// *****  Helper Functions  *********************************************
// **********************************************************************

/*
 ******************************************************************
 * These are the utility functions that handle SD card IO,        *
 * audio playback, error reporting and display, and user alerts.  *
 ******************************************************************
*/

 
// **********************************************************************
/* 
 * LED blinker-- changes LED state if appropriate time has elapsed 
 */
void blinkHelper()
{
  // Check whether enough time has elapsed since the last LED 
  // state change (according to the blinkDelay array of 'off' and 'on'
  // times).  If so, toggle the LED state and reset the clock.
  if ((currentTime - lastToggle) >= blinkDelay[state][ledState])
  {
    // Switch state flag
    if (ledState == HIGH)
      ledState = LOW;
    else
      ledState = HIGH;

    // Reset toggle time
    lastToggle = currentTime;
  }

  // Write the new (or rewrite old) LED state (i.e. turn the LED on or off)
  digitalWrite(ledPin, ledState);
}


// **********************************************************************
/*
 * Print formatted version of time to serial port.  
 * Time value is presented to this function in milliseconds.
 */
void printTime(unsigned long time)
{
  uint32_t days;
  uint32_t hours;
  uint32_t minutes;
  uint32_t seconds;


  // Calculate days/hours/minutes/seconds from time value (in milliseconds) 
  uint32_t remainingTime = time;

  days = remainingTime/Day_inMilliseconds;
  remainingTime = remainingTime - (days * Day_inMilliseconds);

  hours = remainingTime/Hour_inMilliseconds;
  remainingTime = remainingTime - (hours * Hour_inMilliseconds);

  minutes = remainingTime/Minute_inMilliseconds;
  remainingTime = remainingTime - (minutes * Minute_inMilliseconds);

  seconds = remainingTime/Second_inMilliseconds;
  remainingTime = remainingTime - (seconds * Second_inMilliseconds);  // this would be milliseconds

  
  /* The actual print-out */
  if (LONGFORM){			// with text labels, like "2 Days  1 hours : 23 minutes : 45 seconds"
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
  } 
  else {					// ...no labels, like "2 Days  1:23:45"
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
  }
}


// **********************************************************************
/*
 * Play a full file from beginning to end with no pause.
 */
void playcomplete(char *name) {
  // ARM AMPLIFIER
  // Trigger relay to connect amplifier, then wait a moment...
  digitalWrite(relayPin, HIGH);
  if (SERIAL_OUT) {
    putstring_nl("Relay is HIGH");
  }
  delay(ampWarmUpMS);   // Let amplifier warm up for a moment
  
  // call our helper to find and play this name
  playfile(name);
  
  // Vamp while audio is playing
  uint8_t   n = 0;             // column counter for .'s
  uint32_t  lastDot = 0;       // time last . was written
  uint16_t  dotPause = 1000;   // delay between writing .'s
  while (wave.isplaying) {     // 'wave' is the wave (audio) object
    // Vamping...
    currentTime = millis();
    
    if (SERIAL_OUT) {
      if ((currentTime - lastDot) > dotPause) {
        putstring(".");
        if (!(++n % 60))Serial.println();     // Write 60 .'s per line
      }
    }
    
    // Keep on blinking
    blinkHelper();
    
    // do nothing (else) while its playing
  }   // while (wave.isplaying)
  
  // done playing...
  
  // DIS-ARM AMPLIFIER
  // Reset relay to turn off amplifier
  digitalWrite(relayPin, LOW);
  if (SERIAL_OUT) {
    putstring_nl("Relay is LOW");
  }
}


// **********************************************************************
/*
 * Helper file to actually play the file...
 */
void playfile(char *name) {
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  // look in the root directory and open the file
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.print(name); return;
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
  
  // ok time to play! start playback
  wave.play();
}


// **********************************************************************
/*
 * Return the number of bytes currently free in RAM, great for debugging!   
 */
int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 


// **********************************************************************
/*
 * print error message and halt
 */
void error_P(const char *str)
{
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  
  // Record error information and start blinking
  EEPROM.write(errorSourceByte, 111);   // Record error value 111 in EEPROM
  errorBlink(3);
  
  // Shouldn't get here, but just in case...:
  while(1);
}


// **********************************************************************
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  putstring(", ");     // or PgmPrint(", ");
  Serial.println(card.errorData(), HEX);

  // Record error information and start blinking
  EEPROM.write(errorSourceByte, 222);   // Record error value 222 in EEPROM
  EEPROM.write(errorCodeByte, (uint8_t) card.errorCode());   // Record error code
  EEPROM.write(errorDataByte, (uint8_t) card.errorData());   // Record error data
  errorBlink(4);
  
  // Shouldn't get here, but just in case...:
  while(1);
}


// **********************************************************************
/*
 * blink error message forever
 */
void errorBlink(int n)
{
  while (true) {
    // Error Code blinks -- n-number of fast blinks (at 2.5Hz)
    for (int i = 0; i<(n); i++) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(300);
    }
    delay(3000);  // then 3-second pause between displays
  

  }
}


// **********************************************************************
/*
 * Click relay in 'Shave-and-a-haircut' pattern as an audible alert
 */
void twoBits()
{
  /* Click relay in Shave-and-a-haircut */
  digitalWrite(relayPin, HIGH);
  delay(500);
  digitalWrite(relayPin, LOW);
  delay(250);
  digitalWrite(relayPin, HIGH);
  delay(250);
  digitalWrite(relayPin, LOW);
  delay(500);
  digitalWrite(relayPin, HIGH);
  delay(1000);
  digitalWrite(relayPin, LOW);
  delay(500);
  digitalWrite(relayPin, HIGH);
}


// **********************************************************************
/*
 * Double-click relay as an audible alert
 */
void doubleClick()
{
  /* Double-click relay as an audible alert */
  digitalWrite(relayPin, HIGH);
  delay(100);
  digitalWrite(relayPin, LOW);
  delay(100);
  digitalWrite(relayPin, HIGH);
  delay(100);
  digitalWrite(relayPin, LOW);
}


// **********************************************************************
/*
 * Print lightValue to Serial if enough time has elapsed
 */
void printLightValue()
{
  // Check whether it's time
  if ((currentTime - lastLightPrint) >= lightPrintPeriodMS)
  {
    // Print lightLevel to Serial
    putstring("lightValue = ");
    Serial.println(lightValue);
    
    // Reset lastLightPrint time
    lastLightPrint = currentTime;
  }
}

// **********************************************************************
// **********************************************************************