#ifndef _BANNER_H
#define _BANNER_H

#include "emulator/stringtable.h"
#include "revolution/nand.h"
#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BANNER_TITLE_MAX 32
#define BANNER_BUFFER_SIZE 0xF0A0

typedef struct struct_80174988 {
    NANDResult result;
    STStringIndex eStringIndex;
} struct_80174988;

s32 fn_80063F30(char* szBannerFileName, u32 arg1);
s32 fn_800641CC(NANDFileInfo* nandFileInfo, char* szFileName, u32 arg2, s32 arg3, u8 access);
bool fn_80064600(NANDFileInfo* info, s32 arg1);
bool fn_80064634(char* szGameName, char* szEmpty);
bool fn_80064870(void);
s32 fn_80064930(void);
s32 fn_80064960(void);
void fn_8006496C(void);

#ifdef __cplusplus
}
#endif

#endif
