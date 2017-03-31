/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

#define OUTPIN 7
#define REFPIN 13

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(OUTPIN, OUTPUT);     
  pinMode(REFPIN, OUTPUT);
}

void loop() {
  digitalWrite(OUTPIN, HIGH);   // set the LED on
  digitalWrite(REFPIN, HIGH);   // set the LED on
  delay(2000);              // wait for a second
  digitalWrite(OUTPIN, LOW);    // set the LED off
  digitalWrite(REFPIN, LOW);    // set the LED off
  delay(5000);              // wait for a second
}
