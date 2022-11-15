#pragma once
#include <cstdint>

class SspCrc
{
public:

	SspCrc()
	{
		reset();
	}

	void reset()
	{
		crcL = 0xFF;
		crcH = 0xFF;
	}

	void update(const uint8_t b)
	{
		uint8_t Addr = (b ^ crcH);
		crcH = (CRC_TABLE[Addr] >> 8) ^ crcL;
		crcL = (CRC_TABLE[Addr] & 0xFF);
	}

	uint8_t getCrcL() const
	{
		return crcL;
	}

	uint8_t getCrcH() const
	{
		return crcH;
	}

	void update(const uint8_t* data, int len)
	{
		for (;;)
		{
			if (len <= 0)
				break;
			len--;

			update(*data);
			data++;
		}
	}

private:

	static const uint16_t CRC_TABLE[256];
	uint8_t	crcL;
	uint8_t	crcH;
};
