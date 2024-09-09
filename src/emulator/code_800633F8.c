#include "emulator/frame.h"
#include "emulator/xlCoreRVL.h"
#include "emulator/xlHeap.h"
#include "revolution/demo.h"
#include "revolution/gx.h"
#include "revolution/nand.h"
#include "revolution/vi.h"

// temporary, ipa issues
#define STATIC

static bool fn_80064870(void);
static s32 fn_80064930(void);
static s32 fn_80064960(void);
static void fn_8006496C(void);

s32 fn_80064960(void);

u8 lbl_801C8640[0x1000];
void* lbl_8025D140;

GXRenderModeObj* DEMOGetRenderModeObj(void) { return rmode; }

STATIC void fn_80063400(void) {
    GXColor WHITE = {255, 255, 255, 255};
    GXColor BLACK = {0, 0, 0, 255};

    DEMOSetupScrnSpc(GC_FRAME_WIDTH, GC_FRAME_HEIGHT, 100.0f);

    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR);
    GXSetNumChans(1);
    GXSetNumTevStages(1);
    GXSetNumTexGens(0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_ONE, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_KONST, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColor(GX_TEVREG0, BLACK);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_TEX_ST, GX_RGBA4, 0);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition2s16(0, 0);
    GXPosition2s16(1, GC_FRAME_WIDTH);
    GXPosition2s16(0, 1);
    GXPosition2s16(GC_FRAME_WIDTH, GC_FRAME_HEIGHT);
    GXPosition2s16(1, 0);
    GXPosition2s16(GC_FRAME_HEIGHT, 1);
    GXEnd();

    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColor(GX_TEVREG0, WHITE);
}

s32 fn_80063680(void) { return 2; }

#define ARG0_OBJ (*(s32**)arg0)
#define TEMPR5_OBJ ((s32*)temp_r5)

s32 fn_80063688(void** arg0, s32 arg1) {
    s32 temp_r4;
    s32 temp_r5_2;
    void* temp_r5;
    s32 test;

    temp_r5 = arg0[ARG0_OBJ[0xC] * 4];

    if (arg1 & 0x08200000) {
        temp_r4 = ARG0_OBJ[0xD];

        if (temp_r4 > 0) {
            ARG0_OBJ[0xD] = (s32) (temp_r4 - 1);
        }
    } else if (arg1 & 0x10400000) {
        temp_r5_2 = ARG0_OBJ[0xD];

        if (temp_r5_2 < (s32) (((s32*)temp_r5)[0xB] - 1)) {
            ARG0_OBJ[0xD] = (s32) (temp_r5_2 + 1);
        }
    } else if ((((arg1 & 0x20000000) & ~1) | (arg1 & 1)) != 0) {
        if ((((s32*)temp_r5)[0xB] > 0) && ((s32) (((s32*)temp_r5)[(ARG0_OBJ[0xD] * 0x10) + 0x18] != 0))) {
            goto block_10;
        }
    } else {
block_10:
        if (((s32*)temp_r5)[0xC] != 0) {
            test = 0;
        }
    }

    return test;
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

bool fn_80064600(NANDFileInfo* info, s32 arg1) {
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

static s32 fn_80064930(void) { return !NANDDelete("banner.bin"); }

static s32 fn_80064960(void) { return 0xF0A0; }

static void fn_8006496C(void) { fn_80063F30("banner.bin", fn_80064960()); }
