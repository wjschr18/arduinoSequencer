struct ShiftRegister
{
  int latchPin, clockPin, dataPin;

  // This function writes a byte sequence to the shift register.
  void writeByte(byte dataByte)
  {
    // ground latchPin and hold LOW for duration of transmission.
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
    // return the latch pin high to signal chip that it
    // no longer needs to listen for information
    digitalWrite(latchPin, HIGH);  
  }

  void initRegister()
  {
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);  
  }
};
