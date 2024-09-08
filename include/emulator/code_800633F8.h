#ifndef _CODE_800633F8_H
#define _CODE_800633F8_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct GXRenderModeObj* DEMOGetRenderModeObj(void);
s32 fn_80063680(void);
bool fn_80063730(void);
void* OSAllocFromHeap(s32 handle, s32 size);
void OSFreeToHeap(s32 handle, void* p);
bool fn_80064600(struct NANDFileInfo *info, s32 arg1);

#ifdef __cplusplus
}
#endif

#endif
