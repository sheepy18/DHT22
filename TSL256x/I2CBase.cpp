#include "I2CBase.h"

#include <Wire.h>

I2CBase::I2CBase(uint8_t address)
{
	this->address = address;
	errorCode = 0;
	Wire.begin();
}

I2CBase::I2CBase(uint8_t address, int sda, int scl)
{
	this->address = address;
	errorCode = 0;
	Wire.begin(sda, scl);
}

I2CBase::I2CBase(uint8_t address, int sda, int scl, uint32_t clock)
{
	this->address = address;
	errorCode = 0;
	Wire.begin(sda, scl);
	Wire.setClock(clock);
}

I2CBase::~I2CBase() {}

bool I2CBase::readByte(uint8_t cmd, uint8_t& read)
{
	Wire.beginTransmission(address);
	Wire.write(cmd);
	errorCode = Wire.endTransmission();
	if (errorCode == 0)
	{
		Wire.requestFrom(address, (uint8_t)1);
		if (Wire.available() >= 1)
		{
			read = Wire.read();
			return true;
		}
		else
		{
			errorCode = ERRORCODE::EXPECTED_MORE_DATA;
		}
	}
	return false;
}

bool I2CBase::writeByte(uint8_t cmd, uint8_t data)
{
	Wire.beginTransmission(address);
	Wire.write(cmd);
	Wire.write(data);
	errorCode = Wire.endTransmission();
	return errorCode == 0 ? true : false;
}

bool I2CBase::readWord(uint8_t cmd, uint16_t& read)
{
	Wire.beginTransmission(address);
	Wire.write(cmd);
	errorCode = Wire.endTransmission();
	if (errorCode == 0)
	{
		Wire.requestFrom(address, (uint8_t)2);
		if (Wire.available() >= 2)
		{
			read = Wire.read();
			read |= (Wire.read() << 8);
			return true;
		}
		else
		{
			errorCode = ERRORCODE::EXPECTED_MORE_DATA;
		}
	}
	return false;
}

bool I2CBase::writeWord(uint8_t cmd, uint16_t data)
{
	uint8_t high = data >> 8;
	uint8_t low = data & 0xFF;
	Wire.beginTransmission(address);
	Wire.write(cmd);
	Wire.write(low);
	Wire.write(high);
	errorCode = Wire.endTransmission();
	return errorCode == 0 ? true : false;
}


std::string I2CBase::errorCodeToString()
{
	switch (errorCode)
	{
		case SUCCESS: return std::string("successful");
			break;
		case DATA_TO_LONG_FOR_BUFFER: return std::string("data to long for buffer");
			break;
		case RECEIVED_NO_ACK_FOR_ADDR: return std::string("received no acknowledge for address");
			break;
		case RECEIVED_NO_ACK_FOR_DATA: return std::string("received no acknowledge for data");
			break;
		case EXPECTED_MORE_DATA: return std::string("expected more data");
			break;
		default: return std::string("");
	}
}