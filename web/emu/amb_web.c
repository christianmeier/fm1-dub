/* SPDX-License-Identifier: GPL-3.0-only */
/* DUB-1 in the browser: the host simulator (host/amb_host.c: the whole app, UI and instrument,
 * against a simulated FM-1) compiled to WebAssembly and run in an AudioWorklet (from X0X's). The worklet asks
 * for audio; producing it advances the device's clock one millisecond at a time, which runs the
 * engine every 256 samples (the I2S half buffer) and the UI every 16 ms, as on the FM-1. The page
 * sends input, and reads the screen, the lights and the saved objects back through these exports.
 *
 *   web/emu/build.sh   ->  build/emu/amb.wasm (+ the page) */
#include <stdint.h>

static void web_audio(const int32_t *blk, uint32_t n);
#include "../../host/amb_host.c"

#undef __attribute__

/* output ring, stereo float, filled by run_ms, drained by web_render */
#define RING 4096u
static float ring[RING * 2];
static uint32_t ring_w, ring_r;
static void web_audio(const int32_t *blk, uint32_t n)
{
    uint32_t k;
    for (k = 0; k < n; k++) {
        uint32_t i = (ring_w++ % RING) * 2u;
        ring[i] = (float)blk[2 * k] / 8388608.0f;
        ring[i + 1] = (float)blk[2 * k + 1] / 8388608.0f;
    }
}

static float out_l[1024], out_r[1024];
__attribute__((used, visibility("default"))) float *web_out_l(void) { return out_l; }
__attribute__((used, visibility("default"))) float *web_out_r(void) { return out_r; }

/* n frames (<= 1024) into out_l / out_r: the device runs until they exist */
__attribute__((used, visibility("default"))) void web_render(uint32_t n)
{
    uint32_t k;
    if (n > 1024u)
        n = 1024u;
    while (ring_w - ring_r < n)
        run_ms(1);
    for (k = 0; k < n; k++) {
        uint32_t i = (ring_r++ % RING) * 2u;
        out_l[k] = ring[i];
        out_r[k] = ring[i + 1];
    }
}

/* a screen reader (fm1-reader): the page writes a SysEx frame (the bytes between F0 and F7) to web_sysex_buf and
 * calls web_sysex(n); what the firmware says back, whole F0..F7 frames, from web_said_buf (web_said_len bytes;
 * web_said_clear when read) */
static uint8_t said_buf[4096], sx_in[64];
static uint32_t said_n;
static void web_said(const uint8_t *p, uint32_t n)
{
    if (said_n + n <= sizeof said_buf) {
        memcpy(said_buf + said_n, p, n);
        said_n += n;
    }
}
__attribute__((used, visibility("default"))) uint8_t *web_said_buf(void) { return said_buf; }
__attribute__((used, visibility("default"))) uint32_t web_said_len(void) { return said_n; }
__attribute__((used, visibility("default"))) void web_said_clear(void) { said_n = 0; }
__attribute__((used, visibility("default"))) uint8_t *web_sysex_buf(void) { return sx_in; }
__attribute__((used, visibility("default"))) void web_sysex(uint32_t n) { ann_frame(sx_in, n > sizeof sx_in ? sizeof sx_in : n, now_ms); }

__attribute__((used, visibility("default"))) void web_boot(void) { boot(); }
__attribute__((used, visibility("default"))) void web_buttons(uint32_t m) { held_btn = m; }
__attribute__((used, visibility("default"))) void web_keys(uint32_t m) { held_keys = m; }
__attribute__((used, visibility("default"))) void web_enc(int role, int32_t n)
{
    if (role >= 0 && role < NE)
        enc_acc[role] += n;
}
__attribute__((used, visibility("default"))) void web_master(uint32_t v) { master = v > 4096u ? 4096u : v; }
__attribute__((used, visibility("default"))) uint32_t web_lit_buttons(void) { return lit_btn; }
__attribute__((used, visibility("default"))) uint32_t web_lit_keys(void) { return lit_keys; }
__attribute__((used, visibility("default"))) uint16_t *web_fb(void) { return fb; }
__attribute__((used, visibility("default"))) uint32_t web_blits(void) { return blits; }
__attribute__((used, visibility("default"))) uint32_t web_now(void) { return now_ms; }
__attribute__((used, visibility("default"))) uint32_t web_cpu(void) { return cpu_pct; }

