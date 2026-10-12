#ifndef FRONTIERX_NATIVE_APP_IMAGE_H
#define FRONTIERX_NATIVE_APP_IMAGE_H
#include <DACOM.H>
#include <TManager.h>
struct IImageReader;
void COMAPI CreateVFXReader(IImageReader **reader);
void COMAPI CreateBMPReader(IImageReader **reader);
void COMAPI CreateTGAReader(IImageReader **reader);
struct IDrawAgent;
void COMAPI CreateDrawAgent(const char *filename, IComponentFactory *parent,
    DA::FILETYPE type, U32 image, IDrawAgent **agent, BOOL32 hiRes = false);
namespace FrontierX { bool CheckNativeVfxReader(); }
#endif
