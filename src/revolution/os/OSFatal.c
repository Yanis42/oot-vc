#include "mem_funcs.h"
#include "revolution/base.h"
#include "revolution/exi.h"
#include "revolution/gx.h"
#include "revolution/os.h"
#include "revolution/vi.h"
#include "string.h"

/**
 * Framebuffer formatted as 32-bits per two pixels
 * Pixels are written as brightness/chroma pairs
 */
#define FATAL_FB_W 640
#define FATAL_FB_H 480
#define FATAL_FB_SIZE (FATAL_FB_W * FATAL_FB_H * 2)

typedef struct OSFatalParam {
    /* 0x0 */ GXColor textColor;
    /* 0x4 */ GXColor bgColor;
    /* 0x8 */ const char* msg;
} OSFatalParam;

static OSContext FatalContext;
static OSFatalParam FatalParam;

static void Halt(void);

static void ScreenClear(u8* fb, u16 width, u16 height, GXColor yuv) {
    int x, y;

    for (y = 0; y < height; y += 1) {
        for (x = 0; x < width; x += 2) {
            fb[0] = yuv.r;
            fb[1] = yuv.g;

            fb[2] = yuv.r;
            fb[3] = yuv.b;

            fb += sizeof(u32);
        }
    }
}

static void ScreenReport(void* fb, u16 width, u16 height, GXColor msgYUV, s32 x, s32 y, u32 lf, const char* msg) {
    u32 texel[70];
    u32 charWidth;
    u8* tmp;
    u8 bright;
    s32 xPos;
    u32 idx;
    u32 idx2;
    u32 idx3;
    u32 i;
    u32 j;

new_line:
    if (height - 24 < y) {
        return;
    }

    tmp = (u8*)fb + ((x + (y * width)) * 2);
    xPos = x;

    while (*msg != '\0') {
        if (*msg == '\n') {
            msg++;
            y += lf;
            goto new_line;
        } else if (width - 48 < xPos) {
            y += lf;
            goto new_line;
        }

        for (i = 0; i < 24; i++) {
            idx = (i & 7) + ((i / 8) * 24);
            texel[idx] = 0;
            texel[idx + 8] = 0;
            texel[idx + 16] = 0;
        }

        msg = OSGetFontTexel(msg, texel, 0, 6, &charWidth);

        for (i = 0; i < 24; i++) {
            idx = (i & 7) + ((i / 8) * 24);

            for (j = 0; j < 24; j++) {
                idx2 = idx + ((j / 8) * 8);
                bright = (texel[idx2] >> ((7 - (j & 7)) * 4)) & 0x0F;

                if (bright != 0) {
                    // Y component/brightness
                    bright = (((msgYUV.r * (bright * 0xEF)) / 255) / 15) + 16;
                    idx3 = (j + (width * i));
                    tmp[idx3 * 2] = bright;

                    // Order chroma depending on pixel pair
                    if ((xPos + j) % 2 != 0) {
                        tmp[(idx3 * 2) - 1] = msgYUV.g;
                        tmp[(idx3 * 2) + 1] = msgYUV.b;
                    } else {
                        tmp[(idx3 * 2) - 1] = msgYUV.b;
                        tmp[(idx3 * 2) + 1] = msgYUV.g;
                    }
                }
            }
        }

        tmp += charWidth * 2;
        xPos += charWidth;
    }
}

static void ConfigureVideo(u16 width, u16 height) {
    GXRenderModeObj rmode;

    rmode.fbWidth = width;
    rmode.efbHeight = FATAL_FB_H;
    rmode.xfbHeight = height;

    rmode.viXOrigin = 40;
    rmode.viWidth = FATAL_FB_W;
    rmode.viHeight = height;

    switch (VIGetTvFormat()) {
        case VI_TV_FMT_NTSC:
        case VI_TV_FMT_MPAL:
            if (VI_HW_REGS[VI_VICLK] & VI_VICLK_SPEED /* == VI_VICLK_54MHZ */) {
                // Progressive mode
                rmode.viTVmode = VI_TV_INFO(VI_TV_FMT_NTSC, VI_SCAN_MODE_PROG);
                rmode.viYOrigin = 0;
                rmode.xfbMode = VI_XFB_MODE_SF;
            } else {
                // Non-progressive mode
                rmode.viTVmode = VI_TV_INFO(VI_TV_FMT_NTSC, VI_SCAN_MODE_INT);
                rmode.viYOrigin = 0;
                rmode.xfbMode = VI_XFB_MODE_DF;
            }
            break;
        case VI_TV_FMT_EURGB60:
            rmode.viTVmode = VI_TV_INFO(VI_TV_FMT_EURGB60, VI_SCAN_MODE_INT);
            rmode.viYOrigin = 0;
            rmode.xfbMode = VI_XFB_MODE_DF;
            break;
        case VI_TV_FMT_PAL:
            rmode.viTVmode = VI_TV_INFO(VI_TV_FMT_PAL, VI_SCAN_MODE_INT);
            rmode.viYOrigin = 47;
            rmode.xfbMode = VI_XFB_MODE_DF;
            break;
    }

    VIConfigure(&rmode);
    VIConfigurePan(0, 0, FATAL_FB_W, FATAL_FB_H);
}

