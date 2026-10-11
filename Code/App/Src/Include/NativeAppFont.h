#pragma once
#include <DACOM.H>
#include <cstdint>
struct IFontDrawAgent;
struct ITeletype;
namespace FrontierX
{
// An explicit external face substitutes the Windows resource's family. Pixel
// height still comes from that resource and the original screen scaling.
bool SetNativeFontFile(const char *path);
IFontDrawAgent *CreateNativeFontDrawAgent(U32 resource, std::uint32_t color);
std::uint64_t NativeFontDrawCount();
unsigned NativeFontLiveCount();
ITeletype *CreateNativeSourceTeletype();
bool CheckNativeSourceTeletype();
}
