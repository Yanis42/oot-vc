#include "revolution/gx.h"
#include "revolution/os.h"
#include "string.h"

static GXFifoObj DisplayListFifo;
static GXData __savedGXdata;
static GXFifoObj OldCPUFifo;

void GXBeginDisplayList(void* list, u32 size) {
    GXFifoObjImpl* impl = (GXFifoObjImpl*)&DisplayListFifo;

    if (gx->dirtyState != 0) {
        __GXSetDirtyState();
    }

    if (gx->dlSaveContext) {
        memcpy(&__savedGXdata, gx, sizeof(GXData));
    }

    impl->base = list;
    impl->end = (u8*)list + size - 4;
    impl->size = size;
    impl->count = 0;
    impl->readPtr = list;
    impl->writePtr = list;

    gx->inDispList = true;

    GXGetCPUFifo(&OldCPUFifo);
    GXSetCPUFifo(&DisplayListFifo);
    GXResetWriteGatherPipe();
}

u32 GXEndDisplayList(void) {
    u8 wrap;
    bool enabled;
    u32 ctrl;

    GXGetCPUFifo(&DisplayListFifo);
    wrap = GXGetFifoWrap(&DisplayListFifo);
    GXSetCPUFifo(&OldCPUFifo);

    if (gx->dlSaveContext) {
        enabled = OSDisableInterrupts();

        ctrl = gx->cpEnable;
        memcpy(gx, &__savedGXdata, sizeof(GXData));
        gx->cpEnable = ctrl;

        OSRestoreInterrupts(enabled);
    }

    gx->inDispList = false;

    if (!wrap) {
        return GXGetFifoCount(&DisplayListFifo);
    }

    return 0;
}

void GXCallDisplayList(void* list, u32 size) {
    if (gx->dirtyState != 0) {
        __GXSetDirtyState();
    }

    if (gx->vNumNot == 0) {
        __GXSendFlushPrim();
    }

    WGPIPE.c = GX_FIFO_CMD_CALL_DL;
    WGPIPE.p = list;
    WGPIPE.i = size;
}
