#include "FrontierX/EncodedHandle.h"

#include <array>
#include <cstdlib>
#include <iostream>
#include <limits>
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
        const std::array<FrontierX::EncodedHandleValue, 5> values{
            0,
            1,
            3,
            0x40000000U,
            std::numeric_limits<FrontierX::EncodedHandleValue>::max(),
        };
        for (const auto value : values)
        {
            const void * handle = FrontierX::EncodeHandleValue(value);
            Require(FrontierX::DecodeEncodedHandle(handle) == value,
                "encoded handle did not preserve its pointer-width value");
        }

        std::size_t index = 99;
        Require(FrontierX::TryDecodeHandleIndex(FrontierX::EncodeHandleValue(3), 4, index),
            "valid encoded handle index was rejected");
        Require(index == 3, "encoded handle index changed during decoding");

        index = 99;
        Require(!FrontierX::TryDecodeHandleIndex(FrontierX::EncodeHandleValue(4), 4, index),
            "out-of-range encoded handle index was accepted");
        Require(index == 99, "failed index decoding modified the output");

        Require(!FrontierX::TryDecodeHandleIndex(
            FrontierX::EncodeHandleValue(
                std::numeric_limits<FrontierX::EncodedHandleValue>::max()),
            4, index), "invalid-handle sentinel was accepted as an index");

        index = 99;
        Require(FrontierX::TryDecodeOffsetHandleIndex(
            FrontierX::EncodeHandleValue(7), 4, 4, index),
            "valid offset encoded handle index was rejected");
        Require(index == 3, "offset encoded handle index changed during decoding");

        index = 99;
        Require(!FrontierX::TryDecodeOffsetHandleIndex(
            FrontierX::EncodeHandleValue(3), 4, 4, index),
            "encoded handle below the offset base was accepted");
        Require(!FrontierX::TryDecodeOffsetHandleIndex(
            FrontierX::EncodeHandleValue(8), 4, 4, index),
            "out-of-range offset encoded handle index was accepted");
        Require(index == 99, "failed offset index decoding modified the output");

        std::cout << "pointer-width encoded handle semantics passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "encoded handle probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
