#ifndef _RVL_SDK_OS_ADDRESS_H
#define _RVL_SDK_OS_ADDRESS_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void* OSPhysicalToCached(u32 ofs) { return (void*)(ofs + 0x80000000); }

static inline void* OSPhysicalToUncached(u32 ofs) { return (void*)(ofs + 0xC0000000); }

static inline void* OSCachedToPhysical(const void* ofs) { return (u8*)ofs - 0x80000000; }

#define OSUncachedToPhysical(ucaddr) ((u32)((u8*)(ucaddr) - OS_BASE_UNCACHED))

#ifdef __cplusplus
}
#endif

#endif
