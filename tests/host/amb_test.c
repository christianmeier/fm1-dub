/* SPDX-License-Identifier: GPL-3.0-only */
/* The instrument on the host: the pure parts (Euclid, scales, chords, voicings, grooves, names, texts) and
 * the render (sound, no NaN, bounded; the kick on the floor, the chord drifts and holds, never on a
 * diminished fifth, freeze holds, the arrangement moves with Motion, throws, the loops evolve). */
#define _POSIX_C_SOURCE 200809L                /* (fork, pipe: the LFOs' test, one engine run two ways) */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>
#include "amb.h"

static int fails;
#define CHECK(c, ...)                                                                                              \
    do {                                                                                                           \
        if (!(c)) {                                                                                                \
            printf("FAIL %s:%d: ", __FILE__, __LINE__);                                                            \
            printf(__VA_ARGS__);                                                                                   \
            printf("\n");                                                                                          \
            fails++;                                                                                               \
        }                                                                                                          \
    } while (0)

/* MIDI out: counted; the kick's note-ons (36 on channel 10) by the bar they fall in, and each bar's
 * section (as a note-on from the grid sees it; 0xFF: no note that bar) */
static uint32_t midi_count, kicks, kick_off_beat, drum_notes37, claps;
static int bass_lo = 999, bass_hi = -1, bass_pcs;  /* the bass's note-ons (channel 3): its range, its pitch classes */
#define KBARS 512
static uint8_t kick_bar[KBARS], bar_sec[KBARS];
void en_midi_out(uint32_t st, uint32_t d1, uint32_t d2)
{
    uint32_t b = en_bars / 16u;
    midi_count++;
    if ((st & 0xF0u) == 0x90u && d2 > 0u && b < KBARS)
        bar_sec[b] = en_section;
    if (st == 0x92u && d2 > 0u) {
        bass_lo = (int)d1 < bass_lo ? (int)d1 : bass_lo;
        bass_hi = (int)d1 > bass_hi ? (int)d1 : bass_hi;
        bass_pcs |= 1 << (d1 % 12u);
    }
    if (st == 0x99u && d1 == 37u && d2 > 0u)
        drum_notes37++;
    if (st == 0x99u && d1 == 39u && d2 > 0u)
        claps++;
    if (st == 0x99u && d1 == 36u && d2 > 0u) {
        kicks++;
        kick_off_beat += (en_bars & 3u) != 0u;
        if (b < KBARS)
            kick_bar[b]++;
    }
}

static uint32_t pat(const char *s)
{
    uint32_t m = 0;
    int i;
    for (i = 0; s[i]; i++)
        if (s[i] == 'x')
            m |= 1u << i;
    return m;
}

static int popcount(uint32_t m)
{
    int c = 0;
    while (m) {
        c += (int)(m & 1u);
        m >>= 1;
    }
    return c;
}

static int32_t buf[2 * 256];
static float peak, sum2;
static int bad;
static void run(float secs)
{
    int blocks = (int)(secs * 44100.0f / 256.0f), b, j;
    for (b = 0; b < blocks; b++) {
        en_render(buf, 256, 2048);
        for (j = 0; j < 512; j++) {
            float x = (float)buf[j] / 8388608.0f;
            if (x != x)
                bad++;
            if (fabsf(x) > peak)
                peak = fabsf(x);
            sum2 += x * x;
        }
    }
}

/* (1.4.0) the LFOs: a sixteenth at a time, as the LFO pages' trace has it (en_lfo_tr), n of them from now: the
 * chord's (k 0) and the noise's (1); and when the chord's drift took a new place (picks, the sixteenth's index) */
#define LFO_REC 4800
static int8_t lfo_v[2][LFO_REC];
static int lfo_pick[64], lfo_npick;
static void lfo_rec(int n)
{
    uint32_t w = en_lfo_w, p = en_lfo_picks[0];
    int i = 0;
    lfo_npick = 0;
    while (i < n) {
        en_render(buf, 256, 2048);
        if (en_lfo_w != w) {
            w = en_lfo_w;
            lfo_v[0][i] = en_lfo_tr[0][(w - 1u) % EN_LFO_N];
            lfo_v[1][i] = en_lfo_tr[1][(w - 1u) % EN_LFO_N];
            if (en_lfo_picks[0] != p && lfo_npick < 64)
                lfo_pick[lfo_npick++] = i;
            p = en_lfo_picks[0];
            i++;
        }
    }
}
/* one engine, from here two ways (a fork): as it is, and with parameter p set to v; the same sound either way */
static uint32_t out_hash(float secs)
{
    int blocks = (int)(secs * 44100.0f / 256.0f), b, j;
    uint32_t h = 2166136261u;
    for (b = 0; b < blocks; b++) {
        en_render(buf, 256, 2048);
        for (j = 0; j < 512; j++)
            h = (h ^ (uint32_t)buf[j]) * 16777619u;
    }
    return h;
}
static int same_with(int p, int v)
{
    int fd[2];
    pid_t pid;
    uint32_t h0, h1 = 0;
    fflush(stdout);
    if (pipe(fd))
        return -1;
    pid = fork();
    if (pid == 0) {
        en_set(p, v);
        h1 = out_hash(12.0f);
        if (write(fd[1], &h1, sizeof h1) != (ssize_t)sizeof h1)
            _exit(1);
        _exit(0);
    }
    en_set(p, en_param_info(p)->lo);             /* (a command here too: the same drain) */
    h0 = out_hash(12.0f);
    if (read(fd[0], &h1, sizeof h1) != (ssize_t)sizeof h1)
        h1 = ~h0;
    waitpid(pid, 0, 0);
    close(fd[0]);
    close(fd[1]);
    return h0 == h1;
}

/* a new engine, stopped, the count from the top (en_init leaves the transport, en_bars, as they were) */
/* (1.4.0) the kick's rumble alone: one engine, from here two ways (a fork), the rumble at 0 and at v, the same
 * otherwise; the difference's mid (m), its side's energy, and each sample's sixteenth within its beat (q: 0 on the
 * beat, where the kick lands) */
#define RB_TN (6 * 44100)
static float rb_m[RB_TN];
static uint8_t rb_q[RB_TN];
static double rb_side;
static void rumble_ab(int v)
{
    static float alt[2 * RB_TN];
    int fd[2], b, j, i = 0;
    pid_t pid;
    fflush(stdout);
    if (pipe(fd))
        return;
    pid = fork();
    if (pid == 0) {                              /* (the rumble at v: its output, through the pipe) */
        en_set(P_RUMBLE, v);
        for (b = 0; b < RB_TN / 256; b++) {
            en_render(buf, 256, 2048);
            for (j = 0; j < 256; j++, i++)
                alt[2 * i] = (float)buf[2 * j] / 8388608.0f, alt[2 * i + 1] = (float)buf[2 * j + 1] / 8388608.0f;
        }
        for (j = 0; j < (int)sizeof alt; j += (int)write(fd[1], (char *)alt + j, sizeof alt - (size_t)j))
            ;
        _exit(0);
    }
    en_set(P_RUMBLE, 0);
    rb_side = 0.0;
    for (b = 0; b < RB_TN / 256; b++) {
        en_render(buf, 256, 2048);
        for (j = 0; j < 256; j++, i++) {
            alt[2 * i] = (float)buf[2 * j] / 8388608.0f, alt[2 * i + 1] = (float)buf[2 * j + 1] / 8388608.0f;
            rb_q[i] = (uint8_t)((en_bars + 3u) % 4u);   /* (en_bars: the step coming, one past the playing one) */
        }
    }
    {
        static float other[2 * RB_TN];
        size_t got = 0;
        ssize_t r;
        while (got < sizeof other && (r = read(fd[0], (char *)other + got, sizeof other - got)) > 0)
            got += (size_t)r;
        for (i = 0; i < RB_TN; i++) {
            const float dl = other[2 * i] - alt[2 * i], dr = other[2 * i + 1] - alt[2 * i + 1];
            rb_m[i] = 0.5f * (dl + dr);
            rb_side += (double)(0.5f * (dl - dr)) * (double)(0.5f * (dl - dr));
        }
    }
    waitpid(pid, 0, 0);
    close(fd[0]);
    close(fd[1]);
}
static int chord_root_pc(void) { return en_chord_root % 12; }

static void fresh(void)
{
    en_play(0);
    run(0.01f);
    en_init();
    en_bars = 0;
    bad = 0;
    memset(kick_bar, 0, sizeof kick_bar);
    memset(bar_sec, 0xFF, sizeof bar_sec);
}

static uint32_t chord_sig(void)
{
    return (uint32_t)en_chord[0] | (uint32_t)en_chord[1] << 7 | (uint32_t)en_chord[2] << 14 |
           (uint32_t)en_chord[3] << 21;
}

/* the chord playing, by its root: on the degree whose pitch class it is, its tones and name; 0 if none */
static int chord_now(uint32_t pl, int root, int *pcs, char *name)
{
    int st[12], m = en_scale(pl, st), d, n;
    for (d = 0; d < m; d++)
        if ((root + st[d]) % 12 == en_chord_root) {
            n = en_chord_tones(pl, root, d, pcs);
            en_chord_name(pcs, n, name);
            return n;
        }
    return 0;
}

