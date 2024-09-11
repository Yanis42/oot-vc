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

GXRenderModeObj* DEMOGetRenderModeObj(void);
s32 fn_80063680(void);
bool fn_80063730(void);
void* OSAllocFromHeap(s32 handle, s32 size);
void OSFreeToHeap(s32 handle, void* p);
bool fn_80064600(NANDFileInfo* info, s32 arg1);

#ifdef __cplusplus
}
#endif

#endif
