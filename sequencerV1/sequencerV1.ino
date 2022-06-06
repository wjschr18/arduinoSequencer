//int LED1 = 7;
//int LED2 = 8;
//int LED3 = 9;
//int LED4 = 10;
#define TEMPO A0
#define BUZZER 11

// pins connected to 75HC595
int latchPin = 5;
int clockPin = 4;
int ledDataPin = 6;

int potValue = 0;

int steps [8] = {261,293,392,440, 523, 293, 392, 261};
//int leds [4] = {LED1, LED2, LED3, LED4}; 

unsigned long tempo = 1024000;

void setup() {
//  pinMode(LED1, OUTPUT);
//  pinMode(LED2, OUTPUT);
//  pinMode(LED3, OUTPUT);
//  pinMode(LED4, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(ledDataPin, OUTPUT);

  pinMode(TEMPO, INPUT);
  pinMode(BUZZER, INPUT);
  Serial.begin(9600);
}

void loop() {
 
  byte leds = 1;
  
  
  //send LED sequence data
  for (int i = 0; i < 8; i++)
  { 
    // tempo
    potValue = analogRead(TEMPO);
    
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, LOW);
    shiftOut(ledDataPin, clockPin, MSBFIRST, leds);
    
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(latchPin, HIGH);
    // shift the leds
    leds = leds << 1;
    Serial.println(leds);

    noTone(BUZZER);
    tone(BUZZER, steps[i]);
    
    delay(1024-potValue);
  }
  
//  for (int i = 0; i < 4; i++)
//  {
//      int potValue = analogRead(TEMPO);
//      
//      // make sure all LEDS are off
//      for (int j = 0; j < 4; j++)
//      {
//        digitalWrite(leds[j], LOW);
//      }
//      digitalWrite(leds[i], HIGH);
//      noTone(BUZZER);
//      tone(BUZZER, steps[i]);
//      delay(1056 - potValue);
//      Serial.println(potValue);
//  }

}
