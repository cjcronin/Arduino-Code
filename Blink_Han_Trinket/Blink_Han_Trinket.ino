/*
  Blink_Han
  based on example sketch:
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  Christopher J. Cronin
  cjc@caltech.edu
  Sternberg Lab, Caltech
  Pasadena  CA  91125  USA
  January 26, 2015
 */

#define LED_CONTROL 0
#define LED_REF 1

//============================================
// SET THE ON and OFF PERIODS HERE:
unsigned long on_seconds = 5;
unsigned long off_seconds = 10;
//============================================


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(LED_REF, OUTPUT);
  pinMode(LED_CONTROL, OUTPUT);
  
  // initialize serial communication at 9600 bits per second:
//  Serial.begin(9600);
  
  // print out a sample long delay value:
  //Serial.println(off_seconds * 1000);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_REF, HIGH);   // turn the reference LED on (HIGH is the voltage level)
  digitalWrite(LED_CONTROL, LOW);   // set LED control pin LOW to turn external LED ON
//  Serial.print("ON: ");
//  Serial.print(on_seconds); 
//  Serial.println(" seconds");
  delay(on_seconds * 1000);              // stay on this long
  
  digitalWrite(LED_REF, LOW);    // turn the reference LED off by making the voltage LOW
  digitalWrite(LED_CONTROL, HIGH);  // set LED control pin HIGH to turn external LED OFF
//  Serial.print("OFF: ");
//  Serial.print(off_seconds); 
//  Serial.println(" seconds");
  delay(off_seconds * 1000);              // stay off this long
  
}
