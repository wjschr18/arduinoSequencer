 // Arduino Tone Generator & 8-Step Sequencer
// William Schroeder - 4/11/2022

#include "TimerOne.h"

// shift register pins
int latchPin = 2;
int clockPin = 3;
int ledSequencePin = 4;

// multplixer pins
int mAddr1 = 5;
int mAddr2 = 6;
int mAddr3 = 7;
int mInOut = A0;

// potentiometer
int TEMPO = A1;

// pins 9 & 10 are the Tone Generator/Sequencer pins

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
float chromatic[12 * 9]; 

float sequence [8] = {261,293,392,440, 523, 293, 392, 261};
 
void setup()
{
  pinMode(10, OUTPUT);
  Timer1.initialize(500000);         // initialize timer1, and set a 1/2 second period
  //Timer1.pwm(9, 512);                // setup pwm on pin 9, 50% duty cycle
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(ledSequencePin, OUTPUT);
  pinMode(mAddr1, OUTPUT);
  pinMode(mAddr2, OUTPUT);
  pinMode(mAddr3, OUTPUT);
  pinMode(mInOut, INPUT);
  Serial.begin(9600);

  // initialize the chromatic array;
  int octave = 0;
  for (int interval = 0; interval < (12*9)-1; interval++)
  {
    if (interval % 8 == 0) octave++;
    chromatic[interval] = intervals[interval % 11] * pow(2, octave);
  }
}
 
void callback()
{
  digitalWrite(10, digitalRead(10) ^ 1);
}
 
void loop()
{
  tester();
//  byte ledSequence = 0b10000000;
//  
//  for (float freq : sequence)
//  {
//    // shift register code for controlling the LED sequence
//    
//    //ground latchPin and hold low for as long as you are transmitting
//    digitalWrite(latchPin, LOW);
//    shiftOut(ledSequencePin, clockPin, LSBFIRST, ledSequence);
//    //return the latch pin high to signal chip that it
//    //no longer needs to listen for information
//    digitalWrite(latchPin, HIGH);
//    ledSequence = ledSequence >> 1;
//
//    // update input values from multipliexer
//    updateValues();
//    
//    // play note
//    period = (1/freq)*1000000;
//    Timer1.setPeriod(period);
//    
//    delay(1024 - analogRead(TEMPO));
//  }
}

void updateValues()
{
  // If controlling multpliexers w/ shift registers:
  // use bitWrite(bitRead(i,0)) to construct the shiftOut sequence.
  // insert this method inside the LED sequencer register code to use
  // the same clock and latch pins; (1 update per note in sequence).
  
  for (int i = 0; i < 8; i++)
  {
    int a = bitRead(i, 0);
    int b = bitRead(i, 1);
    int c = bitRead(i, 2);
    digitalWrite(mAddr1, a);
    digitalWrite(mAddr2, b);
    digitalWrite(mAddr3, c);
    mValues[i] = analogRead(mInOut);
    Serial.print("pot ");
    Serial.print(i);
    Serial.print("\t");
    Serial.println(mValues[i]);

    sequence[i] = map(mValues[i], 0, 1023, 16.35, 7902.13);
  }   
}

void tester()
{
  Serial.print("Enter array index: ");
  while(!Serial.available()){}
  Serial.println(chromatic[Serial.parseInt()]);  
}











 
