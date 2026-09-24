#include "3dmath.h"

#include <iostream>

int main()
{
    // The native bootstrap has no I3DMathEngine provider yet. The real
    // accessor must report that absence without returning an indeterminate pointer.
    if (__MATH_ENGINE() != nullptr || __MATH_ENGINE() != nullptr)
    {
        std::cerr << "math accessor returned an unregistered provider\n";
        return 1;
    }
    std::cout << "production math accessor rejects missing provider\n";
    return 0;
}
