#ifndef TSL256x_H
#define TSL256x_H

#include "I2CBase.h"

#include <Arduino.h>
#include <stdint.h>
#include <string>

enum REGISTER
{
	CONTROL					= 0x00,
	TIMING					= 0x01,
	TRHESHHOLD_LOW_LOW		= 0x02,
	TRHESHHOLD_LOW_HIGH		= 0x03,
	TRHESHHOLD_HIGH_LOW		= 0x04,
	TRHESHHOLD_HIGH_HIGH	= 0x05,
	INTERRUPT				= 0x06,
	//RESERVED_7            = 0x07,
	CRC						= 0x08,
	//RESERVED_9            = 0x09,
	ID						= 0x0A,
	//RESERVED_B            = 0x0B,
	DATA_CHANNEL_0_LOW		= 0x0C,
	DATA_CHANNEL_0_HIGH		= 0x0D,
	DATA_CHANNEL_1_LOW		= 0x0E,
	DATA_CHANNEL_1_HIGH		= 0x0F
};

#define BLOCK_BIT   0x10
#define WORD_BIT    0x20
#define CLEAR_BIT   0x40
#define COMMAND_BIT 0x80

#define POWER_DOWN  0x0
#define POWER_UP    0x3

enum INTEGRATION_TIME
{
	MS13	=	0x0,
	MS101	=	0x1,
	MS402	=	0x2,
	MANUAL	=	0x3
};

//Time in ms to wait until an integration cycle finished.
enum INTEGRATION_WAIT
{
	DELAY13MS	=	0x010,// 16
	DELAY101MS	=	0x080,//128
	DELAY402MS	=	0x1C0 //448
};

enum INTEGRATION_MANUAL
{
	STOP	= 0x00,
	START	= 0x08,
};

enum INTEGRATION_GAIN
{
	LOW1X	= 0x00,
	HIGH16X = 0x10
};

enum INTERRUPT_CONTROL
{
	DISABLED	= 0x0,
	LEVEL		= 0x1,
	SMB_ALERT	= 0x2,
	TEST_MODE	= 0x3
};

enum DEVICE_PACKAGE_MASK
{
	TSL2560CS		= 0x00,
	TSL2561CS		= 0x01,
	TSL2560TFNCL	= 0x04,
	TSL2561TFNCL	= 0x05,
	NONE			= 0xFF
};

enum PACKAGE
{
	T_FN_CL = 0x0,
	CS		= 0x1
};

class TSL256x : public I2CBase
{
	private:
		//Device Status:
		bool initialized;
		bool powered;
		std::string errorMsg;

		//Device Information:
		uint8_t partNr;
		uint8_t revNr;
		std::string deviceName;
		std::string packageName;
		uint8_t packageType;

		//Configuration Attributes:
		uint8_t activeIntegrationTime;
		uint8_t activeGain;
		
		void setDefault();

		void waitForCycle();
		bool getChannelData(uint16_t& ch0, uint16_t& ch1);
		uint16_t calculateLux(uint16_t& ch0, uint16_t& ch1);
		float calculateLuxWithFloat(uint16_t& ch0, uint16_t& ch1);

	
	public:
		TSL256x(uint8_t address);
		TSL256x(uint8_t address, int sda, int scl);
		TSL256x(uint8_t address, int sda, int scl, uint32_t clock);
		~TSL256x();
		
		int getPartNumber();
		int getRevisionNumber();
		const char* getDeviceName();
		const char* getPackageName();

		bool isInitialized();
		bool isPowered();
		const char* getErrorMsg();
		
		bool init();

		bool turnOn();
		bool turnOff();

		bool setIntegrationTime(uint8_t integrationTime);
		bool setGain(uint8_t integrationGain);

		bool getLux(uint16_t& lux);
		bool getLux(float& lux);

};
#endif //TSL256x_H