int main(void)
{
    int pcs[4], n, i;
    char name[16];

    /* Euclid: the classic patterns */
    CHECK(en_euclid(3, 8, 0) == pat("x..x..x."), "E(3,8) %x", en_euclid(3, 8, 0));
    CHECK(en_euclid(5, 8, 0) == pat("x.xx.xx."), "E(5,8) %x", en_euclid(5, 8, 0));
    CHECK(en_euclid(4, 16, 0) == pat("x...x...x...x..."), "E(4,16)");
    CHECK(en_euclid(0, 16, 0) == 0, "E(0,16)");
    CHECK(en_euclid(7, 7, 0) == 0x7Fu, "E(7,7)");
    for (i = 0; i <= 12; i++)
        CHECK(popcount(en_euclid(i, 12, 5)) == i, "E(%d,12) has %d hits", i, popcount(en_euclid(i, 12, 5)));
    CHECK(en_euclid(1, 7, 3) == 1u << 3, "rotation");

    /* scales and chords */
    {
        int st[12];
        CHECK(en_scale(EN_MODE_POOL[MODE_DORIAN], st) == 7 && st[2] == 3 && st[6] == 10, "Dorian steps");
        CHECK(en_scale(EN_MODE_POOL[MODE_PENT_MIN], st) == 5, "pentatonic minor has 5");
        CHECK(en_scale(0, st) == 1, "the root is always in");
    }
    n = en_chord_tones(EN_MODE_POOL[MODE_IONIAN], 0, 0, pcs);
    CHECK(n == 4 && pcs[0] == 0 && pcs[1] == 4 && pcs[2] == 7 && pcs[3] == 11, "C Ionian I = Cmaj7");
    CHECK(!strcmp(en_chord_name(pcs, n, name), "CM7"), "name %s", name);
    n = en_chord_tones(EN_MODE_POOL[MODE_DORIAN], 2, 0, pcs);
    CHECK(!strcmp(en_chord_name(pcs, n, name), "Dm7"), "D Dorian i: %s", name);
    n = en_chord_tones(EN_MODE_POOL[MODE_DORIAN], 2, 3, pcs);
    CHECK(!strcmp(en_chord_name(pcs, n, name), "G7"), "D Dorian IV: %s", name);
    n = en_chord_tones(EN_MODE_POOL[MODE_IONIAN], 0, 6, pcs);
    CHECK(!strcmp(en_chord_name(pcs, n, name), "Bm7b5"), "C Ionian vii: %s", name);
    n = en_chord_tones(EN_MODE_POOL[MODE_PENT_MAJ], 0, 0, pcs);
    CHECK(n == 3, "pentatonic chords have three tones (%d)", n);
    n = en_chord_tones(1u | 1u << 4 | 1u << 7 | 1u << 9, 0, 0, pcs);   /* four notes: no doubled tones */
    CHECK(n == 3 && pcs[0] != pcs[1] && pcs[1] != pcs[2] && pcs[0] != pcs[2], "4-note pool: distinct");
    n = en_chord_tones(1u | 1u << 4 | 1u << 7, 0, 1, pcs);
    CHECK(n == 3, "3-note pool: all three (%d)", n);

    /* voicings: four rising notes from the root, within two octaves, no pitch class twice, in the pool */
    {
        int iv[4], m, d, s, st[12];
        en_voicing(EN_MODE_POOL[MODE_DORIAN], 0, 0, iv);
        CHECK(iv[0] == 0 && iv[1] == 7 && iv[2] == 10 && iv[3] == 15, "Dorian i open: %d %d %d %d", iv[0], iv[1],
              iv[2], iv[3]);
        en_voicing(EN_MODE_POOL[MODE_DORIAN], 0, 1, iv);
        CHECK(iv[0] == 0 && iv[1] == 10 && iv[2] == 14 && iv[3] == 15, "Dorian i 9th: %d %d %d %d",
              iv[0], iv[1], iv[2], iv[3]);
        for (m = 0; m < EN_NMODES; m++) {
            int ns = en_scale(EN_MODE_POOL[m], st);
            for (d = 0; d < ns; d++)
                for (s = 0; s < 2; s++) {
                    int k, j, ok = 1;
                    en_voicing(EN_MODE_POOL[m], d, s, iv);
                    ok &= iv[0] == 0 && iv[3] <= 26;
                    for (k = 1; k < 4; k++)
                        ok &= iv[k] > iv[k - 1];
                    for (k = 0; k < 4; k++) {
                        for (j = 0; j < k; j++)
                            ok &= iv[k] % 12 != iv[j] % 12;
                        ok &= EN_MODE_POOL[m] >> ((st[d] + iv[k]) % 12) & 1u;
                    }
                    CHECK(ok, "%s degree %d shape %d: %d %d %d %d", EN_MODE_NAME[m], d, s, iv[0], iv[1], iv[2], iv[3]);
                }
        }
    }

    /* grooves, drift */
    CHECK(en_groove(0) == 1u, "Drone: the downbeat");
    for (i = 0; i < 5; i++)
        CHECK(en_groove(i) & 0xFFFFu && !(en_groove(i) >> 16), "groove %d: %x", i, en_groove(i));
    CHECK(en_groove(1) == 0x4444u, "Offbeat: %x", en_groove(1));
    CHECK(en_drift_bars(0) == 2 && en_drift_bars(5) == 64 && en_drift_bars(6) == 0, "drift bars");

    /* texts */
    {
        char t[12];
        static const char *const GRV[5] = {"Drone", "Offbeat", "Dotted", "Sync", "Roller"};   /* (the Groove knob, sparse
                                                                                              * to busy; Notes thins it) */
        en_param_text(P_DRIFT, 6, t);
        CHECK(!strcmp(t, "Hold"), "drift 6: %s", t);
        en_param_text(P_DRIFT, 4, t);
        CHECK(!strcmp(t, "32 bars"), "drift 4: %s", t);
        en_param_text(P_DRIFT, 0, t);
        CHECK(!strcmp(t, "2 bars"), "drift 0: %s", t);
        en_param_text(P_ETIME, 1, t);
        CHECK(!strcmp(t, "1/8."), "etime 1: %s", t);
        en_param_text(P_ETIME, 5, t);
        CHECK(!strcmp(t, "1 bar"), "etime 5: %s", t);
        for (i = 0; i < 5; i++) {
            en_param_text(P_GROOVE, i, t);
            CHECK(!strcmp(t, GRV[i]), "groove %d: %s", i, t);
        }
        en_param_text(P_CVOICE, 0, t);
        CHECK(!strcmp(t, "Saws"), "ctone 0: %s", t);
        en_param_text(P_CVOICE, 1, t);
        CHECK(!strcmp(t, "Keys"), "ctone 1: %s", t);
        en_param_text(P_SWING, 0, t);
        CHECK(!strcmp(t, "50%"), "swing 0: %s", t);
        en_param_text(P_BASSOCT, 1, t);
        CHECK(!strcmp(t, "+1"), "bass oct: %s", t);
        en_param_text(P_MODE, MODE_DORIAN, t);
        CHECK(!strcmp(t, "Dorian"), "mode: %s", t);
        for (i = 0; i < P_NPARAMS; i++) {
            const en_param_t *pi = en_param_info(i);
            CHECK(pi->lo <= pi->def && pi->def <= pi->hi, "%s: default %d out of %d..%d", pi->name, pi->def, pi->lo,
                  pi->hi);
            en_param_text(i, pi->hi, t);
            CHECK(strlen(t) < 10, "%s: text too long", pi->name);
        }
    }

    /* the render: silent at rest, sound when playing, never NaN, bounded */
    en_init();
    CHECK(en_drum_pat[0] == 0x1111u, "four on the floor: %x", en_drum_pat[0]);
    en_set(P_CRACKLE, 0);
    run(1.0f);
    CHECK(peak == 0.0f, "silent before play (%f)", (double)peak);
    en_set(P_CRACKLE, en_param_info(P_CRACKLE)->def);
    en_play(1);
    peak = sum2 = 0.0f;
    run(20.0f);
    CHECK(!bad, "NaN in the output");
    CHECK(peak > 0.1f && peak < 0.95f, "peak %f", (double)peak);
    printf("  20 s playing: peak %.3f, rms %.4f, bars %u, kicks %u\n", (double)peak,
           (double)sqrtf(sum2 / (20.0f * 44100.0f * 2.0f)), en_bars / 16u, kicks);
    CHECK(en_bars / 16u >= 9u, "the grid ran (%u sixteenths)", en_bars);

    /* the kick: four on the floor, on the beats, while Full (Motion 0: always Full; a fill halves a phrase's
     * last bar) */
    {
        uint32_t b, full = 0, two = 0, other = 0;
        fresh();
        en_set(P_MOTION, 0);
        kicks = kick_off_beat = 0;
        en_play(1);
        run(200.0f);
        for (b = 0; b + 1 < en_bars / 16u && b < KBARS; b++) {
            if (kick_bar[b] == 4)
                full++;
            else if (kick_bar[b] == 2 && (b & 7u) == 7u)
                two++;
            else
                other++;
        }
        CHECK(!kick_off_beat, "%u kicks off the beat", kick_off_beat);
        CHECK(!other && full > 80u, "kick bars: %u of four, %u fills, %u else", full, two, other);
        printf("  200 s at Motion 0: %u bars of four kicks, %u fills\n", full, two);
    }

    /* the chord drifts: at 2 bars, it moves every 4 s or so; never onto a diminished fifth; in the pool */
    {
        static const int MODES[2] = {MODE_IONIAN, MODE_DORIAN};
        int mi;
        for (mi = 0; mi < 2; mi++) {
            uint32_t pl = EN_MODE_POOL[MODES[mi]], last;
            int changes = 0, dim = 0, out = 0, k, seen = 0;
            fresh();
            en_set(P_ROOT, 0);
            en_pool(pl);
            en_set(P_DRIFT, 0);
            en_play(1);
            run(0.1f);
            last = chord_sig();
            for (k = 0; k < 4 * 180; k++) {
                int t[4], nt;
                uint32_t s;
                run(0.25f);
                s = chord_sig();
                changes += s != last;
                last = s;
                nt = chord_now(pl, 0, t, name);
                CHECK(nt, "%s: chord root %d off the scale", EN_MODE_NAME[MODES[mi]], en_chord_root);
                if (nt) {
                    seen |= 1 << en_chord_root;
                    dim += strstr(name, "b5") || strstr(name, "dim");
                }
                for (i = 0; i < EN_NPAD; i++) {
                    out += !(pl >> (en_chord[i] % 12) & 1u);
                    dim += en_chord[i] % 12 == (en_chord_root + 6) % 12;
                }
            }
            CHECK(changes >= 30, "%s: %d chord changes in 3 min at 2 bars", EN_MODE_NAME[MODES[mi]], changes);
            CHECK(popcount((uint32_t)seen) >= 3, "%s: only %d roots", EN_MODE_NAME[MODES[mi]], popcount((uint32_t)seen));
            CHECK(!dim, "%s: %d times on a diminished fifth", EN_MODE_NAME[MODES[mi]], dim);
            CHECK(!out, "%s: %d chord notes off the pool", EN_MODE_NAME[MODES[mi]], out);
            printf("  %s, 3 min at 2 bars: %d chord changes, %d roots\n", EN_MODE_NAME[MODES[mi]], changes,
                   popcount((uint32_t)seen));
        }
    }

    /* Drift Hold: one chord */
    {
        uint32_t held;
        int k, moved = 0;
        fresh();
        en_set(P_DRIFT, 6);
        en_play(1);
        run(0.1f);
        held = chord_sig();
        for (k = 0; k < 120; k++) {
            run(1.0f);
            moved += chord_sig() != held;
        }
        CHECK(!moved && en_drift == 0.0f, "Hold: the chord moved (%d s)", moved);
        en_next();                               /* but Next still moves it, on the bar */
        run(2.5f);
        CHECK(chord_sig() != held, "Next while holding: no change");
    }

    /* freeze (dub hold): the chord holds, though Drift is at its shortest; then moves again */
    {
        uint32_t held;
        int k, moved = 0;
        en_set(P_DRIFT, 0);
        run(5.0f);
        en_freeze(1);
        run(0.1f);
        CHECK(en_frozen, "not frozen");
        held = chord_sig();
        for (k = 0; k < 60; k++) {
            run(1.0f);
            moved += chord_sig() != held;
        }
        CHECK(!moved, "the chord moved while frozen (%d s)", moved);
        en_freeze(0);
        for (k = 0, moved = 0; k < 10; k++) {   /* (it may move and come home again: any move counts) */
            run(1.0f);
            moved += chord_sig() != held;
        }
        CHECK(moved, "the chord stayed after the freeze");
    }

    /* Evolve 0, Motion 0: a strict loop, every stab every bar */
    {
        uint32_t first = 0, t0;
        int k, diff = 0;
        fresh();
        en_set(P_EVOLVE, 0);
        en_set(P_MOTION, 0);
        en_set(P_DRIFT, 6);
        en_regrow();
        en_play(1);
        run(2.5f);
        first = en_stab_pat;
        t0 = en_throws;
        for (k = 0; k < 60; k++) {
            run(2.0f);
            diff += en_stab_pat != first;
        }
        CHECK(first && !diff && en_throws == t0, "Evolve 0: the stabs changed (%d of 60, %u throws)", diff, en_throws - t0);
    }

    /* Throws: 0 none (Motion 0: no fills), 100 well over 50 (the same draws) */
    {
        uint32_t n[3];
        int k;
        static const int TV[3] = {0, 50, 100};
        for (k = 0; k < 3; k++) {
            fresh();
            en_seed(7);
            en_set(P_EVOLVE, 100);
            en_set(P_MOTION, 0);
            en_set(P_STABS, 100);
            en_set(P_CTHROW, TV[k]);
            en_regrow();
            en_play(1);
            n[k] = en_throws;
            run(60.0f);
            n[k] = en_throws - n[k];
        }
        printf("  throws in a minute at Throws 0 / 50 / 100: %u / %u / %u\n", n[0], n[1], n[2]);
        CHECK(n[0] == 0 && n[1] > 0 && n[2] * 2u > n[1] * 3u, "Throws: %u / %u / %u", n[0], n[1], n[2]);
    }

    /* the kick's shapes (1.4.0: a switch, Four, Ghost, Half, One): Four (100) four on the floor in every bar it plays;
     * Half (50), mostly half-time, with Evolve's wander some of the others */
    {
        int k, b, four[2] = {0, 0}, half[2] = {0, 0}, one[2] = {0, 0}, played[2] = {0, 0};
        static const int KD[2] = {100, 50};
        for (k = 0; k < 2; k++) {
            fresh();
            en_seed(11);
            en_set(P_KDENS, KD[k]);
            en_set(P_MOTION, 0);
            en_regrow();
            en_play(1);
            run(1.0f);
            for (b = 0; b < 240; b++) {
                uint16_t p;
                run(60.0f / 122.0f * 4.0f);      /* (a bar at 122) */
                p = en_lane[1];
                if (!p || b < 64)                /* (past the piece's opening on four: eight phrases) */
                    continue;
                played[k]++;
                four[k] += p == 0x1111u;
                half[k] += p == 0x0101u;
                one[k] += p == 0x0001u;
            }
        }
        printf("  the kick, Four: four in %d of %d bars; Half: four %d, half %d, one %d of %d\n", four[0], played[0],
               four[1], half[1], one[1], played[1]);
        CHECK(played[0] > 100 && four[0] == played[0], "Four: four in %d of %d", four[0], played[0]);
        CHECK(played[1] > 100 && half[1] + one[1] > played[1] / 5 && four[1] < played[1] * 7 / 10,   /* (past the
                                                  * opening; four here counts Ghost too, Half's neighbour) */
              "Half: four %d, half %d, one %d of %d", four[1], half[1], one[1], played[1]);
    }

    /* (1.4.0, format 18) the noise's Density: 0 no hits; turned, its hits from the next bar, not the next phrase */
    {
        int n0, n1;
        fresh();
        en_seed(5);
        en_set(P_NGRAIN, 50);
        en_regrow();
        en_play(1);
        run(60.0f / 122.0f * 4.0f * 3.0f + 0.1f);   /* (into the third bar of the first phrase) */
        en_set(P_NGRAIN, 100);
        run(60.0f / 122.0f * 4.0f * 1.05f);
        n1 = __builtin_popcount(en_nz_pat);
        en_set(P_NGRAIN, 0);
        run(60.0f / 122.0f * 4.0f * 1.05f);
        n0 = __builtin_popcount(en_nz_pat);
        printf("  the noise's Density: at 100, %d hits in two bars; at 0, %d (each a bar after the turn)\n", n1, n0);
        CHECK(n1 >= 14 && n0 == 0, "noise Density: 100 gave %d, 0 gave %d", n1, n0);
    }

    /* the drums' and the bass's phrase plan: A A A B A A A B, the A bars the same, the two B bars the same */
    {
        int ph_n = 0, ok = 0, b, k;
        uint16_t lane[8][3];
        fresh();
        en_seed(5);
        en_set(P_BNOTES, 75);
        en_set(P_MOTION, 0);                    /* (no Breaks: the bass and hats in every phrase) */
        en_regrow();
        en_play(1);
        run(2.0f);
        for (b = 0; b < 64 && ph_n < 4; b++) {
            int last = en_phrase_bar;
            for (k = 0; k < 4000 && en_phrase_bar == last; k++)
                run(0.006f);                     /* (a block at a time, to the bar's start) */
            run(0.2f);
            lane[en_phrase_bar][0] = en_lane[5], lane[en_phrase_bar][1] = en_lane[2], lane[en_phrase_bar][2] = en_lane[1];
            if (en_phrase_bar == 7) {            /* (a whole phrase seen) */
                int a = 1, j, q;
                for (q = 0; q < 3; q++) {
                    for (j = 1; j < 3; j++)
                        a &= lane[j][q] == lane[0][q];
                    for (j = 4; j < 7; j++)
                        a &= lane[j][q] == lane[0][q];
                    a &= q || lane[3][q] == lane[7][q];   /* (B: the bass; the drums' bar 8 may carry the fill) */
                }
                ok += a;
                ph_n++;
            }
        }
        printf("  the phrase plan: %d of %d phrases A A A B A A A B\n", ok, ph_n);
        CHECK(ph_n >= 3 && ok >= ph_n - 1, "the phrase plan: %d of %d", ok, ph_n);
    }

    /* the bass: Octave -1 a true octave under 0 (E0..Eb1); with Drone, Fifth gives whole bars of the fifth */
    {
        int lo0, hi0;
        fresh();
        en_set(P_DRIFT, 6);
        en_set(P_GROOVE, 4);                     /* (Roller, the knob's top) */
        en_set(P_FIFTH, 0);
        en_regrow();
        en_play(1);
        bass_lo = 999, bass_hi = -1;
        run(20.0f);
        lo0 = bass_lo, hi0 = bass_hi;
        en_set(P_BASSOCT, -1);
        bass_lo = 999, bass_hi = -1;
        run(20.0f);
        CHECK(lo0 >= 28 && hi0 <= 51 && bass_lo == lo0 - 12, "Octave -1: %d..%d, at 0 %d..%d", bass_lo, bass_hi, lo0, hi0);
        en_set(P_BASSOCT, 0);
        en_set(P_GROOVE, 0);
        en_set(P_FIFTH, 100);
        run(4.0f);
        bass_pcs = 0;
        run(20.0f);
        CHECK(bass_pcs == 1 << ((chord_root_pc() + 7) % 12), "Drone at Fifth 100: only the fifth (%x)", bass_pcs);
    }

    /* a piece's parameters all at once (a piece change, the boot): none of them, nor PLAY after them, dropped */
    {
        int16_t pp[P_NPARAMS];
        uint16_t pl;
        int k;
        fresh();
        en_piece(3, pp, &pl);
        for (k = 0; k < P_NPARAMS; k++)
            en_set(k, pp[k]);
        en_pool(pl);
        en_regrow();
        en_play(1);
        run(3.0f);
        CHECK(en_playing && en_bars > 16u && peak > 0.02f, "a piece's parameters, then PLAY: not playing (%u)", en_bars);
    }

    /* the safety valve (the classics pass): under load (en_load over 75, the FM-1's meter) the engine sheds: no new
     * noise hits, the width and the rumble eased off, tails shorter; the music goes on, nothing broken, and back */
    {
        int16_t pp[P_NPARAMS];
        uint16_t pl;
        int k;
        uint32_t w0, w1, w2;
        float p_shed;
        fresh();
        en_piece(7, pp, &pl);                    /* (Haze: Dust, the cloud, the widest load on the FM-1) */
        for (k = 0; k < P_NPARAMS; k++)
            en_set(k, pp[k]);
        en_pool(pl);
        en_regrow();
        en_play(1);
        run(10.0f);
        w0 = en_nev_w;
        en_load = 95;
        peak = 0.0f, bad = 0;
        run(10.0f);
        p_shed = peak;
        w1 = en_nev_w;
        en_load = 40;
        run(10.0f);
        w2 = en_nev_w;
        printf("  the valve: noise events %u in 10 s shedding, %u after; peak %.3f shedding\n", w1 - w0, w2 - w1, (double)p_shed);
        CHECK(bad == 0 && p_shed > 0.02f && w2 - w1 > w1 - w0, "the valve: bad %d, peak %.3f, events %u then %u",
              bad, (double)p_shed, w1 - w0, w2 - w1);
    }

    /* the chords' slots, Loop: Fm7, Bb, (a gap: Bb holds), C6, played in turn a change every two bars, round again
     * (the rest gaps: the loop is four long); the slot playing edited, heard at once; to Endless: the one playing up
     * to slot 1; the labels */
    {
        static const int WANT[6] = {5, 10, 0, 5, 10, 0};
        int k, seen[16], ns = 0, ok = 1, last = -1, holds = 0;
        char nm[16];
        fresh();
        en_set(P_ROOT, 5);
        en_pool(EN_MODE_POOL[MODE_DORIAN]);
        en_set(P_DRIFT, 0);
        en_set(P_CR0, 5), en_set(P_CQ0, EN_CQ_MINOR), en_set(P_CX0, EN_CX_7);
        en_set(P_CR1, 10), en_set(P_CQ1, EN_CQ_MAJOR);
        en_set(P_CR2, 0), en_set(P_CQ2, EN_CQ_EMPTY), en_set(P_CX2, 0);
        en_set(P_CR3, 0), en_set(P_CQ3, EN_CQ_MAJOR), en_set(P_CX3, EN_CX_6);
        for (k = 4; k < EN_NPROG; k++)
            en_set(P_CR0 + k, 0), en_set(P_CQ0 + k, EN_CQ_EMPTY), en_set(P_CX0 + k, 0);
        en_set(P_PMODE, EN_PLOOP);               /* (Loop) */
        en_regrow();
        en_play(1);
        for (k = 0; k < 900 && ns < 16; k++) {
            run(0.1f);
            if (en_chord_root != last) {
                last = en_chord_root;
                seen[ns++] = last;
            }
            holds += en_prog_pos == 2 && en_chord_root == 10;
        }
        for (k = 0; k < 6; k++)
            ok &= seen[k] == WANT[k];
        CHECK(ns >= 6 && ok, "the slots' roots: %d %d %d %d %d %d (%d changes)", seen[0], seen[1], seen[2], seen[3], seen[4],
              seen[5], ns);
        CHECK(holds > 0, "the empty slot didn't hold the chord before it");
        while (en_chord_slot != 0)
            run(0.1f);
        en_set(P_CR0, 7);                        /* the slot playing, edited: G minor 7 now */
        run(0.05f);
        CHECK(en_chord_root == 7, "the slot playing, edited: root %d", en_chord_root);
        en_set(P_PMODE, 1);                      /* to Endless (Wander, by hand: turned round): slot 1 plays, the gaps */
        en_prog_rotate();                        /* stay, the walk goes on */
        run(0.05f);
        CHECK(en_prog_pos == 0 && en_prog_slot[0][1] == EN_CQ_MINOR && en_prog_slot[0][0] == 7,
              "to Endless: slot 1 %d/%d/%d at %d", en_prog_slot[0][0], en_prog_slot[0][1], en_prog_slot[0][2], en_prog_pos);
        CHECK(en_prog_slot[2][1] == 0 && en_prog_slot[2][0] == 0, "to Endless: the gap filled");
        CHECK(!strcmp(en_chord_label(5, EN_CQ_MINOR, EN_CX_7, nm), "Fm7"), "label %s", nm);
        CHECK(!strcmp(en_chord_label(0, EN_CQ_SUS4, EN_CX_7, nm), "C7sus4"), "label %s", nm);
        CHECK(!strcmp(en_chord_label(10, EN_CQ_MAJOR, EN_CX_MAJ7, nm), "BbM7"), "label %s", nm);
        CHECK(!strcmp(en_chord_label(3, EN_CQ_EMPTY, 0, nm), "-"), "label %s", nm);
        {   /* Endless from a fresh piece: eight from the walk; on a change, all up one, the walk writes slot 8; a gap
             * stays a gap; the walk's follow the key */
            int j, before[EN_NPROG][3], walk = 1, up = 1;
            fresh();
            en_set(P_ROOT, 5);
            en_pool(EN_MODE_POOL[MODE_DORIAN]);
            en_set(P_DRIFT, 0);
            en_set(P_PMODE, 0);
            for (j = 0; j < EN_NPROG; j++)
                en_set(P_CR0 + j, 0), en_set(P_CQ0 + j, EN_CQ_EMPTY), en_set(P_CX0 + j, 0);
            en_regrow();
            run(0.05f);
            for (j = 0; j < EN_NPROG; j++)
                walk &= en_prog_slot[j][1] == 0 && en_prog_slot[j][0] >= 1 && en_prog_slot[j][0] <= 7;
            CHECK(walk && en_prog_slot[0][0] == 1, "Endless: a fresh piece's eight from the walk, from home (%d)", en_prog_slot[0][0]);
            en_set(P_CR3, 0), en_set(P_CQ3, EN_CQ_EMPTY);   /* a gap at slot 4 */
            en_set(P_CR5, 0), en_set(P_CQ5, EN_CQ_MAJOR), en_set(P_CX5, EN_CX_7);   /* a C7 written at slot 6 */
            run(0.05f);
            for (j = 0; j < EN_NPROG; j++)
                for (k = 0; k < 3; k++)
                    before[j][k] = en_prog_slot[j][k];
            en_play(1);
            {
                uint32_t g = en_prog_gen;
                for (k = 0; k < 200 && en_prog_gen == g; k++)
                    run(0.05f);
            }
            for (j = 0; j < EN_NPROG - 1; j++)
                for (k = 0; k < 3; k++)
                    up &= en_prog_slot[j][k] == before[j + 1][k];
            CHECK(up, "Endless: the slots didn't move up one");
            CHECK(en_prog_slot[2][0] == 0 && en_prog_slot[2][1] == 0, "Endless: the gap didn't stay a gap");
            CHECK(en_prog_slot[4][1] == EN_CQ_MAJOR && en_prog_slot[4][2] == EN_CX_7, "Endless: the written C7 moved");
            CHECK(en_prog_slot[7][1] == 0 && en_prog_slot[7][0] >= 1, "Endless: the walk didn't write slot 8");
            CHECK(en_prog_pos == 0, "Endless: slot %d plays", en_prog_pos);
            {   /* the walk's character turned by hand: its slots walked again (not the one playing); yours and the
                 * gaps kept */
                int diff = 0, keep = 1, p0[3];
                for (j = 0; j < 3; j++)
                    p0[j] = en_prog_slot[0][j];
                for (j = 0; j < EN_NPROG; j++)
                    for (k = 0; k < 3; k++)
                        before[j][k] = en_prog_slot[j][k];
                for (k = 0; k < 4 && !diff; k++) {   /* (a few tries: the walk may write the same by chance) */
                    en_set(P_PMODE, EN_WALK_FREE);
                    en_prog_rewalk();
                    run(0.02f);
                    for (j = 1; j < EN_NPROG; j++)
                        diff += before[j][1] == 0 && before[j][0] && en_prog_slot[j][0] != before[j][0];
                }
                for (j = 0; j < EN_NPROG; j++)
                    if (before[j][1] || !before[j][0])
                        keep &= en_prog_slot[j][0] == before[j][0] && en_prog_slot[j][1] == before[j][1];
                CHECK(diff > 0, "re-walk: the walk's slots didn't change");
                CHECK(keep, "re-walk: a written slot or a gap changed");
                CHECK(en_prog_slot[0][0] == p0[0] && en_prog_slot[0][1] == p0[1], "re-walk: the slot playing changed");
            }
        }
        {   /* the walk's character: Home, Wander, Free, each more varied (fewer A-B-A bounces, home less often) */
            int st, pp[3] = {0}, hm[3] = {0};
            for (st = 0; st < 3; st++) {
                int r[64], nr = 0, j, last = -1;
                fresh();
                en_set(P_ROOT, 5);
                en_pool(EN_MODE_POOL[MODE_DORIAN]);
                en_set(P_DRIFT, 0);
                en_set(P_PMODE, st);
                for (j = 0; j < EN_NPROG; j++)
                    en_set(P_CR0 + j, 0), en_set(P_CQ0 + j, EN_CQ_EMPTY), en_set(P_CX0 + j, 0);
                en_regrow();
                en_play(1);
                for (j = 0; j < 6000 && nr < 64; j++) {
                    run(0.05f);
                    if (en_chord_root != last || j == 0)
                        last = en_chord_root, r[nr++] = last;
                }
                for (j = 2; j < nr; j++)
                    pp[st] += r[j] == r[j - 2];
                for (j = 0; j < nr; j++)
                    hm[st] += r[j] == 5;
                printf("  the walk, %s: %d changes, %d bounces (A-B-A), %d at home\n", st == 0 ? "Home" : st == 1 ? "Wander" : "Free",
                       nr, pp[st], hm[st]);
            }
            CHECK(pp[1] < pp[0] && pp[2] < pp[0], "the walk: Wander (%d) and Free (%d) bounce less than Home (%d)", pp[1], pp[2], pp[0]);
            CHECK(hm[2] < hm[0], "the walk: Free (%d) at home less than Home (%d)", hm[2], hm[0]);
        }
        for (k = 0; k < EN_NCQ * EN_NCX; k++) {   /* every voicing: four rising notes, within two octaves */
            int iv[4];
            en_chord_iv(k / EN_NCX, k % EN_NCX, iv);
            ok = iv[0] == 0 && iv[1] > iv[0] && iv[2] > iv[1] && iv[3] > iv[2] && iv[3] <= 24;
            CHECK(ok, "voicing %d/%d: %d %d %d %d", k / EN_NCX, k % EN_NCX, iv[0], iv[1], iv[2], iv[3]);
        }
    }

    /* the drums' pattern: Play on Pattern plays the written steps only (the kick on 1 and 11 here), Chance 0 a
     * lane silent, the rim heard; Auto's kick is four on the floor */
    {
        int k, bars = 0, off = 0, ok_rim;
        fresh();
        en_set(P_MOTION, 0);
        en_set(P_DPAT0, (int)(int16_t)(uint16_t)0x0401u);   /* steps 1 and 11 */
        en_set(P_DPAT3, (int)(int16_t)(uint16_t)0x1010u);   /* the rim on 5 and 13 */
        en_set(P_DMODE, 1);
        en_regrow();
        en_play(1);
        kicks = kick_off_beat = 0;
        memset(kick_bar, 0, sizeof kick_bar);
        run(20.0f);
        for (k = 1; k < 9; k++)
            bars += kick_bar[k] == 2;
        CHECK(bars >= 7, "Pattern: two kicks a bar, %d of 8 bars", bars);
        (void)off;
        CHECK(en_lane[1] == 0x0401u && en_lane[3] == 0x1010u, "the lanes: kick %04x, rim %04x", en_lane[1], en_lane[3]);
        ok_rim = drum_notes37 > 10u;
        CHECK(ok_rim, "the rim: %u hits", drum_notes37);
        en_set(P_DCH0, 0);
        kicks = 0;
        run(10.0f);
        CHECK(kicks == 0u, "Chance 0: still %u kicks", kicks);
        en_set(P_DMODE, 0);
        kicks = 0;
        memset(kick_bar, 0, sizeof kick_bar);
        run(10.0f);
        CHECK(kicks >= 16u, "Auto again: %u kicks", kicks);
        claps = 0;                               /* the clap, in Auto: now and then, never every time */
        run(120.0f);
        CHECK(claps >= 2u && claps <= 40u, "Auto's claps in 2 min: %u (of ~120 chances)", claps);
        en_set(P_DMODE, 1);
        en_set(P_DPAT4, (int)(int16_t)(uint16_t)0x1010u);
        en_set(P_DCH4, 100);
        claps = 0;
        run(10.0f);
        CHECK(claps >= 8u, "the clap at Chance 100: %u in 10 s", claps);
    }

    /* the effects grid: each effect alone, then all sixteen at once, then let go: no NaN, under full scale,
     * and back to the music after */
    {
        int e;
        for (e = 0; e < EN_NFX; e++) {
            fresh();
            en_play(1);
            run(2.0f);
            en_fx(1u << e);
            run(3.0f);
            CHECK(!bad && peak < 0.99f, "effect %s: peak %f", EN_FX_NAME[e], (double)peak);
            en_fx(0);
            run(2.0f);
            CHECK(!bad && peak > 0.02f, "after %s: silent (%f)", EN_FX_NAME[e], (double)peak);
        }
        fresh();
        en_play(1);
        run(2.0f);
        en_fx(0xFFFFu);
        run(6.0f);
        CHECK(!bad && peak < 0.995f, "all sixteen effects: peak %f", (double)peak);
        en_fx(0);
        run(4.0f);
        CHECK(!bad && peak > 0.02f && peak < 0.99f, "after all effects: %f", (double)peak);
    }

    /* every voice of every layer: it sounds, without NaN, under full scale */
    {
        static const int VP[4] = {P_CVOICE, P_PVOICE, P_BVOICE, P_KIT};
        int l, v;
        for (l = 0; l < 4; l++)
            for (v = en_param_info(VP[l])->lo; v <= en_param_info(VP[l])->hi; v++) {
                fresh();
                en_set(P_DENSITY, 80);
                en_set(VP[l], v);
                en_regrow();
                en_play(1);
                run(6.0f);
                CHECK(!bad && peak > 0.02f && peak < 0.99f, "voice %d of %s: peak %f", v, en_param_info(VP[l])->name, (double)peak);
            }
    }

    /* the arrangement: at Motion 100, it leaves Full now and then; at Motion 0, never */
    {
        int k, m, secs[EN_NSEC];
        static const int MOT[2] = {100, 0};
        for (m = 0; m < 2; m++) {
            memset(secs, 0, sizeof secs);
            fresh();
            en_set(P_MOTION, MOT[m]);
            en_play(1);
            for (k = 0; k < 600; k++) {
                run(1.0f);
                secs[en_section < EN_NSEC ? en_section : 0]++;
            }
            if (MOT[m]) {                        /* the kick: four a bar while Full (but a fill), none in a Break */
                uint32_t b, kf = 0, kb = 0, nb = 0;
                for (b = 0; b + 1 < en_bars / 16u && b < KBARS; b++) {
                    if (bar_sec[b] == SEC_FULL && (b & 7u) != 7u)
                        kf += kick_bar[b] != 4;
                    if (bar_sec[b] == SEC_BREAK)
                        nb++, kb += kick_bar[b];
                }
                CHECK(!kf && !kb && nb > 20u, "Motion 100: %u Full bars without four kicks, %u kicks in %u Break bars",
                      kf, kb, nb);
            }
            printf("  10 min at Motion %d: Full %d s, Break %d, Groove %d, Thin %d\n", MOT[m], secs[SEC_FULL],
                   secs[SEC_BREAK], secs[SEC_GROOVE], secs[SEC_THIN]);
            if (MOT[m])
                CHECK(secs[SEC_FULL] < 500 && secs[SEC_FULL] > 150 && secs[SEC_BREAK] > 0,
                      "Motion 100: Full %d s, Break %d s", secs[SEC_FULL], secs[SEC_BREAK]);
            else
                CHECK(secs[SEC_FULL] == 600, "Motion 0 left Full (%d s Full)", secs[SEC_FULL]);
        }
    }

    /* throws: with Evolve high, stabs go into the echo */
    {
        uint32_t t0, lo, hi;
        fresh();
        en_set(P_EVOLVE, 0);
        en_set(P_MOTION, 0);
        en_play(1);
        t0 = en_throws;
        run(120.0f);
        lo = en_throws - t0;
        fresh();
        en_set(P_EVOLVE, 100);
        en_play(1);
        t0 = en_throws;
        run(120.0f);
        hi = en_throws - t0;
        printf("  throws in 2 min: %u at Evolve 0, %u at Evolve 100\n", lo, hi);
        CHECK(hi >= 3u && lo == 0u, "throws: %u at Evolve 100, %u at 0", hi, lo);   /* (fewer fills since the classics pass: a fill throws its last stab) */
    }

    /* the loops evolve: with Evolve at 100, the phrases change */
    {
        int8_t before[EN_NLOOP][EN_LOOP_MAX];
        int diff = 0, l;
        en_set(P_DENSITY, 80);
        run(5.0f);
        memcpy(before, (const void *)en_loop_note, sizeof before);
        run(60.0f);
        for (l = 0; l < EN_NLOOP; l++)
            for (i = 0; i < EN_LOOP_MAX; i++)
                diff += before[l][i] != en_loop_note[l][i];
        CHECK(diff > 0, "the loops evolved");
        for (l = 0; l < EN_NLOOP; l++)
            for (i = EN_LOOP_LEN[l]; i < EN_LOOP_MAX; i++)
                CHECK(en_loop_note[l][i] == -1, "loop %d slot %d past its end", l, i);
    }

    /* a new key while playing: the chord's notes in it at once */
    {
        uint32_t pent = EN_MODE_POOL[MODE_PENT_MIN];
        en_set(P_ROOT, 2);
        en_pool(pent);
        en_set(P_PMODE, 0);                      /* (a fresh piece: every slot a gap, so the walk starts from home) */
        for (i = 0; i < EN_NPROG; i++)
            en_set(P_CR0 + i, 0), en_set(P_CQ0 + i, EN_CQ_EMPTY), en_set(P_CX0 + i, 0);
        en_regrow();
        run(0.1f);
        for (i = 0; i < EN_NPAD; i++) {
            CHECK(pent >> ((en_chord[i] - 2 + 120) % 12) & 1u, "chord note %d not in the pool", en_chord[i]);
            CHECK(en_chord[i] >= 36 && en_chord[i] <= 84, "chord note %d out of range", en_chord[i]);
        }
        CHECK(en_chord_root == 2, "regrow: the home chord (root %d)", en_chord_root);
    }

    /* the dub echo at its wildest: Feedback 100, Echo 100, frozen and not; bounded, and it dies away */
    {
        float p1, rms;
        fresh();
        en_set(P_FEEDBACK, 100);
        en_set(P_ECHO, 100);
        en_set(P_EVOLVE, 100);
        en_set(P_MOTION, 100);
        en_play(1);
        bad = 0;
        peak = sum2 = 0.0f;
        run(180.0f);
        p1 = peak;
        rms = sqrtf(sum2 / (180.0f * 44100.0f * 2.0f));
        /* (0.98, from 0.97 in 1.3.0: the master +1.5 dB and the noise bed into the echo put the soft clip a little
         * higher, 0.969 here; the echo is as bounded as it was) */
        CHECK(!bad && peak < 0.98f, "Feedback 100: peak %f", (double)peak);
        en_freeze(1);
        peak = 0.0f;
        run(30.0f);
        CHECK(!bad && peak < 0.98f, "Feedback 100 frozen: peak %f", (double)peak);
        en_freeze(0);
        en_set(P_CRACKLE, 0);
        en_play(0);
        run(60.0f);
        peak = 0.0f;
        run(1.0f);
        printf("  3 min at Feedback 100: peak %.3f, rms %.4f; a minute after stop: %.4f\n", (double)p1,
               (double)rms, (double)peak);
        CHECK(peak < 0.3f, "Feedback 100: still %f a minute after stopping", (double)peak);
    }

    /* (1.4.0, REPORT10) the LFOs' rates: each sine's cycle as long as its rate (1 to 32 bars), on the trace; full
     * depth there at 100 */
    {
        int r, k;
        en_set(P_TEMPO, 140);
        for (r = 0; r < EN_LFO_DRIFT; r++) {
            const int per = 16 << r, n = 2 * per + 40;
            for (k = 0; k < 2; k++) {
                int i, ups = 0, first = -1, last = -1, ok = 1, hi = 0;
                fresh();
                en_set(P_TEMPO, 140);
                en_set(P_CFDEC, 100);
                en_set(P_NLFO, 100);
                en_set(P_CLRATE, r);
                en_set(P_NLRATE, r);
                en_play(1);
                lfo_rec(n);
                for (i = 8; i < n; i++) {        /* (upward through the middle: a cycle apart) */
                    if (lfo_v[k][i - 1] < 0 && lfo_v[k][i] >= 0) {
                        if (last >= 0 && (i - last < per - 1 || i - last > per + 1))
                            ok = 0;
                        if (first < 0)
                            first = i;
                        last = i;
                        ups++;
                    }
                    hi = lfo_v[k][i] > hi ? lfo_v[k][i] : hi;
                }
                CHECK(ok && ups >= 2 && hi > 110, "%s LFO at %d bars: %d crossings %d..%d, its top %d",
                      k ? "noise" : "chord", 1 << r, ups, first, last, hi);
            }
        }
        printf("  the LFOs' rates: 1 to 32 bars, each a cycle as long\n");
    }

    /* Drift: within the depth, gliding (no step over a small bound a sixteenth), a new place every 8 to 32 bars, never
     * a cycle; the same with the same seed, another with another; the noise's its own */
    {
        static int8_t a[600], c[600];
        int i, lo = 127, hi = -127, step = 0, iv_ok = 1, iv_n = 0, iv_min = 1 << 30, iv_max = 0, per = 0, L, same, other;
        fresh();
        en_seed(11);
        en_set(P_TEMPO, 140);
        en_set(P_CFDEC, 0);                      /* (1.4.0: the drift its own knob, the wave off) */
        en_set(P_NLFO, 0);
        en_set(P_CLWALK, 50);
        en_set(P_NLWALK, 50);
        en_play(1);
        lfo_rec(LFO_REC);
        for (i = 0; i < LFO_REC; i++) {
            const int d = i ? lfo_v[0][i] - lfo_v[0][i - 1] : 0;
            lo = lfo_v[0][i] < lo ? lfo_v[0][i] : lo;
            hi = lfo_v[0][i] > hi ? lfo_v[0][i] : hi;
            step = (d < 0 ? -d : d) > step ? (d < 0 ? -d : d) : step;
        }
        for (i = 2; i < lfo_npick; i++) {        /* (the first, at the start, left out) */
            const int g = lfo_pick[i] - lfo_pick[i - 1];
            iv_ok &= g >= 128 && g <= 513;
            iv_min = g < iv_min ? g : iv_min;
            iv_max = g > iv_max ? g : iv_max;
            iv_n++;
        }
        for (L = 128; L <= 2400; L += 4) {       /* (no lag at which it comes back as it was) */
            int sum = 0;
            for (i = 0; i < 2400; i++)
                sum += abs(lfo_v[0][i] - lfo_v[0][i + L]);
            if (sum < 2400 * 4)
                per++;
        }
        for (same = 0, i = 0; i < LFO_REC; i++)
            same += lfo_v[0][i] == lfo_v[1][i];
        printf("  Drift at 50: %d..%d (of +-127, its reach), its biggest step a sixteenth %d, %d new places %d..%d sixteenths apart\n",
               lo, hi, step, iv_n + 1, iv_min, iv_max);
        CHECK(lo >= -127 && hi <= 127 && hi - lo > 100, "Drift: %d..%d at 50", lo, hi);
        CHECK(step <= 8, "Drift: a step of %d in a sixteenth", step);
        CHECK(iv_ok && iv_n >= 8 && iv_max > iv_min, "Drift: places %d..%d sixteenths apart (%d)", iv_min, iv_max, iv_n);
        CHECK(per == 0, "Drift: like itself at %d lags", per);
        CHECK(same < LFO_REC / 3, "Drift: the noise's the chord's (%d of %d alike)", same, LFO_REC);
        fresh();
        en_seed(11);
        en_set(P_TEMPO, 140);
        en_set(P_CFDEC, 0);
        en_set(P_CLWALK, 50);
        en_play(1);
        lfo_rec(600);
        memcpy(a, lfo_v[0], sizeof a);
        fresh();
        en_seed(11);
        en_set(P_TEMPO, 140);
        en_set(P_CFDEC, 0);
        en_set(P_CLWALK, 50);
        en_play(1);
        lfo_rec(600);
        memcpy(c, lfo_v[0], sizeof c);
        same = !memcmp(a, c, sizeof a);
        fresh();
        en_seed(12);
        en_set(P_TEMPO, 140);
        en_set(P_CFDEC, 0);
        en_set(P_CLWALK, 50);
        en_play(1);
        lfo_rec(600);
        other = memcmp(a, lfo_v[0], sizeof a) != 0;
        CHECK(same && other, "Drift: the same seed the same path (%d), another seed another (%d)", same, other);
    }

    /* the bass's and the pings' LFO gone: their old knobs (P_BFDEC, P_PFDEC) change nothing; the chord's does (the test
     * hears a change) */
    {
        int b, pg, ch, nz, cw, nw;
        fresh();
        en_seed(3);
        en_set(P_BASS, 100);
        en_set(P_PINGS, 100);
        en_play(1);
        run(2.0f);
        b = same_with(P_BFDEC, 100);
        pg = same_with(P_PFDEC, 100);
        ch = same_with(P_CFDEC, 100);
        nz = same_with(P_NLFO, 100);             /* (1.4.0: the noise's, a filter on the whole noise: heard) */
        cw = same_with(P_CLWALK, 100);           /* (the drifts too) */
        nw = same_with(P_NLWALK, 100);
        printf("  the old LFO knobs: the bass's %s, the pings' %s; the chord's %s, the noise's %s; the drifts %s, %s\n",
               b ? "nothing" : "heard", pg ? "nothing" : "heard", ch ? "nothing" : "heard", nz ? "nothing" : "heard",
               cw ? "nothing" : "heard", nw ? "nothing" : "heard");
        CHECK(b == 1 && pg == 1 && ch == 0 && nz == 0 && cw == 0 && nw == 0,
              "LFOs: the bass's %d, the pings' %d, the chord's %d, the noise's %d, the drifts %d %d (1: unheard)", b, pg,
              ch, nz, cw, nw);
        en_set(P_TEMPO, 122);
    }

    /* (1.4.0, the user's) the chord's Vary: how often its stabs' pattern turns, Evolve over it. At 50 (the recipes') as
     * Evolve alone turned it: the bars whose stabs differ from the last as many as before (1.3.0 to 1.4.0's LFO pages,
     * here, the same seeds: 203, 167, 166 in 300 bars at Evolve 40); at 0 the pattern still, at any Evolve (the arrangement
     * held: Motion 0); at 100, turning about three times as often */
    {
        static const int OLD[3] = {203, 167, 166};
        int s, k, turns[3], still = 0, still_turns = 0;
        for (s = 1; s <= 3; s++) {
            uint32_t b, last;
            int n = 0;
            fresh();
            en_seed((uint32_t)s * 7u);
            en_set(P_TEMPO, 140);
            en_set(P_EVOLVE, 40);
            en_play(1);
            b = en_bars / 16u, last = en_stab_pat;
            for (k = 0; k < 300;) {
                en_render(buf, 256, 2048);
                if (en_bars / 16u != b) {
                    b = en_bars / 16u, k++;
                    n += en_stab_pat != last;
                    last = en_stab_pat;
                }
            }
            printf("  Vary 50, seed %d: the stabs changed in %d bars of 300 (before Vary: %d)\n", s, n, OLD[s - 1]);
            CHECK(n * 10 >= OLD[s - 1] * 9 && n * 10 <= OLD[s - 1] * 11, "Vary 50: %d bars changed, %d before", n,
                  OLD[s - 1]);
        }
        for (k = 0; k < 3; k++) {                /* the turns: Vary 0, 50, 100 at Evolve 40, 300 bars */
            uint32_t t0;
            fresh();
            en_seed(9);
            en_set(P_TEMPO, 140);
            en_set(P_EVOLVE, 40);
            en_set(P_CVARY, k * 50);
            en_play(1);
            run(1.0f);
            t0 = en_stab_turns;
            while (en_bars < 16u * 301u)
                run(0.5f);
            turns[k] = (int)(en_stab_turns - t0);
        }
        for (s = 0; s < 2; s++) {                /* Vary 0 at Evolve 40 and 100, Motion 0 (Full only): still */
            uint32_t b, last, t0;
            fresh();
            en_seed(13);
            en_set(P_TEMPO, 140);
            en_set(P_EVOLVE, s ? 100 : 40);
            en_set(P_MOTION, 0);
            en_set(P_CVARY, 0);
            en_play(1);
            run(1.0f);
            t0 = en_stab_turns;
            b = en_bars / 16u, last = en_stab_pat;
            for (k = 0; k < 200;) {
                en_render(buf, 256, 2048);
                if (en_bars / 16u != b) {
                    b = en_bars / 16u, k++;
                    still += en_stab_pat != last;
                    last = en_stab_pat;
                }
            }
            still_turns += (int)(en_stab_turns - t0);
        }
        printf("  Vary 0, 50, 100: the stabs' pattern turned %d, %d, %d times in 300 bars; at 0 (Evolve 40, 100) %d bars "
               "changed, %d turns\n", turns[0], turns[1], turns[2], still, still_turns);
        CHECK(turns[0] == 0 && turns[1] > 10 && turns[2] > 2 * turns[1], "Vary: turns %d %d %d", turns[0], turns[1],
              turns[2]);
        CHECK(still == 0 && still_turns == 0, "Vary 0: the stabs changed in %d bars, %d turns", still, still_turns);
        en_set(P_CVARY, 50);
        en_set(P_MOTION, 50);
        en_set(P_TEMPO, 122);
    }

    /* (1.4.0, the user's: the layers' pages) each layer's Vary, as the chord's: how often its part changes, Evolve over
     * it. At 0 still, at any Evolve: no loop note moved, no new plan, the shaker's steps kept, the noise's groove the
     * same (the arrangement held: Motion 0); at 100 more often than at 50 (the probabilities saturate a little: the
     * plan's and the noise's at Evolve 40 near 1 already, so 1.5x at least). (At 50 the music as before, bit for bit:
     * every piece, checked against 1.4.0's Chord pages while building it) */
    {
        static const int VP[4] = {P_PVARY, P_BVARY, P_DVARY, P_NVARY};
        static const char *const VN[4] = {"pings'", "bass's", "drums'", "noise's"};
        int l, k, e, turns[4][3], still[4];
        for (l = 0; l < 4; l++) {
            for (k = 0; k < 3; k++) {            /* Vary 0, 50, 100 at Evolve 40, 200 bars */
                uint32_t t0;
                fresh();
                en_seed(17);
                en_set(P_TEMPO, 140);
                en_set(P_EVOLVE, 40);
                en_set(P_MOTION, 0);
                en_set(VP[l], k * 50);
                en_play(1);
                run(1.0f);
                t0 = en_vary_turns[l];
                while (en_bars < 16u * 201u)
                    run(0.5f);
                turns[l][k] = (int)(en_vary_turns[l] - t0);
            }
            still[l] = 0;
            for (e = 0; e < 2; e++) {            /* Vary 0 at Evolve 100 too */
                uint32_t t0;
                fresh();
                en_seed(23);
                en_set(P_TEMPO, 140);
                en_set(P_EVOLVE, 100);
                en_set(P_MOTION, 0);
                en_set(VP[l], 0);
                en_play(1);
                run(1.0f);
                t0 = en_vary_turns[l];
                while (en_bars < 16u * 101u)
                    run(0.5f);
                still[l] += (int)(en_vary_turns[l] - t0);
            }
            printf("  the %s Vary 0, 50, 100: %d, %d, %d changes in 200 bars (at 0, Evolve 100: %d)\n", VN[l], turns[l][0],
                   turns[l][1], turns[l][2], still[l]);
            CHECK(turns[l][0] == 0 && still[l] == 0 && turns[l][1] > 3 && 2 * turns[l][2] >= 3 * turns[l][1],
                  "the %s Vary: %d %d %d, at 0 (Evolve 100) %d", VN[l], turns[l][0], turns[l][1], turns[l][2], still[l]);
            en_set(VP[l], 50);
        }
        en_set(P_EVOLVE, 40);
        en_set(P_MOTION, 50);
        en_set(P_TEMPO, 122);
    }

    /* (1.4.0, the user's) the chord's Cutoff a knob around the piece's own: its base (the recipe's, P_CUTOFF) and the
     * knob's offset (P_CCUT) as one: 40 and +10 sound as 50 and 0, bit for bit; and the knob heard */
    {
        uint32_t h0, h1 = 0;
        int fd[2];
        pid_t pid;
        fresh();
        en_seed(5);
        en_set(P_CUTOFF, 45);
        en_play(1);
        run(1.0f);
        fflush(stdout);
        if (pipe(fd))
            return 1;
        pid = fork();
        if (pid == 0) {                          /* (from the same state: 40 and +10 here, 50 and 0 there) */
            en_set(P_CUTOFF, 40);
            en_set(P_CCUT, 10);
            h1 = out_hash(6.0f);
            if (write(fd[1], &h1, sizeof h1) != (ssize_t)sizeof h1)
                _exit(1);
            _exit(0);
        }
        en_set(P_CUTOFF, 50);
        en_set(P_CCUT, 0);
        h0 = out_hash(6.0f);
        if (read(fd[0], &h1, sizeof h1) != (ssize_t)sizeof h1)
            h1 = ~h0;
        waitpid(pid, 0, 0);
        close(fd[0]);
        close(fd[1]);
        CHECK(h0 == h1, "Cutoff: 40 and +10 not as 50");
        fresh();
        en_seed(5);
        en_set(P_CUTOFF, 50);
        en_play(1);
        CHECK(same_with(P_CCUT, -30) == 0, "Cutoff's knob not heard");
        printf("  the chord's Cutoff knob: around the piece's own, 40 and +10 as 50 (heard)\n");
        en_set(P_CUTOFF, 45);
        en_set(P_CCUT, 0);
    }

    /* (1.4.0, the user's) the chord's LFO's Destination: Reso or Colour moved (the sound differs), the Cutoff left be; on
     * the Cutoff, the others left be */
    {
        int d, heard[EN_NCLDEST];
        float mx[EN_NCLDEST][EN_NCLDEST];
        for (d = 0; d < EN_NCLDEST; d++) {
            int k, j;
            fresh();
            en_seed(4);
            en_set(P_CFDEC, 100);
            en_set(P_CLRATE, 0);                 /* (a bar's wave: all of it heard in the time) */
            en_set(P_CLDEST, d);
            en_play(1);
            for (j = 0; j < EN_NCLDEST; j++)
                mx[d][j] = 0.0f;
            for (k = 0; k < 600; k++) {          /* (about 3.5 s) */
                en_render(buf, 256, 2048);
                for (j = 0; j < EN_NCLDEST; j++)
                    mx[d][j] = fmaxf(mx[d][j], fabsf(en_lfo_at[j]));
            }
            heard[d] = d ? !same_with(P_CLDEST, d) : 1;   /* (against the Cutoff's, the same engine from here) */
        }
        printf("  the LFO's Destination: on the Cutoff %.2f oct (Reso %.2f, Colour %.2f); on Reso %.2f (Cutoff %.2f), on "
               "Colour %.2f (Cutoff %.2f); Reso %s, Colour %s\n", (double)mx[0][0], (double)mx[0][1], (double)mx[0][2],
               (double)mx[1][1], (double)mx[1][0], (double)mx[2][2], (double)mx[2][0], heard[1] ? "heard" : "unheard",
               heard[2] ? "heard" : "unheard");
        CHECK(mx[0][0] > 1.5f && mx[0][1] == 0.0f && mx[0][2] == 0.0f, "LFO on the Cutoff: %f %f %f", (double)mx[0][0],
              (double)mx[0][1], (double)mx[0][2]);
        CHECK(mx[1][1] > 0.5f && mx[1][0] == 0.0f && mx[1][2] == 0.0f, "LFO on Reso: %f %f %f", (double)mx[1][0],
              (double)mx[1][1], (double)mx[1][2]);
        CHECK(mx[2][2] > 0.5f && mx[2][0] == 0.0f && mx[2][1] == 0.0f, "LFO on Colour: %f %f %f", (double)mx[2][0],
              (double)mx[2][1], (double)mx[2][2]);
        CHECK(heard[1] && heard[2], "LFO's Destination: Reso %d, Colour %d (1: heard)", heard[1], heard[2]);
        en_set(P_CLDEST, 0);
    }

    /* (1.4.0, the user's) the noise's LFO's Destination: the Filter as before; Tone or Air moved (the sound differs),
     * within their knobs' ranges (Tone near its top, Air near its top: never past them), the others left be */
    {
        int d, heard[EN_NNLDEST], inr = 1;
        float mx[EN_NNLDEST][EN_NNLDEST];
        for (d = 0; d < EN_NNLDEST; d++) {
            int k, j;
            fresh();
            en_seed(4);
            en_set(P_CRACKLE, 80);
            en_set(P_NLFO, 100);
            en_set(P_NLWALK, 100);
            en_set(P_NLRATE, 0);                 /* (a bar's wave: all of it heard in the time) */
            en_set(P_NCOL, 40);
            en_set(P_NROOM, 90);
            en_set(P_NLDEST, d);
            en_play(1);
            for (j = 0; j < EN_NNLDEST; j++)
                mx[d][j] = 0.0f;
            for (k = 0; k < 600; k++) {          /* (about 3.5 s) */
                en_render(buf, 256, 2048);
                for (j = 0; j < EN_NNLDEST; j++)
                    mx[d][j] = fmaxf(mx[d][j], fabsf(en_nlfo_at[j]));
                if (0.8f + en_nlfo_at[EN_NLD_TONE] > 1.0001f || 0.8f + en_nlfo_at[EN_NLD_TONE] < -1.0001f ||
                    0.9f + en_nlfo_at[EN_NLD_AIR] > 1.0001f || 0.9f + en_nlfo_at[EN_NLD_AIR] < -0.0001f)
                    inr = 0;
            }
            heard[d] = d ? !same_with(P_NLDEST, d) : 1;   /* (against the Filter's, the same engine from here) */
        }
        printf("  the noise LFO's Destination: on the Filter %.2f (Tone %.2f, Air %.2f); on Tone %.2f (Filter %.2f), on Air "
               "%.2f (Filter %.2f); Tone %s, Air %s; %s their knobs' ranges\n", (double)mx[0][0], (double)mx[0][1],
               (double)mx[0][2], (double)mx[1][1], (double)mx[1][0], (double)mx[2][2], (double)mx[2][0],
               heard[1] ? "heard" : "unheard", heard[2] ? "heard" : "unheard", inr ? "within" : "PAST");
        CHECK(mx[0][0] > 0.5f && mx[0][1] == 0.0f && mx[0][2] == 0.0f, "noise LFO on the Filter: %f %f %f",
              (double)mx[0][0], (double)mx[0][1], (double)mx[0][2]);
        CHECK(mx[1][1] > 0.2f && mx[1][0] == 0.0f && mx[1][2] == 0.0f, "noise LFO on Tone: %f %f %f", (double)mx[1][0],
              (double)mx[1][1], (double)mx[1][2]);
        CHECK(mx[2][2] > 0.1f && mx[2][0] == 0.0f && mx[2][1] == 0.0f, "noise LFO on Air: %f %f %f", (double)mx[2][0],
              (double)mx[2][1], (double)mx[2][2]);
        CHECK(heard[1] && heard[2], "noise LFO's Destination: Tone %d, Air %d (1: heard)", heard[1], heard[2]);
        CHECK(inr, "noise LFO's Destination: past its knob's range");
        en_set(P_NLDEST, 0);
    }

    /* every setting at its extremes: still bounded */
    {
        int p;
        fresh();
        en_play(1);
        for (p = 0; p < P_NPARAMS; p++)
            en_set(p, en_param_info(p)->hi);
        en_set(P_MIDI, 1);
        peak = 0.0f;
        run(30.0f);
        CHECK(!bad && peak < 1.0f, "all at max: peak %f", (double)peak);
        en_set(P_CVOICE, 0);
        run(30.0f);
        CHECK(!bad && peak < 1.0f, "all at max, saws: peak %f", (double)peak);
        printf("  a minute at max: peak %.3f\n", (double)peak);
        for (p = 0; p < P_NPARAMS; p++)
            en_set(p, en_param_info(p)->lo);
        run(30.0f);
        CHECK(!bad, "all at min: NaN");
    }

    /* (1.4.0) the Arrangement page's meters: each part's alone (the others muted) moves its own meter only; the master
     * follows the output (its peak going into the soft clip: the output's own, a little more where it squashes) */
    {
        static const uint32_t M[6] = {EN_M_CHORD, EN_M_PINGS, EN_M_BASS, EN_M_KICK, EN_M_HATS, EN_M_NOISE};
        static const char *const N[6] = {"chord", "pings", "bass", "kick", "hats", "noise"};   /* (1.4.0, the user's: the
                                                  * kick and the hats apart, as the meters' rows) */
        int k, i;
        fresh();
        en_seed(3);                              /* (its own seed: not what the tests before it left) */
        en_play(1);
        en_set(P_CRACKLE, 60);
        en_meter_on = 1;
        run(4.0f);
        for (i = 0; i < EN_NMETER; i++)
            en_meter[i] = 0.0f;
        peak = 0.0f;
        run(4.0f);
        for (i = 0; i < EN_NMETER; i++)
            CHECK(en_meter[i] > 0.003f, "meter %d moves with all playing (%f)", i, (double)en_meter[i]);
        CHECK(en_meter[EN_MT_MASTER] >= peak * 0.99f && en_meter[EN_MT_MASTER] < peak * 1.6f,
              "the master meter %f, the output %f", (double)en_meter[EN_MT_MASTER], (double)peak);
        for (k = 0; k < 6; k++) {
            en_mute(0x3Fu & ~M[k]);
            run(1.0f);
            for (i = 0; i < EN_NMETER; i++)
                en_meter[i] = 0.0f;
            peak = 0.0f;
            run(3.0f);
            for (i = 0; i < 17 && en_meter[k] <= 0.003f; i++)   /* (a part resting, a phrase at most: until it plays; the
                                                  * seed XORs into the engine's state, so what rests depends on the tests
                                                  * before) */
                run(1.0f);
            printf("  %s alone: meters", N[k]);
            for (i = 0; i < EN_NMETER; i++)
                printf(" %.4f", (double)en_meter[i]);
            printf("\n");
            CHECK(en_meter[k] > 0.003f, "%s alone: its meter %f", N[k], (double)en_meter[k]);
            for (i = 0; i < 6; i++)
                if (i != k)
                    CHECK(en_meter[i] < 1e-4f, "%s alone: meter %d at %f", N[k], i, (double)en_meter[i]);
            CHECK(en_meter[EN_MT_MASTER] >= peak * 0.99f, "%s alone: the master %f, the output %f", N[k],
                  (double)en_meter[EN_MT_MASTER], (double)peak);
        }
        en_mute(0);
        en_meter_on = 0;
        for (i = 0; i < EN_MT_MASTER; i++)
            en_meter[i] = 0.0f;
        run(1.0f);
        for (i = 0; i < EN_MT_MASTER; i++)
            CHECK(en_meter[i] == 0.0f, "meter %d measured while off", i);
    }

    /* (1.4.0, the user's: the second try) the kick's rumble: none at 0; at its knob a low bed (under ~150 Hz, nearly
     * nothing over 200), mono, ducked on the kick (the beat's first sixteenth well under its last); the master never
     * over full scale at 100 (Pressure, the loudest kick, and Concrete) */
    {
        int16_t pp[P_NPARAMS];
        uint16_t pl;
        int k, i, pc;
        for (pc = 0; pc < 2; pc++) {
            const int piece = pc ? 4 : 6;
            double e = 0.0, eh = 0.0, eq[4] = {0, 0, 0, 0};
            int nq[4] = {0, 0, 0, 0};
            float lo1 = 0.0f, lo2 = 0.0f, mx = 0.0f, m0 = 0.0f;
            fresh();
            en_piece(piece, pp, &pl);
            for (k = 0; k < P_NPARAMS; k++)
                if (k != P_TUNE && k != P_MIDI)
                    en_set(k, pp[k]);
            en_pool(pl);
            en_set(P_RUMBLE, 0);
            en_regrow();
            en_play(1);
            run(8.0f);
            CHECK(same_with(P_RUMBLE, 0) == 1 && same_with(P_RUMBLE, 35) == 0, "the rumble: none at 0, heard at 35");
            rumble_ab(35);
            for (i = 44100; i < RB_TN; i++) {    /* (the first second left out: the tail building) */
                const float x = rb_m[i];
                float y;
                lo1 += 0.0350f * (x - lo1);      /* (two one-poles at ~250 Hz: what's over them, x - their low) */
                y = x - lo1;
                lo2 += 0.0350f * (y - lo2);
                y -= lo2;
                e += (double)(x * x);
                eh += (double)(y * y);
                eq[rb_q[i]] += (double)(x * x), nq[rb_q[i]]++;
            }
            for (k = 0; k < 4; k++)
                eq[k] /= nq[k] ? nq[k] : 1;
            printf("  the rumble (%s, 35): %.1f dB, %.1f%% of it over ~250 Hz, its side %.1f dB under it; "
                   "the beat's sixteenths %.1f %.1f %.1f %.1f dB\n", en_piece_name(piece),
                   10.0 * log10(e / (RB_TN - 44100) + 1e-20), 100.0 * eh / (e + 1e-20),
                   10.0 * log10(e / (rb_side + 1e-30)), 10.0 * log10(eq[0] + 1e-20), 10.0 * log10(eq[1] + 1e-20),
                   10.0 * log10(eq[2] + 1e-20), 10.0 * log10(eq[3] + 1e-20));
            CHECK(e / (RB_TN - 44100) > 1e-6, "the rumble at 35 heard (%g)", e / (RB_TN - 44100));
            CHECK(eh < 0.05 * e, "the rumble low: %.1f%% over ~250 Hz", 100.0 * eh / (e + 1e-20));
            CHECK(rb_side < 1e-3 * e, "the rumble mono (its side %.1f dB under it)", 10.0 * log10(e / (rb_side + 1e-30)));
            CHECK(eq[0] * 2.0 < eq[3], "the rumble ducked on the kick: the beat's first sixteenth %.1f dB, its last %.1f",
                  10.0 * log10(eq[0] + 1e-20), 10.0 * log10(eq[3] + 1e-20));
            {   /* (a minute, the master's peak each block: at 100 (a fork) and at 0, the same minute) */
                int fd[2], w;
                pid_t pid;
                float *pk;
                fflush(stdout);
                if (pipe(fd))
                    fd[0] = fd[1] = -1;
                pid = fork();
                for (w = 0; w < 2; w++) {
                    if ((pid == 0) != (w == 0))
                        continue;
                    pk = w == 0 ? &mx : &m0;
                    en_set(P_RUMBLE, w == 0 ? 100 : 0);
                    en_meter[EN_MT_MASTER] = 0.0f;
                    for (k = 0; k < 60 * 172; k++) {
                        en_render(buf, 256, 2048);
                        *pk = en_meter[EN_MT_MASTER] > *pk ? en_meter[EN_MT_MASTER] : *pk;
                        en_meter[EN_MT_MASTER] = 0.0f;
                    }
                }
                if (pid == 0) {
                    if (write(fd[1], &mx, sizeof mx) != (ssize_t)sizeof mx)
                        _exit(1);
                    _exit(0);
                }
                if (read(fd[0], &mx, sizeof mx) != (ssize_t)sizeof mx)
                    mx = 9.0f;
                waitpid(pid, 0, 0);
                close(fd[0]);
                close(fd[1]);
            }
            printf("  the rumble (%s, 100): the master's peak %.2f (%.2f without it)\n", en_piece_name(piece), (double)mx,
                   (double)m0);
            CHECK(mx < 1.0f && mx < m0 * 1.25f, "the rumble at 100: the master's peak %.2f (%.2f without it)", (double)mx,
                  (double)m0);
        }
    }

    /* (1.4.0, the user's) the Arrangement page's channel strips: each part's Echo and Plate sends and its Low cut reach
     * the engine (heard against 0; the defaults are the recipes' own: the sound as before, checked by the scenarios and
     * the renders); a Low cut takes a part's lows (the bass alone: under ~120 Hz, 100 against 0) */
    {
        static const uint8_t SP[] = {P_CECHO, P_CPLATE, P_PECHO, P_PPLATE, P_BECHO, P_BPLATE, P_DECHO, P_DPLATE,
                                     P_NECHO, P_NPLATE, P_KECHO, P_KPLATE, P_KLVL};   /* (1.4.0: the Kick strip's too) */
        static const uint8_t LP[] = {P_CLOW, P_PLOW, P_BLOW, P_DLOW, P_NLOW, P_KLOW};
        int16_t pp[P_NPARAMS];
        uint16_t pl;
        unsigned k;
        int heard = 0, low0 = 0, lowh = 0;
        fresh();
        en_piece(0, pp, &pl);
        for (k = 0; k < P_NPARAMS; k++)
            if (k != P_TUNE && k != P_MIDI)
                en_set((int)k, pp[k]);
        en_pool(pl);
        en_set(P_PINGS, 80);
        en_set(P_CRACKLE, 80);
        en_regrow();
        en_play(1);
        run(4.0f);
        for (k = 0; k < sizeof SP; k++)
            heard += same_with(SP[k], 100) == 0;
        for (k = 0; k < sizeof LP; k++) {
            low0 += same_with(LP[k], 0) == 1;
            lowh += same_with(LP[k], 80) == 0;
        }
        printf("  the strips: %d of %u sends heard at 100 against 0; the Low cuts at 0 as off %d of %u, at 80 heard %d\n",
               heard, (unsigned)sizeof SP, low0, (unsigned)sizeof LP, lowh);
        CHECK(heard == (int)sizeof SP && low0 == (int)sizeof LP && lowh == (int)sizeof LP,
              "the strips: sends heard %d, Low cuts off at 0 %d, heard at 80 %d", heard, low0, lowh);
        {   /* the bass alone, its lows: the Low cut at 0 (here) and at 100 (a fork), the same minute otherwise */
            int fd[2], w, b, j;
            pid_t pid;
            double lo[2] = {0.0, 0.0};
            en_mute(0x1Fu & ~(uint32_t)EN_M_BASS);
            fflush(stdout);
            if (pipe(fd))
                fd[0] = fd[1] = -1;
            pid = fork();
            for (w = 0; w < 2; w++) {
                float l1 = 0.0f;
                if ((pid == 0) != (w == 1))
                    continue;
                en_set(P_BLOW, w ? 100 : 0);
                for (b = 0; b < 8 * 172; b++) {
                    en_render(buf, 256, 2048);
                    for (j = 0; j < 256; j++) {
                        l1 += 0.017f * ((float)(buf[2 * j] + buf[2 * j + 1]) * (0.5f / 8388608.0f) - l1);   /* (~120 Hz) */
                        lo[w] += (double)(l1 * l1);
                    }
                }
            }
            if (pid == 0) {
                if (write(fd[1], &lo[1], sizeof lo[1]) != (ssize_t)sizeof lo[1])
                    _exit(1);
                _exit(0);
            }
            if (read(fd[0], &lo[1], sizeof lo[1]) != (ssize_t)sizeof lo[1])
                lo[1] = lo[0];
            waitpid(pid, 0, 0);
            close(fd[0]);
            close(fd[1]);
            en_mute(0);
            printf("  the bass's Low cut at 100: its lows under ~120 Hz %.1f dB against 0\n",
                   10.0 * log10((lo[1] + 1e-20) / (lo[0] + 1e-20)));
            CHECK(lo[0] > 1e-3 && lo[1] < 0.25 * lo[0], "the bass's Low cut at 100: its lows %.1f dB (want under -6)",
                  10.0 * log10((lo[1] + 1e-20) / (lo[0] + 1e-20)));
        }
    }

    /* (1.4.0, the user's: "I can throw the hats to the echo") the hats thrown (EN_PF_HATTHROW, black F#4 held): the hats,
     * the rim and the clap into the echo, their tails ringing after it's let go (the hats alone: the second after it,
     * well over the same without); the kick never with them (the kick alone: the same, to a hair: the echo's feedback
     * a little up while held, on nothing). And the Kick strip's Low cut leaves the rumble be (the kick's ear before it) */
    {
        static const uint32_t ALONE[2] = {EN_M_HATS, EN_M_KICK};
        int16_t pp[P_NPARAMS];
        uint16_t pl;
        unsigned k;
        int w;
        double e[2][2];
        for (w = 0; w < 2; w++) {
            int t, fd[2];
            pid_t pid;
            fresh();
            en_piece(0, pp, &pl);
            for (k = 0; k < P_NPARAMS; k++)
                if (k != P_TUNE && k != P_MIDI)
                    en_set((int)k, pp[k]);
            en_pool(pl);
            en_set(P_HITS, 90);
            en_seed(5);
            en_regrow();
            en_mute(0x3Fu & ~ALONE[w]);
            en_play(1);
            run(4.0f);
            fflush(stdout);                      /* (from here two ways, a fork: thrown or not, the same otherwise; the
                                                  * seed XORs into the engine's state, so two runs would differ) */
            if (pipe(fd))
                break;
            pid = fork();
            t = pid == 0;
            en_perf(t ? EN_PF_HATTHROW : 0u);
            run(2.0f);
            en_perf(0);
            run(0.05f);
            en_mute(0x3Fu);                      /* (the tail alone: everything stopped, the echo ringing on) */
            sum2 = 0.0f;
            run(1.5f);
            e[w][t] = (double)sum2;
            if (t) {
                if (write(fd[1], &e[w][1], sizeof e[w][1]) != (ssize_t)sizeof e[w][1])
                    _exit(1);
                _exit(0);
            }
            if (read(fd[0], &e[w][1], sizeof e[w][1]) != (ssize_t)sizeof e[w][1])
                e[w][1] = -1.0;
            waitpid(pid, 0, 0);
            close(fd[0]);
            close(fd[1]);
        }
        en_mute(0);
        printf("  the hats thrown: the hats' tail %.1f dB over not thrown; the kick's %.2f dB (none)\n",
               10.0 * log10((e[0][1] + 1e-20) / (e[0][0] + 1e-20)), 10.0 * log10((e[1][1] + 1e-20) / (e[1][0] + 1e-20)));
        CHECK(e[0][1] > 2.0 * e[0][0] && e[0][1] > 1e-3, "the hats thrown: their tail %g against %g", e[0][1], e[0][0]);
        CHECK(e[1][1] < 1.05 * e[1][0] + 1e-6 && e[1][1] > 0.95 * e[1][0] - 1e-6, "the kick not thrown: %g against %g",
              e[1][1], e[1][0]);
    }
    {   /* the rumble's bed the same with the kick's Low cut on: the kick alone, under ~60 Hz, the rumble's share (on less
         * off) with the cut at 100 as without it */
        int16_t pp[P_NPARAMS];
        uint16_t pl;
        unsigned k;
        double rb[4];
        int t;
        for (t = 0; t < 4; t++) {                /* (t: bit 0 the rumble on, bit 1 the Low cut) */
            fresh();
            en_piece(6, pp, &pl);                /* (Pressure: its rumble) */
            for (k = 0; k < P_NPARAMS; k++)
                if (k != P_TUNE && k != P_MIDI)
                    en_set((int)k, pp[k]);
            en_pool(pl);
            en_set(P_RUMBLE, t & 1 ? 60 : 0);
            en_set(P_KLOW, t & 2 ? 100 : 0);
            en_seed(5);
            en_regrow();
            en_mute(0x3Fu & ~(uint32_t)EN_M_KICK);
            en_play(1);
            run(6.0f);
            {
                int b, j;
                float l1 = 0.0f;
                rb[t] = 0.0;
                for (b = 0; b < 4 * 172; b++) {
                    en_render(buf, 256, 2048);
                    for (j = 0; j < 256; j++) {
                        l1 += 0.0085f * ((float)(buf[2 * j] + buf[2 * j + 1]) * (0.5f / 8388608.0f) - l1);   /* (~60 Hz) */
                        rb[t] += (double)(l1 * l1);
                    }
                }
            }
        }
        en_mute(0);
        {
            const double r0 = rb[1] - rb[0], r1 = rb[3] - rb[2];
            printf("  the kick's Low cut at 100: the rumble's lows %.1f dB against the cut off (the kick's own %.1f dB)\n",
                   10.0 * log10((fabs(r1) + 1e-20) / (fabs(r0) + 1e-20)), 10.0 * log10((rb[2] + 1e-20) / (rb[0] + 1e-20)));
            CHECK(r0 > 1e-6 && r1 > 0.5 * r0 && r1 < 2.0 * r0, "the kick's Low cut moved the rumble: %g against %g", r1, r0);
        }
    }

    /* (1.4.0, the user's: song structure) Endless as ever, the song's knobs heard nowhere; a song: its intro, main
     * and outro their bars, the parts in in the Kind's order and out in the outro's, the kick One, Half, Four, then
     * the grid stopped (PLAY off) and quiet; the outro now (from the next phrase, in Endless too) and back */
    {
        static uint8_t ph_at[96];
        static uint16_t kl_at[96];
        static const char *const PN[EN_NPART] = {"chord", "pings", "bass", "kick", "hats", "noise"};
        int kind, first_in[EN_NPART], last_in[EN_NPART], i, k, ok;
        fresh();
        en_seed(5);
        en_play(1);
        run(1.0f);
        ok = same_with(P_SINTRO, 16) && same_with(P_SKIND, 1) && same_with(P_SMAIN, 64) && same_with(P_SOUTRO, 16);
        CHECK(ok, "Endless: the song's knobs changed the sound");
        printf("  Endless: the song's knobs heard nowhere\n");
        for (kind = 0; kind < 2; kind++) {
            uint32_t nb = 0;
            fresh();
            en_seed(9 + (uint32_t)kind);
            en_set(P_SMODE, 1);
            en_set(P_SINTRO, 2);
            en_set(P_SMAIN, 4);
            en_set(P_SOUTRO, 2);
            en_set(P_SKIND, kind);
            en_set(P_TEMPO, 140);
            en_play(1);
            memset(ph_at, 0xFF, sizeof ph_at);
            for (i = 0; i < EN_NPART; i++)
                first_in[i] = -1, last_in[i] = -1;
            for (i = 0; i < 30000 && nb < 90u; i++) {
                en_render(buf, 256, 2048);
                if (!en_playing)
                    break;
                if (en_bars) {
                    const uint32_t b = (en_bars - 1u) / 16u;
                    nb = b;
                    if (b < 96u) {
                        ph_at[b] = en_song_ph;
                        if ((en_bars - 1u) % 16u == 8u)
                            kl_at[b] = en_lane[1];
                    }
                    for (k = 0; k < EN_NPART; k++)
                        if (en_fader[k] > 0.01f) {
                            if (first_in[k] < 0)
                                first_in[k] = (int)b;
                            last_in[k] = (int)b;
                        }
                }
            }
            ok = 1;
            for (i = 0; i < 64; i++)
                if (ph_at[i] != (i < 16 ? EN_SG_INTRO : i < 48 ? EN_SG_MAIN : EN_SG_OUTRO) && ok)
                    printf("  (bar %d: place %d)\n", i, ph_at[i]), ok = 0;
            CHECK(ok, "the song's places by bar (kind %d)", kind);
            CHECK(!en_playing && en_song_ph == EN_SG_END && en_bars == 64u * 16u,
                  "the song over: PLAY off at bar 64 (playing %d, place %d, %u sixteenths)", en_playing, en_song_ph, en_bars);
            printf("  a song (16, 32, 16 bars, %s first): in at bars", kind ? "the pulse" : "ambient");
            for (k = 0; k < EN_NPART; k++)
                printf(" %s %d", PN[k], first_in[k]);
            printf("; out after");
            for (k = 0; k < EN_NPART; k++)
                printf(" %s %d", PN[k], last_in[k]);
            printf("\n");
            if (!kind)
                CHECK(first_in[5] == 0 && first_in[5] <= first_in[0] && first_in[0] < first_in[3] &&
                          first_in[3] < first_in[2] && first_in[2] < first_in[1] && first_in[1] < first_in[4],
                      "ambient first: the order in");
            else
                CHECK(first_in[3] == 0 && first_in[2] == 0 && first_in[0] < first_in[1] && first_in[1] < first_in[5] &&
                          first_in[5] < first_in[4],
                      "pulse first: the order in");
            CHECK(first_in[4] >= 10 && first_in[4] < 16, "the hats last, in the intro (%d)", first_in[4]);
            CHECK(last_in[4] < last_in[1] && last_in[1] <= last_in[2] && last_in[2] < last_in[3] &&
                      last_in[3] < last_in[0] && last_in[3] < last_in[5] && last_in[0] >= 60 && last_in[5] >= 60,
                  "the outro's order out: the hats first, the chord and the noise last");
            {   /* the kick in the intro: One, then Half, then Four (its A bars; the B bars, 4 and 8, sparser) */
                int prev = 0, grow = 1, last4 = 1;
                for (i = 0; i < 16; i++) {
                    const int n = popcount(kl_at[i]);
                    if ((i & 3) == 3 || (i & 7) == 7 || !n)
                        continue;
                    grow &= n >= prev;
                    prev = n;
                }
                last4 = popcount(kl_at[14]) == 4;  /* (Four by the intro's last A bar) */
                if (!grow || !last4)
                    for (i = 0; i < 16; i++)
                        printf("  (bar %d: the kick %04X)\n", i, kl_at[i]);
                CHECK(grow && last4 && popcount(kl_at[first_in[3] + 1 < 16 ? first_in[3] + 1 : 15]) <= 2,
                      "the intro's kick: One, Half, Four");
            }
            peak = 0.0f;
            run(40.0f);
            peak = 0.0f;
            run(1.0f);
            en_set(P_CRACKLE, 0);
            CHECK(!en_playing && peak < 0.05f, "after the song, the tails gone (%f)", (double)peak);
        }
        printf("  a song's places and parts, the kick One, Half, Four, the end\n");
        /* the outro now: in Endless, from the next phrase; again in it: back, playing on; and once more, to the end */
        fresh();
        en_seed(3);
        en_set(P_SOUTRO, 2);
        en_play(1);
        while (en_bars < 10u * 16u + 2u)
            run(0.05f);
        en_outro();
        run(0.05f);
        CHECK(en_song_bo == 16u && en_song_be == 32u && en_song_ph == EN_SG_MAIN, "outro now: from bar 16 (%u, %u, %d)",
              en_song_bo, en_song_be, en_song_ph);
        while (en_bars < 17u * 16u + 2u)
            run(0.05f);
        CHECK(en_song_ph == EN_SG_OUTRO, "outro now: in it at bar 17 (%d)", en_song_ph);
        while (en_bars < 22u * 16u + 2u)
            run(0.05f);
        CHECK(en_fader[4] < 0.01f, "outro now: the hats gone (%f)", (double)en_fader[4]);
        en_outro();
        run(0.05f);
        CHECK(en_song_ph == EN_SG_OFF && en_playing, "outro now, again: back to Endless (%d)", en_song_ph);
        run(12.0f);
        for (k = 0, ok = 1; k < EN_NPART; k++)
            ok &= en_fader[k] > 0.999f;
        CHECK(ok, "back: every part up again");
        en_outro();
        run(0.05f);
        while (en_playing && en_bars < 200u * 16u)
            run(0.5f);
        CHECK(!en_playing && en_song_ph == EN_SG_END, "outro now: played to its end, stopped");
        /* a song: the outro asked early, then let go: the outro as planned (or four phrases on, at least) */
        fresh();
        en_seed(4);
        en_set(P_SMODE, 1);
        en_set(P_SINTRO, 0);
        en_set(P_SMAIN, 8);
        en_set(P_SOUTRO, 2);
        en_play(1);
        run(0.05f);
        CHECK(en_song_ph == EN_SG_MAIN && en_song_bo == 64u, "no intro: the main from bar 1, the outro at 64 (%d, %u)",
              en_song_ph, en_song_bo);
        while (en_bars < 9u * 16u + 2u)
            run(0.05f);
        en_outro();
        run(0.05f);
        CHECK(en_song_bo == 16u, "a song's outro now: from bar 16 (%u)", en_song_bo);
        en_outro();
        run(0.05f);
        CHECK(en_song_bo == 64u && en_song_ph == EN_SG_MAIN, "let go: the outro as planned, bar 64 (%u)", en_song_bo);
        en_set(P_SMAIN, 4);                      /* (Main turned as it plays: the outro moves) */
        run(0.05f);
        CHECK(en_song_bo == 32u, "Main turned while it plays: the outro at 32 (%u)", en_song_bo);
        en_set(P_SMODE, 0);                      /* (to Endless: the song let go, it plays on) */
        run(0.05f);
        CHECK(en_song_ph == EN_SG_OFF && en_playing, "Endless again: no outro (%d)", en_song_ph);
        printf("  the outro now, in Endless and in a song, and back\n");
    }

    /* (1.4.0, the user's: the dub mixer) Mixer 0: nothing rides; up: each part's level and sends ride slowly, within
     * their bounds (the levels never over +3 dB), mostly off the phrase lines, a part out and back or a swell now and
     * then, the master never into the clip; Lock rests it; a song's intro and outro left to the song (no moves, the
     * rides at half depth); back to 0: every ride home, exactly */
    {
        float lo[EN_NPART], hi[EN_NPART], elo = 99.0f, ehi = -99.0f, mp = 0.0f, mp0 = 0.0f, ilv = 0.0f;
        uint32_t m0, l0, m1;
        int k, b, ok, evs = 0, was = 0, inev = 0;
        fresh();
        en_seed(9);
        en_meter_on = 1;
        en_play(1);
        m0 = en_mx_moves;
        for (b = 0; b < (int)(60.0f * 44100.0f / 256.0f); b++) {   /* (the master's peak without it) */
            en_render(buf, 256, 2048);
            if (en_meter[EN_MT_MASTER] > mp0)
                mp0 = en_meter[EN_MT_MASTER];
            for (k = 0; k < EN_NMETER; k++)
                en_meter[k] = 0.0f;
        }
        for (k = 0, ok = 1; k < EN_NPART; k++)
            ok &= en_mx_db[k] == 0.0f && en_mx_snd[k][0] == 0.0f && en_mx_snd[k][1] == 0.0f;
        CHECK(ok && en_mx_moves == m0, "Mixer 0: nothing rides (%u moves)", en_mx_moves - m0);
        en_set(P_MIXER, 100);
        for (k = 0; k < EN_NPART; k++)
            lo[k] = 99.0f, hi[k] = -99.0f;
        m0 = en_mx_moves, l0 = en_mx_line;
        for (b = 0; b < (int)(480.0f * 44100.0f / 256.0f); b++) {   /* (eight minutes) */
            en_render(buf, 256, 2048);
            if (en_meter[EN_MT_MASTER] > mp)
                mp = en_meter[EN_MT_MASTER];
            for (k = 0; k < EN_NMETER; k++)
                en_meter[k] = 0.0f;
            for (k = 0; k < EN_NPART; k++) {
                if (en_mx_ev != 1 && en_mx_db[k] < lo[k])   /* (the drift: a part out and back apart) */
                    lo[k] = en_mx_db[k];
                if (en_mx_db[k] > hi[k])
                    hi[k] = en_mx_db[k];
                if (en_mx_snd[k][0] < elo)
                    elo = en_mx_snd[k][0];
                if (en_mx_snd[k][0] > ehi)
                    ehi = en_mx_snd[k][0];
            }
            if (en_mx_ev && !was)
                evs++;
            was = en_mx_ev;
        }
        for (k = 0, ok = 1; k < EN_NPART; k++)
            ok &= hi[k] <= 3.01f && hi[k] - lo[k] >= (k == 3 ? 0.3f : 1.5f) && lo[k] >= -7.01f;
        printf("  the dub mixer at 100, 8 minutes: %u moves, %.0f%% on a phrase's line; %d out-and-back or swells; the "
               "levels %+.1f..%+.1f dB (the chord), %+.1f..%+.1f (the kick); the echo sends %+.1f..%+.1f dB; the master's "
               "peak %.2f (%.2f at 0)\n", en_mx_moves - m0, 100.0 * (en_mx_line - l0) / (double)(en_mx_moves - m0 + 1u),
               evs, (double)lo[0], (double)hi[0], (double)lo[3], (double)hi[3], (double)elo, (double)ehi, (double)mp,
               (double)mp0);
        CHECK(ok, "the dub mixer's level rides within their bounds, and riding (the chord %+.1f..%+.1f, the kick %+.1f..%+.1f)",
              (double)lo[0], (double)hi[0], (double)lo[3], (double)hi[3]);
        CHECK(en_mx_moves - m0 > 200u, "the dub mixer moves (%u in 8 minutes)", en_mx_moves - m0);
        CHECK((en_mx_line - l0) * 100u < 35u * (en_mx_moves - m0) && (en_mx_line - l0) * 100u > 4u * (en_mx_moves - m0),
              "the dub mixer off the phrase lines, mostly (%u of %u on one)", en_mx_line - l0, en_mx_moves - m0);
        CHECK(evs >= 1, "the dub mixer's moves: a part out and back, or a swell (%d)", evs);
        CHECK(elo >= -8.01f && ehi <= 16.01f, "the echo sends' rides within bounds (%+.1f..%+.1f dB)", (double)elo, (double)ehi);
        CHECK(mp < 1.0f && !bad, "the dub mixer at 100: the master never into the clip (%.2f)", (double)mp);
        en_lock(1);                              /* (Lock: no new places, no new moves) */
        run(1.0f);
        m1 = en_mx_moves;
        run(90.0f);
        CHECK(en_mx_moves == m1, "Lock rests the dub mixer (%u moves)", en_mx_moves - m1);
        en_lock(0);
        en_set(P_MIXER, 0);                      /* (back to 0: home, exactly) */
        run(30.0f);
        for (k = 0, ok = 1; k < EN_NPART; k++)
            ok &= en_mx_db[k] == 0.0f && en_mx_snd[k][0] == 0.0f && en_mx_snd[k][1] == 0.0f;
        CHECK(ok && !en_mx_ev, "Mixer back to 0: every ride home");
        /* a song: the intro the song's (no moves; the rides at half depth), the main the mixer's too */
        fresh();
        en_seed(5);
        en_set(P_MIXER, 100);
        en_set(P_SMODE, 1);
        en_set(P_SINTRO, 8);
        en_set(P_SMAIN, 16);
        en_set(P_SOUTRO, 2);
        en_play(1);
        run(0.05f);
        while (en_playing && en_bars < 64u * 16u) {
            run(0.1f);
            inev |= en_mx_ev != 0;
            for (k = 0; k < EN_NPART; k++)
                if (fabsf(en_mx_db[k]) > ilv)
                    ilv = fabsf(en_mx_db[k]);
        }
        CHECK(!inev && ilv <= 3.6f, "a song's intro: the dub mixer's moves wait, its rides half deep (%.1f dB)", (double)ilv);
        {
            uint32_t mm;
            float mlv = 0.0f;
            while (en_playing && en_song_ph == EN_SG_INTRO)
                run(0.1f);
            mm = en_mx_moves;
            while (en_playing && en_song_ph == EN_SG_MAIN) {
                run(0.5f);
                for (k = 0; k < EN_NPART; k++)
                    if (en_mx_ev != 1 && fabsf(en_mx_db[k]) > mlv)
                        mlv = fabsf(en_mx_db[k]);
            }
            CHECK(en_mx_moves > mm + 50u && mlv > ilv, "a song's main: the dub mixer at full depth (%.1f dB, %u moves)",
                  (double)mlv, en_mx_moves - mm);
        }
        printf("  the dub mixer: off at 0, riding within bounds, Lock resting it, a song's intro left to the song\n");
    }

    /* stop: the tails die away (but the vinyl turns on) */
    fresh();
    en_play(1);
    run(20.0f);
    en_play(0);
    run(30.0f);
    peak = 0.0f;
    run(1.0f);
    printf("  30 s after stop, crackle on: %.4f\n", (double)peak);
    CHECK(peak < 0.05f, "quiet after stopping, crackle on (%f)", (double)peak);
    en_set(P_CRACKLE, 0);
    run(1.0f);
    peak = 0.0f;
    run(1.0f);
    CHECK(peak < 0.01f, "quiet after stopping (%f)", (double)peak);
    CHECK(midi_count > 50u, "MIDI out (%u)", midi_count);

    printf(fails ? "amb: %d FAILED\n" : "amb: ok\n", fails);
    return fails != 0;
}
