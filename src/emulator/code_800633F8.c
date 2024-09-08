#include "revolution/gx.h"
#include "revolution/vi.h"
#include "revolution/nand.h"

extern GXRenderModeObj* rmode;
s32 fn_80064960(void);

u8 lbl_801C8640[0x1000];
void* lbl_8025D140;

GXRenderModeObj* DEMOGetRenderModeObj() { return rmode; }

s32 fn_80063680(void) {
    return 2;
}

bool fn_80063730(void) {
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    OSReturnToMenu();

    return false;
}

s16* fn_80063C28(s32 arg0, s32 arg1) {
    s16* sp8;

    xlHeapTake(&sp8, arg1 | 0x70000000);
    return sp8;
}

void fn_80063C54(s32 arg0, void* arg1) {
    void* sp8;

    sp8 = arg1;
    xlHeapFree(&sp8);
}

bool fn_80064600(NANDFileInfo *info, s32 arg1) {
    if (arg1 & 2) {
        NANDClose(info);
    } else {
        NANDSafeClose(info);
    }

    return true;
}

bool fn_80064870(void) {
    NANDFileInfo info;
    void* pBuffer;
    s32 nResult;

    pBuffer = lbl_8025D140;

    nResult = NANDCreate("banner.bin", 0x34, 0);
    if (nResult != NAND_RESULT_EXISTS && nResult != NAND_RESULT_OK) {
        return false;
    }

    if (NANDSafeOpen("banner.bin", &info, 3, lbl_801C8640, sizeof(lbl_801C8640))) {
        return false;
    }

    DCFlushRange(pBuffer, fn_80064960());
    nResult = NANDWrite(&info, pBuffer, fn_80064960());
    NANDSafeClose(&info);

    return nResult >= NAND_RESULT_OK;
}

s32 fn_80064930(void) {
    return !NANDDelete("banner.bin");
}

s32 fn_80064960(void) {
    return 0xF0A0;
}

void fn_8006496C(void) {
    fn_80063F30("banner.bin", fn_80064960());
}
