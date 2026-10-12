#pragma once
#include <DACOM.H>
#include <cstdint>
struct IFontDrawAgent;
struct ITeletype;
struct ISubtitle;
struct GENNODE;
namespace FrontierX
{
// An explicit external face substitutes the Windows resource's family. Pixel
// height still comes from that resource and the original screen scaling.
bool SetNativeFontFile(const char *path);
IFontDrawAgent *CreateNativeFontDrawAgent(U32 resource, std::uint32_t color, bool multiline = false, bool notScaling = false);
IFontDrawAgent *CreateNativeArchetypeFont(GENNODE *archetype, U32 resource, std::uint32_t color, bool multiline, bool notScaling);
std::uint64_t NativeFontDrawCount();
unsigned NativeFontLiveCount();
ITeletype *CreateNativeSourceTeletype();
bool CheckNativeSourceTeletype();
ISubtitle *CreateNativeSourceSubtitle();
bool CheckNativeSourceSubtitle();
std::uint64_t NativeSubtitleDrawCount();
}
