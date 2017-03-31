/*****************************************
 * Artificial Cricket Signal Generator
 *
 * Developed by:
 * Christopher J. Cronin
 * cjc@caltech.edu
 * 
 *****************************************/

/* Declare variables */
// char cricket[] = "acletus";   /* can be "acletus" or "vicinus" */
// char cricket[] = "vicinus";   /* can be "acletus" or "vicinus" */
char cricket[] = "house";   /* for test only */

unsigned int  carrierFrequency;  // Carrier tone frequency [Hz]
unsigned int  pulseRate;         // Pulse rate [Hz]
unsigned int  pulseLength;       // Duration of carrier tone pulse [msec]

unsigned long restLength;        // Duration of silence between pulses [usec]
unsigned long restMSec;          // Whole-msec part of Silence duration [msec]
unsigned int  restUSec;          // Remaining part of Silence duration [usec]

unsigned long pulsePeriod;       // (1/pulseRate)*1000000, [usec]
unsigned long pulseMSec;          // Whole-msec part of Pulse period [msec]
unsigned int  pulseUSec;          // Remaining part of Pulse period [usec]

float dutyCycle = 0.50;          // Target ratio of pulseLength to pulsePeriod

unsigned long minuteOfPulses;    // Number of pulses in 60 seconds


/****************************
 *** Scapteriscus acletus ***
 ****************************/
const unsigned int acletusCW = 2700;       // Carrier Wave Frequency, in Hz
const unsigned int acletusPulseRate = 50;  // Pulse Rate, in Hz (pulses/sec)

/****************************
 *** Scapteriscus vicinus ***
 ****************************/
const unsigned int vicinusCW = 3300;       // Carrier Wave Frequency, in Hz
const unsigned int vicinusPulseRate = 130; // Pulse Rate, in Hz (pulses/sec)

/****************************
 *** HOUSE CRICKET ***
 ****************************/
const unsigned int houseCW = 2700;       // Carrier Wave Frequency, in Hz
const unsigned int housePulseRate = 58; // Pulse Rate, in Hz (pulses/sec)

const unsigned int speakerPin = 8;



void printstate(){
  Serial.print("pulseRate = ");
  Serial.print(pulseRate, DEC);
  Serial.println(" Hz");
  
  Serial.print("pulsePeriod = ");
  Serial.print(pulsePeriod, DEC);
  Serial.println(" usec");
  
  Serial.print("pulseLength = ");
  Serial.print(pulseLength, DEC);
  Serial.println(" msec");
  
  Serial.print("restLength = ");
  Serial.print(restLength, DEC);
  Serial.println(" usec");
  
  Serial.print("restMSec = ");
  Serial.print(restMSec, DEC);
  Serial.println(" msec");
  
  Serial.print("restUSec = ");
  Serial.print(restUSec, DEC);
  Serial.println(" usec");
  
  Serial.print("carrierFrequency = ");
  Serial.print(carrierFrequency, DEC);
  Serial.println(" Hz");
  
  Serial.println(" ------------------------------------------");
  Serial.println(" ");
}



void setup(){
  /* Set up cricket */
  if (strcmp(cricket, "acletus") == 0){
    carrierFrequency = acletusCW;
    pulseRate = acletusPulseRate;
  }
  else if (strcmp(cricket, "house") == 0){  /* for test onlydefaults to vicinus */
    carrierFrequency = houseCW;
    pulseRate = housePulseRate;
  }
  else {  /* defaults to vicinus */
    carrierFrequency = vicinusCW;
    pulseRate = vicinusPulseRate;
  }
  
  /* Establish pulse period and duration */
  pulsePeriod = 1000000 * 1/pulseRate;                       // in usec
  pulseLength = (unsigned int)(dutyCycle*pulsePeriod/1000);  // in msec
  
  pulseMSec = pulsePeriod / 1000;   // in msec
  pulseUSec = pulsePeriod % 1000;   // in usec

  /* Ensure pulseLength is no longer than pulsePeriod 
   * (e.g. in case we set a high duty cycle*/
  if ((pulseLength*1000) >= pulsePeriod) {
    pulseLength = (unsigned int)(pulsePeriod/1000);
  }
  
  /* Establish restLength to fill up the rest of the time in the pulsePeriod */
  restLength = pulsePeriod - (pulseLength*1000);  
  restMSec = restLength / 1000;   // in msec
  restUSec = restLength % 1000;   // in usec
  
  /* Prepare for printing */
  Serial.begin(9600);
  
  
  Serial.println("Start of setup printing");
  printstate();
  
  
  
  
  int oneSecOfPulses = 1 * pulseRate;
  int tenSecOfPulses = 10 * pulseRate;
  minuteOfPulses = 60 * pulseRate;
  
  unsigned long startDelay; 
  unsigned long endDelay;
  unsigned long actualDelay;
  
  unsigned long startTone; 
  unsigned long endTone;
  unsigned long actualTone;
  
  unsigned long startPulse; 
  unsigned long endPulse;
  unsigned long actualPulse;
  
  ////////////////////////////////////////////////////////////////
//  for (int i = 0;  i < oneSecOfPulses; i++){
//  for (int i = 0;  i < tenSecOfPulses; i++){
  // minuteOfPulses = 60 * pulseRate;
   for (int i = 0;  i < minuteOfPulses; i++){
    
//    startPulse = micros();
    
    //startTone = millis();
    tone(speakerPin, carrierFrequency, pulseLength);
    //endTone = millis();
    //actualTone = endTone - startTone;
    //Serial.println(actualTone);
    
    //startDelay = micros();
    
    // Delay between pulses
    //delay(restMSec);
    //delayMicroseconds(restUSec);
    
    // Try pausing the WHOLE pulse period, figuring that tone doesn't block execution
    delay(pulseMSec);
    delayMicroseconds(pulseUSec);
    
    
    //endDelay = micros();
    //actualDelay = endDelay - startDelay;
    // Serial.print("actualDelay [usec] = ");
    //Serial.println(actualDelay);
    // delayMicroseconds(restLength);
    
//    endPulse = micros();
//    actualPulse = endPulse - startPulse;
//    Serial.println(actualPulse);

  }

}



void loop(){
  /*
  int tenSecOfPulses = 10 * pulseRate;
  for (int i = 0;  i < tenSecOfPulses; i++){
  // minuteOfPulses = 60 * pulseRate;
  // for (int i = 0;  i < minuteOfPulses; i++){
    
    if (i == 1){
      printstate();
    }
    
    tone(speakerPin, carrierFrequency, pulseLength);
    delay(restMSec);
    delayMicroseconds(restUSec);
    // delayMicroseconds(restLength);
  }
  */
}
