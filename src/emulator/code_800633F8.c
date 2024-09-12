#include "emulator/code_800633F8.h"
#include "emulator/controller.h"
#include "emulator/frame.h"
#include "emulator/rom.h"
#include "emulator/system.h"
#include "emulator/vc64_RVL.h"
#include "emulator/xlCoreRVL.h"
#include "emulator/xlFile.h"
#include "emulator/xlHeap.h"
#include "macros.h"
#include "mem_funcs.h"

//! TODO: fix inline oddity
#define NO_INLINE2() \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0;         \
    (void)0

static u8 sBannerBuffer[0x1000];

static STStringBase sStringBase[] = {
    {SID_ERROR_INS_SPACE, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_CHOICE_PRESS_A_TO_RETURN_TO_MENU, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_INS_INNODE, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_SYS_CORRUPT, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_DATA_CORRUPT, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_MAX_BLOCKS, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_MAX_FILES, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_NO_CONTROLLER, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_NEED_CLASSIC, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_REMOTE_BATTERY, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_REMOTE_COMMUNICATION, 0, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_BLANK, 0, NULL, 0x00000000, 0x00000000},
    {SID_NONE, 0, NULL, 0x00000000, 0x00000000},
};

static STStringDraw sStringDraw[SI_MAX] = {
    {
        {&sStringBase[SI_ERROR_INS_SPACE], FLAG_COLOR_WHITE, 0, 0},
        {{
            &sStringBase[SI_ERROR_CHOICE_PRESS_A_TO_RETURN_TO_MENU],
            FLAG_COLOR_WHITE,
            0,
            0,
            (UnknownCallback)fn_80063730,
        }},
        1,
        NULL,
        0,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_INS_INNODE], FLAG_COLOR_WHITE, 0, 0},
        {
            {
                &sStringBase[SI_ERROR_CHOICE_PRESS_A_TO_RETURN_TO_MENU],
                FLAG_COLOR_WHITE,
                0,
                0,
                (UnknownCallback)fn_80063730,
            },
        },
        1,
        NULL,
        0,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_SYS_CORRUPT], FLAG_COLOR_WHITE, 0, 0},
        {0},
        0,
        NULL,
        0,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_DATA_CORRUPT], FLAG_COLOR_WHITE, 0, 0},
        {
            {
                &sStringBase[SI_ERROR_CHOICE_PRESS_A_TO_RETURN_TO_MENU],
                FLAG_COLOR_WHITE,
                0x0000,
                0x00000000,
                (UnknownCallback)fn_80063730,
            },
        },
        1,
        NULL,
        0,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_MAX_BLOCKS], FLAG_COLOR_WHITE, 0, 0},
        {0},
        0,
        NULL,
        0,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_MAX_FILES], FLAG_COLOR_WHITE, 0, 0},
        {0},
        0,
        NULL,
        0,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_SYS_CORRUPT], FLAG_COLOR_WHITE, 0, 0},
        {0},
        0,
        NULL,
        0,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_NO_CONTROLLER], FLAG_UNK, 0, 0},
        {
            {
                &sStringBase[SID_NONE],
                FLAG_COLOR_WHITE,
                0,
                0,
                (UnknownCallback)fn_80063680,
            },
        },
        0,
        (UnknownCallback)fn_80042E30,
        120,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_NO_CONTROLLER], FLAG_COLOR_WHITE, 0, 0},
        {
            {
                &sStringBase[SI_ERROR_NEED_CLASSIC],
                FLAG_UNK | FLAG_COLOR_YELLOW,
                0,
                0,
                (UnknownCallback)fn_80063680,
            },
        },
        1,
        (UnknownCallback)fn_80062028,
        120,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_REMOTE_BATTERY], FLAG_COLOR_WHITE, 0, 0},
        {
            {
                &sStringBase[SID_NONE],
                FLAG_COLOR_WHITE,
                0,
                0,
                (UnknownCallback)fn_80063680,
            },
        },
        1,
        (UnknownCallback)fn_80062028,
        0,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_REMOTE_COMMUNICATION], FLAG_COLOR_WHITE, 0, 0},
        {
            {
                &sStringBase[SID_NONE],
                FLAG_COLOR_WHITE,
                0,
                0,
                (UnknownCallback)fn_80063680,
            },
        },
        1,
        (UnknownCallback)fn_80062028,
        0,
        0,
        0,
        0,
    },
    {
        {&sStringBase[SI_ERROR_BLANK], FLAG_UNK, 0, 0},
        {
            {
                &sStringBase[SID_NONE],
                FLAG_COLOR_WHITE,
                0,
                0,
                (UnknownCallback)fn_80063680,
            },
        },
        0,
        (UnknownCallback)fn_80042E30,
        120,
        0,
        0,
        0,
    },
};

