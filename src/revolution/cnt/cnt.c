#include "revolution/cnt.h"
#include "revolution/esp.h"
#include "revolution/ipc.h"
#include "revolution/nand.h"

static const char warning[] = "CAUTION!  Unexpected error code [%d] was found.\n";

static inline CNTResult contentConvertErrorCode(s32 error) {
    int i;

    // clang-format off
    const s32 errorMap[] = {
        0,     CNT_RESULT_OK,
        -1001, CNT_RESULT_UNKNOWN,
        -1002, CNT_RESULT_UNKNOWN,
        -1003, CNT_RESULT_UNKNOWN,
        -1004, CNT_RESULT_OPEN_ERR,
        -1005, CNT_RESULT_UNKNOWN,
        -1006, CNT_RESULT_UNKNOWN,
        -1007, CNT_RESULT_UNKNOWN,
        -1008, CNT_RESULT_OPEN_ERR,
        -1009, CNT_RESULT_READ_ERR,
        -1010, CNT_RESULT_UNKNOWN,
        -1011, CNT_RESULT_UNKNOWN,
        -1012, CNT_RESULT_UNKNOWN,
        -1013, CNT_RESULT_UNKNOWN,
        -1014, CNT_RESULT_UNKNOWN,
        -1015, CNT_RESULT_UNKNOWN,
        -1016, CNT_RESULT_MAXFD,
        -1017, CNT_RESULT_INVALID,
        -1018, CNT_RESULT_UNKNOWN,
        -1019, CNT_RESULT_UNKNOWN,
        -1020, CNT_RESULT_UNKNOWN,
        -1021, CNT_RESULT_UNKNOWN,
        -1022, CNT_RESULT_UNKNOWN,
        -1023, CNT_RESULT_UNKNOWN,
        -1024, CNT_RESULT_OUT_OF_MEMORY,
        -1025, CNT_RESULT_UNKNOWN,
        -1026, CNT_RESULT_UNKNOWN,
        -1027, CNT_RESULT_UNKNOWN,
        -1028, CNT_RESULT_UNKNOWN,
        -1029, CNT_RESULT_UNKNOWN,
        -1030, CNT_RESULT_NOT_ENOUGH_SPACE,
        -1031, CNT_RESULT_UNKNOWN,
        -1032, CNT_RESULT_UNKNOWN,
        -1033, CNT_RESULT_UNKNOWN,
        -1034, CNT_RESULT_UNKNOWN,
        -1035, CNT_RESULT_UNKNOWN,
        -1036, CNT_RESULT_UNKNOWN,
        -1037, CNT_RESULT_UNKNOWN,
        -1038, CNT_RESULT_UNKNOWN,
        IPC_RESULT_OK,                  NAND_RESULT_OK,
        IPC_RESULT_ACCESS,              NAND_RESULT_ACCESS,
        IPC_RESULT_CORRUPT,             NAND_RESULT_CORRUPT,
        IPC_RESULT_ECC_CRIT,            NAND_RESULT_ECC_CRIT,
        IPC_RESULT_EXISTS,              NAND_RESULT_EXISTS,
        -116,                           NAND_RESULT_AUTHENTICATION,
        IPC_RESULT_INVALID,             NAND_RESULT_INVALID,
        IPC_RESULT_MAXBLOCKS,           NAND_RESULT_MAXBLOCKS,
        IPC_RESULT_MAXFD,               NAND_RESULT_MAXFD,
        IPC_RESULT_MAXFILES,            NAND_RESULT_MAXFILES,
        IPC_RESULT_NOEXISTS,            NAND_RESULT_NOEXISTS,
        IPC_RESULT_NOTEMPTY,            NAND_RESULT_NOTEMPTY,
        -104,                           NAND_RESULT_UNKNOWN,
        IPC_RESULT_OPENFD,              NAND_RESULT_OPENFD,
        -117,                           NAND_RESULT_UNKNOWN,
        IPC_RESULT_BUSY,                NAND_RESULT_BUSY,
        IPC_RESULT_ACCESS_INTERNAL,     NAND_RESULT_ACCESS,
        IPC_RESULT_EXISTS_INTERNAL,     NAND_RESULT_EXISTS,
        -3,                             NAND_RESULT_UNKNOWN,
        IPC_RESULT_INVALID_INTERNAL,    NAND_RESULT_INVALID,
        -5,                             NAND_RESULT_UNKNOWN,
        IPC_RESULT_NOEXISTS_INTERNAL,   NAND_RESULT_NOEXISTS,
        -7,                             NAND_RESULT_UNKNOWN,
        IPC_RESULT_BUSY_INTERNAL,       NAND_RESULT_BUSY,
        -9,                             NAND_RESULT_UNKNOWN,
        -10,                            NAND_RESULT_UNKNOWN,
        -11,                            NAND_RESULT_UNKNOWN,
        IPC_RESULT_ECC_CRIT_INTERNAL,   NAND_RESULT_ECC_CRIT,
        -13,                            NAND_RESULT_UNKNOWN,
        -14,                            NAND_RESULT_UNKNOWN,
        -15,                            NAND_RESULT_UNKNOWN,
        -16,                            NAND_RESULT_UNKNOWN,
        -17,                            NAND_RESULT_UNKNOWN,
        -18,                            NAND_RESULT_UNKNOWN,
        -19,                            NAND_RESULT_UNKNOWN,
        -20,                            NAND_RESULT_UNKNOWN,
        -21,                            NAND_RESULT_UNKNOWN,
        IPC_RESULT_ALLOC_FAILED,        NAND_RESULT_ALLOC_FAILED,
        -23,                            NAND_RESULT_UNKNOWN,
    };
    // clang-format on

    i = 0;

    if (error >= 0) {
        return error;
    }

    for (; i < ARRAY_COUNT(errorMap); i += 2) {
        if (error == errorMap[i]) {
            return errorMap[i + 1];
        }
    }

    OSReport(warning, error);
    return CNT_RESULT_UNKNOWN;
}

