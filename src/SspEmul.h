#pragma once
#include <string>
#include <cstdint>
#include <array>
#include <variant>
#include "serial/serial.h"
#include "SspCrc.h"

class SspEmul
{
public:
	void execute(const std::string& portName);

private:
	static const uint8_t STX = 0x7f;
	static const uint8_t NoteValidator0 = 0x00;

	serial::Serial port;
	void processByte(uint8_t b);

	enum class State
	{
		sync,
		len,
		data,
		crcL,
		crcH
	};

	State state;
	uint8_t dataLen;
	SspCrc sspCrc;
	bool lastByteWasStx;
	std::vector<uint8_t> data;
};
