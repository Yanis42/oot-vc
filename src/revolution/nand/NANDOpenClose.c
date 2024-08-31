#include "macros.h"
#include "revolution/nand.h"
#include "revolution/os.h"
#include "stdio.h"

static void nandOpenCallback(s32 result, void* arg);
static void nandSafeCloseCallback(s32 result, void* arg);

static s32 nandOpen(const char* path, u8 mode, NANDCommandBlock* block, bool async, bool priv) {
    IPCOpenMode ipcMode;
    char absPath[64];

    MEMCLR(&absPath);
    ipcMode = IPC_OPEN_NONE;
    nandGenerateAbsPath(absPath, path);

    if (!priv && nandIsPrivatePath(absPath)) {
        return IPC_RESULT_ACCESS;
    }

    switch (mode) {
        case NAND_ACCESS_RW:
            ipcMode = IPC_OPEN_RW;
            break;
        case NAND_ACCESS_READ:
            ipcMode = IPC_OPEN_READ;
            break;
        case NAND_ACCESS_WRITE:
            ipcMode = IPC_OPEN_WRITE;
            break;
    }

    if (async) {
        return ISFS_OpenAsync(absPath, ipcMode, nandOpenCallback, block);
    } else {
        return ISFS_Open(absPath, ipcMode);
    }
}

s32 NANDOpen(const char* path, NANDFileInfo* info, u8 mode) {
    s32 result;

    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    result = nandOpen(path, mode, NULL, false, false);
    if (result >= 0) {
        info->fd = result;
        return NAND_RESULT_OK;
    }

    return nandConvertErrorCode(result);
}

s32 NANDPrivateOpen(const char* path, NANDFileInfo* info, u8 mode) {
    s32 result;

    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    result = nandOpen(path, mode, NULL, false, true);
    if (result >= 0) {
        info->fd = result;
        return NAND_RESULT_OK;
    }

    return nandConvertErrorCode(result);
}

s32 NANDOpenAsync(const char* path, NANDFileInfo* info, u8 mode, NANDAsyncCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    block->info = info;
    return nandConvertErrorCode(nandOpen(path, mode, block, true, false));
}

s32 NANDPrivateOpenAsync(const char* path, NANDFileInfo* info, u8 mode, NANDAsyncCallback callback,
                         NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    block->info = info;
    return nandConvertErrorCode(nandOpen(path, mode, block, true, true));
}

static void nandOpenCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result >= 0) {
        block->info->fd = result;
        block->callback(NAND_RESULT_OK, block);
    } else {
        block->callback(nandConvertErrorCode(result), block);
    }
}

s32 NANDClose(NANDFileInfo* info) {
    s32 result;

    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    result = ISFS_Close(info->fd);

    return nandConvertErrorCode(result);
}

s32 NANDCloseAsync(NANDFileInfo* info, NANDAsyncCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(ISFS_CloseAsync(info->fd, nandCallback, block));
}

void NANDSafeOpen() { fn_800B3958(0); }

s32 NANDSafeClose(NANDFileInfo* info) {
    s32 result;
    char* path;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(info->openPath); i++) {
        // info->openPath[i] = '\0';
    }

    if (!nandIsInitialized()) {
        return -0x80;
    }

    if (info->access == 1) {
        return nandConvertErrorCode(ISFS_Close(info->fd));
    }

    if ((info->access + 0xFE) <= 1) {
        result = ISFS_Close(info->fd);
        if (result != 0) {
            return nandConvertErrorCode(result);
        }

        result = ISFS_Close(info->tempFd);
        if (result != 0) {
            return nandConvertErrorCode(result);
        }

        result = ISFS_Rename(info->tempPath, info->openPath);
        if (result != 0) {
            return nandConvertErrorCode(result);
        }

        nandGetParentDirectory(path, info->tempPath);
        return nandConvertErrorCode(ISFS_Delete(path));
    }

    OSReport("Illegal NANDFileInfo\n");
    return -0x80;
}
