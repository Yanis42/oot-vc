#include "emulator/code_800633F8.h"
#include "emulator/controller.h"
#include "emulator/frame.h"
#include "emulator/rom.h"
#include "emulator/system.h"
#include "emulator/vc64_RVL.h"
#include "emulator/xlCoreRVL.h"
#include "emulator/xlFile.h"
#include "emulator/xlHeap.h"
#include "mem_funcs.h"

static bool fn_80064870(void);
static s32 fn_80064930(void);
static s32 fn_80064960(void);
static void fn_8006496C(void);
char* fn_80064A10(char*, STStringID);
bool fn_80064600(NANDFileInfo* info, s32 arg1);

s32 fn_80064960(void);
bool fn_80063730(void);
s32 fn_80063680(void);

u8 lbl_801C8640[0x1000];
NANDBanner* lbl_8025D140;

typedef struct ST10_Unknown1 {
    /* 0x00 */ SCLanguage nLanguage; // u8?
    /* 0x04 */ char* szErrors;
    /* 0x08 */ char* szSaveComments;
} ST10_Unknown1; // size = 0xC

typedef bool (*UnknownCallback)(void);

typedef struct UnknownData1_Sub {
    /* 0x00 */ STString* pStringEntry;
    /* 0x04 */ s16 unk04;
    /* 0x04 */ s16 unk06;
    /* 0x08 */ s32 unk08;
} UnknownData1_Sub; // size = 0xC

typedef struct Pos {
    /* 0x00 */ s16 y;
    /* 0x02 */ s16 x;
} Pos; // size = 0x4

typedef struct UnknownData2 {
    /* 0x00 */ UnknownData1_Sub textInfo;
    /* 0x0C */ UnknownData1_Sub textAction;
    /* 0x18 */ UnknownCallback unk18;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
    /* 0x2C */ s32 unk2C;
    /* 0x30 */ UnknownCallback unk30;
    /* 0x34 */ Pos unk34;
    /* 0x38 */ s32 unk38;
    /* 0x3C */ s32 unk3C;
} UnknownData2; // size = 0x40

STString lbl_80174580[] = {
    {SID_ERROR_INS_SPACE, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_CHOICE_PRESS_A_TO_RETURN_TO_MENU, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_INS_INNODE, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_SYS_CORRUPT, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_DATA_CORRUPT, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_MAX_BLOCKS, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_MAX_FILES, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_NO_CONTROLLER, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_NEED_CLASSIC, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_REMOTE_BATTERY, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_REMOTE_COMMUNICATION, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_ERROR_BLANK, 0x00000000, NULL, 0x00000000, 0x00000000},
    {SID_NONE, 0x00000000, NULL, 0x00000000, 0x00000000},
};

