#include <iostream>
#include <string>

#define DOCTEST_CONFIG_IMPLEMENT
#include "external/doctest.h"

#include "external/CLI11.hpp"

TEST_CASE("foo test") 
{
    CHECK(2 * 2 == 4);
}

int main(int argc, char* argv[])
{
	doctest::Context context(argc, argv);
	int res = context.run();
	if (context.shouldExit() || res)
		return res;

	CLI::App app{ "SSP" };
	std::string portName;
	app.add_option("-p,--port", portName, "Serial port name")->required();

	try
	{
		app.parse(argc, argv);
	}
	catch (const CLI::ParseError& e)
	{
		return app.exit(e);
	}

	std::cout << portName << '\n';
}
