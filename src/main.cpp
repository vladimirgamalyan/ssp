#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT
#include "external/doctest.h"

TEST_CASE("foo test") {
    CHECK(2 * 2 == 4);
}

int main(int argc, char* argv[])
{
	doctest::Context context(argc, argv);
	int res = context.run();
	if (context.shouldExit() || res)
		return res;
}
