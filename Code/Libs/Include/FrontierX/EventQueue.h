#ifndef FRONTIERX_EVENT_QUEUE_H
#define FRONTIERX_EVENT_QUEUE_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace FrontierX
{
struct EventRecord
{
    std::uint32_t message = 0;
    void * parameter = nullptr;
};

class EventQueue
{
public:
    using Dispatch = void (*)(void * context, std::uint32_t message, void * parameter) noexcept;

    bool Post(std::uint32_t message, void * parameter = nullptr) noexcept;
    void Update(Dispatch dispatch, void * context = nullptr) noexcept;
    bool Peek(std::size_t index, std::uint32_t * message = nullptr,
              void ** parameter = nullptr) const noexcept;

private:
    std::vector<EventRecord> pending_;
    std::vector<EventRecord> delivered_;
};
}

#endif
