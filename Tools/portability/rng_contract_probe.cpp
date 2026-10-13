// Characterize the CRT contract used by MGlobals::IsUpdateFrame, OpAgent
// and RandomNumGenerator. This is not a mission simulation or a parity oracle.
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <TYPEDEFS.H>
#include "RandomGen.h"

static std::vector<int> sequence()
{
    std::srand(1);
    std::vector<int> result;
    for (unsigned int i = 0; i < 16; ++i) result.push_back(std::rand());
    return result;
}

int main(int argc, char **argv)
{
    // Real field/nugget owner, not a reproduced RNG algorithm. Its original
    // Win32 default domain must agree with the explicit 32767 contract.
    for (U16 seed : {U16(0), U16(1), U16(17), U16(254)})
    {
        RandomGen implicit(seed), explicitRange(seed);
        for (unsigned int index = 0; index < 32; ++index)
            if (implicit.rand() != explicitRange.rand(32767)) return 3;
    }
    const auto first = sequence();
    if (first != sequence()) return 1;
    if (argc == 1) return 0;
    if (argc != 2 || std::string(argv[1]) != "--trace") return 2;
    std::cout << "{\"schema\":1,\"seed\":1,\"rand_max\":" << RAND_MAX
              << ",\"draws\":[";
    for (unsigned int i = 0; i < first.size(); ++i)
    {
        if (i) std::cout << ',';
        std::cout << first[i];
    }
    std::cout << "]}\n";
    return std::cout ? 0 : 1;
}
