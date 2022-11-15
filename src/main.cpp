#include <iostream>
#include <string>
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "CLI11.hpp"
#include "SspEmul.h"
#include "spdlog/spdlog.h"

int main(int argc, char* argv[])
{
	doctest::Context context(argc, argv);
	int res = context.run();
	if (context.shouldExit() || res)
		return res;

	spdlog::set_pattern("[%8o] [%L] %v");

	CLI::App app{ "SSP" };
	std::string portName;
	app.add_option("-p,--port", portName, "Serial port name")->required();

	try
	{
		app.parse(argc, argv);
		SspEmul sspEmul;
		sspEmul.execute(portName);
	}
	catch (const CLI::ParseError& e)
	{
		return app.exit(e);
	}
	catch (const std::exception& e)
	{
		spdlog::error("{}", e.what());
	}

	return 0;
}
