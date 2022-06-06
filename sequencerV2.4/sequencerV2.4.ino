// Arduino Tone Generator & 8-Step Sequencer
// William Schroeder - 4/18/2022

#include "TimerOne.h"
#include "Multiplexer.h"
#include "ShiftRegister.h"

// shift registers
ShiftRegister ledSequence = {
  2,  // latchPin
  3,  // clockPin
  4,  // dataPin
};

// multplixers
Multiplexer freqMux = {
  5,  //addr1
  6,  //addr2
  7,  //addr3
  A0, //dataPin
};

// switches & buttons
const int pausePin = 8;
const int stepButtonPin = 9;
// pin 10 is the Tone Generator pin

// vars for step button
int previousState = 0;
int currentState = 0;

// potentiometers
const int TEMPO = A1;
const int MAXSTEPS = A2;

// multiplexer out
int mValues [8];

float frequency = 440.0;  // in hz
float period; // period in microseconds

// chromatic base values; octaves have a 2:1 relationship
#define C 16.35
#define Db 17.32
#define D 18.35
#define Eb 19.45
#define E 20.60
#define F 21.83
#define Gb 23.12
#define G 24.50
#define Ab 25.96
#define A 27.50
#define Bb 29.14
#define B 30.87

float intervals[12] = {C, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B};
float chromatic[107];

float sequence [8];
bool paused = false;
unsigned long pauseTime = 0;
const long pauseFlashInterval = 500; // milliseconds
int flashCount = 0;
int stepIndex;
byte ledData;

void setup()
{
  pinMode(10, OUTPUT);
  Timer1.initialize(500000);         // initialize timer1, and set a 1/2 second period
  //Timer1.pwm(9, 512);                // setup pwm on pin 9, 50% duty cycle
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
  ledSequence.initRegister();
  freqMux.initMox();
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
  ledData = 0b10000000;
  stepIndex = 0;

  // operations performed each step of the sequence
  while (stepIndex < getStepSize())
  {
    // updating values & LEDs
    updateFreqValue(stepIndex);
    ledSequence.writeByte(ledData);
    //updateLEDs(ledData);
    
    paused = digitalRead(pausePin);

    // play a note
    period = (1 / sequence[stepIndex]) * 1000000;
    Timer1.setPeriod(period);

    // paused; able to increment the sequence step while paused.
    while (paused)
    { 
      unsigned long currentMillis = millis();
      // flash the LED of the step that is paused
      if (currentMillis - pauseTime >= pauseFlashInterval)
      {
        pauseTime = currentMillis;
        if (flashCount % 2 == 0) ledSequence.writeByte(0b00000000);
        else ledSequence.writeByte(ledData);
        flashCount++;
      }
      
      stepIndex = updateStepButton(stepIndex);
      // freq values can change while paused
      updateFreqValue(stepIndex);
      // play current freq
      period = (1 / sequence[stepIndex]) * 1000000;
      Timer1.setPeriod(period);

      paused = digitalRead(pausePin); // check if unpaused
    }

    delay(1024 - analogRead(TEMPO));
    stepIndex++;  // increment the sequence
    
    ledData = ledData >> 1; // shift LED sequence
  }
}

// This function updates an array of values from the multiplexer.
// These values are then mapped to a chromatic scale of 8 octaves
// and sent to the sequence array, where frequencies are stored.
void updateFreqValue(int stepIndex)
{
  // select 1 of 8 mox channels to read from
  freqMux.channelSelect(stepIndex);
  // map pot value to chromatic scale and update the sequence
  sequence[stepIndex] =
    chromatic[map(freqMux.readValue(), 0, 1023, 0, 106)];
}

// This function initializes the array from which notes
// can be chosen from; octaves have a 2:1 relationship
void initNotes()
{
  // initialize the chromatic array;
  int octave = 0;
  for (int interval = 0; interval < (12 * 9) - 1; interval++)
  {
    if (interval % 12 == 0 && interval != 0) octave++;
    chromatic[interval] = intervals[interval % 12] * pow(2, octave);
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
      if (stepIndex > getStepSize() -1) 
      {
        stepIndex = 0;
        ledData = 0b10000000;
      } 
      else ledData = ledData >> 1;

      ledSequence.writeByte(ledData);
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
  // when mapping @ 1023 there were some issues with
  // reliably hitting the 8th step due to varying resistances.
  return map(analogRead(MAXSTEPS), 0, 1022, 1, 8);
}

void initKey(float key_interval[], float key[])
{
  for (int i = 0; i < 62; i++)
  {
    int octave = 0;
    if (i % 7 == 0 && i != 0) octave++;
    key[i] = key_interval[i%7]*pow(2, octave);  
  }
}
