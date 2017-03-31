/*
  Blink Morse
  Blinks an LED to send a Morse Code message.
 */
#define d dot();
#define D dash();

int ditMS;
int dit;
int dah;

int led = 1;
int led_control = 0;   // this is the pin used for dimming/illuminating a high power LED with Han's dimmer cord


//****************************************************
// ****************************************************
// PUT STRING HERE:
//
String inString = "Hello world.";
//String inString = "Stop looking at me.";
//String inString = "I am ARDUINO, hear me roar.";

//
// ****************************************************
// ****************************************************

int stringLength = inString.length();
// was int stringLength = outString.length();


void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(led, OUTPUT);     
  pinMode(led_control, OUTPUT);     
  
  /* Force string to lower case */
  inString.toLowerCase();
// was String outString = inString.toLowerCase();


  
  ditMS = 200;
  dit = 1*ditMS;
  dah = 3*ditMS;
  
  
  /* Prepare for printing */
//  Serial.begin(9600);


}

// ------------------------------------------------------
void dot(){
  digitalWrite(led, HIGH);          // set the LED on
  digitalWrite(led_control, LOW);   // set the led controller on
  delay(dit);                       // pause a dit length
  digitalWrite(led, LOW);    // set the LED off
  digitalWrite(led_control, HIGH);   // set the led controller off
  delay(dit);                // pause a dit length
}

void dash(){
  digitalWrite(led, HIGH);          // set the LED on
  digitalWrite(led_control, LOW);   // set the led controller on
  delay(dah);                       // pause a dit length
  digitalWrite(led, LOW);    // set the LED off
  digitalWrite(led_control, HIGH);   // set the led controller off
  delay(dit);                // pause a dit length
}

// ------------------------------------------------------

void btwChars(){
 delay(4*dit); 
}

void btwWords(){
  delay(2*dah);
}

void longPause(){
  delay(6*dah);
}

// ------------------------------------------------------

void playChar(char letter){
  switch (letter) {
    case 'a': d D        break;
    case 'b': d D D D    break;
    case 'c': D d D d    break;
    case 'd': D d d      break;
    case 'e': d          break;
    case 'f': d d D d    break;
    case 'g': D D d      break;
    case 'h': d d d d    break;
    case 'i': d d        break;
    case 'j': d D D D    break;
    case 'k': D d D      break;
    case 'l': d D d d    break;
    case 'm': D D        break;
    case 'n': D d        break;
    case 'o': D D D      break;
    case 'p': d D D d    break;
    case 'q': D D d D    break;
    case 'r': d D d      break;
    case 's': d d d      break;
    case 't': D          break;
    case 'u': d d D      break;
    case 'v': d d d D    break;
    case 'w': d D D      break;
    case 'x': D d d D    break;
    case 'y': D d D D    break;
    case 'z': D D d d    break;
    case '1': d D D D D    break;
    case '2': d d D D D    break;
    case '3': d d d D D    break;
    case '4': d d d d D    break;
    case '5': d d d d d    break;
    case '6': D d d d d    break;
    case '7': D D d d d    break;
    case '8': D D D d d    break;
    case '9': D D D D D    break;
    case '0': D D D D D    break;
    case '.': d D d D d D    break;
    case ',': D D d d D D    break;
    case '?': d d D D d d    break;
    case ' ': btwWords();    break;

    // default: 
      // if nothing else matches, do the default
      // default is optional
  }
  btwChars();
}



void loop() {
  for (int i = 0; i < stringLength; i++){
//    Serial.print(inString[i]);
    playChar(inString[i]);
// was    Serial.print(outString[i]);
// was    playChar(outString[i]);
  }
  
//  Serial.println();
  longPause();

/*
 playChar('a');
 playChar('b');
 playChar('a');
 playChar('a');
 btwWords();
*/


}
