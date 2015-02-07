/*
 ************************************************
 ************** BANAANIPIANO *********************
 ************************************************

 //////////////////////////////////////////////////
 //////// THIS CODE IS LOOSELY BASED ON: //////////
 ////////  MaKey MaKey FIRMWARE v1.4.1   //////////
 ( by: Eric Rosenbaum, Jay Silver, and Jim Lindblom
 MIT Media Lab & Sparkfun, release: 7/5/2012 )
 //////////////////////////////////////////////////

 Veikko Kero 5/2014
 */

////////////////////////
// DEFINED CONSTANTS////
////////////////////////

#define BUFFER_LENGTH    3      // 3 bytes gives us 24 samples
#define NUM_INPUTS       9      // one whole octave, 8 keys
#define NUM_SCALES       6      // amount of scales available 
#define NUM_LEDS         6      // number of scale indicator leds (same as scales, obviously...)
//#define TARGET_LOOP_TIME 694  // (1/60 seconds) / 24 samples = 694 microseconds per sample
//#define TARGET_LOOP_TIME 758  // (1/55 seconds) / 24 samples = 758 microseconds per sample
#define TARGET_LOOP_TIME 744    // (1/56 seconds) / 24 samples = 744 microseconds per sample 

#define DEF_CHANNEL 1           // default MIDI Channel 

#include "settings.h"
#include "selector_Leds.h"

/////////////////////////
// STRUCT ///////////////
/////////////////////////
typedef struct {
  byte pinNumber;
  byte scale;
  byte measurementBuffer[BUFFER_LENGTH];
  boolean oldestMeasurement;
  byte bufferSum;
  boolean pressed;
  boolean prevPressed;
}
MakeyMakeyInput;

MakeyMakeyInput inputs[NUM_INPUTS];

///////////////////////////////////
// VARIABLES //////////////////////
///////////////////////////////////
byte Oct = 36;      //default octave C2
byte select = 1;    //default scale chromatic C

int bufferIndex = 0;
byte byteCounter = 0;
byte bitCounter = 0;

int pressThreshold;
int releaseThreshold;
boolean inputChanged;

// Pin Numbers
byte pinNumbers[NUM_INPUTS] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8
};

byte potPin = 5;
byte octUp = A3;
byte octDwn = A4;

byte potValue = 0;         // value for the potentiometer
byte analogThreshold = 2;  // threshold value for the noise
int reading = 0;           // reading from the potentiometer
int previousReading = 0;   // reading from the previous loop

// timing
int loopTime = 0;
int prevTime = 0;
int loopCounter = 0;

///////////////////////////
// FUNCTIONS //////////////
///////////////////////////
void initializeArduino();
void initializeInputs();
void updateMeasurementBuffers();
void updateBufferSums();
void updateBufferIndex();
void noteOn();
void noteOff();
void controlChange();
void updateScale();
void updateOctave();
void updateInputStates();
void potAction();
void addDelay();

//////////////////////
// SETUP /////////////
//////////////////////
void setup()
{
  Serial.begin(9600);
  initializeArduino();
  initializeInputs();
}

////////////////////
// MAIN LOOP ///////
////////////////////
void loop()
{
  updateMeasurementBuffers();
  updateBufferSums();
  updateBufferIndex();
  updateScale();
  updateOctave();
  updateInputStates();
  potAction();
  addDelay();
}

//////////////////////////
// INITIALIZE ARDUINO
//////////////////////////

