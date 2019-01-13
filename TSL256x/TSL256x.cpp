#include "TSL256x_Def.h"
#include "TSL256x.h"
#include <math.h>

TSL256x::TSL256x(uint8_t address) 
	: I2CBase(address)
{
	setDefault();
}

TSL256x::TSL256x(uint8_t address, int sda, int scl) 
	: I2CBase(address, sda, scl)
{
	setDefault();
}

TSL256x::TSL256x(uint8_t address, int sda, int scl, uint32_t clock) 
	: I2CBase(address, sda, scl, clock)
{
	setDefault();
}

TSL256x::~TSL256x() {}

bool TSL256x::init()
{
	uint8_t id;
	if (!readByte(COMMAND_BIT | REGISTER::ID, id))
	{
		errorMsg = "Could not read Device ID: ";
		return false;
	}

	partNr = id >> 4;//Shift 4x right
	revNr = id & 0xF;//Erase 4x upper bits

	switch (partNr)
	{
		case DEVICE_PACKAGE_MASK::TSL2560CS:
			deviceName	= "TSL2560";
			packageName = "CS";
			packageType = 1;
			break;
		case DEVICE_PACKAGE_MASK::TSL2561CS:
			deviceName	= "TSL2561";
			packageName = "CS";
			packageType = 1;
			break;
		case DEVICE_PACKAGE_MASK::TSL2560TFNCL:
			deviceName	= "TSL2560";
			packageName = "T/FN/CL";
			packageType = 0;
			break;
		case DEVICE_PACKAGE_MASK::TSL2561TFNCL:
			deviceName	= "TSL2561";
			packageName = "T/FN/CL";
			packageType = 0;
			break;
		case DEVICE_PACKAGE_MASK::NONE:
		default:
			deviceName	= "No device!";
			packageName = "NONE";
			packageType = -1;
			return false;
	}

	initialized = true;
	return true;
}

bool TSL256x::turnOn()
{
	if (writeByte(COMMAND_BIT | REGISTER::CONTROL, POWER_UP))
	{
		powered = true;
		return true;
	}
	else
	{
		errorMsg = "Error sending power on command: " + errorCodeToString();
		return false;
	}
}

bool TSL256x::turnOff()
{
	if (writeByte(COMMAND_BIT | REGISTER::CONTROL, POWER_DOWN))
	{
		powered = false;
		return true;
	}
	else
	{
		errorMsg = "Error sending power off command: " + errorCodeToString();
		return false;
	}
}

bool TSL256x::setIntegrationTime(uint8_t integrationTime)
{
	if (!initialized || !powered)
	{
		errorMsg = "Device is not initialized or powered";
		return false;
	}
		
	if (writeByte(COMMAND_BIT || REGISTER::TIMING, integrationTime | activeGain))
	{
		activeIntegrationTime = integrationTime;
		return true;
	}
	else
	{
		errorMsg = "Error sending integration Time command: " + errorCodeToString();
		return false;
	}
}

bool TSL256x::setGain(uint8_t integrationGain)
{
	if (!initialized || !powered)
	{
		errorMsg = "Device is not initialized or powered";
		return false;
	}

	if (writeByte(COMMAND_BIT || REGISTER::TIMING, activeIntegrationTime | integrationGain))
	{
		activeGain = integrationGain;
		return true;
	}
	else
	{
		errorMsg = "Error sending integration gain command: " + errorCodeToString();
		return false;
	}
}

int TSL256x::getPartNumber()
{
	return (int)partNr;
}

int TSL256x::getRevisionNumber()
{
	return (int)revNr;
}

const char* TSL256x::getDeviceName()
{
	return deviceName.c_str();
}

const char* TSL256x::getPackageName()
{
	return packageName.c_str();
}

const char* TSL256x::getErrorMsg()
{
	return errorMsg.c_str();
}

bool TSL256x::isInitialized()
{
	return initialized;
}

bool TSL256x::isPowered()
{
	return powered;
}

bool TSL256x::getLux(uint16_t& lux)
{
	if (initialized && powered)
	{
		uint16_t ch0, ch1;
		waitForCycle();
		getChannelData(ch0, ch1);
		lux = calculateLux(ch0, ch1);
		return true;
	}
	else
	{
		errorMsg = "Device is not initialized or powered";
		return false;
	}
}

bool TSL256x::getLux(float& lux)
{
	if (initialized && powered)
	{
		uint16_t ch0, ch1;
		waitForCycle();
		getChannelData(ch0, ch1);
		lux = calculateLuxWithFloat(ch0, ch1);
		return true;
	}
	else
	{
		errorMsg = "Device is not initialized or powered";
		return false;
	}
}

/* Private Methods */

void TSL256x::setDefault()
{
	initialized = false;
	powered = false;
	activeIntegrationTime = INTEGRATION_TIME::MS402;
	activeGain = INTEGRATION_GAIN::LOW1X;
}

void TSL256x::waitForCycle()
{
	uint32_t wait = 0;
	switch (activeIntegrationTime)
	{
		case INTEGRATION_TIME::MS13: wait = (uint32_t)INTEGRATION_WAIT::DELAY13MS;
			break;
		case INTEGRATION_TIME::MS101: wait = (uint32_t)INTEGRATION_WAIT::DELAY101MS;
			break;
		default: wait = (uint32_t)INTEGRATION_WAIT::DELAY402MS;
			break;
	}
	/*Arduino delay function*/
	delay(wait);
}

