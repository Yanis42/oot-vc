#include "emulator/code_80083508.h"
#include "emulator/errordisplay.h"

static inline CNTResult simulatorTestResult(CNTResult result) {
    u8 unused[32] = {0};

    if (result < 0) {
        switch (result) {
            case -5011:
                errorDisplayShow(3);
                break;
            case -5013:
            case -5012:
                errorDisplayShow(17);
                break;
            case -5008:
                errorDisplayShow(18);
                break;
            case -5063:
                errorDisplayShow(19);
                break;
            case -5127:
            case -5000:
            case -5001:
            default:
                break;
        }
    }

    return result;
}

CNTResult __simulatorCNTInitHandleNAND(s32 contentNum, CNTHandleNAND* handle, MEMAllocator* memAlloc) {
    return simulatorTestResult(contentInitHandleNAND(contentNum, handle, memAlloc));
}

CNTResult __simulatorCNTReleaseHandleNAND(CNTHandleNAND* handle) {
    return simulatorTestResult(contentReleaseHandleNAND(handle));
}

CNTResult __simulatorCNTOpenNAND(CNTHandleNAND* handle, const char* path, CNTFileInfoNAND* info) {
    return simulatorTestResult(contentOpenNAND(handle, path, info));
}

CNTResult __fn_80083778(CNTFileInfoNAND* info, s32 arg1, s32 arg2) {
    return simulatorTestResult(fn_80110CD4(info, arg1, arg2));
}

CNTResult __simulatorCNTReadNAND(CNTFileInfoNAND* info, void* dst, u32 len, s32 offset) {
    return simulatorTestResult(contentReadNAND(info, dst, len, offset));
}

CNTResult __simulatorCNTCloseNAND(CNTFileInfoNAND* info) { return simulatorTestResult(contentCloseNAND(info)); }
