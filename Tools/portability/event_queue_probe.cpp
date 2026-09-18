#include "FrontierX/EventQueue.h"

#include <array>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

struct DispatchContext
{
    FrontierX::EventQueue * queue = nullptr;
    std::array<std::uint32_t, 4> messages{};
    std::array<void *, 4> parameters{};
    std::size_t count = 0;
    bool nested_post_succeeded = false;
};

void Dispatch(void * opaque, std::uint32_t message, void * parameter) noexcept
{
    auto & context = *static_cast<DispatchContext *>(opaque);
    if (context.count < context.messages.size())
    {
        context.messages[context.count] = message;
        context.parameters[context.count] = parameter;
        ++context.count;
    }
    if (message == 10)
        context.nested_post_succeeded = context.queue->Post(30, parameter);
}
}

int main()
{
    try
    {
        FrontierX::EventQueue queue;
        std::uint32_t message = 99;
        void * parameter = reinterpret_cast<void *>(1);
        Require(!queue.Peek(0, &message, &parameter), "new queue exposes a delivered event");
        Require(message == 99 && parameter == reinterpret_cast<void *>(1),
            "failed Peek modified outputs");

        int first = 1;
        int second = 2;
        Require(queue.Post(10, &first) && queue.Post(20, &second), "initial Post failed");
        Require(!queue.Peek(0), "pending event became visible before Update");

        DispatchContext context;
        context.queue = &queue;
        queue.Update(Dispatch, &context);
        Require(context.count == 3 && context.messages[0] == 10 && context.messages[1] == 20 &&
            context.messages[2] == 30, "callback Post was not dispatched in FIFO order");
        Require(context.parameters[0] == &first && context.parameters[1] == &second &&
            context.parameters[2] == &first && context.nested_post_succeeded,
            "event parameter identity or nested Post result changed");

        const std::array<std::uint32_t, 3> expected_messages{10, 20, 30};
        const std::array<void *, 3> expected_parameters{&first, &second, &first};
        for (std::size_t index = 0; index < expected_messages.size(); ++index)
        {
            message = 0;
            parameter = nullptr;
            Require(queue.Peek(index, &message, &parameter), "delivered event is missing");
            Require(message == expected_messages[index] && parameter == expected_parameters[index],
                "Peek order or parameter identity changed");
        }
        Require(!queue.Peek(expected_messages.size()), "Peek accepted an out-of-range index");

        int later = 4;
        Require(queue.Post(40, &later), "later Post failed");
        Require(queue.Peek(0, &message, &parameter) && message == 10 && parameter == &first,
            "Post replaced the last delivered batch before Update");
        queue.Update(nullptr);
        Require(queue.Peek(0, &message, &parameter) && message == 40 && parameter == &later,
            "null-dispatch Update did not publish the pending batch");
        Require(!queue.Peek(1), "null-dispatch Update published an extra event");

        queue.Update(nullptr);
        Require(!queue.Peek(0), "empty Update retained the previous delivered batch");

        std::cout << "portable event queue semantics passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "event queue probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
