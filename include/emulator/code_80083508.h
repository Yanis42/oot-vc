#ifndef _CODE_80083508_H
#define _CODE_80083508_H

#include "macros.h"
#include "revolution/cnt.h"
#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_OOT || IS_MT

#define simulatorCNTInitHandleNAND contentInitHandleNAND
#define simulatorCNTReleaseHandleNAND contentReleaseHandleNAND
#define simulatorCNTOpenNAND contentOpenNAND
#define simulatorCNTReadNAND contentReadNAND
#define simulatorCNTCloseNAND contentCloseNAND

#elif IS_MM

CNTResult __simulatorCNTInitHandleNAND(s32 contentNum, CNTHandleNAND* handle, MEMAllocator* memAlloc);
CNTResult __simulatorCNTReleaseHandleNAND(CNTHandleNAND* handle);
CNTResult __simulatorCNTOpenNAND(CNTHandleNAND* handle, const char* path, CNTFileInfoNAND* info);
CNTResult __fn_80083778(CNTFileInfoNAND* info, s32 arg1, s32 arg2);
CNTResult __simulatorCNTReadNAND(CNTFileInfoNAND* info, void* dst, u32 len, s32 offset);
CNTResult __simulatorCNTCloseNAND(CNTFileInfoNAND* info);

#define simulatorCNTInitHandleNAND __simulatorCNTInitHandleNAND
#define simulatorCNTReleaseHandleNAND __simulatorCNTReleaseHandleNAND
#define simulatorCNTOpenNAND __simulatorCNTOpenNAND
#define simulatorCNTReadNAND __simulatorCNTReadNAND
#define simulatorCNTCloseNAND __simulatorCNTCloseNAND

#endif

#ifdef __cplusplus
}
#endif

#endif
