/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

int pin = 13;
 int delaytime;

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(pin, OUTPUT);     
   delaytime = 10;
  // Serial.begin(9600);        // connect to the serial port

}

void loop() {
  digitalWrite(pin, HIGH);   // set the LED on
  delay(delaytime);              // wait for a second
  digitalWrite(pin, LOW);    // set the LED off
  delay(delaytime);              // wait for a second
  // delaytime++;
  // Serial.println(delaytime);
}
