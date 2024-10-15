#include "emulator/xlFile.h"
#include "emulator/xlFileRVL.h"
#include "emulator/xlHeap.h"
#include "revolution/os.h"

bool xlFileGetSize(s32* pnSize, char* szFileName) {
    tXL_FILE* pFile;

    if (xlFileOpen(&pFile, XLFT_BINARY, szFileName)) {
        if (pnSize != NULL) {
            *pnSize = pFile->nSize;
        }

        if (!xlFileClose(&pFile)) {
            SAFE_FAILED("xlFile.c", 65);
            return false;
        }

        return true;
    }

    return false;
}

bool xlFileLoad(char* szFileName, void** pTarget) {
    s32 pnSize;
    tXL_FILE* pFile;

    if (xlFileGetSize(&pnSize, szFileName)) {
        if (!xlHeapTake(pTarget, pnSize | 0x30000000)) {
            SAFE_FAILED("xlFile.c", 93);
            return false;
        }

        if (!xlFileOpen(&pFile, XLFT_BINARY, szFileName)) {
            SAFE_FAILED("xlFile.c", 98);
            return false;
        }

        if (!xlFileGet(pFile, *pTarget, pnSize)) {
            SAFE_FAILED("xlFile.c", 99);
            return false;
        }

        if (!xlFileClose(&pFile)) {
            SAFE_FAILED("xlFile.c", 100);
            return false;
        }

        return true;
    }

    return false;
}
