#include "hb_sdk.h"
#include "lvgl/lvgl.h"
#include <stdint.h>
#include <stdbool.h>

#define PATCH_ADDR 0x082B2BD6u
#define PATCH_SIZE 28

/*
 * Original code at 0x082B2BD6:
 *
 * vmov      s2,r12
 * vldr.32   s0,[r1,#4]
 * vcvt.f64  d0,s0
 * vcvt.f64  d1,s2
 * vmla.f64  d0,d8,d1
 * vcvt.u32  s0,d0
 * vstr.32   s0,[r1,#4]
 */
static const uint8_t original_bytes[PATCH_SIZE] = {
    0x01,0xEE,0x10,0xCA,
    0x91,0xED,0x01,0x0A,
    0xB8,0xEE,0x40,0x0B,
    0xB8,0xEE,0x41,0x1B,
    0x08,0xEE,0x01,0x0B,
    0xBC,0xEE,0xC0,0x0B,
    0x81,0xED,0x01,0x0A
};

/*
 * Replacement:
 *
 * ldr     r0, [r1,#4]
 * add.w   r0, r0, r12, lsl #7
 * str     r0, [r1,#4]
 *
 * timestamp += frame_count * 128
 *
 * Remaining bytes are Thumb NOPs.
 */
static const uint8_t patched_bytes[PATCH_SIZE] = {
    0x48,0x68,
    0x00,0xEB,0xCC,0x10,
    0x48,0x60,

    0x00,0xBF,
    0x00,0xBF,
    0x00,0xBF,
    0x00,0xBF,
    0x00,0xBF,
    0x00,0xBF,
    0x00,0xBF,
    0x00,0xBF,
    0x00,0xBF,
    0x00,0xBF
};

static bool bytes_match(const uint8_t *expected)
{
    volatile const uint8_t *p =
        (volatile const uint8_t *)PATCH_ADDR;

    for (int i = 0; i < PATCH_SIZE; i++) {
        if (p[i] != expected[i])
            return false;
    }

    return true;
}

static bool apply_patch(void)
{
    if (bytes_match(patched_bytes))
        return true;

    /*
     * Safety check:
     * refuse to write unless the firmware bytes are exactly
     * what we expect.
     */
    if (!bytes_match(original_bytes))
        return false;

    volatile uint8_t *p =
        (volatile uint8_t *)PATCH_ADDR;

    for (int i = 0; i < PATCH_SIZE; i++)
        p[i] = patched_bytes[i];

    hb_icache_invalidate(PATCH_ADDR, PATCH_SIZE);

    return bytes_match(patched_bytes);
}

HB_APP_ENTRY(payload_entry)
{
    hb_trace_init();

    lv_obj_t *scr = lv_screen_active();

    lv_obj_set_style_bg_color(
        scr, lv_color_hex(0x05060A), 0);
    lv_obj_set_style_bg_opa(
        scr, LV_OPA_COVER, 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "NanoBtFix");
    lv_obj_set_style_text_color(
        title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(
        title, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t *status = lv_label_create(scr);

    if (bytes_match(patched_bytes)) {
        lv_label_set_text(
            status,
            "ALREADY PATCHED\nRTP TS = 128/frame");
    }
    else if (bytes_match(original_bytes)) {
        if (apply_patch()) {
            lv_label_set_text(
                status,
                "PATCHED\nRTP TS = 128/frame");
        }
        else {
            lv_label_set_text(
                status,
                "WRITE FAILED");
        }
    }
    else {
        lv_label_set_text(
            status,
            "MISMATCH\nPatch not applied");
    }

    lv_obj_set_style_text_color(
        status, lv_color_hex(0xB0B0B0), 0);

    lv_obj_set_style_text_align(
        status, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_align(
        status, LV_ALIGN_CENTER, 0, 25);
}
