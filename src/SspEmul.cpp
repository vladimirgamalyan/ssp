#include "SspEmul.h"
#include <iostream>
#include <cassert>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "SspCrc.h"

void SspEmul::execute(const std::string& portName)
{
	spdlog::info("use port: {}", portName);
	
	serial::Timeout timeout(100, 100, 100, 100, 100);

	port.setPort(portName);
	port.setBaudrate(9600);
	port.setParity(serial::parity_none);
	port.setStopbits(serial::stopbits_two);
	port.setTimeout(timeout);
	port.open();
	port.setDTR(true);

	bool s = false;
	uint8_t seqSlaveId = 0;
	std::vector<uint8_t> data;
	for (;;)
	{
		uint8_t b;

		if (!s)
		{
			readByteStxDedup(s, seqSlaveId);
			continue;
		}

		if ((seqSlaveId & 0x7f) != NoteValidator0)
		{
			spdlog::warn("received seqSlaveId: {0:x}", seqSlaveId);
			s = false;
			continue;
		}

		SspCrc sspCrc;
		sspCrc.update(seqSlaveId);

		uint8_t l = readByteStxDedup(s, seqSlaveId);
		if (s)
			continue;

		sspCrc.update(l);
		data.clear();

		while (l--)
		{
			b = readByteStxDedup(s, seqSlaveId);
			if (s)
				break;
			sspCrc.update(b);
			data.push_back(b);
		}

		if (s)
			continue;

		uint8_t crcl = readByteStxDedup(s, seqSlaveId);
		if (s)
			continue;

		uint8_t crch = readByteStxDedup(s, seqSlaveId);
		if (s)
			continue;

		if (sspCrc.getCrcL() != crcl || sspCrc.getCrcH() != crch)
		{
			spdlog::warn("invalid packet crc");
			s = false;
			continue;
		}

		spdlog::info("received packet: {}", spdlog::to_hex(data));
	}
}

uint8_t SspEmul::readByte()
{
	for (;;)
	{
		uint8_t b;
		size_t r = port.read(&b, 1);
		if (r == 1)
			return b;
		assert(r == 0);
	}
}

uint8_t SspEmul::readByteStxDedup(bool& resync, uint8_t& seqSlaveId)
{
	resync = false;

	uint8_t b = readByte();
	if (b == STX)
	{
		b = readByte();
		if (b != STX)
		{
			seqSlaveId = b;
			resync = true;
		}
	}

	return b;
}
