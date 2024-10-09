#include "emulator/xlPostRVL.h"
#include "emulator/frame.h"
#include "emulator/system.h"
#include "emulator/vc64_RVL.h"
#include "macros.h"

#if IS_OOT
bool xlPostSetup(void) { return true; }

bool xlPostReset(void) { return true; }

#elif IS_MM

typedef struct struct_8017B1E0 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ void* unk_04[5];
} struct_8017B1E0; // size = 0x18

typedef struct struct_80200864 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ u8 unk_08[0x10];
    /* 0x18 */ s32 unk_18;
} struct_80200864; // size = 0x1C

extern struct_8017B1E0 lbl_8017B1E0;
extern s32 lbl_80200654;
u32 lbl_801FF7DC = 2;
extern struct_80200864* lbl_80200864;
extern s32 lbl_80201700;
extern s32 lbl_80201704;

void fn_80087394(void) {
    struct_80200864* temp_r31;
    u16 temp_r3;
    u16 temp_r4;

    temp_r31 = lbl_80200864;

    if (temp_r31->unk_18 != 0) {
        fn_800A4EDC(0x43300000);
        temp_r4 = temp_r31->unk_04;
        fn_800ADF8C(temp_r4, lbl_80201704, lbl_80201704, (f32)temp_r4, (f32)temp_r31->unk_06, lbl_80201704,
                    lbl_80201700);
    } else {
        temp_r3 = temp_r31->unk_04;
        fn_800ADFC8(temp_r3, lbl_80201704, lbl_80201704, (f32)temp_r3, (f32)temp_r31->unk_06, lbl_80201704,
                    lbl_80201700);
    }

    fn_800A9EE8();
    fn_800AC600();
    fn_800AA544(0);
}

void fn_8008745C(void) {
    SYSTEM_FRAME(gpSystem)->nMode = 0;
    SYSTEM_FRAME(gpSystem)->nModeVtx = -1;
    frameDrawReset(SYSTEM_FRAME(gpSystem), 0x5FFED);

    GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    GXSetColorUpdate(GX_ENABLE);
    GXCopyDisp(lbl_8017B1E0.unk_04[lbl_80200654 * 2], GX_TRUE);
    GXDrawDone();
    fn_800A42A4(lbl_80200864);
    fn_800A4DF0(lbl_8017B1E0.unk_04[lbl_80200654 * 2]);
    VIFlush();
    VIWaitForRetrace();

    lbl_80200654 = (lbl_80200654 + 1) % lbl_801FF7DC;
}

s32 fn_80087534(void) {
    u32 temp_r3;

    fn_80086DDC();
    fn_800A4DF0(lbl_8017B1E0.unk_04[lbl_80200654 * 2]);
    temp_r3 = lbl_80200654 + 1;
    lbl_80200654 = temp_r3;

    if (temp_r3 >= lbl_801FF7DC) {
        lbl_80200654 = 0;
    }

    fn_800A4CDC();
    fn_800A3D24();

    if (lbl_80200864->unk_00 & 1) {
        fn_800A3D24();
    }

    fn_800A42A4(lbl_80200864);
    return true;
}

bool xlPostText(const char* fmt, const char* file, s32 line, ...) { return true; }

#endif
