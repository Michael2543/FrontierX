#include "FrontierX/ParticleContext.h"

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
        int value = 0;
        const auto context = FrontierX::ParticleContextFromPointer(&value);
        Require(context != 0, "non-null pointer encoded as the null context");
        Require(FrontierX::PointerFromParticleContext<int>(context) == &value,
            "particle context did not preserve pointer identity");
        Require(FrontierX::PointerFromParticleContext<int>(0) == nullptr,
            "zero particle context did not decode as null");
        Require(sizeof(FrontierX::ParticleContext) == sizeof(void *),
            "particle context is not pointer-width");

        std::cout << "pointer-width particle context passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "particle-context probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
