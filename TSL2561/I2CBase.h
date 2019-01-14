#ifndef I2CBASE_H
#define I2CBASE_H

#include <stdint.h>
#include <string>

enum ERRORCODE
{
	//Wire lib error codes:
	SUCCESS						= 0x0,
	DATA_TO_LONG_FOR_BUFFER		= 0x1,
	RECEIVED_NO_ACK_FOR_ADDR	= 0x2,
	RECEIVED_NO_ACK_FOR_DATA	= 0x3,
	OTHER_ERROR					= 0x4,

	//Added error codes:
	EXPECTED_MORE_DATA			= 0x10
};

class I2CBase
{
	private:
		uint8_t address;
		uint8_t errorCode;

	public:
		I2CBase(uint8_t address);
		I2CBase(uint8_t address, int sda, int scl);
		I2CBase(uint8_t address, int sda, int scl, uint32_t clock);
		~I2CBase();

		/*Methods*/
		bool readByte(uint8_t cmd, uint8_t& read);
		bool writeByte(uint8_t cmd, uint8_t data);

		bool readWord(uint8_t cmd, uint16_t& read);
		bool writeWord(uint8_t cmd, uint16_t data);

		std::string errorCodeToString();

};
#endif //I2CBASE_H