/*
This code is for indicating scale selection with LEDs
*/

//Pin numbers:
const int rightButton = 13;    // the number of the pushbutton pin
const int leftButton = A2;    // the number of the pushbutton pin
const byte leds[NUM_LEDS] = {12, 11, 10, 9, A0, A1};   // the number of the LED pins

// Variables:
int ledCounter = 0;                  // start led and counter for the led array

// Functions:
void juggleLeds();
void nextLed();
void previousLed();

///////////////////////////
// LED JUGGLE
///////////////////////////
void juggleLeds() {                //harashoo light show for starters

  for (int a = 0; a < 3; a++) {      //repeat 3 times

    for (int i = 0; i < NUM_LEDS; i++) {    //led train moves left

      digitalWrite(leds[i - 1], LOW);
      delay(40);
      digitalWrite(leds[i], HIGH);
      delay(40);
    }

    for (int i = NUM_LEDS; i > 0; i--) {    //and other way

      digitalWrite(leds[i + 1], LOW);
      delay(40);
      digitalWrite(leds[i], HIGH);
      delay(40);
    }
    digitalWrite(leds[0], LOW);
    delay(40);
  }

  delay(50);

  for (int i = 0; i < 4; i++) {      //finally blink leds, repeat 4 times
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(leds[i], HIGH);
    }
    delay(200);
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(leds[i], LOW);
    }
    delay(200);
  }
}

///////////////////////////
// MOVE LEDS
///////////////////////////
void nextLed() {
  digitalWrite(leds[ledCounter], LOW);
  ++ledCounter;                                 //increment
  if (ledCounter > (NUM_LEDS-1)) ledCounter = 0;           //limit the increment according to the amount of leds
  digitalWrite(leds[ledCounter], HIGH);         //shut previous LED
}

void previousLed() {
  digitalWrite(leds[ledCounter], LOW);          //shut previous LED
  --ledCounter;                                 //increment
  if (ledCounter < 0) ledCounter = (NUM_LEDS-1);           //limit the increment according to the amount of leds
  digitalWrite(leds[ledCounter], HIGH);
}
