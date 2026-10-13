// Characterize the CRT contract used by MGlobals::IsUpdateFrame, OpAgent
// and RandomNumGenerator. This is not a mission simulation or a parity oracle.
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

static std::vector<int> sequence()
{
    std::srand(1);
    std::vector<int> result;
    for (unsigned int i = 0; i < 16; ++i) result.push_back(std::rand());
    return result;
}

int main(int argc, char **argv)
{
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