/* saved objects: the page keeps them (localStorage) and gives them back before boot */
__attribute__((used, visibility("default"))) uint8_t *web_store(uint32_t obj) { return obj < OBJ_NOBJ ? store[obj] : 0; }
__attribute__((used, visibility("default"))) int web_store_len(uint32_t obj) { return obj < OBJ_NOBJ ? store_len[obj] : -1; }
__attribute__((used, visibility("default"))) void web_store_set_len(uint32_t obj, int n)
{
    if (obj < OBJ_NOBJ)
        store_len[obj] = n < 0 ? -1 : n > (int)PLAT_STORE_MAX ? (int)PLAT_STORE_MAX : n;
}
__attribute__((used, visibility("default"))) uint32_t web_store_writes(void) { return store_writes; }
__attribute__((used, visibility("default"))) uint32_t web_nobj(void) { return OBJ_NOBJ; }
__attribute__((used, visibility("default"))) uint32_t web_store_max(void) { return PLAT_STORE_MAX; }

#if BOARD_SMK37
/* The SMK-37 build (web/emu/build.sh smk37 -> build/emu-smk37/): its own controls, as host/amb_host.c's script
 * drives them. The keybed: 37 keys, C3 = bit 0 (lo: keys 0..31, hi: keys 32..36); F3..G5 (keys 5..31) are also
 * the FM-1's 27 keys. The pads: held, bit p = pad p (0..7 the top row); each pad keeps the key it pressed in the
 * layout the UI asked for until it is let go, as the HAL does. */
static uint32_t web_pad_mask;
__attribute__((used, visibility("default"))) void web_keybed(uint32_t lo, uint32_t hi)
{
    uint64_t m = (uint64_t)lo | (uint64_t)(hi & 0x1Fu) << 32;
    kb_hits |= m & ~kb_held;
    kb_held = m;
    held_keys = (uint32_t)(m >> 5) & ((1u << NKEYS) - 1u);
}
__attribute__((used, visibility("default"))) void web_pads(uint32_t m)
{
    uint32_t pd, on = m & ~web_pad_mask, off = web_pad_mask & ~m;
    web_pad_mask = m & 0xFFFFu;
    for (pd = 0; pd < SMK_NPADS; pd++) {
        if (off >> pd & 1u) {
            if (pad_key_of[pd])
                pad_keys &= ~(1u << (pad_key_of[pd] - 1));
            pad_key_of[pd] = 0;
        }
        if (on >> pd & 1u) {
            if (pad_layout == SMK_PADS_OWN) {
                pad_hits |= 1u << pd;
                pad_key_of[pd] = 0;
            } else {
                pad_key_of[pd] = 1 + WHITE_K[pad_layout == SMK_PADS_GRID ? pd : pd < 8 ? pd + 8 : pd - 8];
                pad_keys |= 1u << (pad_key_of[pd] - 1);
            }
        }
    }
}
__attribute__((used, visibility("default"))) void web_smk_buttons(uint32_t m) { smk_btn_held = m & ((1u << NSB) - 1u); }
__attribute__((used, visibility("default"))) void web_fader(int i, int v)
{
    if ((uint32_t)i < 4u)
        fader_v[i] = v < 0 ? 0 : v > 127 ? 127 : v;
}
__attribute__((used, visibility("default"))) void web_wheels(int pitch, int mod)
{
    wheel_p = pitch < -127 ? -127 : pitch > 127 ? 127 : pitch;
    wheel_m = mod < 0 ? 0 : mod > 127 ? 127 : mod;
    wheel_new = 1;
}
__attribute__((used, visibility("default"))) uint8_t *web_pad_rgb(void) { return &pad_rgb[0][0]; }   /* 16 x r g b */
__attribute__((used, visibility("default"))) uint32_t web_smk_lit(void) { return smk_lit; }
__attribute__((used, visibility("default"))) int web_pad_layout(void) { return pad_layout; }
#endif