static char path[] = "/dev/es";
static s32 lbl_8025CD48 = -1;

void fn_800FEFB8(void) {

    if (lbl_8025CD48 < 0) {
        lbl_8025CD48 = IOS_Open(path, IPC_OPEN_NONE);
    }
}

s32 contentInitHandleNAND(s32 contentNum, CNTHandleNAND* handle, MEMAllocator* memAlloc) {
    // ? var_r3;
    // ARCHeader* temp_r3_2;
    // ARCHeader* temp_r3_3;
    // MEMAllocator* temp_r31;
    // s32 temp_r3;
    // s32 temp_r5;
    // s32 temp_ret;
    // s32 var_r28;
    // u32 temp_r26;
    // u32 temp_r4;

    // *(sp + (-0x160 - ((s32) sp & 0x1F))) = sp;
    // sp->unk4 = (s32) saved_reg_lr;
    // temp_ret = _savegpr_26();
    // temp_r3 = temp_ret;
    // temp_r4 = (u32) (u64) temp_ret;
    // temp_r31 = M2C_ERROR(/* Read from unset register $r5 */);
    // if ((s32) lbl_8025CD48 < 0) {
    //     var_r28 = -0x3F9;
    // } else {
    //     arg10 = temp_r3;
    //     arg46 = &arg10;
    //     arg47 = 4;
    //     var_r28 = IOS_Ioctlv(lbl_8025CD48, 9, 1, 0, (IPCIOVector* ) &arg46);
    // }
    // if (var_r28 < 0) {
    //     var_r3 = -0x138A;
    // } else {
    //     temp_r3_2 = MEMAllocFromAllocator(temp_r31, 0x20U);
    //     if (temp_r3_2 == NULL) {
    //         var_r3 = -0x1389;
    //     } else if (ESP_ReadContentFile(var_r28, temp_r3_2, 0x20U) < 0) {
    //         fn_800B164C(temp_r31, temp_r3_2);
    //         var_r3 = -0x138B;
    //     } else {
    //         temp_r5 = temp_r3_2->files.offset;
    //         temp_r26 = (temp_r5 + 0x1F) & 0xFFFFFFE0;
    //         fn_800B164C(temp_r31, temp_r3_2, temp_r5);
    //         if (ESP_SeekContentFile(var_r28, 0, 0) < 0) {
    //             var_r3 = -0x138C;
    //         } else {
    //             temp_r3_3 = MEMAllocFromAllocator(temp_r31, (temp_r26 + 0x1F) & 0xFFFFFFE0);
    //             if (temp_r3_3 == NULL) {
    //                 var_r3 = -0x1389;
    //             } else if (ESP_ReadContentFile(var_r28, temp_r3_3, temp_r26) < 0) {
    //                 fn_800B164C(temp_r31, temp_r3_3);
    //                 var_r3 = -0x138B;
    //             } else {
    //                 ARCInitHandle(temp_r3_3, (ARCHandle* ) &arg8);
    //                 var_r3 = 0;
    //                 temp_r4->unk0 = arg8;
    //                 temp_r4->unk4 = arg9;
    //                 temp_r4->unk8 = argA;
    //                 temp_r4->unkC = argB;
    //                 temp_r4->unk10 = argC;
    //                 temp_r4->unk14 = argD;
    //                 temp_r4->unk18 = argE;
    //                 temp_r4->unk1C = var_r28;
    //                 temp_r4->unk20 = temp_r31;
    //             }
    //         }
    //     }
    // }
    // _restgpr_26(var_r3);
}

