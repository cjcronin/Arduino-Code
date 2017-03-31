/*
 * cricketWAV.pde
 *
 *
 * Sequentially plays all WAV files on the SD card in the attached 
 * Wave Shield, but only at dusk.  Specifically:
 * - When sketch begins system is "armed" and waiting for the perceived
 *   light value to drop below the programmed level (indicating darkness).
 * - When darkness is sensed, sketch waits a short while then re-checks 
 *   the light level.  If the light level is above the threshold the 
 *   sketch returns to the armed state and continues looking for darkness.
 * - If darkness is sensed a second time, the sketch waits a short again 
 *   then re-checks the light level a third time.  If the third light level 
 *   check is above the threshold the sketch returns to the armed state 
 *   and continues looking for darkness.
 * - If darkness is sensed a third time, the sketch enables the attached 
 *   relay which turns on the attached amplifier, then starts playing all
 *   of the WAV files on the SD card in the Wave Shield.
 * - When the WAV file(s) finishes playing the sketch goes to "sleep" until
 *   the next afternoon.  
 * - When the sketch verifies that enough sleep time has elapsed, the sketch
 *   re-arms and begins looking for darkness again.
 *
 *
 * Sketch based upon daphc.pde from http://code.google.com/p/wavehc/
 * and described in http://www.ladyada.net/make/waveshield/libraryhc.html
 *
 * Christopher J. Cronin
 * Sternberg Lab, Caltech
 * Pasadena, CA
 * cjc@caltech.edu
 * March 22, 2011
 */



/********************************************************/
/* Uncomment the next line to use In-Lab settings       */
#define LAB              // Use In-Lab settings
/********************************************************/
/* Uncomment the next line to print time in long format */
// #define LONGFORM      // Print verbose time values
/********************************************************/




/*
 * This example plays every .WAV file it finds on the SD card in a loop
 */
#include "WaveHC.h"
#include "WaveUtil.h"
#include <avr/eeprom.h>   // For reading/writing EEPROM memory

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

uint8_t dirLevel; // indent level for file/dir names    (for prettyprinting)
dir_t dirBuf;     // buffer for directory reads


int sensorPin = A0;    // input pin from the photocell/potentiometer voltage divider
static const uint8_t  relayPin  = 8;     // pin to control relay
int ledPin = 13;       // pin for the LED (13 == built-in led)
int lightValue = 0;    // variable to store the value coming from the photocell

boolean armed;
unsigned long lastTrigger;
uint16_t nextEmptyEEPROMAddressBlock = 0;
uint8_t  blockSize                   = sizeof(uint32_t);
uint16_t dwordsInEEPROM              = 256;  // for Arduino UNO
uint32_t resetValue                  = (uint32_t)0 - 1;     // maximum unsigned long value: take this as a power-reset flag

/* Conversions */
unsigned int  minutesPerHour   = 60;
unsigned int  secondsPerMinute = 60;
unsigned int  millisecondsPerSecond = (unsigned int) 1000;
unsigned long millisecondsPerMinute = (unsigned long)60 * millisecondsPerSecond;
unsigned long millisecondsPerHour   = (unsigned long)60 * millisecondsPerMinute;
/*   ===> could save some RAM this way maybe:
 static const uint8_t minutesPerHour = 60;
 static const uint8_t secondsPerMinute = 60;
 static const uint16_t millisecondsPerSecond = 1000;
 static const uint16_t millisecondsPerMinute = 1000 * 60;
 static const uint32_t millisecondsPerHour = 1000 * 60 * 60;
 */

/* Settings */
#ifdef LAB   // <=== USE LAB SETTINGS
// LAB
unsigned int LIGHT_THRESHOLD     = 500;  // analog value read at sensor pin
unsigned int DAYLIGHT_THRESHOLD  = 700;  // analog value read at sensor pin
unsigned int lightcheckDelayMS   = (unsigned int) 10 * millisecondsPerSecond;  // millisecond delay before re-checking light level
unsigned long triggerDelayMS     = (unsigned long) 2 * millisecondsPerMinute;  // millisecond delay before re-arming light check
unsigned int shortTriggerDelayMS = (unsigned int)  1 * millisecondsPerMinute;  // short (just-in-case needed) millisecond delay before re-arming
#else
// FIELD     //  <=== DEFAULT TO FIELD SETTINGS
unsigned int LIGHT_THRESHOLD     = 75;   // analog value read at sensor pin
unsigned int DAYLIGHT_THRESHOLD  = 150;  // analog value read at sensor pin
unsigned long lightcheckDelayMS  = (unsigned long) 10 * millisecondsPerMinute;  // millisecond delay before re-checking light level
unsigned long triggerDelayMS     = (unsigned long) 20 * millisecondsPerHour;    // millisecond delay before re-arming light check
unsigned int shortTriggerDelayMS = (unsigned int)  1  * millisecondsPerMinute;  // short (just-in-case needed) millisecond delay before re-arming
#endif


