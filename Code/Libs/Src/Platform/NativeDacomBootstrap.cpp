#include "DACOM.H"
#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeHeap.h"
#include "FrontierX/ProfileParserFactory.h"
#include "HeapObj.h"

#include <limits>

namespace
{
struct NativeDacomOwner
{
    NativeDacomOwner()
    {
        FrontierX::RegisterNativeHeap(&manager);
        RegisterProfileParsers(&manager);
    }

    FrontierX::NativeComponentManager manager;
};
}

extern "C" ICOManager * DACOM_CDECL DACOM_Acquire()
{
    static NativeDacomOwner owner;
    return &owner.manager;
}

extern "C" BOOL32 DACOM_CDECL InitializeDAHeap(long heap_size, long grow_size, long flags)
{
    constexpr auto maximum = static_cast<unsigned long>(std::numeric_limits<U32>::max());
    if (heap_size < 0 || grow_size < 0 || flags < 0 ||
        static_cast<unsigned long>(heap_size) > maximum ||
        static_cast<unsigned long>(grow_size) > maximum ||
        static_cast<unsigned long>(flags) > maximum)
        return FALSE;
    return FrontierX::ConfigureNativeHeap(
        DACOM_Acquire(), static_cast<U32>(heap_size), static_cast<U32>(grow_size),
        static_cast<U32>(flags)) ? TRUE : FALSE;
}
