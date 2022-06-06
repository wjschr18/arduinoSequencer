struct Multiplexer
{
  int addr1, addr2, addr3, dataPin;

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
};
