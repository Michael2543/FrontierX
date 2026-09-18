#include "FrontierX/DacomRuntime.h"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

struct LeftBase
{
    int left = 0;
};

struct RightBase
{
    int right = 0;
};

struct Derived : LeftBase, RightBase
{
    int member = 0;
};

struct TokenNode
{
    TokenNode * pNext = nullptr;
    FrontierX::ConnectionToken connectionToken = 0;
};
}

int main()
{
    try
    {
        Derived value;
        const auto rightOffset = FrontierX::BaseSubobjectOffset<RightBase, Derived>();
        const auto actualRightOffset = static_cast<std::size_t>(
            reinterpret_cast<unsigned char *>(static_cast<RightBase *>(&value)) -
            reinterpret_cast<unsigned char *>(&value));
        Require(rightOffset == actualRightOffset,
            "base-subobject offset does not match an actual multiply inherited object");

        const auto memberOffset = static_cast<std::size_t>(
            reinterpret_cast<unsigned char *>(&value.member) -
            reinterpret_cast<unsigned char *>(&value));
        Require(FrontierX::MemberOffset(&Derived::member) == memberOffset,
            "member offset does not match an actual object");
        Require(FrontierX::MemberAtOffset<int>(&value, memberOffset) == &value.member,
            "member lookup changed the target address");
        Require(FrontierX::OwnerFromMember<Derived>(&value.member, memberOffset) == &value,
            "owner recovery changed the target address");

        TokenNode third{nullptr, 3};
        TokenNode first{&third, 1};
        Require(FrontierX::NextConnectionToken(&first) == 2,
            "connection token allocation did not reuse the lowest gap");
        Require(FrontierX::NextConnectionToken<TokenNode>(nullptr) == 1,
            "empty connection list did not start at token one");

        TokenNode second{&third, 2};
        first.pNext = &second;
        Require(FrontierX::NextConnectionToken(&first, 3) == 0,
            "bounded connection token exhaustion was not reported");
        Require(FrontierX::DacomReferenceFlag > 0xFFFFFFFFULL,
            "x86_64 reference flag did not use pointer-width storage");

        std::cout << "pointer-width DACOM runtime helpers passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "DACOM runtime probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