/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))

// Function definitions (we define them here, but the code is below)
void play(FatReader &dir);
void twoBits();
void doubleClick();
void printTime(unsigned long time);




//////////////////////////////////// SETUP
void setup() 
{
  Serial.begin(9600);           // set up Serial library at 9600 bps for debugging

  /* Indicator LED */
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);  
  // turn the ledPin on to show program running
  digitalWrite(ledPin, HIGH);

  /* Relay set-up */
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);   // Ensure relay is off at start

  /* Timer set-up */
  armed = true;
  lastTrigger = millis();

  putstring_nl("\nWave test!");  // say we woke up!

  putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(FreeRam());

  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    error("Card init. failed!");  // Something went wrong, lets print out why
  }

  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);

  // Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {   // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                           // we found one, lets bail
  }
  if (part == 5) {                     // if we ended up not finding one  :(
    error("No valid FAT partition!");  // Something went wrong, lets print out why
  }

  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?

  // Try to open the root directory
  if (!root.openRoot(vol)) {
    error("Can't open root dir!");      // Something went wrong,
  }

  // Whew! We got past the tough parts.
  putstring_nl("Files found (* = fragmented):");

  // Print out all of the files in all the directories.
  root.ls(LS_R | LS_FLAG_FRAGMENTED);


  /***********************************/
  /*    Test-print variables here    */
  //           Serial.println(shortTriggerDelayMS);
  /***********************************/

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

}      // setup()




//////////////////////////////////// LOOP
void loop() 
{ 
  if (armed) {

    /*******************************************************/
    /* FIRST light check */
    lightValue = analogRead(sensorPin);
    putstring("lightValue = ");
    Serial.println(lightValue);

    delay(1 * millisecondsPerSecond);

    if (lightValue < LIGHT_THRESHOLD){

      putstring("lightValue (first check) = ");
      Serial.println(lightValue);
      putstring("  (lightValue must be less than LIGHT_THRESHOLD: ");
      Serial.print(LIGHT_THRESHOLD);
      putstring_nl(")");

      // turn the ledPin off to indicate darkness sensed
      digitalWrite(ledPin, LOW);

      /* Click relay as an audio indicator */
      doubleClick();

      putstring("Passed FIRST light check.  Waiting ");
      Serial.print(lightcheckDelayMS, DEC);
      putstring_nl(" milliseconds till SECOND light check.");
      delay(lightcheckDelayMS);

      /*******************************************************/
      /* SECOND light check */
      lightValue = analogRead(sensorPin);
      putstring("lightValue (second check) = ");
      Serial.println(lightValue);
      if (lightValue < LIGHT_THRESHOLD){

        /* Click relay as an audio indicator */
        doubleClick();
        delay(500);
        doubleClick();

        putstring("Passed SECOND light check.  Waiting ");
        Serial.print(lightcheckDelayMS, DEC);
        putstring_nl(" milliseconds till THIRD light check.");
        delay(lightcheckDelayMS);

        /*******************************************************/
        /* THIRD (Final) light check */
        lightValue = analogRead(sensorPin);
        putstring("lightValue (third check) = ");
        Serial.println(lightValue);
        if (lightValue < LIGHT_THRESHOLD){
          /* Light level is consistantly low, so begin playing... */

          armed = false;

          putstring("Passed THIRD light check.  Starting audio playback...");


          /*************** AUDIO PLAYBACK - START **********************/
          /* Note current time */
          lastTrigger = millis();
          putstring("\nStarting playback at:");
          printTime(lastTrigger);

          /* Record trigger time in EEPROM */
          if (nextEmptyEEPROMAddressBlock < dwordsInEEPROM){    // don't try writing if memory is full
            eeprom_write_dword((uint32_t*)nextEmptyEEPROMAddressBlock, (uint32_t)lastTrigger);
            nextEmptyEEPROMAddressBlock += blockSize;  // to get us to the next open spot
          }

          /* Trigger relay to connect amplifier, then wait a moment... */
          digitalWrite(relayPin, HIGH);
          putstring_nl("Relay is HIGH");
          delay(1000);

          /* Trigger playing WAV files */
          root.rewind();
          play(root);

          // turn the ledPin back on to show program running
          digitalWrite(ledPin, HIGH);

          /* Reset relay to turn off amplifier */
          digitalWrite(relayPin, LOW);
          putstring_nl("Relay is LOW");
          /*************** AUDIO PLAYBACK - END ************************/



          /*************** SLEEPING - START **********************/
          /* Wait a LONG TIME before re-arming */
          Serial.println();
          putstring("Finished playing.  Now pausing ");
          Serial.print(triggerDelayMS, DEC);
          putstring_nl(" milliseconds before re-arming.");
          delay(triggerDelayMS);
          /*************** SLEEPING - END ************************/

          /*************** WAKING BACK UP - START **********************/
          /* Assume it is still dark out, then wait for light */
          boolean stillDark = true;
          boolean waitingForFirstLight = true;

          while (stillDark){
            waitingForFirstLight = true;
            while (waitingForFirstLight) {
              lightValue = analogRead(sensorPin);
              if (lightValue > DAYLIGHT_THRESHOLD){
                waitingForFirstLight = false;
              }
              delay(1000);
            }

            /* Wait a minute and re-prove light - Just to make sure... */
            delay(1 * millisecondsPerMinute);

            lightValue = analogRead(sensorPin);
            if (lightValue > DAYLIGHT_THRESHOLD) {
              stillDark = false;  // We've proven light twice, otherwise go back to first light check
            }
          }


          /* At this point we've re-proven daylight...  Proceed */
          /*************** WAKING BACK UP - END ************************/

        } // Third light check
      }   // Second light check
    }     // First light check
  }       // if armed
  /*******************************************************/


  /*************** RE-ARMING - START **********************/
  // Ensure ledPin is on to show program running
  digitalWrite(ledPin, HIGH);

  /* Ensure relay is set LOW to disconnect amplifier */
  digitalWrite(relayPin, LOW);


  if (!armed) {
    /* But (belt & suspenders) ensure we've waited long enough before re-arming */
    while (millis() < lastTrigger + triggerDelayMS){
      putstring_nl("...Belt & suspenders pause...");
      delay(shortTriggerDelayMS);   // Wait a short while before re-checking time
    }
  } 

  /* It's time once again... */
  if (!armed){
    armed = true;
    putstring_nl("Re-armed (after triggerDelayMS AND 'belt & suspenders' pauses).");
  }
  /*************** RE-ARMING - END ************************/


}      // loop()