UnknownData2 lbl_80174688[] = {
    {
        {&lbl_80174580[0], 0x0000, 0x0000, 0x00000000},
        {&lbl_80174580[1], 0x0000, 0x0000, 0x00000000},
        (UnknownCallback)fn_80063730,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000001,
        NULL,
        {0, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[2], 0x0000, 0x0000, 0x00000000},
        {&lbl_80174580[1], 0x0000, 0x0000, 0x00000000},
        (UnknownCallback)fn_80063730,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000001,
        NULL,
        {0, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[3], 0x0000, 0x0000, 0x00000000},
        {NULL, 0x00000000, 0x00000000},
        NULL,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        NULL,
        {0, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[4], 0x0000, 0x0000, 0x00000000},
        {&lbl_80174580[1], 0x0000, 0x0000, 0x00000000},
        (UnknownCallback)fn_80063730,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000001,
        NULL,
        {0, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[5], 0x0000, 0x0000, 0x00000000},
        {NULL, 0x00000000, 0x00000000},
        NULL,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        NULL,
        {0, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[6], 0x0000, 0x0000, 0x00000000},
        {NULL, 0x00000000, 0x00000000},
        NULL,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        NULL,
        {0, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[3], 0x0000, 0x0000, 0x00000000},
        {NULL, 0x00000000, 0x00000000},
        NULL,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        NULL,
        {0, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[7], 0x0002, 0x0000, 0x00000000},
        {&lbl_80174580[12], 0x0000, 0x0000, 0x00000000},
        (UnknownCallback)fn_80063680,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        (UnknownCallback)fn_80042E30,
        {120, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[7], 0x0000, 0x0000, 0x00000000},
        {&lbl_80174580[8], 0x0003, 0x0000, 0x00000000},
        (UnknownCallback)fn_80063680,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000001,
        (UnknownCallback)fn_80062028,
        {120, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[9], 0x0000, 0x0000, 0x00000000},
        {&lbl_80174580[12], 0x0000, 0x0000, 0x00000000},
        (UnknownCallback)fn_80063680,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000001,
        (UnknownCallback)fn_80062028,
        {0, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[10], 0x0000, 0x0000, 0x00000000},
        {&lbl_80174580[12], 0x0000, 0x0000, 0x00000000},
        (UnknownCallback)fn_80063680,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000001,
        (UnknownCallback)fn_80062028,
        {0, 0},
        0x00000000,
        0x00000000,
    },
    {
        {&lbl_80174580[11], 0x0002, 0x0000, 0x00000000},
        {&lbl_80174580[12], 0x0000, 0x0000, 0x00000000},
        (UnknownCallback)fn_80063680,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        (UnknownCallback)fn_80042E30,
        {120, 0},
        0x00000000,
        0x00000000,
    },
};

NANDResult lbl_80174988[] = {
    NAND_RESULT_ACCESS,
    12,
    NAND_RESULT_ALLOC_FAILED,
    NAND_RESULT_OK,
    NAND_RESULT_BUSY,
    12,
    NAND_RESULT_CORRUPT,
    2,
    NAND_RESULT_ECC_CRIT,
    3,
    NAND_RESULT_EXISTS,
    12,
    NAND_RESULT_INVALID,
    6,
    NAND_RESULT_MAXBLOCKS,
    4,
    NAND_RESULT_MAXFD,
    5,
    NAND_RESULT_MAXFILES,
    5,
    NAND_RESULT_NOEXISTS,
    12,
    NAND_RESULT_NOTEMPTY,
    12,
    NAND_RESULT_OPENFD,
    12,
    NAND_RESULT_AUTHENTICATION,
    3,
    NAND_RESULT_UNKNOWN,
    6,
    -128,
    6,
    NAND_RESULT_OK,
    12,
};

ST10_Unknown1 lbl_80174A58[] = {
    {SC_LANG_JP, "Errors_VC64ErrorStrings_jp.bin", "saveComments_saveComments_jp.bin"},
    {SC_LANG_NONE, NULL, NULL},
};

u8 lbl_8025C888[] = {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03};

u8 lbl_8025C890[] = {0x01, 0x01, 0x01, 0x02, 0x03, 0x04, 0x04, 0x04};

s32 lbl_8025D12C;
s32 lbl_8025D130;

char* bufferErrors;
char* bufferSaveComments;
OSFontHeader* fontHeader;

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
            ARG0_OBJ[0xD] = (s32)(temp_r4 - 1);
        }
    } else if (arg1 & 0x10400000) {
        temp_r5_2 = ARG0_OBJ[0xD];

        if (temp_r5_2 < (s32)(((s32*)temp_r5)[0xB] - 1)) {
            ARG0_OBJ[0xD] = (s32)(temp_r5_2 + 1);
        }
    } else if ((((arg1 & 0x20000000) & ~1) | (arg1 & 1)) != 0) {
        if ((((s32*)temp_r5)[0xB] > 0) && ((s32)(((s32*)temp_r5)[(ARG0_OBJ[0xD] * 0x10) + 0x18] != 0))) {
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

void fn_80063764(STString* arg0) {
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

    temp_r29 = fontHeader->cellWidth;
    DEMOGetROMFontSize(&nSize, &nSpace);
    nSize *= 0x10;
    nSpace *= 0x10;
    arg0->unk10 = 0;
    arg0->unk0C = 0;

    if (arg0->eSTStringID != SID_NONE) {
        arg0->szString = fn_80064A10(bufferErrors, arg0->eSTStringID);

        if (arg0->szString != NULL) {
            var_r27 = arg0->szString;
            arg0->unk04 = 1;

            while (*var_r27 != '\0') {
                temp_r3_2 = OSGetFontWidth(var_r27, &widthOut);

                if (*var_r27 == '\n') {
                    arg0->unk04++;
                    *var_r27 = '\0';
                }

                var_r27 = temp_r3_2;
            }

            var_r3 = arg0->szString;
            temp_r27 = arg0->unk04;

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
                        arg0->unk04++;
                    }
                }

                var_r3++;
            }

            arg0->unk10 = (s32)((arg0->unk04 * ((s32)(fontHeader->leading * nSize) / (s32)temp_r29)) + 15) / 16;
        }
    }
}

