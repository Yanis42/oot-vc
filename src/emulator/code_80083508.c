#include "revolution/types.h"
#include "revolution/cnt.h"
#include "emulator/errordisplay.h"

s32 wrapper_contentInitHandleNAND(s32 contentNum, CNTHandleNAND *handle, MEMAllocator *memAlloc) {
    s32 result = contentInitHandleNAND(contentNum, handle, memAlloc);

    if (result < 0) {
        switch (result) {
            case -5127:
                break;
            case -5011:
                errorDisplayShow(ERROR_SYS_CORRUPT);
                break;
            default:
                errorDisplayShow(ERROR_SYS_CORRUPT_REDOWNLOAD);
                break;
            case -5008:
                errorDisplayShow(ERROR_SYS_MEM_CANT_ACCESS);
                break;
            case -5063:
                errorDisplayShow(ERROR_SYS_MEM_CANT_ACCESS_2);
                break;
        }
    }

    return result;
}