// Intel IPP RGBToYCbCr algorithm (+0.5f?)
static GXColor RGB2YUV(GXColor rgb) {
    GXColor yuv;

    // clang-format off
    f32 y =  ( 0.257f * rgb.r + 0.504f * rgb.g + 0.098f * rgb.b + 16.0f)  + 0.5f;
    f32 cb = (-0.148f * rgb.r - 0.291f * rgb.g + 0.439f * rgb.b + 128.0f) + 0.5f;
    f32 cr = ( 0.439f * rgb.r - 0.368f * rgb.g - 0.071f * rgb.b + 128.0f) + 0.5f;
    // clang-format on

    yuv.r = CLAMP(16.0f, 235.0f, y);
    yuv.g = CLAMP(16.0f, 240.0f, cb);
    yuv.b = CLAMP(16.0f, 240.0f, cr);
    yuv.a = 0;

    return yuv;
}

void OSFatal(GXColor textColor, GXColor bgColor, const char* msg) {
    s32 retraceCount;
    s64 start;
    OSBootInfo* bootInfo;

    bootInfo = (OSBootInfo*)OSPhysicalToCached(OS_PHYS_BOOT_INFO);

    OSDisableInterrupts();

    OSDisableScheduler();
    OSClearContext(&FatalContext);
    OSSetCurrentContext(&FatalContext);
    __OSStopAudioSystem();

    VIInit();
    __OSUnmaskInterrupts(OS_INTR_MASK(OS_INTR_PI_VI));
    VISetBlack(true);
    VIFlush();
    VISetPreRetraceCallback(NULL);
    VISetPostRetraceCallback(NULL);

    OSEnableInterrupts();

    retraceCount = VIGetRetraceCount();
    while ((s32)VIGetRetraceCount() - retraceCount < 1) {}

    start = OSGetTime();
    do {
        if (__OSCallShutdownFunctions(0, 0)) {
            break;
        }
    } while (OSGetTime() - start < OS_MSEC_TO_TICKS(1000));

    OSDisableInterrupts();
    __OSCallShutdownFunctions(1, 0);

    EXISetExiCallback(EXI_CHAN_0, NULL);
    EXISetExiCallback(EXI_CHAN_2, NULL);
    while (!EXILock(EXI_CHAN_0, EXI_DEV_INT, NULL)) {
        EXISync(EXI_CHAN_0);
        EXIDeselect(EXI_CHAN_0);
        EXIUnlock(EXI_CHAN_0);
    }
    EXIUnlock(EXI_CHAN_0);

    while ((EXI_CHAN_PARAMS[EXI_CHAN_0].cr & EXI_CR_TSTART) == 1) {
        ;
    }

    __OSSetExceptionHandler(OS_ERR_DECREMENTER, OSDefaultExceptionHandler);

    GXAbortFrame();

    OSSetArenaLo((void*)0x81400000);
    OSSetArenaHi(bootInfo->fstStart);

    FatalParam.textColor = textColor;
    FatalParam.bgColor = bgColor;
    FatalParam.msg = msg;
    OSSwitchFiber(Halt, OSGetArenaHi());
}

static inline GXColor RGB2YUV2(GXColor rgb) {
    f32 Y;
    f32 Cb;
    f32 Cr;
    GXColor yuv;

    Y = 0.5f + (16.0f + ((0.098f * (f32)rgb.b) + ((0.257f * (f32)rgb.r) + (0.504f * (f32)rgb.g))));
    Cb = 0.5f + (128.0f + ((0.439f * (f32)rgb.b) + ((-0.148f * (f32)rgb.r) - (0.291f * (f32)rgb.g))));
    Cr = 0.5f + (128.0f + (((0.439f * (f32)rgb.r) - (0.368f * (f32)rgb.g)) - (0.071f * (f32)rgb.b)));

    yuv.r = (Y > 235.0f) ? 235.0f : (Y < 16.0f) ? 16.0f : Y;
    yuv.g = (Cb > 240.0f) ? 240.0f : (Cb < 16.0f) ? 16.0f : Cb;
    yuv.b = (Cr > 240.0f) ? 240.0f : (Cr < 16.0f) ? 16.0f : Cr;
    yuv.a = 0;

    return yuv;
}

static void Halt(void) {
    OSFontHeader* msgFont;
    const char* msg;
    s32 retraceCount;
    size_t msgLen;
    void* msgBuf;
    void* fb;
    OSFatalParam* params;

    OSEnableInterrupts();

    params = &FatalParam;
    msg = params->msg;

    msgLen = strlen(msg) + 1;
    msgBuf = OSAllocFromMEM1ArenaLo(msgLen, 32);
    params->msg = memmove(msgBuf, msg, msgLen);

    msgFont = (OSFontHeader*)OSAllocFromMEM1ArenaLo(0xA1004, 32);
    OSLoadFont(msgFont, OSGetArenaLo());

    fb = OSAllocFromMEM1ArenaLo(FATAL_FB_SIZE, 32);
    ScreenClear(fb, FATAL_FB_W, FATAL_FB_H, RGB2YUV2(params->bgColor));
    VISetNextFrameBuffer(fb);
    ConfigureVideo(FATAL_FB_W, FATAL_FB_H);
    VIFlush();

    retraceCount = VIGetRetraceCount();
    while ((s32)VIGetRetraceCount() - retraceCount < 2) {}

    ScreenReport(fb, FATAL_FB_W, FATAL_FB_H, RGB2YUV2(params->textColor), 48, 100, msgFont->leading, params->msg);
    DCFlushRange(fb, FATAL_FB_SIZE);
    VISetBlack(false);
    VIFlush();

    retraceCount = VIGetRetraceCount();
    while ((s32)VIGetRetraceCount() - retraceCount < 1) {}

    OSDisableInterrupts();
    OSReport("%s\n", params->msg);
    PPCHalt();
}
