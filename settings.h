#include "Arduino.h"

/*
/////////////////////////////////////////////
// SCALES AND NOISE CANCELLATION SETTINGS  //
/////////////////////////////////////////////
*/

int C_chromatic[NUM_INPUTS] = {
  0, //C
  1, //Db
  2, //D
  3, //Eb
  4, //E
  5, //F
  6, //Gb
  7, //G
  8  //Ab
};

int Dm[NUM_INPUTS] = {
  2,  //D
  4,  //E
  5,  //F
  7,  //G
  9,  //A
  10, //Bb
  12, //C
  14, //D
  16  //E
};

int Gm[NUM_INPUTS] = {
  7,  //G
  9,  //A
  10, //Bb
  12, //C
  14, //D
  15, //Eb
  17, //F
  19, //G
  21  //A
};

int Am[NUM_INPUTS] = {
  9,  //A
  11, //B
  12, //C
  14, //D
  16, //E
  17, //F
  19, //G
  21, //A
  23  //B
};

int Am_pentatonic[NUM_INPUTS] = {
  9,  //A
  12, //C
  14, //D
  16, //E
  19, //G
  21, //A
  24, //C
  26, //D
  28  //E
};

int Bm[NUM_INPUTS] = {
  11,  //B
  13,  //Db
  14,  //D
  16,  //E
  18,  //Gb
  19,  //G
  21,  //A
  23,  //B
  25   //Db
};

///////////////////////////
// NOISE CANCELLATION /////
///////////////////////////
#define SWITCH_THRESHOLD_OFFSET_PERC  5    // number between 1 and 49
// larger value protects better against noise oscillations, but makes it harder to press and release
// recommended values are between 2 and 20
// default value is 5

#define SWITCH_THRESHOLD_CENTER_BIAS 55   // number between 1 and 99
// larger value makes it easier to "release" keys, but harder to "press"
// smaller value makes it easier to "press" keys, but harder to "release"
// recommended values are between 30 and 70
// 50 is "middle" 2.5 volt center
// default value is 55
// 100 = 5V (never use this high)
// 0 = 0 V (never use this low
