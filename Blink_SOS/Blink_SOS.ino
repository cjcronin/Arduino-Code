/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
int ditMS;
int dit;
int dah;
int led;

void setup() {                
  led = 13;
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(led, OUTPUT);     
  
  ditMS = 150;
  dit = 1*ditMS;
  dah = 3*ditMS;

}

void loop() {
  /* S */
  digitalWrite(led, HIGH);   // set the LED on
  delay(dit);              // wait for a second
  digitalWrite(led, LOW);    // set the LED off
  delay(dit);              // wait for a second

  digitalWrite(led, HIGH);   // set the LED on
  delay(dit);              // wait for a second
  digitalWrite(led, LOW);    // set the LED off
  delay(dit);              // wait for a second

  digitalWrite(led, HIGH);   // set the LED on
  delay(dit);              // wait for a second
  digitalWrite(led, LOW);    // set the LED off

  delay(dah);

  /* O */
  digitalWrite(led, HIGH);   // set the LED on
  delay(dah);              // wait for a second
  digitalWrite(led, LOW);    // set the LED off
  delay(dit);              // wait for a second

  digitalWrite(led, HIGH);   // set the LED on
  delay(dah);              // wait for a second
  digitalWrite(led, LOW);    // set the LED off
  delay(dit);              // wait for a second

  digitalWrite(led, HIGH);   // set the LED on
  delay(dah);              // wait for a second
  digitalWrite(led, LOW);    // set the LED off

  delay(dah);

  /* S */
  digitalWrite(led, HIGH);   // set the LED on
  delay(dit);              // wait for a second
  digitalWrite(led, LOW);    // set the LED off
  delay(dit);              // wait for a second

  digitalWrite(led, HIGH);   // set the LED on
  delay(dit);              // wait for a second
  digitalWrite(led, LOW);    // set the LED off
  delay(dit);              // wait for a second

  digitalWrite(led, HIGH);   // set the LED on
  delay(dit);              // wait for a second
  digitalWrite(led, LOW);    // set the LED off

  delay(dah);
  delay(dah);
  delay(dah);

}
