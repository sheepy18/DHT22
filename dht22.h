#ifndef DHT22_H
#define DHT22_H

#include "Arduino.h";

namespace sensors
{
  class DHT22
  {
    using Value = float;
    using Pin = uint8_t;
    using Data = uint8_t;
    using Error = uint8_t; 
     /*
     * Errorcodes:
     * 0 == success
     * 1 == error on set2Output
     * 2 == error on startCommunication
     * 3 == error on set2Input
     * 4 == error on waitForInitalBits
     * 5 == error on readDataBits
     * 6 == chesum not valid
     */
    
    public:
       struct MeasureValues
       {
          DHT22::Value temperature;
          DHT22::Value humidity;    
       };
    
       DHT22( Pin dataArg ) : data{ dataArg }{}
       Pin getDataPin() { return data; }
       Data* getBuffer() { return buff; }             
       Error readSensor(); 
       Value getTemperature();
       Value getHumidity();
       MeasureValues getTempAndHumi();
       
  
   private:
     Pin data;  
     Data buff[5];

     bool set2Output();
     bool set2Input();
     bool startCommunication();
     bool waitForInitalBits();
     bool readDataBits();
     void resetBuffer();
     bool isCheckSumValid();
     
  };
}
#endif