void initializeArduino() {
  /* Set up input pins
   DEactivate the internal pull-ups, since we're using external resistors */
  for (int i = 0; i < NUM_INPUTS; i++)
  {
    pinMode(pinNumbers[i], INPUT);
    digitalWrite(pinNumbers[i], LOW);
  }

  pinMode(rightButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(octUp, INPUT);
  pinMode(octDwn, INPUT);

  for (int i = 0; i < NUM_LEDS; i++) pinMode(leds[i], OUTPUT);    //set led pins
  juggleLeds();
  // set the default state
  digitalWrite(leds[ledCounter], HIGH);
}

///////////////////////////
// INITIALIZE INPUTS
///////////////////////////

void initializeInputs() {

  float thresholdPerc = SWITCH_THRESHOLD_OFFSET_PERC;
  float thresholdCenterBias = SWITCH_THRESHOLD_CENTER_BIAS / 50.0;
  float pressThresholdAmount = (BUFFER_LENGTH * 8) * (thresholdPerc / 100.0);
  float thresholdCenter = ( (BUFFER_LENGTH * 8) / 2.0 ) * (thresholdCenterBias);
  pressThreshold = int(thresholdCenter + pressThresholdAmount);
  releaseThreshold = int(thresholdCenter - pressThresholdAmount);

  for (int i = 0; i < NUM_INPUTS; i++) {
    inputs[i].pinNumber = pinNumbers[i];
    inputs[i].scale = i;                  //default scale chromatic C

    for (int j = 0; j < BUFFER_LENGTH; j++) {
      inputs[i].measurementBuffer[j] = 0;
    }
    inputs[i].oldestMeasurement = 0;
    inputs[i].bufferSum = 0;

    inputs[i].pressed = false;
    inputs[i].prevPressed = false;
  }
}


//////////////////////////////
// UPDATE MEASUREMENT BUFFERS
//////////////////////////////
void updateMeasurementBuffers() {

  for (int i = 0; i < NUM_INPUTS; i++) {

    // store the oldest measurement, which is the one at the current index,
    // before we update it to the new one
    // we use oldest measurement in updateBufferSums
    byte currentByte = inputs[i].measurementBuffer[byteCounter];
    inputs[i].oldestMeasurement = (currentByte >> bitCounter) & 0x01;

    // make the new measurement
    boolean newMeasurement = digitalRead(inputs[i].pinNumber);

    // invert so that true means the switch is closed
    newMeasurement = !newMeasurement;

    // store it
    if (newMeasurement) {
      currentByte |= (1 << bitCounter);
    }
    else {
      currentByte &= ~(1 << bitCounter);
    }
    inputs[i].measurementBuffer[byteCounter] = currentByte;
  }
}

///////////////////////////
// UPDATE BUFFER SUMS
///////////////////////////

void updateBufferSums() {

  // the bufferSum is a running tally of the entire measurementBuffer
  // add the new measurement and subtract the old one

  for (int i = 0; i < NUM_INPUTS; i++) {
    byte currentByte = inputs[i].measurementBuffer[byteCounter];
    boolean currentMeasurement = (currentByte >> bitCounter) & 0x01;
    if (currentMeasurement) {
      inputs[i].bufferSum++;
    }
    if (inputs[i].oldestMeasurement) {
      inputs[i].bufferSum--;
    }
  }
}

///////////////////////////
// UPDATE BUFFER INDEX
///////////////////////////

void updateBufferIndex() {
  bitCounter++;
  if (bitCounter == 8) {
    bitCounter = 0;
    byteCounter++;
    if (byteCounter == BUFFER_LENGTH) {
      byteCounter = 0;
    }
  }
}

///////////////////////////
// MIDI COMMANDS
///////////////////////////

void noteOn(byte pitch, byte velocity) {
  MIDIEvent noteOn = {0x09, 0x90 | DEF_CHANNEL, pitch, velocity};
  MIDIUSB.write(noteOn);
}

void noteOff(byte pitch, byte velocity) {
  MIDIEvent noteOff = {0x08, 0x80 | DEF_CHANNEL, pitch, velocity};
  MIDIUSB.write(noteOff);
}

void controlChange(byte ccNumber, byte value) {
  MIDIEvent controlChange = {0x0B, 0xB0 | DEF_CHANNEL, ccNumber, value};
  MIDIUSB.write(controlChange);
}

///////////////////////////
// CHECK AND UPDATE SCALE
///////////////////////////
void updateScale() {
  if (digitalRead(leftButton) || digitalRead(rightButton)) {    //check input
    delay(50);                                                  //a short debounce

    if (digitalRead(leftButton)) {
      delay(200);
      ++select;                                 //increment the selection
      if (select > (NUM_SCALES)) select = 1;    //limit the increment (start over)
      nextLed();                                //change led
    }
    if (digitalRead(rightButton)) {
      delay(200);
      --select;                                 //decrement the selection
      if (select < 1) select = 6;               //limit the decrement (continue from the top)
      previousLed();                            //change led
    }

    switch (select) {
      case 1:
        for (int i = 0; i < NUM_INPUTS; i++) {    //set new scale
          inputs[i].scale = C_chromatic[i];
        }
        break;
      case 2:
        for (int i = 0; i < NUM_INPUTS; i++) {    //set new scale
          inputs[i].scale = Dm[i];
        }
        break;
      case 3:
        for (int i = 0; i < NUM_INPUTS; i++) {    //set new scale
          inputs[i].scale = Gm[i];
        }
        break;
      case 4:
        for (int i = 0; i < NUM_INPUTS; i++) {    //set new scale
          inputs[i].scale = Am[i];
        }
        break;
      case 5:
        for (int i = 0; i < NUM_INPUTS; i++) {    //set new scale
          inputs[i].scale = Am_pentatonic[i];
        }
        break;
      case 6:
        for (int i = 0; i < NUM_INPUTS; i++) {    //set new scale
          inputs[i].scale = Bm[i];
        }
        break;
    }
    Serial.println(select);
    for (int i = 0; i < NUM_INPUTS; i++) {
      Serial.print(inputs[i].scale);
    }
    Serial.print(' ');
  }
}

///////////////////////////
// UPDATE OCTAVE
///////////////////////////

void updateOctave() {
  if (digitalRead(octDwn) || digitalRead(octUp)) {    //check input
    delay(50);                                              //a short debounce

    if (digitalRead(octDwn)) {
      delay(150);
      Oct -= 12;
    }
    if (digitalRead(octUp)) {
      delay(150);
      Oct += 12;   
    }
  }
  if (Oct < 0) Oct = 0;         // limit lowest octave to 0 (C-1)
  if (Oct > 120) Oct = 120;     // limit highest octave to 120 (C9)
}

///////////////////////////
// UPDATE INPUT STATES
///////////////////////////

void updateInputStates() {
  inputChanged = false;
  for (int i = 0; i < NUM_INPUTS; i++) {
    inputs[i].prevPressed = inputs[i].pressed; // store previous pressed state (only used for mouse buttons)
    if (inputs[i].pressed) {
      if (inputs[i].bufferSum < releaseThreshold) {
        inputChanged = true;
        inputs[i].pressed = false;
        noteOff((Oct + inputs[i].scale), 100);
      }
    }
    else if (!inputs[i].pressed) {
      if (inputs[i].bufferSum > pressThreshold) {  // input becomes pressed
        inputChanged = true;
        inputs[i].pressed = true;
        noteOn((Oct + inputs[i].scale), 100);
      }
    }
  }
}

///////////////////////////
// POTENTIOMETER ACTION
///////////////////////////

void potAction() {
  reading = analogRead(potPin);                            // read the input pin
  if (abs(previousReading - reading) > analogThreshold) {  // check if the input has changed
    previousReading = reading;                             // store the reading for the next loop
    potValue = map(reading, 0, 1023, 0, 127);              // scale it to use it with MIDI CC
    controlChange(48, potValue);                           // CC49: General Purpose Controller 2 LSB
  }
}

///////////////////////////
// ADD DELAY
///////////////////////////

void addDelay() {

  loopTime = micros() - prevTime;
  if (loopTime < TARGET_LOOP_TIME) {
    int wait = TARGET_LOOP_TIME - loopTime;
    delayMicroseconds(wait);
  }

  prevTime = micros();
}