static struct_80174988 lbl_80174988[] = {
    {NAND_RESULT_ACCESS, SI_MAX},
    {NAND_RESULT_ALLOC_FAILED, SI_ERROR_INS_SPACE},
    {NAND_RESULT_BUSY, SI_MAX},
    {NAND_RESULT_CORRUPT, SI_ERROR_INS_INNODE},
    {NAND_RESULT_ECC_CRIT, SI_ERROR_SYS_CORRUPT},
    {NAND_RESULT_EXISTS, SI_MAX},
    {NAND_RESULT_INVALID, SI_ERROR_MAX_FILES},
    {NAND_RESULT_MAXBLOCKS, SI_ERROR_DATA_CORRUPT},
    {NAND_RESULT_MAXFD, SI_ERROR_MAX_BLOCKS},
    {NAND_RESULT_MAXFILES, SI_ERROR_MAX_BLOCKS},
    {NAND_RESULT_NOEXISTS, SI_MAX},
    {NAND_RESULT_NOTEMPTY, SI_MAX},
    {NAND_RESULT_OPENFD, SI_MAX},
    {NAND_RESULT_AUTHENTICATION, SI_ERROR_SYS_CORRUPT},
    {NAND_RESULT_UNKNOWN, SI_ERROR_MAX_FILES},
    {NAND_RESULT_FATAL_ERROR, SI_ERROR_MAX_FILES},
    {NAND_RESULT_OK, SI_MAX},
};

static STFiles sSTFiles[] = {
    {SC_LANG_JP, "Errors_VC64ErrorStrings_jp.bin", "saveComments_saveComments_jp.bin"},
    {SC_LANG_NONE, NULL, NULL},
};

static NANDBanner* sBannner;
static void* sBufferErrorStrings;
static void* sBufferSaveCommentStrings;
static bool lbl_8025D130;
static s32 lbl_8025D12C;
static OSFontHeader* sFontHeader;

GXRenderModeObj* DEMOGetRenderModeObj(void) { return rmode; }

