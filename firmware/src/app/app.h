/* SPDX-License-Identifier: GPL-3.0-only */
/* DUB-1 app: the project (everything that is saved) and the app's entry points. Included by the unity
 * build after gfx.c (drawing helpers in scope) and by the host simulator. */
#pragma once
#include <stdint.h>
#include "plat.h"
#include "../dsp/amb.h"

#ifndef EN_BUILD
#define EN_BUILD "host"
#endif
#ifndef EN_VERSION                       /* a release build passes its own (tools/build.py --release) */
#define EN_VERSION "DEV"
#endif
#define PROJ_MAGIC 0x31425544u           /* "DUB1": AMB-1's projects ("ENO1") give the recipes */
#define PROJ_WAVE_MARK 0xB1u           /* (format 30, the LFO pages' layout B) proj.rsv[0] in a save whose LFOs
                                         * have a Wave (P_CLWAVE, P_NLWAVE: 0..100); a format 30 save without it is
                                         * the step 2 test builds' (zzo, zzp), those slots their Shape (0..4) */
#define PROJ_FORMAT 33u                  /* 33: a Set's Transition (proj.trans, in the room at the end: the layout
                                         * 32's; 32's read, LP);
                                         * 32: the variety plan's step 4: no dead steps: Stabs the count (1..6);
                                         * Hits', the bass's Density's and the noise's Density's last step faded
                                         * in (the bass's from its first note alone to all); the hats', the pings'
                                         * and the noise's Tone, the noise's Decay, the echo's Tone and the Pad's
                                         * Attack to their ends; 31's (and older) as they sounded (en_zones31);
                                         * 31: the variety plan's step 3: Motion the arrangement only (P_MOTION:
                                         * Breathes 0-39, the parts ridden out and back and the kick out a bar or
                                         * two; Sections 40-69, Thin and Groove; Breaks 70-100), the Mixer's moves
                                         * folded in (P_MIXER retired, unread); 30's (and older) Motion and Mixer
                                         * as Motion, as they sounded (en_motion30); 8-30 read;
                                         * 30: the variety plan's step 2: Drift, Form's KNOB 4 (P_MOVE: the LFOs,
                                         * the slow arc, the echo's slow moves, the mixer's rides), each LFO's Trim
                                         * on it (P_CLTRIM, P_NLTRIM; their Depth and Drift unread); (layout B) each
                                         * LFO's Wave, the wave's share against the wander, in its Shape's slots
                                         * (P_CLWAVE, P_NLWAVE, 0..100; the wave a sine);
                                         * the throws on the chord's Vary (P_TVARY: the amount they're set at); the
                                         * Mixer its moves alone, on no knob; from the old knobs as they sounded
                                         * (en_move29); 8-29 read, and step 2's test builds' 30 (no PROJ_WAVE_MARK: their
                                         * Shape a Wave, Wander 0, the others 50);
                                         * 29: the variety plan's step 1: Evolve is Form's Vary (P_VARY, its
                                         * value kept), the layers' Varys trims on it (P_CVARY .. P_NVARY, -50..+50:
                                         * the old factor's amount as Vary + trim, 0 Hold), the piece's looseness
                                         * its own (P_LOOSE, from the recipes; Evolve's before); 8-28 read;
                                         * 28: Robert's: each strip's Low cut a centred Filter (P_CLOW .. P_NLOW,
                                         * P_KLOW, -50..+50: left a low-pass, right a high-pass; 27's 0..100
                                         * converted to the high-pass's same cutoff), five Japanese scales (P_MODE's
                                         * range), the tempo from 40 BPM; 27 read;
                                         * 27: the virtual dub mixer (P_MIXER, each piece's; 8-26 read, it
                                         * from the recipes);
                                         * 26: song structure (P_SMODE, the player's, global as Tune: Endless;
                                         * each piece's Intro, Main, Outro and Kind, P_SINTRO .. P_SKIND; 8-25 read,
                                         * they from the recipes);
                                         * 25: the noise LFO's Destination (P_NLDEST: Filter, Tone, Air; 8-24 read,
                                         * it from the recipes: Filter, as it was);
                                         * 24: the kick and the hats apart (six parts): the Kick strip's level,
                                         * sends and Low cut (P_KLVL .. P_KLOW; 8-23 read, they from the recipes:
                                         * x1, none, off; a 23's drums Low cut the kick's too, as it was);
                                         * 23: the Arrangement page's channel strips: each part's Echo and Plate
                                         * sends, its Low cut (P_CECHO .. P_NLOW; 8-22 read, they from the recipes:
                                         * the sends as each piece sent, the Low cuts off);
                                         * 22: the layers' pages: their Vary (P_PVARY, P_BVARY, P_DVARY, P_NVARY),
                                         * the chord's Cutoff a knob around the piece's own (P_CCUT; 8-21 read, the
                                         * Varys from the recipes, a Cutoff you'd set its offset from the recipe's);
                                         * 21: the Chord's pages: its Vary, its LFO's Destination (P_CVARY,
                                         * P_CLDEST; 8-20 read, they from the recipes);
                                         * 20: the LFOs' waves (their Shape, now P_CLWAVE, P_NLWAVE; 8-19 read, they Sine);
                                         * 19: the chord's and the noise's LFO rates (P_CLRATE, P_NLRATE; 8-18 read,
                                         * they from the recipes, the noise's LFO depth converted: its scale shallower);
                                         * 18: the noise's Density and Air 0..100, 0 none (8-17 read, converted);
                                         * 17: the kick's rumble (P_RUMBLE; 8-16 read, it from the recipes);
                                         * 16: the noise's LFO (P_NLFO; 8-15 read, it from the recipes);
                                         * 15: the Noise page's second (Width, Plate, Echo) and the hats' length
                                         * (P_NWIDTH..P_HLEN; 8-14 read, the new ones from the recipes);
                                         * 14: the chord's Ghost, the kick's density, the echo's Blur, Tone and
                                         * Width, the shimmer's Tune (P_CGHOST..P_NTUNE; 8-13 read, the new ones from the
                                         * recipes);
                                         * 13: Throws, Ghost, Notes, Hats level (P_CTHROW..P_HATSLVL), the groove five
                                         * patterns again with Notes (12's ladder converted; 8-12 read, the new ones
                                         * from the recipes);
                                         * 12: the groove a ladder (0 Drone .. 8 Roller; 8-11 read, converted);
                                         * 11: the noise's five (P_NCOL..P_NCHAR; 8-10 read, the noise from the recipes);
                                         * 10: the chords' slots as yours, the walk's or gaps (9's and 8's read, an empty
                                         * slot a gap); 9: each piece's ping loops (8, DUB-1 1.0.0's, read and kept;
                                         * older projects give the recipes) */
