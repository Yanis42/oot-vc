#ifndef _RVL_SDK_GX_FIFO_H
#define _RVL_SDK_GX_FIFO_H

#include "revolution/gx/GXInternal.h"
#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

GX_DECL_PUBLIC_STRUCT(GXFifoObj, 128);

typedef struct __GXFifoObj {
    u8* base;
    u8* top;
    u32 size;
    u32 hiWatermark;
    u32 loWatermark;
    void* rdPtr;
    void* wrPtr;
    s32 count;
    GXBool wrap;
    GXBool bind_cpu;
    GXBool bind_gp;
} __GXFifoObj;

void GXGetGPStatus(u8*, u8*, u8*, u8*, u8*);

void GXSetCPUFifo(GXFifoObj*);
bool GXGetCPUFifo(GXFifoObj*);

u32 GXGetFifoCount(GXFifoObj*);
u8 GXGetFifoWrap(GXFifoObj*);

#ifdef __cplusplus
}
#endif

#endif
