#ifndef DHT22_H
#define DHT22_H

#include "Arduino.h";

namespace sensors
{
  class Dht22
  {
    using Pin = uint8_t;
    using Data = int8_t;
    
    public:
       Dht22( Pin dataArg ) : data{ dataArg }{}
       Pin getDataPin() { return data; }
       Data* getData() { return buffer; }
       
       uint8_t readSensor(); 
  
   private:
     Pin data;  
     Data buffer[5];

     bool set2Output();
     bool set2Input();
     bool startCommunication();
     bool waitForInitalBits();
     bool readDataBits();
     
  };
}
#endif