#define NPIECES EN_NPIECES

typedef struct {
    int16_t par[P_NPARAMS];
    uint16_t pool;
    uint8_t pings[EN_PINGS_N];           /* its ping loops as they were left (en_pings_get) */
    uint8_t pings_ok;                    /* 0: none kept (a recipe, or a project from 1.0.0): grown afresh */
    uint8_t screen;                      /* its screen view: 0 ripples, 1 scope, 2 spectrum, 5 tape (3, 4: 1.4.0's echo trail and fifths, gone: ripples; before 1.2.0: 0) */
} slot_t;

typedef struct {
    uint32_t magic, format;
    int16_t par[P_NPARAMS];              /* the piece playing, as it is now (amb.h P_*) */
    uint16_t pool;                       /* its scale, relative to the root (bit 0 = the root) */
    uint8_t leds;                        /* lights: 0 off, 1 keys and the buttons' glow, 2 keys only */
    uint8_t piece;                       /* which piece */
    uint8_t screen;                      /* the Dub page's middle: 0 ripples, 1 scope, 2 spectrum, 5 tape (ui_screen_ok) */
    uint8_t values;                      /* (1.4.0) the knobs' numbers: 0 shown a moment as a page opens, 1 always
                                          * (was rsv0, 0 in older saves: Peek) */
    slot_t slot[NPIECES];                /* every piece as it was left (Tune and MIDI out are global) */
    uint8_t rsv[15];                     /* room to grow (rsv[0]: PROJ_WAVE_MARK) */
    uint8_t trans;                       /* (format 33, the user's) a Set's Transition, the Song mode page's KNOB 2:
                                          * EN_TR_* (0 LP, 1 Segue, 2 Mix); global as Song mode (was rsv[15], 0 in
                                          * older saves: LP) */
} project_t;

extern project_t proj;
/* (1.4.0, the variety plan's step 3: RAM) one scratch the UI's thread lends out in turn, never two at once: the boot's
 * project_load (a saved project read and converted, once, before the UI runs) and the spectrum's FFT (draw_spec: a
 * frame's work, nothing kept from one frame to the next). Before, each its own (3,760 + 4,096 B) */
typedef union {
    project_t load;
    float fft[2][EN_SPEC_N];
} app_scratch_t;
extern app_scratch_t app_scratch;

void project_defaults(void);
int project_load(void);                  /* 0 = loaded; else defaults are in place */
int project_save(void);                  /* 0 = saved and verified */
void project_apply(void);                /* every value to the engine (boot, load) */
const char *piece_name(int i);
void project_piece(int i);               /* the playing piece kept, piece i taken up as it was left */
void project_piece_reset(void);          /* the playing piece back to its recipe */

void ui_init(void);
void ui_frame(void);
void ui_input_only(void);
int ui_dirty(void);
void ui_say(const char *a, const char *b);
#define UI_NSCREEN 6                     /* (1.4.0) the screen views' stored range: 0 ripples, 1 scope, 2 spectrum, 5 tape (3, 4,
                                          * the echo trail and the fifths, gone: the user's; the values kept, so a save
                                          * needs no format step) */
static inline uint8_t ui_screen_ok(uint32_t v) { return v < 3u || v == 5u ? (uint8_t)v : 0u; }   /* (gone: ripples) */
#define UI_SPLASH_N 26                   /* the launch screen's frames (~1.4 s at 55 ms) */
void ui_splash(int f);                   /* frame f of the launch screen (the last: as it stays) */
