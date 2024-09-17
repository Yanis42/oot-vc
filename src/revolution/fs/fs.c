#include "revolution/fs.h"
#include "revolution/ipc.h"
#include "revolution/os.h"
#include "string.h"

#define FS_HEAP_SIZE 0x1500

// Longest name obtainable from ReadDir ioctl
#define FS_DIR_NAME_MAX (12 + 1)

/**
 * To make the double NULL checks look less weird.
 * I think they must have written something like this, as the debug version
 * shows there is no inlined function causing this.
 */
#define FS_DELETE(x)       \
    if ((x) != NULL) {     \
        iosFree(hId, (x)); \
    }

/**
 * Used to identify the async operation that resulted in _isfsFuncCb being
 * called. Some operations do not require anything else to be done (will use
 * CB_STATE_NONE), but in some cases _isfsFuncCb must finish the task.
 */
typedef enum {
    CB_STATE_NONE,
    CB_STATE_GET_STATS,
    CB_STATE_READ_DIR,
    CB_STATE_GET_ATTR,
    CB_STATE_GET_USAGE,
    CB_STATE_GET_FILE_STATS,
} FSCallbackState;

typedef enum {
    FS_IOCTL_CREATE_DIR = 3,
    FS_IOCTL_READ_DIR = 4,
    FS_IOCTL_GET_ATTR = 6,
    FS_IOCTL_DELETE_PATH = 7,
    FS_IOCTL_RENAME_PATH = 8,
    FS_IOCTL_CREATE_FILE = 9,
    FS_IOCTL_GET_FILE_STATS = 11,
    FS_IOCTL_SHUTDOWN_FS = 13,

    FS_IOCTLV_GET_USAGE = 12
} FSIoctl;

/**
 * Ioctl data for FS functions.
 * Some functions share the same structure (such as Create/GetAttr).
 * NOTE: Some functions instead use the work buffer (ioctlWork).
 */

// For ISFS_Create*/ISFS_GetAttr
typedef struct FSFileIoctl {
    /* 0x0 */ u32 ownerId;
    /* 0x4 */ u16 groupId;
    /* 0x6 */ char path[FS_MAX_PATH];
    /* 0x46 */ u8 ownerPerm;
    /* 0x47 */ u8 groupPerm;
    /* 0x48 */ u8 otherPerm;
    /* 0x49 */ u8 attr;
    char UNK_0x4A[0x4C - 0x4A];
} FSFileIoctl;

// For ISFS_Rename
typedef struct FSRenameIoctl {
    /* 0x0 */ char from[FS_MAX_PATH];
    /* 0x40 */ char to[FS_MAX_PATH];
} FSRenameIoctl;

/**
 * Async context data for FS functions.
 * Used to store arguments for retrieval in _isfsFuncCb.
 */

// For ISFS_GetStatsAsync
typedef struct FSGetStatsAsyncCtx {
    /* 0x0 */ FSStats* statsOut;
} FSGetStatsAsyncCtx;

// For ISFS_ReadDirAsync
typedef struct FSReadDirAsyncCtx {
    /* 0x0 */ u32* fileCountOut;
} FSReadDirAsyncCtx;

// For ISFS_GetAttrAsync
typedef struct FSGetAttrAsyncCtx {
    /* 0x0 */ u32* ownerIdOut;
    /* 0x4 */ u16* groupIdOut;
    /* 0x8 */ u32* attrOut;
    /* 0xC */ u32* ownerPermOut;
    /* 0x10 */ u32* groupPermOut;
    /* 0x14 */ u32* otherPermOut;
} FSGetAttrAsyncCtx;

// For ISFS_GetUsageAsync
typedef struct FSGetUsageAsyncCtx {
    /* 0x0 */ u32* blockCountOut;
    /* 0x4 */ u32* fileCountOut;
} FSGetUsageAsyncCtx;

