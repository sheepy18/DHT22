#ifndef DHT22_H
#define DHT22_H

#include "Arduino.h";

namespace sensors
{
  class DHT22
  {
    using Pin = uint8_t;
    using Data = int8_t;
    using Error = uint8_t; //0 == success
    
    public:
       DHT22( Pin dataArg ) : data{ dataArg }{}
       Pin getDataPin() { return data; }
       Data* getBuffer() { return buffer; }
       
       Error readSensor(); 
  
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