/////////////////////////////////// HELPERS
/*
 * print error message and halt
 */
void error_P(const char *str)
{
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
/*
 * play recursively - possible stack overflow if subdirectories too nested
 */
void play(FatReader &dir)
{
  FatReader file;
  while (dir.readDir(dirBuf) > 0) {    // Read every file in the directory one at a time

    // Skip it if not a subdirectory and not a .WAV file
    if (!DIR_IS_SUBDIR(dirBuf)
      && strncmp_P((char *)&dirBuf.name[8], PSTR("WAV"), 3)) {
      continue;
    }

    Serial.println();            // clear out a new line

    for (uint8_t i = 0; i < dirLevel; i++) {
      Serial.print(' ');       // this is for prettyprinting, put spaces in front
    }
    if (!file.open(vol, dirBuf)) {        // open the file in the directory
      error("file.open failed");          // something went wrong
    }

    if (file.isDir()) {                   // check if we opened a new directory
      putstring("Subdir: ");
      printEntryName(dirBuf);
      dirLevel += 2;                      // add more spaces
      // play files in subdirectory
      play(file);                         // recursive!
      dirLevel -= 2;    
    }
    else {
      // Aha! we found a file that isnt a directory
      putstring("Playing ");
      printEntryName(dirBuf);              // print it out
      if (!wave.create(file)) {            // Figure out, is it a WAV proper?
        putstring(" Not a valid WAV");     // ok skip it
      } 
      else {
        Serial.println();                  // Hooray it IS a WAV proper!
        wave.play();                       // make some noise!

        uint8_t n = 0;
        while (wave.isplaying) {// playing occurs in interrupts, so we print dots in realtime
          putstring(".");
          if (!(++n % 32))Serial.println();
          delay(100);
        }       
        sdErrorCheck();                    // everything OK?
        // if (wave.errors)Serial.println(wave.errors);     // wave decoding errors
      }
    }
  }
}

/********************************************/
/*
 * Click relay in Shave-and-a-haircut pattern
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

/*
 * Double-click relay
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


/*
 * Print formatted version of time to serial port
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

