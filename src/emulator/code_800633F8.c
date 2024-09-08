#include "revolution/gx.h"
#include "revolution/vi.h"
#include "revolution/nand.h"
#include "emulator/xlHeap.h"
#include "emulator/xlCoreRVL.h"

static bool fn_80064870(void);
static s32 fn_80064930(void);
static s32 fn_80064960(void);
static void fn_8006496C(void);

s32 fn_80064960(void);

u8 lbl_801C8640[0x1000];
void* lbl_8025D140;

GXRenderModeObj* DEMOGetRenderModeObj(void) { return rmode; }

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

void* OSAllocFromHeap(s32 handle, s32 size) {
    void* pHeap;

    xlHeapTake(&pHeap, size | 0x70000000);

    return pHeap;
}

void OSFreeToHeap(s32 handle, void* p) {
    void* pHeap = p;

    xlHeapFree(&pHeap);
}

bool fn_80064600(NANDFileInfo *info, s32 arg1) {
    if (arg1 & 2) {
        NANDClose(info);
    } else {
        NANDSafeClose(info);
    }

    return true;
}

static bool fn_80064870(void) {
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

static s32 fn_80064930(void) {
    return !NANDDelete("banner.bin");
}

static s32 fn_80064960(void) {
    return 0xF0A0;
}

static void fn_8006496C(void) {
    fn_80063F30("banner.bin", fn_80064960());
}
