struct Multiplexer
{
  int addr1, addr2, addr3, dataPin;

// If controlling multpliexers w/ shift registers:
// use bitWrite(bitRead(i,0)) to construct the shiftOut sequence.
// insert this method inside the LED sequencer register code to use
// the same clock and latch pins; (1 update per note in sequence).
  void channelSelect(int addr)
  {
    int a  = bitRead(addr, 0);
    int b  = bitRead(addr, 1);
    int c  = bitRead(addr, 2);
    digitalWrite(addr1, a);
    digitalWrite(addr2, b);
    digitalWrite(addr3, c);
  }
  
  int readValue()
  {
    return analogRead(dataPin);  
  }
  
  void writeValue(int value)
  {
    if (value > 255) value = 255;
    if (value < 0) value = 0;
    analogWrite(dataPin, value);  
  }

  // to use multiplexer for output data, use writeMode = true.
  void initMox(bool writeMode = false)
  {
    pinMode(addr1, OUTPUT);
    pinMode(addr2, OUTPUT);
    pinMode(addr3, OUTPUT); 

    if (writeMode) pinMode(dataPin, OUTPUT);
    else pinMode(dataPin, INPUT);
  }
};
