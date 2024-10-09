#include "emulator/disk.h"
#include "emulator/cpu.h"
#include "emulator/system.h"
#include "emulator/vc64_RVL.h"
#include "macros.h"

_XL_OBJECTTYPE gClassDD = {
#if IS_OOT
    "DD",
#elif IS_MM
    "DISK",
#endif
    sizeof(Disk),
    NULL,
    (EventFunc)diskEvent,
};

bool diskPutROM8(Disk* pDisk, u32 nAddress, s8* pData) {
#if IS_OOT
    return false;
#elif IS_MM
    return true;
#endif
}

bool diskPutROM16(Disk* pDisk, u32 nAddress, s16* pData) {
#if IS_OOT
    return false;
#elif IS_MM
    return true;
#endif
}

bool diskPutROM32(Disk* pDisk, u32 nAddress, s32* pData) {
#if IS_OOT
    return false;
#elif IS_MM
    return true;
#endif
}

bool diskPutROM64(Disk* pDisk, u32 nAddress, s64* pData) {
#if IS_OOT
    return false;
#elif IS_MM
    return true;
#endif
}

bool diskGetROM8(Disk* pDisk, u32 nAddress, s8* pData) {
    *pData = 0;
    return true;
}

bool diskGetROM16(Disk* pDisk, u32 nAddress, s16* pData) {
    *pData = 0;
    return true;
}

bool diskGetROM32(Disk* pDisk, u32 nAddress, s32* pData) {
    *pData = 0;
    return true;
}

bool diskGetROM64(Disk* pDisk, u32 nAddress, s64* pData) {
    *pData = 0;
    return true;
}

bool diskPutDrive8(Disk* pDisk, u32 nAddress, s8* pData) {
#if IS_OOT
    return false;
#elif IS_MM
    return true;
#endif
}

bool diskPutDrive16(Disk* pDisk, u32 nAddress, s16* pData) {
#if IS_OOT
    return false;
#elif IS_MM
    return true;
#endif
}

bool diskPutDrive32(Disk* pDisk, u32 nAddress, s32* pData) {
    nAddress &= 0x00FFFFFF;

    switch (nAddress) {
        case 0x508:
            return false;
        case 0x510:
        default:
            break;
    }

    return true;
}

bool diskPutDrive64(Disk* pDisk, u32 nAddress, s64* pData) {
#if IS_OOT
    return false;
#elif IS_MM
    return true;
#endif
}

bool diskGetDrive8(Disk* pDisk, u32 nAddress, s8* pData) { return true; }

bool diskGetDrive16(Disk* pDisk, u32 nAddress, s16* pData) { return true; }

bool diskGetDrive32(Disk* pDisk, u32 nAddress, s32* pData) {
    nAddress &= 0x00FFFFFF;

    switch (nAddress) {
        case 0x508:
            *pData = 0;
            break;
        case 0x510:
            return false;
        default:
            break;
    }

    return true;
}

bool diskGetDrive64(Disk* pDisk, u32 nAddress, s64* pData) { return true; }

#if IS_OOT
bool diskGetBlock(void* pObject, CpuBlock* pBlock) {
    if (pBlock->pfUnknown != NULL) {
        if (!pBlock->pfUnknown(pBlock, 1)) {
            return false;
        }
    }

    return true;
}
#endif

bool diskEvent(Disk* pDisk, s32 nEvent, void* pArgument) {
    switch (nEvent) {
        case 2:
#if IS_MM
            pDisk->pHost = pArgument;
#endif
            break;
        case 0x1002:
            switch (((CpuDevice*)pArgument)->nType) {
                case 0:
#if IS_OOT
                    if (!cpuSetGetBlock(SYSTEM_CPU(gpSystem), pArgument, (GetBlockFunc)diskGetBlock)) {
                        return false;
                    }
#endif

                    if (!cpuSetDevicePut(SYSTEM_CPU(SYSTEM_PTR(pDisk)), pArgument, (Put8Func)diskPutDrive8,
                                         (Put16Func)diskPutDrive16, (Put32Func)diskPutDrive32,
                                         (Put64Func)diskPutDrive64)) {
                        return false;
                    }

                    if (!cpuSetDeviceGet(SYSTEM_CPU(SYSTEM_PTR(pDisk)), pArgument, (Get8Func)diskGetDrive8,
                                         (Get16Func)diskGetDrive16, (Get32Func)diskGetDrive32,
                                         (Get64Func)diskGetDrive64)) {
                        return false;
                    }
                    break;
                case 1:
                    if (!cpuSetDevicePut(SYSTEM_CPU(SYSTEM_PTR(pDisk)), pArgument, (Put8Func)diskPutROM8,
                                         (Put16Func)diskPutROM16, (Put32Func)diskPutROM32, (Put64Func)diskPutROM64)) {
                        return false;
                    }

                    if (!cpuSetDeviceGet(SYSTEM_CPU(SYSTEM_PTR(pDisk)), pArgument, (Get8Func)diskGetROM8,
                                         (Get16Func)diskGetROM16, (Get32Func)diskGetROM32, (Get64Func)diskGetROM64)) {
                        return false;
                    }
                    break;
            }
            break;
        case 0:
        case 1:
        case 3:
            break;
        case 0x1003:
#if IS_OOT
        case 0x1004:
        case 0x1007:
#endif
            break;
        default:
            return false;
    }

    return true;
}