void fn_80063910(UnknownData2* arg0) {
    STString* temp_r3;
    UnknownData2* var_r5;
    s32 var_r6;

    arg0->unk3C = 0;

    if (arg0->textInfo.pStringEntry != NULL) {
        arg0->textInfo.unk08 = arg0->textInfo.pStringEntry->unk10;
    } else {
        arg0->textInfo.unk08 = 0;
    }

    if (arg0->unk2C > 0) {
        arg0->textInfo.unk08 += DEMOGetRFTextHeight("\n");
    }

    var_r5 = arg0;
    arg0->unk3C = arg0->textInfo.unk08;

    for (var_r6 = 0; var_r6 < arg0->unk2C; var_r6++) {
        if (var_r5->textAction.pStringEntry != NULL) {
            var_r5->textAction.unk08 = var_r5->textAction.pStringEntry->unk10;
        } else {
            var_r5->textAction.unk08 = 0;
        }

        arg0->unk3C += var_r5->textAction.unk08;
        var_r5++;
    }
}

void fn_800639D4(UnknownData1_Sub* arg0, s16 nHeight, s32 arg2, GXColor color) {
    char* szString;
    s32 i;
    STString* pStringEntry;
    s32 nY;
    s32 nTextHeight;

    pStringEntry = arg0->pStringEntry;

    if (pStringEntry != NULL) {
        nTextHeight = DEMOGetRFTextHeight("");

        nY = nHeight;
        color.a = (arg0->unk06 * 255) / 15;

        if (arg0->unk06 < 0xF) {
            arg0->unk06++;
        }

        GXSetTevColor(GX_TEVREG0, color);
        szString = pStringEntry->szString;

        for (i = 0; i < pStringEntry->unk04; i++) {
            DEMOPrintf((GC_FRAME_WIDTH - DEMOGetRFTextWidth(szString)) / 2, nY, 1, szString, arg2);

            while (*szString != '\0') {
                szString++;
            }

            szString++;
            nY += nTextHeight;
        }
    }
}
void fn_80063AFC(UnknownData2* arg0) {
    UnknownData1_Sub* var_r27;
    UnknownData2* temp_r30;
    UnknownData2* var_r28;
    s32 var_r31;
    s16 var_r31_2;
    s32 temp_r0;
    GXColor var_r0;
    s32 var_r26;

    GXColor YELLOW = {255, 255, 0, 255};
    GXColor WHITE = {255, 255, 255, 255};

    temp_r30 = (arg0 + arg0->unk2C);
    var_r31 = temp_r30->unk34.y;

    if (var_r31 == 0) {
        var_r31 = (GC_FRAME_HEIGHT - temp_r30->unk3C) / 2;
    }

    fn_800639D4(&temp_r30->textInfo, var_r31, temp_r30->unk38, WHITE);

    var_r28 = temp_r30;
    var_r27 = &temp_r30->textAction;
    var_r31_2 = var_r31 + temp_r30->textInfo.unk08;

    for (var_r26 = 0; var_r26 < temp_r30->unk2C; var_r26++) {
        if (var_r26 == arg0->unk34.y && !(var_r28->textAction.unk04 & 1)) {
            var_r0 = YELLOW;
        } else {
            var_r0 = WHITE;
        }

        fn_800639D4(var_r27, var_r31_2, 0, var_r0);
        var_r27++;
        var_r28++;
        var_r31_2 += var_r28->unk24;
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
    UnknownData2* pUnknownData2;
    STString* pSTEntry; // r31
    s32 i; // r29
    ST10_Unknown1* var_r29;
    u32 temp_r4;

    temp_r4 = SCGetLanguage();
    var_r29 = &lbl_80174A58[0];

    while (var_r29->szErrors != NULL) {
        if (var_r29->nLanguage == temp_r4) {
            break;
        }
        var_r29++;
    }

    if (var_r29->szErrors == NULL) {
        var_r29 = &lbl_80174A58[0];
    }

    xlFileLoad(var_r29->szErrors, (void**)&bufferErrors);
    xlFileLoad(var_r29->szSaveComments, (void**)&bufferSaveComments);
    fontHeader = DEMOInitROMFont();

    pSTEntry = &lbl_80174580[0];
    for (i = 0; i < ARRAY_COUNT(lbl_80174580); i++) {
        fn_80063764(pSTEntry);
        pSTEntry++;
    }

    pUnknownData2 = &lbl_80174688[0];
    for (i = 0; i < ARRAY_COUNT(lbl_80174688); i++) {
        fn_80063910(pUnknownData2);
        pUnknownData2++;
    }

    fn_80064634(fn_80064A10(bufferSaveComments, SID_COMMENT_GAME_NAME),
                fn_80064A10(bufferSaveComments, SID_COMMENT_EMPTY));
}

void fn_80063D78(s32 arg0) {
    s32 sp44;
    s32 sp40;
    s32 spC;
    s32 sp8;
    s32 temp_r5;
    s32 temp_r6;
    s32 var_r29;
    s32 var_r30;
    s32 var_r31;
    s32 var_r5;
    UnknownData2* var_r7;
    s32* sp10;

    sp40 = -1;

    if (!fn_800607B0(SYSTEM_HELP(gpSystem), 0)) {
        return;
    }

    temp_r5 = sp40 + 1;
    var_r7 = &lbl_80174688[arg0];
    sp40 = temp_r5;
    sp10[temp_r5] = var_r7->textInfo.pStringEntry->eSTStringID;
    sp44 = 0;

    if (var_r7->textInfo.unk04 & 2) {
        var_r7->textInfo.unk08 = 0;
    } else {
        var_r7->textInfo.unk08 = 0xF;
    }

    for (var_r5 = 0; var_r5 < lbl_80174688[arg0].unk2C; var_r5++) {
        if (var_r7->textAction.unk04 & 2) {
            var_r7->textAction.unk08 = 0;
        } else {
            var_r7->textAction.unk08 = 0xF;
        }

        var_r7 += 1;
    }

    VISetBlack(false);

    do {
        var_r30 = 0;
        var_r31 = 0;

        for (var_r29 = 0; var_r29 < 4; var_r29++) {
            fn_80062C18(SYSTEM_CONTROLLER(gpSystem), var_r29, &spC, &sp8, NULL, NULL, NULL, NULL);
            var_r31 |= spC;
            var_r30 |= sp8;
        }

        xlCoreBeforeRender();
        fn_80063400();
        fn_80063AFC((UnknownData2*)&sp10[0]);
        fn_8005F7E4(SYSTEM_HELP(gpSystem));
        fn_80007020();
    } while (!fn_80063688((void**)&sp10, var_r31 & (var_r31 ^ var_r30)));

    if (fn_800607B0(SYSTEM_HELP(gpSystem), 1)) {
        return;
    }
}

static inline void fn_80063F30_Inline(NANDResult result) {
    if (result == NAND_RESULT_OK) {
        NANDResult* var_r4 = &lbl_80174988[0];

        while (var_r4 != NULL) {
            if (var_r4[0] != result) {
                if (var_r4[1] == 12) {
                    fn_80063D78(var_r4[1]);
                }
            } else {
                var_r4 += 2;
                continue;
            }

            break;
        }
    }
}

s32 fn_80063F30(char* arg0, u32 arg1) {
    NANDFileInfo nandFileInfo;
    u32 length;
    s32 openResult;

    length = 0;
    openResult = NANDSafeOpen(arg0, &nandFileInfo, 1, lbl_801C8640, ARRAY_COUNT(lbl_801C8640));

    switch (openResult) {
        case NAND_RESULT_AUTHENTICATION:
        case NAND_RESULT_ECC_CRIT:
            return 1;
        case NAND_RESULT_NOEXISTS:
            return 2;
        default:
            fn_80063F30_Inline(openResult);
            break;
        case NAND_RESULT_OK:
            fn_80063F30_Inline(NANDGetLength(&nandFileInfo, &length));
            fn_80063F30_Inline(NANDSafeClose(&nandFileInfo));

            if (length == arg1) {
                break;
            }

            return 1;
    }

    return 0;
}

s32 fn_800640BC(const char szFileName[9], u32 arg1, s32 arg2) {
    NANDFileInfo arg10;
    char arg8[0x20];

    NANDResult var_r29;
    s32 var_r31;

    // *(sp + (-0x100 - ((s32) sp & 0x1F))) = sp;
    // sp->unk4 = (s32) saved_reg_lr;
    // sp->unk-4 = (s32) saved_reg_r31;
    // sp->unk-8 = (s32) saved_reg_r30;
    // sp->unk-C = (s32) saved_reg_r29;

    var_r29 = NANDCreate(szFileName, 0x34, 0);
    if (var_r29 != NAND_RESULT_OK && var_r29 != NAND_RESULT_EXISTS) {
        return 0;
    }

    var_r29 = NANDSafeOpen(szFileName, &arg10, 3, lbl_801C8640, ARRAY_COUNT(lbl_801C8640));
    if (var_r29 != NAND_RESULT_OK) {
        return 0;
    }

    memset(arg8, arg2, ARRAY_COUNT(arg8));
    DCFlushRange(arg8, ARRAY_COUNT(arg8));

    var_r31 = 0;
    while (var_r31 < (arg1 >> 5)) {
        var_r29 = NANDWrite(&arg10, &arg8, 0x20);

        if (var_r29 >= NAND_RESULT_OK) {
            var_r31 += 1;
            continue;
        }

        break;
    }

    var_r29 = NANDSafeClose(&arg10);
    if (var_r29 < NAND_RESULT_OK) {
        return 0;
    }

    return 1;
}

static inline bool fn_800641CC_Inline(char* szFileName, NANDFileInfo* info, u8 access, void* buffer, s32 len) {
    if (access & 2) {
        return NANDOpen(szFileName, info, access);
    } else {
        return NANDSafeOpen(szFileName, info, access, buffer, len);
    }

    return true;
}

s32 fn_800641CC(NANDFileInfo* nandFileInfo, char* szFileName, u32 arg2, s32 arg3, s32 arg4) {
    NANDFileInfo sp50;
    u32 spC;
    u32 sp8;
    s32 temp_r3;
    s32 temp_r3_2;
    s32 temp_r3_3;
    s32 temp_r3_4;
    s32 temp_r3_5;
    s32 temp_r3_6;
    s32 temp_r3_7;
    s32 temp_r3_8;
    s32 temp_r3_9;
    s32 var_r3;
    s32 var_r3_2;

    s32* var_r4;
    s32* var_r4_2;
    s32* var_r4_3;
    s32* var_r4_5;
    u32 temp_r25;
    u32 var_r4_4;

    s32 sp10[0x10];
    s32 i;

    if (lbl_8025D130 == 0) {
        // MEMCLR(sp10);

        for (i = 0; i < ARRAY_COUNT(sp10); i++) {
            sp10[i] = 0;
        }

        if (NANDGetHomeDir((char*)sp10) != 0) {
            fn_80063D78(6);
        }

        if (fn_800B48C4((char*)sp10) != 0) {
            fn_80063D78(6);
        }

        lbl_8025D130 = 1;
    }

    temp_r25 = ((s32)(arg2 + 0x3FFF) / 16384) << 0xE;

    while (true) {
        lbl_8025D12C = 0;
        fn_8006496C();
        sp8 = 0;
        // lbl_8025D12C |= M2C_ERROR(/* Read from unset register $r3 */) * 0x10;

        fn_80063F30(szFileName, temp_r25);

        temp_r3_7 = lbl_8025D12C | var_r3;
        lbl_8025D12C = temp_r3_7;
        if (temp_r3_7 & 0x11) {
            fn_80063D78(3);
            // if ((s32) M2C_ERROR(/* Read from unset register $r3 */) == 0) {
            //     return 0;
            // }
            if (lbl_8025D12C & 0x10) {
                fn_80064930();
            }
            if ((lbl_8025D12C & 1) && (NANDDelete(szFileName) != 0)) {}
            continue;
        }

        if (temp_r3_7 & 0x22) {
            var_r3_2 = 0;
            var_r4_4 = 0;

            if (temp_r3_7 & 0x20) {
                var_r4_4 = 1;
                var_r3_2 = (s32)(((s32)(fn_80064960() + 0x3FFF) / 16384) << 0xE) / 16384;
            }

            if (lbl_8025D12C & 2) {
                var_r4_4 += 1;
                var_r3_2 += (s32)temp_r25 / 16384;
            }

            // lbl_80174688->unk78 = var_r4_4;
            // lbl_80174688->unk38 = (s32) ((var_r3_2 << 0xE) + 0x1FFFF) / 131072;

            fn_80063F30_Inline(NANDCheck(var_r3_2, var_r4_4, &spC));

            if (spC & 5) {
                fn_80063D78(0);
                // if ((s32) M2C_ERROR(/* Read from unset register $r3 */) == 0) {
                //     return 0;
                // }
                continue;
            }

            if (spC & 0xA) {
                fn_80063D78(1);
                // if ((s32) M2C_ERROR(/* Read from unset register $r3 */) == 0) {
                //     return 0;
                // }
                continue;
            }

            if ((!(lbl_8025D12C & 0x20) || fn_80064870()) && (lbl_8025D12C & 2)) {
                fn_800640BC(szFileName, temp_r25, arg3);
            }
            continue;
        }

        if (fn_800641CC_Inline(szFileName, nandFileInfo, arg4, lbl_801C8640, ARRAY_COUNT(lbl_801C8640)) == 0) {
            return 1;
        }
        continue;
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

bool fn_80064634(char* arg0, char* arg1) {
    wchar_t subtitle[BANNER_TITLE_MAX];
    wchar_t title[BANNER_TITLE_MAX];
    TPLPalette* tplPal;
    s32 temp_cr0_lt;
    s32 temp_r0_3;
    s32 temp_r3;
    s32 var_ctr;
    s32 var_ctr_2;
    s32 var_r29;
    s32 var_r4;
    s32 var_r4_2;
    wchar_t* var_r3;
    wchar_t* var_r3_2;
    u32 var_r27_2;
    char temp_r0;
    u8 temp_r0_2;
    u8* var_r28;
    u8* var_r31;
    NANDBanner* temp_r26;
    char* var_r25;
    char* var_r27;
    u8* var_r30;

    var_r25 = arg0;
    var_r27 = arg1;

    xlHeapTake((void**)&lbl_8025D140, 0x10000 | 0x70000000);

    temp_r26 = lbl_8025D140;

    if (!xlFileLoad("save_banner.tpl", (void**)&tplPal)) {
        return false;
    }

    TPLBind(tplPal);
    memset(temp_r26, 0, 0xF0A0);
    memset(&title, 0, 0x80);

    if (var_r25 != NULL) {
        var_r3 = &title[0];
        var_r4 = 0;

        for (var_ctr = 0; var_ctr < 31; var_ctr++) {
            if (var_r25[0] != 0x00 || var_r25[1] != -0x45) {
                temp_r0 = var_r25[1];
                var_r3[0] = temp_r0;
                var_r3[1] = var_r25[0];
                var_r4 += 1;
                var_r25 += 2;
                var_r3 += 2;
            }
        }

        if (var_r4 == 0) {
            *var_r3 = ' ';
            var_r3 += 2;
        }

        *var_r3 = '\0';
    }

    if (var_r27 != NULL) {
        var_r3_2 = &subtitle[0];
        var_r4_2 = 0;

        for (var_ctr = 0; var_ctr < 31; var_ctr++) {
            if (var_r27[0] != 0x00 || var_r27[1] != -0x45) {
                temp_r0_2 = var_r27[1];
                var_r3_2[0] = temp_r0_2;
                var_r3_2[1] = var_r27[0];
                var_r4_2 += 1;
                var_r27 += 2;
                var_r3_2 += 2;
            }
        }

        if (var_r4_2 == 0) {
            *var_r3_2 = ' ';
            var_r3_2 += 2;
        }

        *var_r3_2 = '\0';
    }

    NANDInitBanner(temp_r26, 0x10, &title[0], &subtitle[4]);
    memcpy(temp_r26->bannerTexture, tplPal->descriptors->texHeader->data, ARRAY_COUNT(temp_r26->bannerTexture));

    var_r30 = temp_r26->iconTexture[0];
    var_r27_2 = 0;
    var_r31 = &lbl_8025C890[0];
    var_r29 = 0;
    var_r28 = &lbl_8025C888[0];

    while (temp_cr0_lt != 0) {
        memcpy(var_r30, &tplPal->descriptors->texHeader->data[*var_r31 * 8], ARRAY_COUNT(temp_r26->iconTexture));
        var_r27_2++;
        temp_r0_3 = *var_r28 << var_r29;
        temp_cr0_lt = var_r27_2 < 8U;
        temp_r3 = temp_r26->iconSpeed & ~(3 << var_r29);
        var_r29 += 2;
        var_r28++;
        temp_r26->iconSpeed = (temp_r3 | temp_r0_3);
        var_r31++;
        var_r30 += 0x1200;
    }

    if (temp_cr0_lt != 0) {
        temp_r26->iconSpeed = (temp_r26->iconSpeed & ~(3 << (var_r27_2 * 2)));
    }

    xlHeapFree((void**)&tplPal);
    return 1;
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

s32* fn_80064980(char* pStrings, STStringID eStringID) {
    s32 temp_r0;
    char temp_r8;
    s32 var_r5;
    s32 var_r6;
    s32 temp_r5;
    STStringID temp_r0_2;
    u32 var_r7;

    temp_r8 = pStrings[0x20];

    if (pStrings == NULL || (temp_r5 = pStrings[8], ((temp_r5 == 0) != 0))) {
        return NULL;
    }

    var_r7 = temp_r5 - 1;
    var_r6 = 0;
    var_r5 = var_r7 / 2;

    while (true) {
        if (var_r7 < var_r6) {
            break;
        }

        temp_r0 = var_r5 * pStrings[0x1E];
        temp_r0_2 = temp_r8 + temp_r0;

        if (temp_r0_2 == eStringID) {
            return (s32*)&temp_r0_2;
        }

        if (temp_r0_2 < eStringID) {
            var_r6 = var_r5 + 1;
        } else if (temp_r0_2 > eStringID) {
            var_r7 = var_r5 - 1;
        }

        var_r5 = var_r6 + ((var_r7 - var_r6) / 2);
    }

    return NULL;
}

char* fn_80064A10(char* pStrings, STStringID eStringID) {
    s32* temp_r3;

    if (pStrings == NULL) {
        return NULL;
    }

    temp_r3 = fn_80064980(pStrings, eStringID);

    if (temp_r3 != 0) {
        return &pStrings[temp_r3[1]];
    }

    return NULL;
}
