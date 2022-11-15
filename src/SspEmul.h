#pragma once
#include <string>
#include <cstdint>
#include <array>
#include <variant>
#include "serial/serial.h"

class SspEmul
{
public:
	void execute(const std::string& portName);

private:
	static const uint8_t STX = 0x7F;
	static const uint8_t NoteValidator0 = 0x00;

	serial::Serial port;
	std::array<uint8_t, 512> buffer;
	uint8_t readByte();
	uint8_t readByteStxDedup(bool& resync, uint8_t& seqSlaveId);
};
