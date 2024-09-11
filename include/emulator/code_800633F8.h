#ifndef _CODE_800633F8_H
#define _CODE_800633F8_H

#include "revolution/demo.h"
#include "revolution/gx.h"
#include "revolution/nand.h"
#include "revolution/os.h"
#include "revolution/sc.h"
#include "revolution/tpl.h"
#include "revolution/types.h"
#include "revolution/vi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ST10_MAGIC 'ST10'
#define ENCODING_NAME_LENGTH 24
#define BANNER_TITLE_MAX 28

typedef enum STTableID {
    TID_NONE = 0,
    TID_ERRORS = 0x2842C987,
    TID_COMMENTS = 0x3D2C2A7C
} STTableID;

typedef enum STStringID {
    SID_NONE = 0,
    SID_ERROR_BLANK = 0xA4E943CC,
    SID_ERROR_CHOICE_OK = 0xF6C5198F,
    SID_ERROR_CHOICE_PRESS_A_TO_RETURN_TO_MENU = 0xBB628DD1,
    SID_ERROR_DATA_CORRUPT = 0x14A5B6E2,
    SID_ERROR_DVD_STATE_FATAL_ERROR = 0xEBB077F2, // Brawl Demo only
    SID_ERROR_DVD_STATE_NO_DISK = 0x3A1F93A8, // Brawl Demo only
    SID_ERROR_DVD_STATE_RETRY = 0x25D38BC6, // Brawl Demo only
    SID_ERROR_DVD_STATE_WRONG_DISK = 0x7527596C, // Brawl Demo only
    SID_ERROR_FATAL = 0x27A3CCC2,
    SID_ERROR_GAMECUBE_CONTROLLER_CONNECTED = 0xD6E8882D,
    SID_ERROR_INS_INNODE = 0xE136B8C3,
    SID_ERROR_INS_SPACE = 0xFBB27B1C,
    SID_ERROR_INS_SPACE_PLURAL = 0x92FBDC3E, // Brawl Demo only
    SID_ERROR_MAX_BLOCKS = 0xC0192EA3,
    SID_ERROR_MAX_FILES = 0x5D3A795B,
    SID_ERROR_NEED_CLASSIC = 0x2F8DCDD7,
    SID_ERROR_NO_CONTROLLER = 0x1786067E,
    SID_ERROR_NWC24_CORRUPTED_FILE = 0x19C2F27E, // Brawl Demo only
    SID_ERROR_NWC24_ERROR_BUFFER = 0xB958D7EE, // Brawl Demo only
    SID_ERROR_NWC24_FATAL_ERROR = 0xECC5F9D8, // Brawl Demo only
    SID_ERROR_NWC24_SYSTEM_MENU_UPDATE_REQUIRED = 0x0522BEC0, // Brawl Demo only
    SID_ERROR_NWC24_UNAVAILABLE = 0xE1347C92, // Brawl Demo only
    SID_ERROR_REMOTE_BATTERY = 0x80E449BF,
    SID_ERROR_REMOTE_COMMUNICATION = 0xB35ABA8B,
    SID_ERROR_SYS_CORRUPT = 0x57D16861,
    SID_ERROR_TIME_UP_OF_THE_TRIAL_VER = 0xA2B7EF38, // Brawl Demo only
    SID_COMMENT_GAME_NAME = 0x30690AFB,
    SID_COMMENT_EMPTY = 0x30690AFD
} STStringID;

/**
 * @brief Defines an entry to the ST10 string table
 * @param nTextId Text identifier.
 * @param nTextOffset1 File offset to the string.
 * @param nTextOffset2 Another file offset to the string. Same value as above?
 * @param nTextSize1 String size.
 * @param nTextSize2 Another string size. Same as above?
 */
typedef struct STHeaderTableEntry {
    /* 0x00 */ s32 nTextId;
    /* 0x04 */ s32 nTextOffset1;
    /* 0x08 */ s32 nTextOffset2;
    /* 0x0C */ s16 nTextSize1;
    /* 0x0E */ s16 nTextSize2;
} STHeaderTableEntry; // size = 0x10

/**
 * @brief The StringTable header.
 * @param magic Format identifier. Always "ST10".
 * @param eTableID The table's ID. See the STTableID enum.
 * @param szEncoding The encoding's name.
 * @param entries List of strings entries.
 */
typedef struct STHeader {
    /* 0x00 */ s32 magic;
    /* 0x04 */ STTableID eTableID;
    /* 0x08 */ char* szEncoding[ENCODING_NAME_LENGTH];
    /* 0x20 */ STHeaderTableEntry* entries;
} STHeader; // size = 0x24

/**
 * @brief The String Table.
 * @param header String Table Header. Hosts the informations to find strings.
 * @param szStrings The strings.
 */
typedef struct StringTable {
    /* 0x00 */ STHeader* header;
    /* 0x04 */ char* szStrings;
} StringTable; // size = 0x8

typedef struct STString {
    /* 0x00 */ STStringID eSTStringID;
    /* 0x04 */ s32 unk04;
    /* 0x08 */ char* szString;
    /* 0x0C */ s32 unk0C;
    /* 0x10 */ s32 unk10;
} STString; // size = 0x10

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

GXRenderModeObj* DEMOGetRenderModeObj(void);
void fn_80063400(void);
s32 fn_80063680(void);
s32 fn_80063688(void** arg0, s32 arg1);
bool fn_80063730(void);
void fn_80063764(STString* arg0);
void fn_80063910(UnknownData2* arg0);
void fn_80063AFC(UnknownData2* arg0);
void* OSAllocFromHeap(s32 handle, s32 size);
void OSFreeToHeap(s32 handle, void* p);
void fn_80063C7C(void);
void fn_80063D78(s32 arg0);
s32 fn_80063F30(char* arg0, u32 arg1);
s32 fn_800641CC(NANDFileInfo* nandFileInfo, char* szFileName, u32 arg2, s32 arg3, s32 arg4);
bool fn_80064600(NANDFileInfo* info, s32 arg1);
bool fn_80064634(char* arg0, char* arg1);
bool fn_80064870(void);
s32 fn_80064930(void);
s32 fn_80064960(void);
void fn_8006496C(void);
s32* fn_80064980(char* pStrings, STStringID eStringID);
char* fn_80064A10(char* pStrings, STStringID eStringID);

#ifdef __cplusplus
}
#endif

#endif
