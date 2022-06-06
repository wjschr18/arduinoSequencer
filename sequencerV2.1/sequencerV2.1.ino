 // Arduino Tone Generator & 8-Step Sequencer
// William Schroeder - 4/8/2022

#include "TimerOne.h"
#include "Multiplexer.h"

// shift register pins
int latchPin = 2;
int clockPin = 3;
int ledSequencePin = 4;

// multplixers
Multiplexer mox1 = {
  5,  //addr1
  6,  //addr2
  7,  //addr3
  A0, //dataPin
};
//int mAddr1 = 5;
//int mAddr2 = 6;
//int mAddr3 = 7;
//int mInOut = A0;

// switches & buttons
int pausePin = 8;
int stepButtonPin = 9;
// pin 10 is the Tone Generator pin

// vars for step button
int previousState = 0;
int currentState = 0;

// potentiometers
int TEMPO = A1;
int MAXSTEPS = A2;

// multiplexer out
int mValues [8];

float frequency = 440.0;  // in hz
float period; // period in microseconds

// chromatic base values; octaves have a 2:1 relationship
float C = 16.35;
float Db = 17.32;
float D = 18.35;
float Eb = 19.45;
float E = 20.60;
float F = 21.83;
float Gb = 23.12;
float G = 24.50;
float Ab = 25.96;
float A = 27.50;
float Bb = 29.14;
float B = 30.87;

float intervals[12] = {C, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B};
float chromatic[107]; 

float sequence [8];
bool paused = false;
int stepIndex;
byte ledSequence;
 
void setup()
{
  pinMode(10, OUTPUT);
  Timer1.initialize(500000);         // initialize timer1, and set a 1/2 second period
  //Timer1.pwm(9, 512);                // setup pwm on pin 9, 50% duty cycle
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(ledSequencePin, OUTPUT);
  pinMode(mox1.addr1, OUTPUT);
  pinMode(mox1.addr2, OUTPUT);
  pinMode(mox1.addr3, OUTPUT);
  pinMode(mox1.dataPin, INPUT);
  pinMode(pausePin, INPUT);
  pinMode(stepButtonPin, INPUT);
  pinMode(TEMPO, INPUT);
  pinMode(MAXSTEPS, INPUT);
  //Serial.begin(9600);
  initNotes();
}
 
void callback()
{
  digitalWrite(10, digitalRead(10) ^ 1);
}
 
void loop()
{
  // reset LED sequence & step count
  ledSequence = 0b10000000;
  stepIndex = 0;
  
  // operations performed each step of the sequence
  while (stepIndex < getStepSize())
  {
    paused = digitalRead(pausePin); 

    // updating peripherials
    updateFreqValue(stepIndex);
    updateLEDs(ledSequence);
    ledSequence = ledSequence >> 1;

    // play a note
    period = (1/sequence[stepIndex])*1000000;
    Timer1.setPeriod(period);

    // paused; able to increment the sequence step while paused.
    while (paused)
    { 
      stepIndex = updateStepButton(stepIndex);
      // change freq values while paused
      updateFreqValue(stepIndex);
      // play note
      period = (1/sequence[stepIndex])*1000000;
      Timer1.setPeriod(period);

      paused = digitalRead(pausePin); // check if unpaused
    }
    
    delay(1024 - analogRead(TEMPO));
    stepIndex++;  // increment the sequence
  }
}

// This function updates an array of values from the multiplexer.
// These values are then mapped to a chromatic scale of 8 octaves
// and sent to the sequence array, where frequencies are stored.
void updateFreqValue(int stepIndex)
{
  // If controlling multpliexers w/ shift registers:
  // use bitWrite(bitRead(i,0)) to construct the shiftOut sequence.
  // insert this method inside the LED sequencer register code to use
  // the same clock and latch pins; (1 update per note in sequence).
  mox1.channelSelect(stepIndex);
  
//    int a = bitRead(stepIndex, 0);
//    int b = bitRead(stepIndex, 1);
//    int c = bitRead(stepIndex, 2);
//    digitalWrite(mAddr1, a);
//    digitalWrite(mAddr2, b);
//    digitalWrite(mAddr3, c);
//    mValues[stepIndex] = analogRead(mInOut);
  sequence[stepIndex] = chromatic[map(mox1.readValue(), 0, 1023, 0, 106)];
  
  // map pot value to chromatic scale and update the sequence
//    sequence[stepIndex] = chromatic[map(mValues[stepIndex], 0, 1023, 0, 106)];
}   

// This function controls the LED sequence by shifting a
// byte with only one flipped bit to the shift register.
void updateLEDs(byte ledSequence)
{
// ground latchPin and hold LOW for duration of transmission.
    digitalWrite(latchPin, LOW);
    shiftOut(ledSequencePin, clockPin, LSBFIRST, ledSequence);
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(latchPin, HIGH);  
}

// This function initiates the array from which notes
// can be chosen from; octaves have a 2:1 relationship
void initNotes()
{
  // initialize the chromatic array;
  int octave = 0;
  for (int interval = 0; interval < (12*9)-1; interval++)
  {
    if (interval % 11 == 0 && interval != 0) octave++;
    chromatic[interval] = intervals[interval % 11] * pow(2, octave);
  }
}

// This function detects a button press and updates
// the stepIndex of the sequence accordingly.
int updateStepButton(int stepIndex)
{
  currentState = digitalRead(stepButtonPin);

  // state change = button press
  if (currentState != previousState)
  {
    // detect rising edges; set to LOW for falling edges
    if (currentState == HIGH)
    {
      stepIndex++;
      if (stepIndex >= 8) stepIndex = 0;
      updateLEDs(ledSequence);
      ledSequence = ledSequence >> 1;
      if (ledSequence == 0) ledSequence = 0b10000000;
    }
    // avoid bouncing
    delay(50);
  }
  // for the next loop iteration
  previousState = currentState; 
  return stepIndex; 
}

// This function returns the value read from the step size pot.
// valid steps are 1-8
int getStepSize()
{
  return map(analogRead(MAXSTEPS), 0, 1023, 1, 8);
}
 
