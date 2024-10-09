#include "emulator/codeRVL.h"
#include "macros.h"

_XL_OBJECTTYPE gClassCode = {
    "Code",
    sizeof(Code),
    NULL,
    (EventFunc)codeEvent,
};

#if IS_MM
static void* gpBufferFunction;
static u32* ganDataCode;

bool fn_80007844(void) { return true; }

bool fn_8000784C(void) { return true; }
#endif

bool codeEvent(Code* pCode, s32 nEvent, void* pArgument) {
    switch (nEvent) {
        case 2:
#if IS_MM
            gpBufferFunction = NULL;
            ganDataCode = NULL;
#endif
            break;
        case 0:
        case 1:
        case 3:
        case 5:
        case 6:
            break;
        default:
            return false;
    }

    return true;
}
