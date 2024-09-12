#include "emulator/errordisplay.h"
#include "emulator/controller.h"
#include "emulator/frame.h"
#include "emulator/rom.h"
#include "emulator/system.h"
#include "emulator/vc64_RVL.h"
#include "emulator/xlCoreRVL.h"
#include "emulator/xlFile.h"
#include "emulator/xlHeap.h"
#include "macros.h"

#include "emulator/banner.h"
#include "revolution/demo.h"

#include "revolution/nand.h"
#include "revolution/os.h"

#include "revolution/vi.h"

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

STStringDraw sStringDraw[] = {
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
                &sStringBase[SI_NULL],
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
                &sStringBase[SI_NULL],
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
                &sStringBase[SI_NULL],
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
                &sStringBase[SI_NULL],
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

struct_80174988 lbl_80174988[] = {
    {NAND_RESULT_ACCESS, SI_NULL},
    {NAND_RESULT_ALLOC_FAILED, SI_ERROR_INS_SPACE},
    {NAND_RESULT_BUSY, SI_NULL},
    {NAND_RESULT_CORRUPT, SI_ERROR_INS_INNODE},
    {NAND_RESULT_ECC_CRIT, SI_ERROR_SYS_CORRUPT},
    {NAND_RESULT_EXISTS, SI_NULL},
    {NAND_RESULT_INVALID, SI_ERROR_MAX_FILES},
    {NAND_RESULT_MAXBLOCKS, SI_ERROR_DATA_CORRUPT},
    {NAND_RESULT_MAXFD, SI_ERROR_MAX_BLOCKS},
    {NAND_RESULT_MAXFILES, SI_ERROR_MAX_BLOCKS},
    {NAND_RESULT_NOEXISTS, SI_NULL},
    {NAND_RESULT_NOTEMPTY, SI_NULL},
    {NAND_RESULT_OPENFD, SI_NULL},
    {NAND_RESULT_AUTHENTICATION, SI_ERROR_SYS_CORRUPT},
    {NAND_RESULT_UNKNOWN, SI_ERROR_MAX_FILES},
    {NAND_RESULT_FATAL_ERROR, SI_ERROR_MAX_FILES},
    {NAND_RESULT_OK, SI_NULL},
};

static STFiles sSTFiles[] = {
    {SC_LANG_JP, "Errors_VC64ErrorStrings_jp.bin", "saveComments_saveComments_jp.bin"},
    {SC_LANG_NONE, NULL, NULL},
};

static void* sBufferErrorStrings;
static void* sBufferSaveCommentStrings;
bool lbl_8025D130;
s32 lbl_8025D12C;
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
