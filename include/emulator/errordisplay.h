#ifndef _CODE_800633F8_H
#define _CODE_800633F8_H

#include "emulator/banner.h"
#include "emulator/stringtable.h"
#include "revolution/gx.h"
#include "revolution/sc.h"
#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FADE_TIMER_MAX 15

#define FLAG_COLOR_WHITE (0 << 0)
#define FLAG_COLOR_YELLOW (1 << 0)
#define FLAG_UNK (1 << 1)

typedef struct STFiles {
    /* 0x00 */ SCLanguage eLanguage;
    /* 0x04 */ char* szErrors;
    /* 0x08 */ char* szSaveComments;
} STFiles; // size = 0xC

typedef struct STString {
    /* 0x00 */ struct STStringDraw* apStringDraw[SI_NULL];
    /* 0x30 */ STStringIndex eStringIndex;
    /* 0x34 */ s32 iAction;
} STString; // size = 0x38

typedef s32 (*UnknownCallback)(struct STString*);

typedef struct STStringBase {
    /* 0x00 */ STStringID eSTStringID;
    /* 0x04 */ s32 nLines;
    /* 0x08 */ char* szString;
    /* 0x0C */ s32 unk0C;
    /* 0x10 */ s32 unk10;
} STStringBase; // size = 0x10

typedef struct TextInfo {
    /* 0x00 */ STStringBase* pBase;
    /* 0x04 */ s16 nFlags; // bitfield
    /* 0x06 */ s16 nFadeInTimer;
    /* 0x08 */ s32 nShiftY; // Y position relative to nStartY
} TextInfo; // size = 0xC

typedef struct TextAction {
    /* 0x00 */ TextInfo textInfo;
    /* 0x0C */ UnknownCallback unk0C;
} TextAction; // size = 0x10

typedef struct STStringDraw {
    /* 0x00 */ TextInfo textInfo;
    /* 0x0C */ TextAction textAction[2];
    /* 0x2C */ s32 nAction;
    /* 0x30 */ UnknownCallback unk30;
    /* 0x34 */ s16 nStartY;
    /* 0x36 */ s16 unk36; // unused?
    /* 0x38 */ s32 unk38;
    /* 0x3C */ s32 unk3C;
} STStringDraw; // size = 0x40

GXRenderModeObj* DEMOGetRenderModeObj(void);
void fn_80063400(void);
s32 fn_80063680(STString* pSTString);
s32 fn_80063688(STString* arg0, s32 arg1);
s32 fn_80063730(STString* pSTString);
void fn_80063764(STStringBase* pStringBase);
void fn_80063910(STStringDraw* pStringDraw);
void fn_80063AFC(STString* pStringDraw);
void* OSAllocFromHeap(s32 handle, s32 size);
void OSFreeToHeap(s32 handle, void* p);
void fn_80063C7C(void);
bool fn_80063D78(STStringIndex eStringIndex);

extern STStringDraw sStringDraw[12];
extern struct_80174988 lbl_80174988[17];
extern bool lbl_8025D130;
extern s32 lbl_8025D12C;

#ifdef __cplusplus
}
#endif

#endif