CNTResult contentOpenNAND(CNTHandleNAND* handle, const char* path, CNTFileInfoNAND* info) {
    ARCFileInfo arcInfo;

    if (!ARCOpen(&handle->arcHandle, path, &arcInfo)) {
        return CNT_RESULT_OPEN_ERR;
    }

    info->handle = handle;
    info->offset = arcInfo.offset;
    info->length = arcInfo.size;
    info->position = 0;

    return CNT_RESULT_OK;
}

CNTResult contentFastOpenNAND(CNTHandleNAND* handle, s32 entrynum, CNTFileInfoNAND* info) {
    ARCFileInfo arcInfo;

    if (!ARCFastOpen(&handle->arcHandle, entrynum, &arcInfo)) {
        return CNT_RESULT_OPEN_ERR;
    }

    info->handle = handle;
    info->offset = arcInfo.offset;
    info->length = arcInfo.size;
    info->position = 0;

    return CNT_RESULT_OK;
}

s32 contentConvertPathToEntrynumNAND(CNTHandleNAND* info, const char* path) {
    return ARCConvertPathToEntrynum(&info->arcHandle, path);
}

u32 contentGetLengthNAND(CNTFileInfoNAND* info) { return info->length; }

CNTResult contentSeekNAND(CNTFileInfoNAND* info, u32 offset, s32 whence) {
    u32 position;

    switch (whence) {
        case 0:
            position = offset;
            break;
        case 1:
            position = info->position + offset;
            break;
        case 2:
            position = info->length + offset;
            break;
        default:
            return CNT_RESULT_FATAL;
    }

    if (position > info->length) {
        return CNT_RESULT_INVALID;
    }

    info->position = position;

    return CNT_RESULT_OK;
}

CNTResult contentReadNAND(CNTFileInfoNAND* info, void* dst, u32 len, s32 offset) {
    if (info->position + offset > info->length) {
        return CNT_RESULT_INVALID;
    }

    if (ESP_SeekContentFile(info->handle->fd, info->offset + info->position + offset, IPC_SEEK_BEG) < 0) {
        return CNT_RESULT_SEEK_ERR;
    }

    return contentConvertErrorCode(ESP_ReadContentFile(info->handle->fd, dst, len));
}

CNTResult contentCloseNAND(CNTFileInfoNAND* info) { return CNT_RESULT_OK; }

CNTResult contentReleaseHandleNAND(CNTHandleNAND* handle) {
    s32 var_f1;
    s32 var_r3;
    IPCIOVector* vectors;

    // *(sp + (-0x3C0 - ((s32) sp & 0x1F))) = sp;
    // sp->unk4 = (s32) saved_reg_lr;
    // sp->unk-4 = (s32) saved_reg_r31;
    var_f1 = fn_800B164C(handle->memAlloc, handle->arcHandle.header);

    if (lbl_8025CD48 < 0 || handle->fd < 0) {
        var_r3 = -1017;
    } else {
        vectors[0].base = &var_f1;
        vectors[0].length = sizeof(s32);
        vectors[4].base = &handle->fd;
        vectors[4].length = sizeof(s32);
        var_r3 = IOS_Ioctlv(lbl_8025CD48, 11, 1, 0, vectors);
    }

    return contentConvertErrorCode(var_r3);
}

bool contentOpenDirNAND(CNTHandleNAND* handle, const char* path, ARCDir* dir) {
    return ARCOpenDir(&handle->arcHandle, path, dir);
}
