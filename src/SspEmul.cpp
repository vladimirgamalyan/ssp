#include "SspEmul.h"
#include <iostream>
#include <cassert>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"

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

	state = State::sync;
	lastByteWasStx = false;

	for (;;)
	{
		uint8_t b;
		size_t r = port.read(&b, 1);
		assert(r == 0 || r == 1);
		if (r == 1)
			processByte(b);
	}
}

void SspEmul::processByte(uint8_t b)
{
	if (lastByteWasStx)
	{
		lastByteWasStx = false;

		if (b == STX)
			return;
		 
		state = State::sync;

		if ((b & 0x7f) == NoteValidator0)
		{
			sspCrc.reset();
			sspCrc.update(b);
			data.clear();
			state = State::len;
			return;
		}
	}

	lastByteWasStx = (b == STX);

	switch (state)
	{
	case State::sync:
		break;

	case State::len:
		dataLen = b;
		sspCrc.update(b);
		if (dataLen)
			state = State::data;
		else
			state = State::crcL;
		break;

	case State::data:
		sspCrc.update(b);
		data.push_back(b);
		--dataLen;
		if (!dataLen)
			state = State::crcL;
		break;

	case State::crcL:
		if (b != sspCrc.getCrcL())
			state = State::sync;
		else
			state = State::crcH;
		break;

	case State::crcH:
		if (b != sspCrc.getCrcH())
			state = State::sync;
		else
		{
			spdlog::info("received: {}", spdlog::to_hex(data));
			state = State::sync;
		}
		break;

	default:
		throw std::exception();
	}
}
