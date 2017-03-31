/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(13, OUTPUT);     
}

void loop() {
  digitalWrite(13, HIGH);   // set the LED on
  delay(750);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(500);              // wait for a second

  digitalWrite(13, HIGH);   // set the LED on
  delay(250);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(350);              // wait for a second

  digitalWrite(13, HIGH);   // set the LED on
  delay(250);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(350);              // wait for a second
}