// For ISFS_GetFileStatsAsync
typedef struct FSGetFileStatsAsyncCtx {
    /* 0x0 */ FSFileStats* statsOut;
} FSGetFileStatsAsyncCtx;

/**
 * Generic command-block structure used by all FS functions
 */
typedef struct FSCommandBlock {
    // Data for ioctl
    /* 0x0 */ union {
        FSFileIoctl fileIoctl;
        FSRenameIoctl renameIoctl;
        u8 ioctlWork[0x100];
    };

    // User callback settings
    /* 0x100 */ FSAsyncCallback callback;
    /* 0x104 */ void* callbackArg;

    // Data for FS IPC callback (_isfsFuncCb)
    /* 0x108 */ FSCallbackState callbackState;
    /* 0x10C */ union {
        FSGetStatsAsyncCtx getStatsCtx;
        FSReadDirAsyncCtx readDirCtx;
        FSGetAttrAsyncCtx getAttrCtx;
        FSGetUsageAsyncCtx getUsageCtx;
        FSGetFileStatsAsyncCtx getFileStatsCtx;
        u8 forceUnionSize[0x140 - 0x10C];
    };
} FSCommandBlock;

static s32 __fsFd = -1;
static char* __devfs = NULL;
static u32 _asynCnt = 0;
static s32 hId;

static s32 _FSGetStatsCb(s32 result, FSCommandBlock* block);
static s32 _FSReadDirCb(s32 result, FSCommandBlock* block);
static s32 _FSGetAttrCb(s32 result, FSCommandBlock* block);
static s32 _FSGetUsageCb(s32 result, FSCommandBlock* block);
static s32 _FSGetFileStatsCb(s32 result, FSCommandBlock* block);

// Not part of MSL, but implemented in IPC.
size_t strnlen(const char* str, size_t maxlen);

