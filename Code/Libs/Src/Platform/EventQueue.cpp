#include "FrontierX/EventQueue.h"

#include <new>

namespace FrontierX
{
bool EventQueue::Post(std::uint32_t message, void * parameter) noexcept
{
    try
    {
        pending_.push_back(EventRecord{message, parameter});
        return true;
    }
    catch (const std::bad_alloc &)
    {
        return false;
    }
}

void EventQueue::Update(Dispatch dispatch, void * context) noexcept
{
    std::size_t index = 0;
    while (index < pending_.size())
    {
        const EventRecord event = pending_[index++];
        if (dispatch) dispatch(context, event.message, event.parameter);
    }

    delivered_.clear();
    delivered_.swap(pending_);
}

bool EventQueue::Peek(std::size_t index, std::uint32_t * message, void ** parameter) const noexcept
{
    if (index >= delivered_.size()) return false;
    if (message) *message = delivered_[index].message;
    if (parameter) *parameter = delivered_[index].parameter;
    return true;
}
}
