#include "InProgressAnim.h"
#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

namespace
{
class NativeInProgressAnim final : public IPANIM
{
public:
    NativeInProgressAnim()
    {
        if (CQFLAGS.bInProgressAnimActive)
        {
            std::fputs("Native mission load progress is already active\n", stderr);
            std::abort();
        }
        CQFLAGS.bInProgressAnimActive = static_cast<BOOL32>(-1);
    }

    ~NativeInProgressAnim() { CQFLAGS.bInProgressAnimActive = 0; }

    GENRESULT QueryInterface(const C8 *name, void **interface) override
    {
        if (!interface) return GR_INVALID_PARMS;
        *interface = nullptr;
        if (name && (std::strcmp(name, "IPANIM") == 0 ||
                     std::strcmp(name, "IDAComponent") == 0))
        {
            *interface = static_cast<IPANIM *>(this);
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references_; }
    U32 Release() override
    {
        const U32 remaining = --references_;
        if (!remaining) delete this;
        return remaining;
    }

    void Start() override { running_ = true; }
    void Stop() override { running_ = false; }

    void SetProgress(SINGLE percent) override
    {
        if (!std::isfinite(percent) || percent < 0.0F || percent > 1.0F)
        {
            std::fputs("Native mission load progress is invalid\n", stderr);
            std::abort();
        }
        progress_ = percent;
        if (running_)
            std::fprintf(stderr, "Native mission loading: %u%% (resource %u)\n",
                static_cast<unsigned>(percent * 100.0F), stringId_);
    }

    SINGLE GetProgress() override { return progress_; }

    void UpdateString(U32 resourceId) override
    {
        (void)_localLoadStringW(resourceId);
        stringId_ = resourceId;
        if (running_)
            std::fprintf(stderr, "Native mission loading: %u%% (resource %u)\n",
                static_cast<unsigned>(progress_ * 100.0F), stringId_);
    }

private:
    std::atomic<U32> references_{1};
    SINGLE progress_ = 0.0F;
    U32 stringId_ = 0;
    bool running_ = true;
};
}

void __stdcall CreateInProgressAnim(IPANIM **animation)
{
    if (!animation)
    {
        std::fputs("Native mission load progress output is null\n", stderr);
        std::abort();
    }
    *animation = new (std::nothrow) NativeInProgressAnim;
    if (!*animation)
    {
        std::fputs("Native mission load progress allocation failed\n", stderr);
        std::abort();
    }
}
#endif