s32 ISFS_OpenLib(void) {
    static bool firstFl = true;
    static void* lo;
    static void* hi;

    s32 ret = IPC_RESULT_OK;
    u8* base;

    if (firstFl) {
        lo = IPCGetBufferLo();
        hi = IPCGetBufferHi();
    }

    __devfs = (char*)ROUND_UP_PTR(lo, 32);

    if (firstFl && __devfs + FS_MAX_PATH > hi) {
        OSReport("APP ERROR: Not enough IPC arena\n");
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    strcpy(__devfs, "/dev/fs");
    __fsFd = IOS_Open(__devfs, 0);

    if (__fsFd < 0) {
        ret = __fsFd;
        goto end;
    }

    base = (u8*)__devfs;

    if (firstFl && base + FS_MAX_PATH + FS_HEAP_SIZE > hi) {
        OSReport("APP ERROR: Not enough IPC arena\n");
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    if (firstFl) {
        IPCSetBufferLo(base + FS_MAX_PATH + FS_HEAP_SIZE);
        firstFl = false;
    }

    hId = iosCreateHeap(base, FS_MAX_PATH + FS_HEAP_SIZE);
    if (hId < 0) {
        ret = IPC_RESULT_ALLOC_FAILED;
    }

end:
    return ret;
}

static s32 _isfsFuncCb(s32 result, void* arg) {
    FSCommandBlock* block = (FSCommandBlock*)arg;

    if (result >= IPC_RESULT_OK) {
        switch (block->callbackState) {
            case CB_STATE_GET_STATS:
                _FSGetStatsCb(result, block);
                break;
            case CB_STATE_READ_DIR:
                _FSReadDirCb(result, block);
                break;
            case CB_STATE_GET_ATTR:
                _FSGetAttrCb(result, block);
                break;
            case CB_STATE_GET_USAGE:
                _FSGetUsageCb(result, block);
                break;
        }
    }

    _asynCnt = 0;
    if (block->callback != NULL) {
        block->callback(result, block->callbackArg);
    }

    FS_DELETE(block);
    return result;
}

static s32 _FSGetStatsCb(s32 result, FSCommandBlock* block) {
    if (result == IPC_RESULT_OK) {
        memcpy(block->getStatsCtx.statsOut, block->ioctlWork, sizeof(FSStats));
    }

    return IPC_RESULT_OK;
}

s32 ISFS_CreateDir(const char* path, u32 attr, u32 ownerPerm, u32 groupPerm, u32 otherPerm) {
    FSCommandBlock* block;
    s32 ret;
    size_t len;

    block = NULL;

    if (path == NULL || __fsFd < 0 || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        ret = IPC_RESULT_INVALID;
        goto end;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    memcpy(block->fileIoctl.path, path, len + 1);

    block->fileIoctl.attr = attr;
    block->fileIoctl.ownerPerm = ownerPerm;
    block->fileIoctl.groupPerm = groupPerm;
    block->fileIoctl.otherPerm = otherPerm;

    ret = IOS_Ioctl(__fsFd, FS_IOCTL_CREATE_DIR, &block->fileIoctl, sizeof(FSFileIoctl), NULL, 0);

end:
    if (block != NULL) {
        FS_DELETE(block);
    }

    return ret;
}

s32 ISFS_CreateDirAsync(const char* path, u32 attr, u32 ownerPerm, u32 groupPerm, u32 otherPerm,
                        FSAsyncCallback callback, void* callbackArg) {
    FSCommandBlock* block;
    size_t len;

    if (path == NULL || __fsFd < 0 || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        return IPC_RESULT_INVALID;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_NONE;

    memcpy(block->fileIoctl.path, path, len + 1);

    block->fileIoctl.attr = attr;
    block->fileIoctl.ownerPerm = ownerPerm;
    block->fileIoctl.groupPerm = groupPerm;
    block->fileIoctl.otherPerm = otherPerm;

    return IOS_IoctlAsync(__fsFd, FS_IOCTL_CREATE_DIR, &block->fileIoctl, sizeof(FSFileIoctl), NULL, 0, _isfsFuncCb,
                          block);
}

s32 ISFS_ReadDir(const char* path, char* filesOut, u32* fileCountOut) {
    s32 ret;
    FSCommandBlock* block;
    char* pathWork;
    IPCIOVector* vectors;
    size_t len;
    u32* countWork;
    u32 inCount;
    u32 outCount;

    block = NULL;

    if (path == NULL || fileCountOut == NULL || __fsFd < 0 || (u32)filesOut % 32 != 0 ||
        (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        ret = IPC_RESULT_INVALID;
        goto end;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    // Directory path
    vectors = (IPCIOVector*)block->ioctlWork;
    pathWork = ROUND_UP_PTR((u8*)vectors + (sizeof(IPCIOVector) * 4), 32);
    memcpy(pathWork, path, len + 1);

    // Input: Directory path
    vectors[0].base = pathWork;
    vectors[0].length = FS_MAX_PATH;
    // Input: Max file count
    countWork = ROUND_UP_PTR(pathWork + FS_MAX_PATH, 32);
    vectors[1].base = countWork;
    vectors[1].length = sizeof(u32);

    if (filesOut != NULL) {
        inCount = 2;
        outCount = 2;

        // Out param initially contains max file count (length of filesOut)
        *countWork = *fileCountOut;

        // Output: File names
        vectors[2].base = filesOut;
        vectors[2].length = *fileCountOut * FS_DIR_NAME_MAX;
        // Output: File count
        vectors[3].base = countWork;
        vectors[3].length = sizeof(u32);
    } else {
        inCount = 1;
        outCount = 1;
    }

    ret = IOS_Ioctlv(__fsFd, FS_IOCTL_READ_DIR, inCount, outCount, vectors);
    if (ret == IPC_RESULT_OK) {
        *fileCountOut = *countWork;
    }

end:
    if (block != NULL) {
        FS_DELETE(block);
    }

    return ret;
}

static s32 _FSReadDirCb(s32 result, FSCommandBlock* block) {
    u8* pathWork;
    if (result == IPC_RESULT_OK) {
        pathWork = ROUND_UP_PTR(block->ioctlWork + (sizeof(IPCIOVector) * 4), 32);
        *block->readDirCtx.fileCountOut = *(u32*)ROUND_UP_PTR((u8*)pathWork + FS_MAX_PATH, 32);
    }

    return IPC_RESULT_OK;
}

s32 ISFS_ReadDirAsync(const char* path, char* filesOut, u32* fileCountOut, FSAsyncCallback callback,
                      void* callbackArg) {
    FSCommandBlock* block;
    char* pathWork;
    IPCIOVector* vectors;
    size_t len;
    u32* countWork;
    u32 inCount;
    u32 outCount;

    block = NULL;

    if (path == NULL || fileCountOut == NULL || __fsFd < 0 || (u32)filesOut % 32 != 0 ||
        (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        return IPC_RESULT_INVALID;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_READ_DIR;
    block->readDirCtx.fileCountOut = fileCountOut;

    // Directory path
    vectors = (IPCIOVector*)block->ioctlWork;
    pathWork = ROUND_UP_PTR((u8*)vectors + (sizeof(IPCIOVector) * 4), 32);
    memcpy(pathWork, path, len + 1);

    // Input: Directory path
    vectors = (IPCIOVector*)block->ioctlWork; // Second cast is required
    vectors[0].base = pathWork;
    vectors[0].length = FS_MAX_PATH;
    // Input: Max file count
    countWork = ROUND_UP_PTR(pathWork + FS_MAX_PATH, 32);
    vectors[1].base = countWork;
    vectors[1].length = sizeof(u32);

    if (filesOut != NULL) {
        inCount = 2;
        outCount = 2;

        // Out param initially contains max file count (length of filesOut)
        *countWork = *fileCountOut;

        // Output: File names
        vectors[2].base = filesOut;
        vectors[2].length = *fileCountOut * FS_DIR_NAME_MAX;
        // Output: File count
        vectors[3].base = countWork;
        vectors[3].length = sizeof(u32);
    } else {
        inCount = 1;
        outCount = 1;
    }

    return IOS_IoctlvAsync(__fsFd, FS_IOCTL_READ_DIR, inCount, outCount, vectors, _isfsFuncCb, block);
}

s32 ISFS_GetAttr(const char* path, u32* ownerIdOut, u16* groupIdOut, u32* attrOut, u32* ownerPermOut, u32* groupPermOut,
                 u32* otherPermOut) {
    s32 ret;
    FSFileIoctl* fileIoctl;
    size_t len;
    FSCommandBlock* block;

    block = NULL;

    if (path == NULL || __fsFd < 0 || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH || ownerIdOut == NULL ||
        groupIdOut == NULL || attrOut == NULL || ownerPermOut == NULL || groupPermOut == NULL || otherPermOut == NULL) {
        ret = IPC_RESULT_INVALID;
        goto end;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    memcpy(block->ioctlWork, path, len + 1);
    fileIoctl = ROUND_UP_PTR(block->ioctlWork + FS_MAX_PATH, 32);

    ret = IOS_Ioctl(__fsFd, FS_IOCTL_GET_ATTR, block->ioctlWork, FS_MAX_PATH, fileIoctl, sizeof(FSFileIoctl));

    if (ret == IPC_RESULT_OK) {
        *ownerIdOut = fileIoctl->ownerId;
        *groupIdOut = fileIoctl->groupId;
        *attrOut = fileIoctl->attr;
        *ownerPermOut = fileIoctl->ownerPerm;
        *groupPermOut = fileIoctl->groupPerm;
        *otherPermOut = fileIoctl->otherPerm;
    }

end:
    if (block != NULL) {
        FS_DELETE(block);
    }

    return ret;
}

static s32 _FSGetAttrCb(s32 result, FSCommandBlock* block) {
    FSFileIoctl* fileIoctl;

    if (result == IPC_RESULT_OK) {
        fileIoctl = (FSFileIoctl*)ROUND_UP_PTR(block->ioctlWork + FS_MAX_PATH, 32);

        *block->getAttrCtx.ownerIdOut = fileIoctl->ownerId;
        *block->getAttrCtx.groupIdOut = fileIoctl->groupId;
        *block->getAttrCtx.attrOut = fileIoctl->attr;
        *block->getAttrCtx.ownerPermOut = fileIoctl->ownerPerm;
        *block->getAttrCtx.groupPermOut = fileIoctl->groupPerm;
        *block->getAttrCtx.otherPermOut = fileIoctl->otherPerm;
    }

    return IPC_RESULT_OK;
}

s32 ISFS_GetAttrAsync(const char* path, u32* ownerIdOut, u16* groupIdOut, u32* attrOut, u32* ownerPermOut,
                      u32* groupPermOut, u32* otherPermOut, FSAsyncCallback callback, void* callbackArg) {
    size_t len;
    FSCommandBlock* block;

    block = NULL;

    if (path == NULL || __fsFd < 0 || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH || ownerIdOut == NULL ||
        groupIdOut == NULL || attrOut == NULL || ownerPermOut == NULL || groupPermOut == NULL || otherPermOut == NULL) {
        return IPC_RESULT_INVALID;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    block->getAttrCtx.ownerIdOut = ownerIdOut;
    block->getAttrCtx.groupIdOut = groupIdOut;
    block->getAttrCtx.attrOut = attrOut;
    block->getAttrCtx.ownerPermOut = ownerPermOut;
    block->getAttrCtx.groupPermOut = groupPermOut;
    block->getAttrCtx.otherPermOut = otherPermOut;

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_GET_ATTR;

    memcpy(block->ioctlWork, path, len + 1);

    return IOS_IoctlAsync(__fsFd, FS_IOCTL_GET_ATTR, block->ioctlWork, FS_MAX_PATH,
                          ROUND_UP_PTR(block->ioctlWork + FS_MAX_PATH, 32), sizeof(FSFileIoctl), _isfsFuncCb, block);
}

s32 ISFS_Delete(const char* path) {
    s32 ret;
    size_t len;
    FSCommandBlock* block;

    block = NULL;

    if (path == NULL || __fsFd < 0 || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        ret = IPC_RESULT_INVALID;
        goto end;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    memcpy(block->ioctlWork, path, len + 1);

    ret = IOS_Ioctl(__fsFd, FS_IOCTL_DELETE_PATH, block->ioctlWork, FS_MAX_PATH, NULL, 0);

end:
    if (block != NULL) {
        FS_DELETE(block);
    }

    return ret;
}

s32 ISFS_DeleteAsync(const char* path, FSAsyncCallback callback, void* callbackArg) {
    size_t len;
    FSCommandBlock* block;

    block = NULL;

    if (path == NULL || __fsFd < 0 || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        return IPC_RESULT_INVALID;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    memcpy(block->ioctlWork, path, len + 1);
    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_NONE;

    return IOS_IoctlAsync(__fsFd, FS_IOCTL_DELETE_PATH, block->ioctlWork, FS_MAX_PATH, NULL, 0, _isfsFuncCb, block);
}

s32 ISFS_Rename(const char* from, const char* to) {
    s32 ret;
    size_t lenFrom;
    size_t lenTo;
    FSCommandBlock* block;

    block = NULL;

    if (from == NULL || to == NULL || __fsFd < 0 || (lenFrom = strnlen(from, FS_MAX_PATH)) == FS_MAX_PATH ||
        (lenTo = strnlen(to, FS_MAX_PATH)) == FS_MAX_PATH) {
        ret = IPC_RESULT_INVALID;
        goto end;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    memcpy(block->renameIoctl.from, from, lenFrom + 1);
    memcpy(block->renameIoctl.to, to, lenTo + 1);

    ret = IOS_Ioctl(__fsFd, FS_IOCTL_RENAME_PATH, &block->renameIoctl, sizeof(FSRenameIoctl), NULL, 0);

end:
    if (block != NULL) {
        FS_DELETE(block);
    }

    return ret;
}

s32 ISFS_GetUsage(const char* path, s32* blockCountOut, s32* fileCountOut) {
    s32 ret;
    u32* blockCountWork;
    u32* fileCountWork;
    char* pathWork;
    IPCIOVector* vectors;
    FSCommandBlock* block;
    size_t len;

    block = NULL;

    if (path == NULL || __fsFd < 0 || blockCountOut == NULL || fileCountOut == NULL ||
        (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        ret = IPC_RESULT_INVALID;
        goto end;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    // Directory path
    vectors = (IPCIOVector*)block->ioctlWork;
    pathWork = ROUND_UP_PTR((u8*)vectors + (sizeof(IPCIOVector) * 3), 32);
    memcpy(pathWork, path, len + 1);

    // Input: Directory path
    vectors[0].base = pathWork;
    vectors[0].length = FS_MAX_PATH;

    blockCountWork = ROUND_UP_PTR(pathWork + FS_MAX_PATH, 32);
    fileCountWork = ROUND_UP_PTR((u8*)blockCountWork + sizeof(u32), 32);

    // Output: Number of blocks used by directory
    vectors[1].base = blockCountWork;
    vectors[1].length = sizeof(u32);
    // Output: Number of files in directory
    vectors[2].base = fileCountWork;
    vectors[2].length = sizeof(u32);

    ret = IOS_Ioctlv(__fsFd, FS_IOCTLV_GET_USAGE, 1, 2, vectors);
    if (ret == IPC_RESULT_OK) {
        *blockCountOut = *blockCountWork;
        *fileCountOut = *fileCountWork;
    }

end:
    if (block != NULL) {
        FS_DELETE(block);
    }

    return ret;
}

static s32 _FSGetUsageCb(s32 result, FSCommandBlock* block) {
    u8* work;

    if (result == IPC_RESULT_OK) {
        work = ROUND_UP_PTR(block->ioctlWork + (sizeof(IPCIOVector) * 4), 32);

        work = ROUND_UP_PTR(work + FS_MAX_PATH, 32);
        *block->getUsageCtx.blockCountOut = *(u32*)work;

        work = ROUND_UP_PTR(work + sizeof(u32), 32);
        *block->getUsageCtx.fileCountOut = *(u32*)work;
    }

    return IPC_RESULT_OK;
}

s32 ISFS_CreateFile(const char* path, u32 attr, u32 ownerPerm, u32 groupPerm, u32 otherPerm) {
    FSCommandBlock* block;
    s32 ret;
    size_t len;

    block = NULL;

    if (path == NULL || __fsFd < 0 || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        ret = IPC_RESULT_INVALID;
        goto end;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    memcpy(block->fileIoctl.path, path, len + 1);

    block->fileIoctl.attr = attr;
    block->fileIoctl.ownerPerm = ownerPerm;
    block->fileIoctl.groupPerm = groupPerm;
    block->fileIoctl.otherPerm = otherPerm;

    ret = IOS_Ioctl(__fsFd, FS_IOCTL_CREATE_FILE, &block->fileIoctl, sizeof(FSFileIoctl), NULL, 0);

end:
    if (block != NULL) {
        FS_DELETE(block);
    }

    return ret;
}

s32 ISFS_CreateFileAsync(const char* path, u32 attr, u32 ownerPerm, u32 groupPerm, u32 otherPerm,
                         FSAsyncCallback callback, void* callbackArg) {
    FSCommandBlock* block;
    size_t len;

    if (path == NULL || __fsFd < 0 || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        return IPC_RESULT_INVALID;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_NONE;

    memcpy(block->fileIoctl.path, path, len + 1);

    block->fileIoctl.attr = attr;
    block->fileIoctl.ownerPerm = ownerPerm;
    block->fileIoctl.groupPerm = groupPerm;
    block->fileIoctl.otherPerm = otherPerm;

    return IOS_IoctlAsync(__fsFd, FS_IOCTL_CREATE_FILE, &block->fileIoctl, sizeof(FSFileIoctl), NULL, 0, _isfsFuncCb,
                          block);
}

s32 ISFS_Open(const char* path, IPCOpenMode mode) {
    s32 ret;
    size_t len;
    FSCommandBlock* block;

    block = NULL;

    if (path == NULL || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        ret = IPC_RESULT_INVALID;
        goto end;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        ret = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

    memcpy(block->ioctlWork, path, len + 1);

    ret = IOS_Open((const char*)block->ioctlWork, mode);

end:
    if (block != NULL) {
        FS_DELETE(block);
    }

    return ret;
}

s32 ISFS_OpenAsync(const char* path, IPCOpenMode mode, FSAsyncCallback callback, void* callbackArg) {
    size_t len;
    FSCommandBlock* block;

    block = NULL;

    if (path == NULL || (len = strnlen(path, FS_MAX_PATH)) == FS_MAX_PATH) {
        return IPC_RESULT_INVALID;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_NONE;

    memcpy(block->ioctlWork, path, len + 1);

    return IOS_OpenAsync((const char*)block->ioctlWork, mode, _isfsFuncCb, block);
}

s32 ISFS_GetFileStats(s32 fd, FSFileStats* statsOut) {
    if (statsOut == 0 || (u32)statsOut % 32 != 0) {
        return IPC_RESULT_INVALID;
    }

    return IOS_Ioctl(fd, FS_IOCTL_GET_FILE_STATS, NULL, 0, statsOut, sizeof(FSFileStats));
}

s32 ISFS_Seek(s32 fd, s32 offset, IPCSeekMode mode) { return IOS_Seek(fd, offset, mode); }

s32 ISFS_SeekAsync(s32 fd, s32 offset, IPCSeekMode mode, FSAsyncCallback callback, void* callbackArg) {
    FSCommandBlock* block;

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_NONE;

    return IOS_SeekAsync(fd, offset, mode, _isfsFuncCb, block);
}

s32 ISFS_Read(s32 fd, void* dst, s32 len) {
    if (dst == NULL || (u32)dst % 32 != 0) {
        return IPC_RESULT_INVALID;
    }

    return IOS_Read(fd, dst, len);
}

s32 ISFS_ReadAsync(s32 fd, void* dst, s32 len, FSAsyncCallback callback, void* callbackArg) {
    FSCommandBlock* block;

    if (dst == NULL || (u32)dst % 32 != 0) {
        return IPC_RESULT_INVALID;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_NONE;

    return IOS_ReadAsync(fd, dst, len, _isfsFuncCb, block);
}

s32 ISFS_Write(s32 fd, const void* src, s32 len) {
    if (src == NULL || (u32)src % 32 != 0) {
        return IPC_RESULT_INVALID;
    }

    return IOS_Write(fd, src, len);
}

s32 ISFS_WriteAsync(s32 fd, const void* src, s32 len, FSAsyncCallback callback, void* callbackArg) {
    FSCommandBlock* block;

    if (src == NULL || (u32)src % 32 != 0) {
        return IPC_RESULT_INVALID;
    }

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_NONE;

    return IOS_WriteAsync(fd, src, len, _isfsFuncCb, block);
}

s32 ISFS_Close(s32 fd) { return IOS_Close(fd); }

s32 ISFS_CloseAsync(s32 fd, FSAsyncCallback callback, void* callbackArg) {
    FSCommandBlock* block;

    block = (FSCommandBlock*)iosAllocAligned(hId, sizeof(FSCommandBlock), 32);
    if (block == NULL) {
        return IPC_RESULT_BUSY;
    }

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->callbackState = CB_STATE_NONE;

    return IOS_CloseAsync(fd, _isfsFuncCb, block);
}
