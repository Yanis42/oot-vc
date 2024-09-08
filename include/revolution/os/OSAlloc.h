#ifndef _RVL_SDK_OS_ALLOC_H
#define _RVL_SDK_OS_ALLOC_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern volatile s32 __OSCurrHeap;

void* OSAllocFromHeap(s32 handle, s32 size);
void OSFreeToHeap(s32 handle, void* p);
s32 OSSetCurrentHeap(s32 handle);
void* OSInitAlloc(void* start, void* end, s32 numHeaps);
s32 OSCreateHeap(void* start, void* end);

extern void* fn_80063C28(s32 handle, s32 arg1);

#define OSAlloc(size) fn_80063C28(__OSCurrHeap, size)
#define OSFree(ptr) OSFreeToHeap(__OSCurrHeap, ptr)

#ifdef __cplusplus
}
#endif

#endif