void fn_80063400(void) {
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

s32 fn_80063680(STString* pSTString) { return 2; }

s32 fn_80063688(STString* pSTString, s32 arg1) {
    STStringDraw* pStringDraw = pSTString->apStringDraw[pSTString->eStringIndex];

    if (arg1 & 0x08200000) {
        if (pSTString->iAction > 0) {
            pSTString->iAction--;
        }
    } else if (arg1 & 0x10400000) {
        if (pSTString->iAction < pStringDraw->nAction - 1) {
            pSTString->iAction++;
        }
    } else if ((arg1 & 0x20000000 & ~1) | (arg1 & 1)) {
        if (pStringDraw->nAction > 0 && pStringDraw->textAction[pSTString->iAction].unk0C != NULL) {
            return pStringDraw->textAction[pSTString->iAction].unk0C(pSTString);
        }
    } else if (pStringDraw->unk30 != NULL) {
        return pStringDraw->unk30(pSTString);
    }

    return 0;
}

s32 fn_80063730(STString* pSTString) {
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    OSReturnToMenu();

    return false;
}

void fn_80063764(STStringBase* pStringBase) {
    u32 widthOut;
    s16 nSize;
    s16 nSpace;
    char* var_r27;
    char* var_r31;
    char* var_r3;
    s32 var_r30;
    u16 temp_r29;
    s32 i;
    s32 temp_r27;
    char* temp_r3_2;

    temp_r29 = sFontHeader->cellWidth;
    DEMOGetROMFontSize(&nSize, &nSpace);
    nSize *= 0x10;
    nSpace *= 0x10;
    pStringBase->unk10 = 0;
    pStringBase->unk0C = 0;

    if (pStringBase->eSTStringID != SID_NONE) {
        pStringBase->szString = fn_80064A10(sBufferErrorStrings, pStringBase->eSTStringID);

        if (pStringBase->szString != NULL) {
            var_r27 = pStringBase->szString;
            pStringBase->nLines = 1;

            while (*var_r27 != '\0') {
                temp_r3_2 = OSGetFontWidth(var_r27, &widthOut);

                if (*var_r27 == '\n') {
                    pStringBase->nLines++;
                    *var_r27 = '\0';
                }

                var_r27 = temp_r3_2;
            }

            var_r3 = pStringBase->szString;
            temp_r27 = pStringBase->nLines;

            for (i = 0; i < temp_r27; i++) {
                var_r30 = 0;
                var_r31 = NULL;

                while (*var_r3 != '\0') {
                    if (*var_r3 == ' ' || *var_r3 == '\t') {
                        var_r31 = var_r3;
                    }

                    var_r3 = OSGetFontWidth(var_r3, &widthOut);
                    var_r30 = ((s32)(nSize * widthOut) / (s32)temp_r29) + (var_r30 + nSpace);

                    if (((s32)((s32)(var_r30 + 15) / 16) > 0x230) && (var_r31 != NULL)) {
                        *var_r31 = 0;
                        var_r3 = var_r31 + 1;
                        var_r30 = 0;
                        var_r31 = NULL;
                        pStringBase->nLines++;
                    }
                }

                var_r3++;
            }

            pStringBase->unk10 =
                (s32)((pStringBase->nLines * ((s32)(sFontHeader->leading * nSize) / (s32)temp_r29)) + 15) / 16;
        }
    }
}

void fn_80063910(STStringDraw* pStringDraw) {
    s32 i;

    pStringDraw->unk3C = 0;

    if (pStringDraw->textInfo.pBase != NULL) {
        pStringDraw->textInfo.nShiftY = pStringDraw->textInfo.pBase->unk10;
    } else {
        pStringDraw->textInfo.nShiftY = 0;
    }

    if (pStringDraw->nAction > 0) {
        pStringDraw->textInfo.nShiftY += DEMOGetRFTextHeight("\n");
    }

    pStringDraw->unk3C = pStringDraw->textInfo.nShiftY;

    for (i = 0; i < pStringDraw->nAction; i++) {
        if (pStringDraw->textAction[i].textInfo.pBase != NULL) {
            pStringDraw->textAction[i].textInfo.nShiftY = pStringDraw->textAction[i].textInfo.pBase->unk10;
        } else {
            pStringDraw->textAction[i].textInfo.nShiftY = 0;
        }

        pStringDraw->unk3C += pStringDraw->textAction[i].textInfo.nShiftY;
    }
}

void fn_800639D4(TextInfo* arg0, s32 nHeight, s32 arg2, GXColor color) {
    char* szString;
    s32 i;
    STStringBase* pBase;
    s32 nY;
    s32 nTextHeight;

    pBase = arg0->pBase;

    if (pBase != NULL) {
        nTextHeight = DEMOGetRFTextHeight("");

        nY = nHeight;
        color.a = (arg0->nFadeInTimer * 255) / FADE_TIMER_MAX;

        if (arg0->nFadeInTimer < FADE_TIMER_MAX) {
            arg0->nFadeInTimer++;
        }

        GXSetTevColor(GX_TEVREG0, color);
        szString = pBase->szString;

        for (i = 0; i < pBase->nLines; i++) {
            DEMOPrintf((GC_FRAME_WIDTH - DEMOGetRFTextWidth(szString)) / 2, nY, 1, szString, arg2);

            while (*szString != '\0') {
                szString++;
            }

            szString++;
            nY += nTextHeight;
        }
    }
}

void fn_80063AFC(STString* pSTString) {
    s32 nHeight;
    STStringDraw* pStringDraw;
    GXColor var_r0;
    s32 i;

    GXColor YELLOW = {255, 255, 0, 255};
    GXColor WHITE = {255, 255, 255, 255};

    pStringDraw = pSTString->apStringDraw[pSTString->eStringIndex];

    nHeight = pStringDraw->nStartY;
    if (nHeight == 0) {
        nHeight = (GC_FRAME_HEIGHT - pStringDraw->unk3C) / 2;
    }

    fn_800639D4(&pStringDraw->textInfo, nHeight, pStringDraw->unk38, WHITE);

    nHeight += pStringDraw->textInfo.nShiftY;

    for (i = 0; i < pStringDraw->nAction;) {
        if (i == pSTString->iAction && !(pStringDraw->textAction[i].textInfo.nFlags & FLAG_COLOR_YELLOW)) {
            var_r0 = YELLOW;
        } else {
            var_r0 = WHITE;
        }

        fn_800639D4(&pStringDraw->textAction[i].textInfo, nHeight, 0, var_r0);
        i++;
        nHeight += pStringDraw->textAction[i].textInfo.nShiftY;
    }
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

void fn_80063C7C(void) {
    STStringDraw* pStringDraw;
    STStringBase* pStringBase;
    s32 iStringDraw;
    s32 iStringBase;
    STFiles* var_r29;
    u32 eLanguage;

    eLanguage = SCGetLanguage();
    var_r29 = &sSTFiles[0];

    while (var_r29->szErrors != NULL) {
        if (var_r29->eLanguage == eLanguage) {
            break;
        }
        var_r29++;
    }

    if (var_r29->szErrors == NULL) {
        var_r29 = &sSTFiles[0];
    }

    xlFileLoad(var_r29->szErrors, (void**)&sBufferErrorStrings);
    xlFileLoad(var_r29->szSaveComments, (void**)&sBufferSaveCommentStrings);
    sFontHeader = DEMOInitROMFont();

    pStringBase = &sStringBase[SI_ERROR_INS_SPACE];
    for (iStringBase = 0; iStringBase < ARRAY_COUNT(sStringBase); iStringBase++) {
        fn_80063764(pStringBase);
        pStringBase++;
    }

    pStringDraw = &sStringDraw[0];
    for (iStringDraw = 0; iStringDraw < ARRAY_COUNT(sStringDraw); iStringDraw++) {
        fn_80063910(pStringDraw);
        pStringDraw++;
    }

    fn_80064634(fn_80064A10(sBufferSaveCommentStrings, SID_COMMENT_GAME_NAME),
                fn_80064A10(sBufferSaveCommentStrings, SID_COMMENT_EMPTY));
}

static inline void fn_80063D78_inline(STStringDraw* pStringDraw) {
    s32 i;

    if (pStringDraw->textInfo.nFlags & FLAG_UNK) {
        pStringDraw->textInfo.nFadeInTimer = 0;
    } else {
        pStringDraw->textInfo.nFadeInTimer = FADE_TIMER_MAX;
    }

    for (i = 0; i < pStringDraw->nAction; i++) {
        if (pStringDraw->textAction[i].textInfo.nFlags & FLAG_UNK) {
            pStringDraw->textAction[i].textInfo.nFadeInTimer = 0;
        } else {
            pStringDraw->textAction[i].textInfo.nFadeInTimer = FADE_TIMER_MAX;
        }
    }
}

bool fn_80063D78(STStringIndex eStringIndex) {
    STString sp10;
    s32 var_r31;
    s32 var_r30;
    s32 iController;
    s32 spC;
    s32 sp8;
    s32 temp_r3;

    sp10.eStringIndex = SI_NONE;

    if (!fn_800607B0(SYSTEM_HELP(gpSystem), 0)) {
        return false;
    }

    sp10.eStringIndex++;
    sp10.apStringDraw[sp10.eStringIndex] = &sStringDraw[eStringIndex];
    sp10.iAction = 0;

    fn_80063D78_inline(&sStringDraw[eStringIndex]);
    VISetBlack(false);

    do {
        var_r30 = 0;
        var_r31 = 0;

        for (iController = 0; iController < PAD_MAX_CONTROLLERS; iController++) {
            fn_80062C18(SYSTEM_CONTROLLER(gpSystem), iController, &spC, &sp8, NULL, NULL, NULL, NULL);
            var_r31 |= spC;
            var_r30 |= sp8;
        }

        xlCoreBeforeRender();
        fn_80063400();
        fn_80063AFC(&sp10);
        fn_8005F7E4(SYSTEM_HELP(gpSystem));
        fn_80007020();
        temp_r3 = fn_80063688(&sp10, var_r31 & (var_r31 ^ var_r30));
    } while (temp_r3 == 0);

    if (fn_800607B0(SYSTEM_HELP(gpSystem), 1)) {
        return temp_r3 != 1;
    }

    return false;
}

static inline void fn_80063F30_Inline(NANDResult result) {
    struct_80174988* var_r4;

    if (result == NAND_RESULT_OK) {
        return;
    }

    for (var_r4 = lbl_80174988; var_r4->result != NAND_RESULT_OK; var_r4++) {
        if (var_r4->result == result) {
            if (var_r4->eStringIndex == SI_MAX) {
                return;
            }
            fn_80063D78(var_r4->eStringIndex);
            return;
        }
    }
}

static inline s32 __fn_80063F30(char* szBannerFileName, u32 arg1) {
    NANDFileInfo nandFileInfo;
    u32 length;
    s32 openResult;

    length = 0;
    openResult = NANDSafeOpen(szBannerFileName, &nandFileInfo, 1, sBannerBuffer, ARRAY_COUNT(sBannerBuffer));

    switch (openResult) {
        case NAND_RESULT_AUTHENTICATION:
        case NAND_RESULT_ECC_CRIT:
            return 1;
        case NAND_RESULT_NOEXISTS:
            return 2;
        default:
            fn_80063F30_Inline(openResult);
        case NAND_RESULT_OK:
            fn_80063F30_Inline(NANDGetLength(&nandFileInfo, &length));
            break;
    }

    fn_80063F30_Inline(NANDSafeClose(&nandFileInfo));

    if (length == arg1) {
        return 1;
    } else {
        return 0;
    }
}

s32 fn_80063F30(char* szBannerFileName, u32 arg1) {
    NO_INLINE2();
    return __fn_80063F30(szBannerFileName, arg1);
}

s32 fn_800640BC(char* szFileName, u32 arg1, s32 arg2) {
    NANDFileInfo arg10;
    char arg8[0x20];
    NANDResult result;
    s32 var_r31;

    result = NANDCreate(szFileName, 0x34, 0);
    if (result != NAND_RESULT_OK && result != NAND_RESULT_EXISTS) {
        return 0;
    }

    result = NANDSafeOpen(szFileName, &arg10, 3, sBannerBuffer, ARRAY_COUNT(sBannerBuffer));
    if (result != NAND_RESULT_OK) {
        return 0;
    }

    memset(arg8, arg1, ARRAY_COUNT(arg8));
    DCFlushRange(arg8, ARRAY_COUNT(arg8));

    var_r31 = 0;
    while (var_r31 < (arg1 >> 5)) {
        result = NANDWrite(&arg10, &arg8, 0x20);

        if (result >= NAND_RESULT_OK) {
            var_r31 += 1;
            continue;
        }

        break;
    }

    result = NANDSafeClose(&arg10);
    if (result < NAND_RESULT_OK) {
        return 0;
    }

    return 1;
}

static inline s32 fn_800641CC_Inline(char* szFileName, NANDFileInfo* info, u8 access, void* buffer, s32 len) {
    if (access & 2) {
        return NANDOpen(szFileName, info, access);
    } else {
        return NANDSafeOpen(szFileName, info, access, buffer, len);
    }

    return NAND_RESULT_OK;
}

s32 fn_800641CC(NANDFileInfo* nandFileInfo, char* szFileName, u32 arg2, s32 arg3, u8 access) {
    s32 sp10[16];
    s32 i;
    u32 temp_r25;
    s32 var_r3;
    s32 var_r3_2;
    u32 spC;
    u32 var_r4_4;
    s32 var_r3_3;

    if (!lbl_8025D130) {
        for (i = 0; i < ARRAY_COUNT(sp10); i++) {
            sp10[i] = 0;
        }

        if (NANDGetHomeDir((char*)sp10) != NAND_RESULT_OK) {
            fn_80063D78(SI_ERROR_MAX_FILES);
        }

        if (fn_800B48C4((char*)sp10) != NAND_RESULT_OK) {
            fn_80063D78(SI_ERROR_MAX_FILES);
        }

        lbl_8025D130 = true;
    }

    temp_r25 = ((s32)(arg2 + 0x3FFF) / 16384) << 0xE;

    while (true) {
        lbl_8025D12C = 0;
        fn_8006496C();
        lbl_8025D12C |= var_r3 * 0x10;

        var_r3 = __fn_80063F30(szFileName, temp_r25);

        lbl_8025D12C |= var_r3;

        if (lbl_8025D12C & 0x11) {
            if (!fn_80063D78(SI_ERROR_SYS_CORRUPT)) {
                return 0;
            }

            if (lbl_8025D12C & 0x10) {
                fn_80064930();
            }

            if ((lbl_8025D12C & 1) && NANDDelete(szFileName) != NAND_RESULT_OK) {
                return 0;
            }
        } else if (lbl_8025D12C & 0x22) {
            var_r3_2 = 0;
            var_r4_4 = 0;

            if (lbl_8025D12C & 0x20) {
                var_r4_4 = 1;
                var_r3_2 = (s32)(((s32)(fn_80064960() + 0x3FFF) / 16384) << 0xE) / 16384;
            }

            if (lbl_8025D12C & 2) {
                var_r4_4 += 1;
                var_r3_2 += temp_r25;
            }

            sStringDraw[SI_ERROR_CHOICE_PRESS_A_TO_RETURN_TO_MENU].unk38 = var_r4_4;
            sStringDraw[SI_ERROR_INS_SPACE].unk38 = ((var_r3_2 << 0xE) + 0x1FFFF) / 131072;

            fn_80063F30_UnknownInline(NANDCheck(var_r3_2, var_r4_4, &spC));

            if (spC & 5) {
                if (!fn_80063D78(SI_ERROR_INS_SPACE)) {
                    return 0;
                }
            } else if (spC & 0xA) {
                if (!fn_80063D78(SI_ERROR_CHOICE_PRESS_A_TO_RETURN_TO_MENU)) {
                    return 0;
                }
            } else if ((!(lbl_8025D12C & 0x20) || fn_80064870()) && (lbl_8025D12C & 2)) {
                fn_800640BC(szFileName, temp_r25, arg3);
            }
        } else if (fn_800641CC_Inline(szFileName, nandFileInfo, access, sBannerBuffer, ARRAY_COUNT(sBannerBuffer)) ==
                   NAND_RESULT_OK) {
            return 1;
        }
    }
}

bool fn_80064600(NANDFileInfo* info, s32 arg1) {
    if (arg1 & 2) {
        NANDClose(info);
    } else {
        NANDSafeClose(info);
    }

    return true;
}

static u8 lbl_8025C888[] = {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03};
static u8 lbl_8025C890[] = {0x01, 0x01, 0x01, 0x02, 0x03, 0x04, 0x04, 0x04};

static inline void fn_80064634_inline(char* src, wchar_t* dest, s32 max) {
    s32 i;
    s32 nSize;

    nSize = 0;

    for (i = 0; i < max; i++) {
        if (src[0] == 0x00 && src[1] == (char)0xBB) {
            break;
        }

        // TODO
        ((char*)dest)[1] = *src++;
        ((char*)dest)[0] = *src++;

        dest++;
        nSize++;
    }

    if (nSize == 0) {
        *dest++ = ' ';
    }

    *dest = '\0';
}

bool fn_80064634(char* szGameName, char* szEmpty) {
    wchar_t subtitle[BANNER_TITLE_MAX];
    wchar_t title[BANNER_TITLE_MAX];
    TPLPalette* tplPal;

    u32 i;
    NANDBanner* temp_r26;

    xlHeapTake((void**)&sBannner, 0x10000 | 0x70000000);

    temp_r26 = sBannner;

    if (!xlFileLoad("save_banner.tpl", (void**)&tplPal)) {
        return false;
    }

    TPLBind(tplPal);

    memset(temp_r26, 0, 0xF0A0);
    memset(&title, 0, 0x80);

    if (szGameName != NULL) {
        fn_80064634_inline(szGameName, title, 31);
    }

    if (szEmpty != NULL) {
        fn_80064634_inline(szEmpty, subtitle, 31);
    }

    NANDInitBanner(temp_r26, 0x10, &title[0], &subtitle[0]);
    memcpy(temp_r26->bannerTexture, tplPal->descriptors[0].texHeader->data, ARRAY_COUNT(temp_r26->bannerTexture));

    for (i = 0; i < 8; i++) {
        memcpy(temp_r26->iconTexture[i * 0x240], tplPal->descriptors[lbl_8025C890[i]].texHeader->data,
               ARRAY_COUNT(temp_r26->iconTexture));
        temp_r26->iconSpeed = (temp_r26->iconSpeed & ~(3 << (i * 2))) | (lbl_8025C888[i] << (i * 2));
    }

    if (i < 8) {
        temp_r26->iconSpeed = (temp_r26->iconSpeed & ~(3 << (i * 2)));
    }

    xlHeapFree((void**)&tplPal);
    return 1;
}

static bool fn_80064870(void) {
    NANDFileInfo info;
    void* pBuffer;
    s32 nResult;

    pBuffer = sBannner;

    nResult = NANDCreate("banner.bin", 0x34, 0);
    if (nResult != NAND_RESULT_EXISTS && nResult != NAND_RESULT_OK) {
        return false;
    }

    if (NANDSafeOpen("banner.bin", &info, 3, sBannerBuffer, sizeof(sBannerBuffer))) {
        return false;
    }

    DCFlushRange(pBuffer, 0xF0A0); // fn_80064960()?
    nResult = NANDWrite(&info, pBuffer, 0xF0A0); // fn_80064960()?
    NANDSafeClose(&info);

    return nResult >= NAND_RESULT_OK;
}

static s32 fn_80064930(void) { return !NANDDelete("banner.bin"); }

static s32 fn_80064960(void) { return 0xF0A0; }

static void fn_8006496C(void) {
    fn_80063F30("banner.bin", 0xF0A0); // fn_80064960()?
}

/**
 * @brief Get the string table entry corresponding to the string ID
 * @param pStringTable Pointer to the string table
 * @param eStringID The ID of the string table entry to get
 * @return STHeaderTableEntry pointer of the corresponding string table entry if found, else NULL
 */
STHeaderTableEntry* fn_80064980(StringTable* pStringTable, STStringID eStringID) {
    u32 nStringID;
    s32 lo;
    s32 hi;
    s32 mid;
    u8* pEntries = (u8*)(&pStringTable->header.entries);

    if (pStringTable == NULL || pStringTable->header.nEntries == 0) {
        return NULL;
    }

    hi = pStringTable->header.nEntries - 1;
    lo = 0;
    mid = hi / 2;

    while (hi >= lo) {
        STHeaderTableEntry* entry = (STHeaderTableEntry*)(pEntries + mid * pStringTable->header.nSizeEntry);

        if (entry->nStringID == eStringID) {
            return entry;
        }

        if (entry->nStringID < eStringID) {
            lo = mid + 1;
        } else if (entry->nStringID > eStringID) {
            hi = mid - 1;
        }

        mid = lo + ((hi - lo) / 2);
    }

    return NULL;
}

/**
 * @brief Get the string from the string table corresponding to the string ID
 * @param pSTBuffer Pointer to the string table buffer
 * @param eStringID The ID of the string to get
 * @return char pointer of the corresponding string if found, else NULL
 */
char* fn_80064A10(void* pSTBuffer, STStringID eStringID) {
    STHeaderTableEntry* pTableEntry;

    if (pSTBuffer == NULL) {
        return NULL;
    }

    pTableEntry = fn_80064980(pSTBuffer, eStringID);

    if (pTableEntry != NULL) {
        return (char*)pSTBuffer + pTableEntry->nTextOffset1;
    }

    return NULL;
}
