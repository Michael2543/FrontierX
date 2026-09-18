#include "FrontierX/StackAddress.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}
}

int main()
{
    try
    {
        Require(FrontierX::AreAddressesWithinDistance(100, 109, 10),
            "ascending address distance was rejected");
        Require(FrontierX::AreAddressesWithinDistance(109, 100, 10),
            "descending address distance was rejected");
        Require(!FrontierX::AreAddressesWithinDistance(100, 110, 10),
            "exclusive distance boundary was accepted");
        Require(FrontierX::AreAddressesWithinDistance(0, 0, 1),
            "identical addresses were rejected");

        int stackValue = 0;
        Require(FrontierX::IsAddressNearCurrentStack(&stackValue, 8196),
            "nearby stack object was not recognized");

        std::cout << "pointer-width stack-address checks passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "stack-address probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