bool TSL256x::getChannelData(uint16_t& ch0, uint16_t& ch1)
{
	if (!readWord(COMMAND_BIT | WORD_BIT | (uint8_t)REGISTER::DATA_CHANNEL_0_LOW, ch0)) 
	{
		errorMsg = "Error reading data Channel 0: " + errorCodeToString();
		return false;
	}
	if (!readWord(COMMAND_BIT | WORD_BIT | (uint8_t)REGISTER::DATA_CHANNEL_1_LOW, ch1)) 
	{
		errorMsg = "Error reading data Channel 1: " + errorCodeToString();
		return false;
	}
	return true;
}

uint16_t TSL256x::calculateLux(uint16_t& ch0, uint16_t& ch1)
{
	unsigned long chScale = 0;
	unsigned long channel1 = 0;
	unsigned long channel0 = 0;

	switch (activeIntegrationTime)
	{
		case INTEGRATION_TIME::MS13: chScale = CHSCALE_TINT0;
			break;
		case INTEGRATION_TIME::MS101: chScale = CHSCALE_TINT1;
			break;
		default: chScale = (1 << CH_SCALE);
			break;
	}

	//Set Scale if Gain is not 16x
	if (!activeGain)//Should be false for 0x0 == 1X Gain
		chScale <<= 4;

	//Scale the channel values
	channel0 = (ch0 * chScale) >> CH_SCALE;
	channel1 = (ch1 * chScale) >> CH_SCALE;

	// find the ratio of the channel values (Channel1/Channel0)
	// protect against divide by zero
	unsigned long ratio1 = 0;
	if (channel0 != 0)
		ratio1 = (channel1 << (RATIO_SCALE + 1)) / channel0;

	// round the ratio value
	unsigned long ratio = (ratio1 + 1) >> 1;

	unsigned int b, m;

	if (packageType == PACKAGE::CS)
	{
		if ((ratio >= 0) && (ratio <= K1C))
		{
			b = B1C; m = M1C;
		}
		else if (ratio <= K2C)
		{
			b = B2C; m = M2C;
		}
		else if (ratio <= K3C)
		{
			b = B3C; m = M3C;
		}
		else if (ratio <= K4C)
		{
			b = B4C; m = M4C;
		}
		else if (ratio <= K5C)
		{
			b = B5C; m = M5C;
		}
		else if (ratio <= K6C)
		{
			b = B6C; m = M6C;
		}
		else if (ratio <= K7C)
		{
			b = B7C; m = M7C;
		}
		else //(ratio > K8C)
		{
			b = B8C; m = M8C;
		}
	}
	else //packageType == PACKAGE::T_FN_CL
	{
		if ((ratio >= 0) && (ratio <= K1T))
		{
			b = B1T; m = M1T;
		}
		else if (ratio <= K2T)
		{
			b = B2T; m = M2T;
		}
		else if (ratio <= K3T)
		{
			b = B3T; m = M3T;
		}
		else if (ratio <= K4T)
		{
			b = B4T; m = M4T;
		}
		else if (ratio <= K5T)
		{
			b = B5T; m = M5T;
		}
		else if (ratio <= K6T)
		{
			b = B6T; m = M6T;
		}
		else if (ratio <= K7T)
		{
			b = B7T; m = M7T;
		}
		else //(ratio > K8T)
		{
			b = B8T; m = M8T;
		}
	}

	unsigned long temp = ((channel0 * b) - (channel1 * m));

	// do not allow negative lux value
	if (temp < 0)
		temp = 0;

	// round lsb (2^(LUX_SCALE-1))
	temp += (1 << (LUX_SCALE - 1));

	// strip off fractional portion
	unsigned long value = temp >> LUX_SCALE;

	return (uint16_t)value;
}

float TSL256x::calculateLuxWithFloat(uint16_t& ch0, uint16_t& ch1)
{
	float lux = 0.0f;
	float ratio = (float)ch1 / (float)ch0;

	if (packageType == PACKAGE::CS)
	{
		if (0.0f < ratio && ratio <= 0.52f)
		{
			lux = 0.0315f * (float)ch0 - 0.0593f * (float)ch0 * powf(ratio, 1.4f);
		}
		else if (0.52f < ratio && ratio <= 0.65f)
		{
			lux = 0.0229f * (float)ch0 - 0.0291f * (float)ch1;
		}
		else if (0.65f < ratio && ratio <= 0.8f) 
		{
			lux = 0.0157f * (float)ch0 - 0.018f * (float)ch1;
		}
		else if (0.8f < ratio && ratio <= 1.3f) 
		{
			lux = 0.00338f * (float)ch0 - 0.0026f * (float)ch1;
		}
		else // > 1.3
		{
			lux = 0;
		}
	}
	else //packageType == PACKAGE::T_FN_CL
	{
		if (0.0f < ratio && ratio <= 0.50f)
		{
			lux = 0.0304 * (float)ch0 - 0.062 * (float)ch0 * powf(ratio, 1.4f);
		}
		else if (0.50f < ratio && ratio <= 0.61f)
		{
			lux = 0.0224 * (float)ch0 - 0.031 * (float)ch1;
		}
		else if (0.61f < ratio && ratio <= 0.8f)
		{
			lux = 0.0128 * (float)ch0 - 0.0153 * (float)ch1;
		}
		else if (0.8f < ratio && ratio <= 1.3f)
		{
			lux = 0.00146 * (float)ch0 - 0.00112 * (float)ch1;
		}
		else // > 1.3
		{
			lux = 0;
		}
	}
	return lux;
}

