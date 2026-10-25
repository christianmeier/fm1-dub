/* SPDX-License-Identifier: GPL-3.0-only */
/* The instrument (amb.h). One compilation unit, -O2. */
#include "amb.h"
#include "fastmath.h"

#ifdef EN_HOST
#define EN_POOL
#else
#define EN_POOL __attribute__((section(".pool")))
#endif
#define BARRIER() __asm__ volatile("" ::: "memory")

const char *const EN_NOTE_NAME[12] = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};
/* (Ionian and Aeolian shown as Major and Minor) */
const char *const EN_MODE_NAME[EN_NMODES] = {"Major", "Dorian", "Phrygian", "Lydian", "Mixolyd.", "Minor",
                                             "Pent maj", "Pent min"};
#define B(a, b, c, d, e, f, g) (1u << (a) | 1u << (b) | 1u << (c) | 1u << (d) | 1u << (e) | 1u << (f) | 1u << (g))
const uint16_t EN_MODE_POOL[EN_NMODES] = {
    B(0, 2, 4, 5, 7, 9, 11), B(0, 2, 3, 5, 7, 9, 10), B(0, 1, 3, 5, 7, 8, 10), B(0, 2, 4, 6, 7, 9, 11),
    B(0, 2, 4, 5, 7, 9, 10), B(0, 2, 3, 5, 7, 8, 10), B(0, 2, 4, 7, 9, 9, 9),  B(0, 3, 5, 7, 10, 10, 10)};
#undef B
const uint8_t EN_LOOP_LEN[EN_NLOOP] = {13, 17, 23, 29};
const uint8_t EN_DRUM_N[EN_NDRUM] = {16, 16, 16, 16, 16};
const char *const EN_SEC_NAME[EN_NSEC] = {"Full", "Break", "Groove", "Thin"};

static const en_param_t PARAMS[P_NPARAMS] = {
    {"Chord", 0, 100, 70},    {"Pings", 0, 100, 35},    {"Bass", 0, 100, 60},     {"Drums", 0, 100, 65},
    {"Density", 0, 100, 30},  {"Evolve", 0, 100, 40},   {"Drift", 0, 6, 4},       {"Hits", 0, 100, 50},
    {"Echo", 0, 100, 55},     {"Echo time", 0, 5, 1},   {"Reverb", 0, 100, 35},   {"Feedback", 0, 100, 60},
    {"Cutoff", 0, 100, 45},   {"Decay", 0, 100, 35},    {"Ping tone", 0, 100, 40}, {"Motion", 0, 100, 50},
    {"Tune", -50, 50, 0},     {"MIDI out", 0, 1, 1},    {"Tempo", 90, 140, 122},  {"Root", 0, 11, 5},
    {"Mode", 0, EN_NMODES - 1, MODE_DORIAN}, {"Ping oct", -2, 2, 0}, {"Chord voice", 0, EN_NCV - 1, 0},
    {"Bass oct", -1, 1, 0},   {"Groove", 0, 4, 1},      {"Fifth", 0, 100, 20},
    {"Swing", 0, 100, 10},    {"Hats", 0, 100, 50},
    {"Stabs", 0, 100, 30},    {"Crackle", 0, 100, 35},  {"Duck", 0, 100, 50},     {"Size", 0, 100, 55},
    {"Cloud", 0, 100, 30},
    {"Ping voice", 0, EN_NPV - 1, 0}, {"Bass voice", 0, EN_NBV - 1, 0}, {"Kit", 0, EN_NKIT - 1, 0},
    {"Attack", 0, 100, 0},   {"Colour", -50, 50, 0},   {"Resonance", -50, 50, 0}, {"Env amount", -50, 50, 0},
    {"Tone", -50, 50, 0},     {"Ratio", -50, 50, 0},    {"Ping len", -50, 50, 0}, {"Bend", 0, 100, 0},
    {"Bite", -50, 50, 0},     {"Shape", -50, 50, 0},    {"Gate", -50, 50, 0},     {"Glide", -50, 50, 0},
    {"Kick tune", -12, 12, 0}, {"Kick len", -50, 50, 0}, {"Punch", -50, 50, 0},   {"Hat tone", -50, 50, 0},
    {"Chord alg", 0, 9, 0},   {"Chord ratio", -50, 50, 0}, {"Chord fb", 0, 100, 0}, {"Chord LFO", 0, 100, 0},
    {"Ping alg", 0, 9, 0},    {"Ping ratio", -50, 50, 0}, {"Ping fb", 0, 100, 0}, {"Ping LFO", 0, 100, 0},
    {"Bass alg", 0, 9, 0},    {"Bass ratio", -50, 50, 0}, {"Bass fb", 0, 100, 0}, {"Bass LFO", 0, 100, 0},
    {"Root 1", 0, 11, 0}, {"Root 2", 0, 11, 0}, {"Root 3", 0, 11, 0}, {"Root 4", 0, 11, 0},
    {"Root 5", 0, 11, 0}, {"Root 6", 0, 11, 0}, {"Root 7", 0, 11, 0}, {"Root 8", 0, 11, 0},
    {"Kind 1", 0, EN_NCQ - 1, 0}, {"Kind 2", 0, EN_NCQ - 1, 0}, {"Kind 3", 0, EN_NCQ - 1, 0}, {"Kind 4", 0, EN_NCQ - 1, 0},
    {"Kind 5", 0, EN_NCQ - 1, 0}, {"Kind 6", 0, EN_NCQ - 1, 0}, {"Kind 7", 0, EN_NCQ - 1, 0}, {"Kind 8", 0, EN_NCQ - 1, 0},
    {"Type 1", 0, EN_NCX - 1, 0}, {"Type 2", 0, EN_NCX - 1, 0}, {"Type 3", 0, EN_NCX - 1, 0}, {"Type 4", 0, EN_NCX - 1, 0},
    {"Type 5", 0, EN_NCX - 1, 0}, {"Type 6", 0, EN_NCX - 1, 0}, {"Type 7", 0, EN_NCX - 1, 0}, {"Type 8", 0, EN_NCX - 1, 0},
    {"Play", 0, 6, 1},
    {"Kick steps", -32768, 32767, 0}, {"Shaker steps", -32768, 32767, 0}, {"Open steps", -32768, 32767, 0},
    {"Rim steps", -32768, 32767, 0},  {"Clap steps", -32768, 32767, 0},
    {"Kick acc", -32768, 32767, 0},   {"Shaker acc", -32768, 32767, 0},   {"Open acc", -32768, 32767, 0},
    {"Rim acc", -32768, 32767, 0},    {"Clap acc", -32768, 32767, 0},
    {"Kick chance", 0, 100, 100}, {"Shaker chance", 0, 100, 100}, {"Open chance", 0, 100, 100}, {"Rim chance", 0, 100, 100},
    {"Clap chance", 0, 100, 5},
    {"Accent", 0, 100, 50}, {"Drum play", 0, 1, 0},
    {"Noise tone", -50, 50, 0}, {"Noise density", 0, 100, 56}, {"Air", 0, 100, 66}, {"Noise drift", -50, 50, 0},
    {"Noise", 0, EN_NNCHAR - 1, 0},
    {"Throws", 0, 100, 50},   {"Wet", 0, 100, 50},    {"Notes", 0, 100, 100},   {"Hats level", 0, 100, 50},
    {"Chord wet", 0, 100, 50}, {"Kick density", 0, 100, 100}, {"Blur", 0, 100, 50}, {"Echo tone", 0, 100, 50},
    {"Width", 0, 100, 50},    {"Noise decay", 0, 100, 50},
    {"Noise width", 0, 100, 50}, {"Noise plate", 0, 100, 50}, {"Noise echo", 0, 100, 50}, {"Hat len", -50, 50, 0},
    {"Noise LFO", 0, 100, 0}, {"Rumble", 0, 100, 0},
    {"Chord rate", 0, EN_LFO_DRIFT - 1, 2}, {"Noise rate", 0, EN_LFO_DRIFT - 1, 2},
    {"Chord shape", 0, EN_NLSHAPE - 1, 0}, {"Noise shape", 0, EN_NLSHAPE - 1, 0},
    {"Chord drift", 0, 100, 0}, {"Noise wander", 0, 100, 0},
    {"Chord vary", 0, 100, 50}, {"Chord LFO to", 0, EN_NCLDEST - 1, 0},
    {"Pings vary", 0, 100, 50}, {"Bass vary", 0, 100, 50}, {"Drums vary", 0, 100, 50}, {"Noise vary", 0, 100, 50},
    {"Cutoff ofs", -50, 50, 0},
    {"Chord echo", 0, 100, 50}, {"Chord plate", 0, 100, 50}, {"Pings echo", 0, 100, 50}, {"Pings plate", 0, 100, 50},
    {"Bass echo", 0, 100, 0},   {"Bass plate", 0, 100, 50},  {"Hats echo", 0, 100, 50},  {"Hats plate", 0, 100, 50},
    {"Chord low cut", 0, 100, 0}, {"Pings low cut", 0, 100, 0}, {"Bass low cut", 0, 100, 0}, {"Hats low cut", 0, 100, 0},
    {"Noise low cut", 0, 100, 0},
    {"Kick level", 0, 100, 50}, {"Kick echo", 0, 100, 0}, {"Kick plate", 0, 100, 0}, {"Kick low cut", 0, 100, 0},
    {"Noise LFO to", 0, EN_NNLDEST - 1, 0},
    {"Song mode", 0, EN_NSMODE - 1, 0}, {"Intro", 0, 16, 4}, {"Main", 4, 64, 20}, {"Outro", 0, 16, 4},
    {"Intro kind", 0, EN_NSKIND - 1, 0},
};
const en_param_t *en_param_info(int i) { return (i >= 0 && i < P_NPARAMS) ? &PARAMS[i] : &PARAMS[0]; }

/* the pieces: eight recipes for the whole machine (the FM-1's PRESETS, the web app's cards). Tune and
 * MIDI out are taken from here by neither: they belong to the player, not the piece */
/*                  Cho Pin Bas Dru  Den Evo Dri Hit  Ech ET Rev Fdb  Cut Dec PTo Mot  Tun MI Tem Rt Md Oc
 *                  CV BO Grv Fif Swi Hat  Stb Crk Dck Siz  Clo PV BV Kit
 *                  then the sound pages' sixteen, all 0: the voices as they are; then the noise's five (its controls, its character);
 *                  then Throws, Ghost, Notes, Hats level (50, 50, 100, 50: the pieces as they were);
 *                  then the chord's Ghost, the kick's density, the echo's Blur, Tone and Width, the noise's Decay;
 *                  then the noise's Width, Plate and Echo, the hats' length; then the noise's LFO, the kick's rumble;
 *                  then the chord's and the noise's LFO rates, their waves (0 Sine), their drifts (the random walk);
 *                  then the chord's LFO's rate and the noise's (0..5: 1 2 4 8 16 32 bars, 6: Drift; 1.4.0);
 *                  then the chord's Vary (50: as Evolve alone turned its stabs), its LFO's destination (0: Cutoff);
 *                  then the pings', the bass's, the drums' and the noise's Vary (50: as Evolve alone), the chord's
 *                  Cutoff knob (0: the piece's own, Cut above; 1.4.0);
 *                  then the channel strips' sends (the chord's, the pings', the bass's, the drums' Echo and Plate: 50 as
 *                  they sent before, the bass's echo 0, none before) and their Low cuts (0, off; 1.4.0);
 *                  then the Kick strip's: its level (50: x1), its Echo and Plate (0: none, as before), its Low cut (0);
 *                  then the noise's LFO's destination (0: Filter, as before; 1.4.0);
 *                  then Song mode (0, Endless: the player's, as Tune), the song's Intro, Main and Outro in phrases (4,
 *                  20, 4: 32, 160 and 32 bars) and its intro's Kind (0 Ambient first, 1 Pulse first; 1.4.0)
 * (Dri: Drift, 2 4 8 16 32 64 bars or Hold; ET: Echo time, 1/8 1/8. 1/4 1/4. 1/2 1 bar; Grv: the knob's, Drone,
 *  Offbeat, Dotted, Sync, Roller (GROOVE_NAME); CV, PV, BV, Kit: the voices, EN_CV_NAME ...) */
static const struct {
    const char *name;
    int16_t v[P_NPARAMS];
} PIECES[EN_NPIECES] = {
    {"Basic",      {52, 30, 68, 80, 30, 55, 5, 10, 55, 1, 35, 62, 40, 25, 40, 30, 0, 1, 122, 5, 1, 0,
                    0, 0, 1, 20, 5, 50, 30, 8, 50, 55, 30, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    0, 56, 66, 0, 0,
                    70, 70, 90, 40,
                    45, 100, 25, 50, 40, 50,
                    50, 50, 50, 0,
                    25, 21,
                    2, 2,
                    0, 0,
                    30, 20,
                    50, 0,
                    50, 50, 50, 50, 0,
                    50, 50, 50, 50, 0, 50, 50, 50,
                    0, 0, 0, 0, 0,
                    50, 0, 0, 0,
                    0,
                    0, 4, 20, 4, 1}},
    {"Chamber",    {55, 20, 62, 72, 20, 35, 6, 40, 70, 4, 45, 72, 50, 55, 30, 30, 0, 1, 128, 9, 5, 0,
                    2, 0, 0, 10, 0, 40, 5, 12, 45, 70, 45, 2, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    -5, 44, 78, 0, 3,
                    70, 60, 100, 55,
                    35, 75, 50, 50, 60, 50,
                    50, 50, 50, 0,
                    30, 35,
                    2, 3,
                    0, 0,
                    45, 25,
                    50, 0,
                    50, 50, 50, 50, 0,
                    50, 50, 50, 50, 0, 50, 50, 50,
                    0, 0, 0, 0, 0,
                    50, 0, 0, 0,
                    0,
                    0, 4, 20, 4, 0}},
    {"Steppers",   {52, 35, 80, 82, 35, 35, 6, 45, 50, 1, 30, 50, 55, 25, 50, 25, 0, 1, 126, 2, 1, 0,
                    0, 0, 2, 35, 45, 60, 25, 5, 60, 50, 20, 1, 1, 2,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    5, 63, 61, 0, 0,
                    70, 70, 90, 60,
                    50, 100, 30, 50, 45, 50,
                    50, 50, 50, 0,
                    30, 14,
                    1, 2,
                    0, 0,
                    25, 20,
                    50, 0,
                    50, 50, 50, 50, 0,
                    50, 50, 50, 50, 0, 50, 50, 50,
                    0, 0, 0, 0, 0,
                    50, 0, 0, 0,
                    0,
                    0, 4, 20, 4, 1}},
    {"Fog",        {50, 34, 58, 62, 40, 55, 5, 42, 75, 2, 55, 72, 46, 80, 25, 72, 0, 1, 120, 7, 5, 0,
                    3, -1, 0, 15, 0, 30, 5, 30, 35, 80, 60, 0, 0, 3,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    -10, 44, 75, 10, 1,
                    70, 65, 100, 50,
                    55, 75, 50, 50, 85, 50,
                    85, 50, 50, 0,
                    0, 62,
                    2, 2,
                    0, 0,
                    45, 35,
                    50, 0,
                    50, 50, 50, 50, 0,
                    50, 50, 50, 50, 0, 50, 50, 50,
                    0, 0, 0, 0, 0,
                    50, 0, 0, 0,
                    0,
                    0, 4, 20, 4, 0}},
    {"Concrete",   {58, 45, 70, 85, 25, 50, 4, 55, 50, 1, 25, 62, 60, 20, 60, 35, 0, 1, 136, 0, 7, 0,
                    6, 0, 4, 30, 10, 75, 30, 5, 50, 45, 15, 2, 3, 2,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    0, 56, 66, 10, 0,
                    70, 65, 85, 65,
                    50, 100, 20, 50, 55, 50,
                    50, 50, 50, 0,
                    45, 56,
                    2, 1,
                    0, 0,
                    35, 25,
                    50, 0,
                    50, 50, 50, 50, 0,
                    50, 50, 50, 50, 0, 50, 50, 50,
                    0, 0, 0, 0, 0,
                    50, 0, 0, 0,
                    0,
                    0, 4, 20, 4, 1}},
    {"Night Drive", {48, 45, 68, 78, 45, 60, 3, 35, 60, 3, 35, 65, 50, 40, 45, 45, 0, 1, 120, 10, 1, 0,
                    1, 0, 1, 25, 13, 55, 20, 15, 50, 60, 35, 4, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    0, 38, 58, 0, 1,
                    80, 75, 90, 58,
                    55, 100, 50, 50, 85, 50,
                    50, 50, 50, 0,
                    40, 49,
                    3, 3,
                    0, 0,
                    40, 25,
                    50, 0,
                    50, 50, 50, 50, 0,
                    50, 50, 50, 50, 0, 50, 50, 50,
                    0, 0, 0, 0, 0,
                    50, 0, 0, 0,
                    0,
                    0, 4, 20, 4, 0}},
    {"Pressure",   {55, 20, 75, 92, 20, 45, 6, 38, 55, 1, 20, 55, 48, 30, 35, 25, 0, 1, 126, 4, 2, 0,
                    5, 0, 1, 10, 5, 65, 25, 30, 70, 40, 40, 3, 4, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    -10, 19, 69, -10, 1,
                    70, 70, 90, 55,
                    60, 100, 50, 50, 75, 50,
                    85, 50, 50, 0,
                    20, 42,
                    2, 2,
                    0, 0,
                    25, 20,
                    50, 0,
                    50, 50, 50, 50, 0,
                    50, 50, 50, 50, 0, 50, 50, 50,
                    0, 0, 0, 0, 0,
                    50, 0, 0, 0,
                    0,
                    0, 4, 20, 4, 1}},
    {"Haze",       {45, 42, 58, 55, 50, 60, 4, 35, 80, 3, 60, 70, 45, 80, 25, 62, 0, 1, 116, 3, 0, 1,
                    3, 0, 0, 20, 10, 35, 5, 20, 30, 85, 70, 2, 0, 4,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    5, 69, 72, 15, 2,
                    70, 70, 100, 50,
                    50, 50, 50, 50, 70, 50,
                    50, 50, 50, 0,
                    0, 28,
                    2, 2,
                    0, 0,
                    45, 30,
                    50, 0,
                    50, 50, 50, 50, 0,
                    50, 50, 50, 50, 0, 50, 50, 50,
                    0, 0, 0, 0, 0,
                    50, 0, 0, 0,
                    0,
                    0, 4, 20, 4, 0}},
};

const char *en_piece_name(int i) { return PIECES[(unsigned)i < EN_NPIECES ? i : 0].name; }
void en_piece(int i, int16_t *par, uint16_t *pool)
{
    int p;
    i = (unsigned)i < EN_NPIECES ? i : 0;
    for (p = 0; p < P_NPARAMS; p++)
        par[p] = PIECES[i].v[p];
    *pool = EN_MODE_POOL[PIECES[i].v[P_MODE]];
}

const char *const EN_CV_NAME[EN_NCV] = {"Saws", "Keys", "Organ", "Pad", "Bell", "Tonewheel", "Noise"};
const char *const EN_LFO_SHAPE[EN_NLSHAPE] = {"Sine", "Triangle", "Ramp up", "Ramp down"};
const char *const EN_CLDEST_NAME[EN_NCLDEST] = {"Cutoff", "Reso", "Colour"};
const char *const EN_NLDEST_NAME[EN_NNLDEST] = {"Filter", "Tone", "Air"};
const char *const EN_SMODE_NAME[EN_NSMODE] = {"Endless", "Song", "Set"};
const char *const EN_SKIND_NAME[EN_NSKIND] = {"Ambient", "Pulse"};
const char *const EN_PV_NAME[EN_NPV] = {"Blip", "Marimba", "Glass", "Wood", "Drop"};
const char *const EN_BV_NAME[EN_NBV] = {"Sub", "Punch", "Rubber", "Pluck", "Growl"};
const char *const EN_KIT_NAME[EN_NKIT] = {"Tight", "Deep", "Punch", "Soft", "Dusty"};
const char *const EN_CQ_NAME[EN_NCQ] = {"Empty", "Major", "Minor", "Sus2", "Sus4", "Dim", "Aug"};
const char *const EN_CX_NAME[EN_NCX] = {"Triad", "6", "7", "Maj7", "9", "11", "Add9"};
static const char *const ETIME_NAME[6] = {"1/8", "1/8.", "1/4", "1/4.", "1/2", "1 bar"};
static const float ETIME_BEATS[6] = {0.5f, 0.75f, 1.0f, 1.5f, 2.0f, 4.0f};
/* the Groove knob (P_GROOVE): the five patterns from sparse to busy (GROOVE_PAT: the engine's GROOVE below); Notes
 * (P_BNOTES) how many of its notes play (the rest left out at random, bar by bar); Energy (PERFORM) thins it further,
 * or fills a thin one */
#define NGROOVE 5
static const uint8_t GROOVE_PAT[NGROOVE] = {0, 1, 2, 4, 3};
static const char *const GROOVE_NAME[NGROOVE] = {"Drone", "Offbeat", "Dotted", "Sync", "Roller"};
/* the bass's grooves (bit i: a note on sixteenth i) and how long each note holds, in sixteenths (Offbeat's
 * held past the next kick, and Roller's nearly to its next note: the kick lands on a bass the duck shapes, not
 * in silence) */
static const uint16_t GROOVE[5] = {0x0001u, 0x4444u, 0x0909u, 0x4949u, 0x0481u};
static const float GROOVE_GATE[5] = {15.0f, 3.25f, 2.2f, 1.8f, 2.6f};
static const float GROOVE_LVL[5] = {1.0f, 0.7f, 1.0f, 0.9f, 1.0f};   /* (the longer ones lower: Offbeat's held more
                                                                      * than twice as long, and under the kick) */
static const uint8_t DRIFT_BARS[7] = {2, 4, 8, 16, 32, 64, 0};

uint32_t en_groove(int g) { return GROOVE[(unsigned)g < 5u ? g : 0]; }
int en_groove_pat(int knob) { return GROOVE_PAT[(unsigned)knob < NGROOVE ? knob : 0]; }
int en_drift_bars(int v) { return DRIFT_BARS[(unsigned)v < 7u ? v : 6]; }
int en_lfo_bars(int v) { return (unsigned)v < EN_LFO_DRIFT ? 1 << v : 0; }   /* (1, 2, 4 .. 32; Drift 0) */

static char *itoa_s(int v, char *b)
{
    char t[8];
    int n = 0, neg = v < 0;
    if (neg)
        v = -v;
    do {
        t[n++] = (char)('0' + v % 10);
        v /= 10;
    } while (v && n < 6);
    if (neg)
        *b++ = '-';
    while (n)
        *b++ = t[--n];
    *b = 0;
    return b;
}

void en_param_text(int i, int v, char *b)
{
    static const char *const ONOFF[2] = {"Off", "On"};
    const char *s = 0;
    if (i == P_ETIME)
        s = ETIME_NAME[(unsigned)v < 6u ? v : 0];
    else if (i == P_MIDI)
        s = ONOFF[v ? 1 : 0];
    else if (i == P_CVOICE)
        s = EN_CV_NAME[(unsigned)v < EN_NCV ? v : 0];
    else if (i == P_PVOICE)
        s = EN_PV_NAME[(unsigned)v < EN_NPV ? v : 0];
    else if (i == P_BVOICE)
        s = EN_BV_NAME[(unsigned)v < EN_NBV ? v : 0];
    else if (i == P_KIT)
        s = EN_KIT_NAME[(unsigned)v < EN_NKIT ? v : 0];
    else if (i >= P_CR0 && i <= P_CR7)
        s = EN_NOTE_NAME[(unsigned)v < 12u ? v : 0];
    else if (i >= P_CQ0 && i <= P_CQ7)
        s = EN_CQ_NAME[(unsigned)v < EN_NCQ ? v : 0];
    else if (i >= P_CX0 && i <= P_CX7)
        s = EN_CX_NAME[(unsigned)v < EN_NCX ? v : 0];
    else if (i == P_PMODE)
        s = v & EN_PLOOP ? "Loop" : "Endless";
    else if (i == P_NCHAR)
        s = EN_NCHAR_NAME[(unsigned)v < EN_NNCHAR ? v : 0];
    else if (i == P_DMODE)
        s = v ? "Pattern" : "Auto";
    else if (i == P_GROOVE)
        s = GROOVE_NAME[(unsigned)v < NGROOVE ? v : 0];
    else if (i == P_MODE)
        s = EN_MODE_NAME[(unsigned)v < EN_NMODES ? v : 0];
    else if (i == P_ROOT)
        s = EN_NOTE_NAME[(unsigned)v < 12u ? v : 0];
    else if (i == P_DRIFT && !en_drift_bars(v))
        s = "Hold";
    else if ((i == P_CLRATE || i == P_NLRATE) && !en_lfo_bars(v))
        s = "Drift";                             /* (1.4.0: the LFOs' rates, the last a wander) */
    else if ((i == P_CLRATE || i == P_NLRATE) && en_lfo_bars(v) == 1)
        s = "1 bar";
    else if (i == P_CLSHAPE || i == P_NLSHAPE)
        s = EN_LFO_SHAPE[(unsigned)v < EN_NLSHAPE ? v : 0];
    else if (i == P_CLDEST)
        s = EN_CLDEST_NAME[(unsigned)v < EN_NCLDEST ? v : 0];   /* (1.4.0: where the chord's LFO goes) */
    else if (i == P_NLDEST)
        s = EN_NLDEST_NAME[(unsigned)v < EN_NNLDEST ? v : 0];   /* (1.4.0: where the noise's LFO goes) */
    else if (i == P_SMODE)
        s = EN_SMODE_NAME[(unsigned)v < EN_NSMODE ? v : 0];   /* (1.4.0: song structure) */
    else if (i == P_SKIND)
        s = EN_SKIND_NAME[(unsigned)v < EN_NSKIND ? v : 0];
    else if ((i == P_SINTRO || i == P_SOUTRO) && !v)
        s = "None";
    if (s) {
        int k = 0;
        while (s[k] && k < 9) {
            b[k] = s[k];
            k++;
        }
        b[k] = 0;
        return;
    }
    if (i == P_DRIFT || i == P_CLRATE || i == P_NLRATE || i == P_SINTRO || i == P_SMAIN || i == P_SOUTRO) {
        b = itoa_s(i == P_DRIFT ? en_drift_bars(v) : i >= P_SINTRO ? 8 * v : en_lfo_bars(v), b);   /* (the song's:
                                                  * phrases, shown as bars) */
        *b++ = ' ';
        *b++ = 'b';
        *b++ = 'a';
        *b++ = 'r';
        *b++ = 's';
        *b = 0;
        return;
    }
    if (i == P_FEEDBACK) {                       /* (1.3.0) as the tail's length, to -60 dB: 1.5 .. 15 s */
        int ds = (int)(15.0f * fm_exp2f((float)v * (1.0f / 30.0f)) + 0.5f);   /* (in tenths) */
        b = itoa_s(ds / 10, b);
        *b++ = '.';
        *b++ = (char)('0' + ds % 10);
        *b++ = ' ';
        *b++ = 's';
        *b = 0;
        return;
    }
    if (i == P_SWING) {                          /* as a swing percentage: 50% (straight) .. 70% */
        b = itoa_s(50 + v / 5, b);
        *b++ = '%';
        *b = 0;
        return;
    }
    if ((i == P_CALG || i == P_PALG || i == P_BALG)) {
        if (!v) {
            b[0] = 'V', b[1] = 'o', b[2] = 'i', b[3] = 'c', b[4] = 'e', b[5] = 0;   /* the voice's own */
        } else if (v == 9) {
            b[0] = '2', b[1] = '-', b[2] = 'o', b[3] = 'p', b[4] = 0;
        } else {
            itoa_s(v, b);
        }
        return;
    }
    if ((i == P_TUNE || i == P_POCT || i == P_BASSOCT || (i >= P_CATT && i < P_CR0 && i != P_CFDEC) ||
         (i >= P_NCOL && i <= P_NDRIFT && i != P_NGRAIN && i != P_NROOM)) && v > 0)
        *b++ = '+';                              /* (1.4.0: the chord's LFO a depth, 0..100, as the noise's: no sign) */
    itoa_s(v, b);
}

/* ------------------------------------------------------- the pure parts --- */
uint32_t en_euclid(int k, int n, int rot)
{
    /* Bjorklund's algorithm (as Toussaint's "The Euclidean Algorithm Generates Traditional Musical
     * Rhythms"): k groups "x" and n - k groups ".", the remainders folded onto the heads until at most
     * one remains. Groups are bit strings, LSB first: (bits, length). */
    uint32_t gb[32], m = 0;
    uint8_t gl[32];
    int ng, heads, tails, i, pos = 0;
    if (n <= 0 || n > 32 || k <= 0)
        return 0;
    if (k > n)
        k = n;
    for (i = 0; i < n; i++) {
        gb[i] = i < k ? 1u : 0u;
        gl[i] = 1;
    }
    ng = n;
    heads = k;
    tails = n - k;
    while (tails > 1 && heads > 0) {
        int pairs = heads < tails ? heads : tails;
        for (i = 0; i < pairs; i++) {           /* a tail onto each head */
            gb[i] |= gb[ng - pairs + i] << gl[i];
            gl[i] = (uint8_t)(gl[i] + gl[ng - pairs + i]);
        }
        if (heads > tails) {                    /* the unpaired heads become the new tails */
            tails = heads - pairs;
            heads = pairs;
        } else {
            tails -= pairs;
            heads = pairs;
        }
        ng = heads + tails;
    }
    for (i = 0; i < ng; i++) {
        uint32_t j;
        for (j = 0; j < gl[i]; j++)
            if (gb[i] >> j & 1u)
                m |= 1u << (((pos + (int)j + rot) % n + n) % n);
        pos += gl[i];
    }
    return m;
}

int en_scale(uint32_t pool, int *steps)
{
    int i, m = 0;
    pool |= 1u;
    for (i = 0; i < 12; i++)
        if (pool >> i & 1u)
            steps[m++] = i;
    return m;
}

/* the chord's tones in stacking order: the root, then every other scale step (thirds in a seven-note
 * scale, fourths in a pentatonic), then what's left; up to max of them, distinct */
static int stack(uint32_t pool, int root, int deg, int *pcs, int max)
{
    static const uint8_t OFF[7] = {0, 2, 4, 6, 1, 3, 5};
    int st[12], m = en_scale(pool, st), n = 0, i, j;
    deg = ((deg % m) + m) % m;
    for (i = 0; i < 7 && n < max; i++) {
        int pc = (root + st[(deg + OFF[i]) % m]) % 12, dup = 0;
        for (j = 0; j < n; j++)
            dup |= pcs[j] == pc;
        if (!dup)
            pcs[n++] = pc;
    }
    return n;
}

int en_chord_tones(uint32_t pool, int root, int deg, int *pcs)
{
    int st[12];
    return stack(pool, root, deg, pcs, en_scale(pool, st) >= 6 ? 4 : 3);
}

void en_voicing(uint32_t pool, int deg, int shape, int *iv)
{
    int t[5], n = stack(pool, 0, deg, t, 5), i, j, r = t[0], d[5];
    for (i = 0; i < 5; i++)                      /* intervals above the chord's root; a short pool repeats */
        d[i] = (t[i < n ? i : i % n] - r + 12) % 12;
    if (shape) {                                 /* root, 7th, 10th, 14th: a 9th without its fifth */
        iv[0] = 0;
        iv[1] = d[3];
        iv[2] = d[1] + 12;
        iv[3] = d[n >= 5 ? 4 : 2] + 12;
    } else {                                     /* root, 5th, 7th, 10th: the open 7th */
        iv[0] = 0;
        iv[1] = d[2];
        iv[2] = d[3];
        iv[3] = d[1] + 12;
    }
    for (i = 1; i < 4; i++) {                    /* ascending, no two alike */
        for (j = i; j > 0 && iv[j] < iv[j - 1]; j--) {
            int x = iv[j];
            iv[j] = iv[j - 1];
            iv[j - 1] = x;
        }
    }
    for (i = 1; i < 4; i++)
        while (iv[i] <= iv[i - 1])
            iv[i] += 12;
}

const char *en_chord_name(const int *pcs, int n, char *buf)
{
    uint32_t iv = 0;
    int i, k = 0, third, fifth;
    const char *q = "", *r = EN_NOTE_NAME[pcs[0] % 12];
    for (i = 1; i < n; i++)
        iv |= 1u << ((pcs[i] - pcs[0] + 12) % 12);
    third = iv >> 4 & 1u ? 4 : iv >> 3 & 1u ? 3 : 0;
    fifth = iv >> 7 & 1u ? 7 : iv >> 6 & 1u ? 6 : iv >> 8 & 1u ? 8 : 0;
    if (third == 4) {
        q = fifth == 8 ? "aug" : iv >> 11 & 1u ? "M7" : iv >> 10 & 1u ? "7" : iv >> 9 & 1u ? "6"
            : iv >> 2 & 1u ? "add9" : "";
    } else if (third == 3) {
        if (fifth == 6)
            q = iv >> 10 & 1u ? "m7b5" : iv >> 9 & 1u ? "dim7" : "dim";
        else
            q = iv >> 10 & 1u ? "m7" : iv >> 11 & 1u ? "mM7" : iv >> 9 & 1u ? "m6" : iv >> 2 & 1u ? "madd9" : "m";
    } else {
        q = iv >> 5 & 1u ? (iv >> 10 & 1u ? "7sus4" : "sus4") : iv >> 2 & 1u ? "sus2" : "5";
    }
    while (*r)
        buf[k++] = *r++;
    while (*q)
        buf[k++] = *q++;
    buf[k] = 0;
    return buf;
}

/* ------------------------------------------------------------ commands --- */
enum { C_SET, C_POOL, C_PLAY, C_NEXT, C_FREEZE, C_SEED, C_PANIC, C_CLOCK, C_REGROW, C_MUTE, C_PERF, C_FX, C_PINGS, C_MACRO, C_PROT, C_REWALK, C_PNUDGE, C_LOCK, C_OUTRO };
#define QN 512u                                 /* (a piece is every parameter at once: room for twice over and more) */
_Static_assert(QN >= 2u * (P_NPARAMS + 8), "the command queue can't hold a piece's parameters twice over");
static uint32_t q[QN];
static volatile uint32_t q_w, q_r;
static void post(uint32_t c, int a, int b)
{
    uint32_t w = q_w;
    if (w - q_r >= QN)
        return;                                   /* full: dropped (never, at the UI's rate) */
    q[w % QN] = c << 24 | ((uint32_t)a & 0xFFu) << 16 | ((uint32_t)b & 0xFFFFu);
    BARRIER();
    q_w = w + 1;
}
void en_set(int p, int v) { post(C_SET, p, v); }
void en_pool(uint32_t pool) { post(C_POOL, 0, (int)(pool & 0xFFFu)); }
void en_play(int on) { post(C_PLAY, on, 0); }
void en_next(void) { post(C_NEXT, 0, 0); }
void en_freeze(int on) { post(C_FREEZE, on, 0); }
void en_seed(uint32_t s) { post(C_SEED, 0, (int)(s & 0xFFFFu)); }
void en_panic(void) { post(C_PANIC, 0, 0); }
void en_clock(int msg) { post(C_CLOCK, msg, 0); }
void en_regrow(void) { post(C_REGROW, 0, 0); }
void en_mute(uint32_t m) { post(C_MUTE, (int)(m & 0xFFu), 0); }
void en_perf(uint32_t f) { post(C_PERF, (int)(f & 0xFFu), 0); }
void en_fx(uint32_t m) { post(C_FX, 0, (int)(m & 0xFFFFu)); }
void en_macro(int i, int v) { post(C_MACRO, i, v + 50); }   /* (sent as 0..100) */
void en_lock(int on) { post(C_LOCK, on != 0, 0); }
void en_outro(void) { post(C_OUTRO, 0, 0); }
void en_prog_rotate(void) { post(C_PROT, 0, 0); }
void en_prog_rewalk(void) { post(C_REWALK, 0, 0); }
void en_ping_nudge(void) { post(C_PNUDGE, 0, 0); }
const char *const EN_FX_NAME[EN_NFX] = {"Throw", "Time x2", "Tape stop", "Swell", "Low-pass", "High-pass", "Wah",
                                        "Phone", "Verb", "Crash", "Freeze", "Wash", "Stutter", "Gate", "Siren", "Riser"};

/* -------------------------------------------------------------- state --- */
volatile uint8_t en_playing, en_frozen, en_ext;
volatile uint8_t en_chord[EN_NPAD], en_chord_root, en_chord_n;
volatile float en_drift, en_pad_level, en_bass_level;
volatile uint8_t en_loop_pos[EN_NLOOP];
volatile int8_t en_loop_note[EN_NLOOP][EN_LOOP_MAX];
volatile float en_loop_glow[EN_NLOOP];
volatile uint8_t en_drum_pos[EN_NDRUM];
volatile uint32_t en_drum_pat[EN_NDRUM];
volatile float en_drum_glow[EN_NDRUM];
volatile uint32_t en_bars;
volatile float en_arc;
volatile uint8_t en_pulse_rest;
volatile uint8_t en_pad_note[EN_NPAD];
volatile float en_pad_amp[EN_NPAD];
volatile uint8_t en_section, en_phrase_bar;
volatile uint32_t en_stab_pat, en_throws, en_stabs;
volatile float en_stab_vel, en_echo_ms, en_echo_g, en_echo_in, en_duck;
volatile uint8_t en_stab_thrown;
volatile uint16_t en_lane[6];
volatile float en_bass_gate = 3.0f;
volatile int8_t en_bass_octn = -1;
volatile int8_t en_bass_off[16];
volatile float en_stab_t60 = 0.3f;
volatile uint32_t en_nz_pat;
volatile uint8_t en_nz_vel[32], en_nz_step;
volatile float en_nz_t60 = 0.3f;
volatile uint16_t en_lane_sh, en_lane_op, en_lane_png;
volatile uint8_t en_kick_v[16], en_hat_v[16];
volatile uint16_t en_stab_thr;
volatile uint8_t en_stab_v[16];
volatile int16_t en_scope[EN_SCOPE_N];
volatile uint32_t en_scope_w;
volatile int16_t en_spec[EN_SPEC_N];
volatile uint32_t en_spec_w;
volatile uint8_t en_view_layer;
volatile uint32_t en_hits[5];
volatile float en_hit_vel[5];
volatile float en_echo_pk;
volatile float en_meter[EN_NMETER], en_duck_lo[2] = {1.0f, 1.0f};
volatile uint8_t en_meter_on;
volatile uint8_t en_load;
static int32_t shed_left;                        /* samples the shedding lasts (a heavy block: half a second) */
#define SHED (shed_left > 0)
volatile uint8_t en_nev_k[EN_NEV_N], en_nev_s[EN_NEV_N], en_nev_p[EN_NEV_N];
volatile uint32_t en_nev_w;
static void nev(int k, float s, float p)         /* a noise event, for the screen (only the ones that show) */
{
    uint32_t w = en_nev_w;
    if (s < 0.08f)
        return;
    en_nev_k[w % EN_NEV_N] = (uint8_t)k;
    en_nev_s[w % EN_NEV_N] = (uint8_t)(fm_minf(s, 1.0f) * 255.0f);
    en_nev_p[w % EN_NEV_N] = (uint8_t)(fm_clampf(p, 0.0f, 1.0f) * 255.0f);
    en_nev_w = w + 1u;
}
static void hit(int l, float vel) { en_hit_vel[l] = vel; en_hits[l]++; }
static float scope_acc, spec_acc;
static uint32_t scope_ph;
static uint32_t mutes, perf, fxm, fxm_was;      /* performance: the layers muted, the moves held; the effects held */
static float perf_sw, perf_echo, mb_c[4], mb_s[4];   /* (mb: Open's and Shut's filter on the music, the kick left out) */                /* the filter pushed open (+) or shut (-), the echo pushed */

static int16_t par[P_NPARAMS];
/* the noise's controls (P_NCOL .. P_NCHAR: render_crackle reads them) */
typedef struct {
    float colour, grain, room, drift;           /* dark .. bright; smooth .. granular; the Air's gain (0 .. 2.3); steady
                                                 * .. restless */
    uint8_t character;                          /* NCHAR[] */
    uint8_t hits;                               /* (1.4.0, format 18) Density: the hits in two bars, 0..16 (0: none, the
                                                 * clicks gone too); grain, the clicks' density, from it (nz_dens) */
} noise_ctl_t;
static noise_ctl_t nctl;
static float throw_mul = 1.0f, ping_dry = 0.5f, hats_lvl = 1.0f;   /* Throws, Ghost, Hats level (apply) */
static float chord_dry = 0.5f, chord_snd = 1.5f;   /* the chord's Ghost: its dry, its sends (apply) */
/* (1.4.0, the Arrangement page's channel strips) each part's sends, the chord's, the pings', the bass's and the drums'
 * Echo and Plate: what each sent before, times this (1 at 50); each part's Low cut, a one-pole's coefficient (0 off:
 * the chord, the pings, the bass, the drums, the noise; lcut4 the noise's rings', at their quarter rate) */
static float sx[4][2] = {{1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}}, lcut[6], lcut4;
/* (1.4.0, format 24, the user's: the kick and the hats apart) sx[3] and lcut[3] the Hats strip's (the shaker, the open hat,
 * the rim, the clap); lcut[5] the Kick strip's Low cut, kick_lvl its level (x1 at 50), kick_snd its Echo and Plate
 * (absolute: the kick sent nothing before); hat_thr the hats' throw, eased (PERFORM's, EN_PF_HATTHROW) */
static float kick_lvl = 1.0f, kick_snd[2], hat_thr;
static float ec_g = 0.6f, ec_lpc = 0.68f, ec_wide = 1.0f;   /* the echo's Blur, Tone, Width (and the chord's and the
                                                             * pings': widen) */
static int gpat(void) { return GROOVE_PAT[(unsigned)par[P_GROOVE] < NGROOVE ? par[P_GROOVE] : 0]; }   /* the pattern */
static int8_t mac[EN_NMAC];                     /* the master actions, -50..50 */
static int8_t pofs[P_NPARAMS];
static float dub_up;                            /* Dub turned up, 0..1: the echo darker, feeding back past its knob */                  /* what they add to the piece's parameters */
static int pe(int p)                            /* a parameter as heard: the piece's, moved by the master actions */
{
    int v = par[p] + pofs[p];
    return v < PARAMS[p].lo ? PARAMS[p].lo : v > PARAMS[p].hi ? PARAMS[p].hi : v;
}
static uint32_t pool = 0xAADu;                  /* Ionian until told */
static int nsteps, steps[12];                   /* the scale */
static int deg;                                 /* the chord's scale degree */
static int chord_pcs[4], chord_nt;
static float tunefac = 1.0f;
static uint32_t rng = 0x9E3779B9u;

static uint32_t rnd(void)
{
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}
static float frand(void) { return (float)(rnd() >> 8) * (1.0f / 16777216.0f); }
static int irand(int n) { return n > 0 ? (int)((rnd() >> 8) % (uint32_t)n) : 0; }

static void midi(uint32_t st, uint32_t d1, uint32_t d2)
{
    if (par[P_MIDI])
        en_midi_out(st, d1, d2);
}

static float rel(int p) { return (float)par[p] * 0.02f; }   /* a sound page's value, -1 .. +1 */

static float mtof(float n) { return 440.0f * tunefac * fm_exp2f((n - 69.0f) * (1.0f / 12.0f)); }

/* a soft clip, cheap: x - 4/27 x^3, flat at +-1.5 (to +-1) */
static inline float soft(float x)                /* tanh, nearly: x (27 + x^2) / (27 + 9 x^2), 1 past 3 */
{
    float x2;
    if (x > 3.0f)
        return 1.0f;
    if (x < -3.0f)
        return -1.0f;
    x2 = x * x;
    return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}
static inline float sat(float x)
{
    if (x > 1.5f)
        return 1.0f;
    if (x < -1.5f)
        return -1.0f;
    return x - 0.148148148f * x * x * x;
}

/* a sine table: 1024 points a turn, plus one */
#define SN 1024
static float sint[SN + 1];
static inline float sine(float ph)              /* ph in turns, any value >= 0 */
{
    float x = (ph - (float)(int)ph) * (float)SN;
    int i = (int)x;
    float f = x - (float)i;
    return sint[i] + (sint[i + 1] - sint[i]) * f;
}

static inline float blep_saw(float t, float dt)
{
    float y = 2.0f * t - 1.0f;
    if (t < dt) {
        float x = t / dt;
        y -= x + x - x * x - 1.0f;
    } else if (t > 1.0f - dt) {
        float x = (t - 1.0f) / dt;
        y -= x * x + x + x + 1.0f;
    }
    return y;
}

static void svf_coefs(float *c, float f, float k)   /* TPT SVF (Zavalishin): a1, a2, a3, k */
{
    float g = fm_tanf(FM_PI * fm_clampf(f, 40.0f, 16000.0f) * (1.0f / EN_SR));
    c[0] = 1.0f / (1.0f + g * (g + k));
    c[1] = g * c[0];
    c[2] = g * c[1];
    c[3] = k;
}

/* ---------------------------------------------------------- the FM core --- */
/* Four sine operators, as the 4-op FM synths have them (the DX21 / TX81Z family's eight algorithms; op 4 can
 * feed back on itself), and a ninth "algorithm", two operators only (2 into 1), for the light voices. Each
 * operator: a ratio to the note, a level (a modulator's: its index, in turns; a carrier's: its share of the
 * sound) and a decay to a sustain (a share of its level). The levels are worked out at the control rate
 * (every 32 samples), the operators every sample. The algorithms, 4 -> 3 meaning 4 modulates 3:
 *   1: 4 -> 3 -> 2 -> 1            2: (3 + 4) -> 2 -> 1         3: (3 -> 2) + 4 -> 1
 *   4: (4 -> 3) + 2 -> 1           5: 4 -> 3, 2 -> 1 (two pairs) 6: 4 -> 1, 2 and 3
 *   7: 4 -> 3, and 2 and 1         8: all four, side by side     9: 2 -> 1 (two operators) */
#define FM_NALG 9
#define FM_ALG2 8                                /* the two-operator one */
static const uint8_t FM_CARR[FM_NALG] = {1, 1, 1, 1, 5, 7, 7, 15, 1};   /* the carriers (bit k: op k + 1) */
typedef struct {
    float ratio, level, dec, sus;               /* the ratio, the level, the decay (s) to the sustain (0..1 of the level) */
} fmop_t;
typedef struct {
    uint8_t alg;                                /* 0..8 */
    float fb;                                   /* op 4's feedback (turns) */
    fmop_t op[4];
} fmpatch_t;
typedef struct {
    float ph[4], inc[4], e[4], kd[4], lv[4], fb1, fb2;
} fmvoice_t;

/* a note: the operators' rates (rmul: op 2's ratio moved), their envelopes from the top (dmul: the modulators'
 * decays stretched) */
static void fm_note(fmvoice_t *v, const fmpatch_t *p, float f, float rmul, float qmul, float dmul)
{
    int k;
    for (k = 0; k < 4; k++) {
        float r = p->op[k].ratio * (k == 1 ? rmul : k >= 2 ? qmul : 1.0f), d = p->op[k].dec * (k ? dmul : 1.0f);
        v->inc[k] = f * r * (1.0f / EN_SR);
        v->e[k] = 1.0f;
        v->kd[k] = d > 0.0f ? fm_expf(-32.0f / (d * EN_SR)) : 1.0f;
    }
}

static void fm_tune(fmvoice_t *v, const fmpatch_t *p, float f, float rmul, float qmul)   /* a slide: the rates only */
{
    int k;
    for (k = 0; k < 4; k++)
        v->inc[k] = f * p->op[k].ratio * (k == 1 ? rmul : k >= 2 ? qmul : 1.0f) * (1.0f / EN_SR);
}

/* the control rate: the envelopes a step on, the levels (ms: the modulators' scaled, as Colour and Tone have it) */
static void fm_ctl(fmvoice_t *v, const fmpatch_t *p, int alg, float ms)
{
    int k;
    for (k = 0; k < 4; k++) {
        float s = p->op[k].sus;
        v->e[k] = s + (v->e[k] - s) * v->kd[k];
        v->lv[k] = p->op[k].level * v->e[k] * ((FM_CARR[alg] >> k & 1u) ? 1.0f : ms);
    }
}

/* one sample (dk: the pitch now, a multiple of the note's; fb: op 4's feedback) */
static inline float fm_sample(fmvoice_t *v, int alg, float fb, float dk)
{
    float o2, o3, o4, out, *ph = v->ph, *l = v->lv;
    int k;
    if (alg == FM_ALG2) {
        o2 = sine(ph[1] + 8.0f + fb * (v->fb1 + v->fb2)) * l[1];
        v->fb2 = v->fb1;
        v->fb1 = o2;
        out = sine(ph[0] + o2 + 8.0f) * l[0];
        ph[0] += v->inc[0] * dk;
        ph[1] += v->inc[1] * dk;
        for (k = 0; k < 2; k++)
            if (ph[k] >= 1.0f)
                ph[k] -= 1.0f;
        return out;
    }
    o4 = sine(ph[3] + 8.0f + fb * (v->fb1 + v->fb2)) * l[3];   /* (the feedback: the last two, averaged) */
    v->fb2 = v->fb1;
    v->fb1 = o4;
    switch (alg) {
    case 0: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + o3 + 8.0f) * l[1]; out = sine(ph[0] + o2 + 8.0f) * l[0]; break;
    case 1: o3 = sine(ph[2] + 8.0f) * l[2]; o2 = sine(ph[1] + o3 + o4 + 8.0f) * l[1]; out = sine(ph[0] + o2 + 8.0f) * l[0]; break;
    case 2: o3 = sine(ph[2] + 8.0f) * l[2]; o2 = sine(ph[1] + o3 + 8.0f) * l[1]; out = sine(ph[0] + o2 + o4 + 8.0f) * l[0]; break;
    case 3: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + 8.0f) * l[1]; out = sine(ph[0] + o2 + o3 + 8.0f) * l[0]; break;
    case 4: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + 8.0f) * l[1]; out = sine(ph[0] + o2 + 8.0f) * l[0] + o3; break;
    case 5: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + o4 + 8.0f) * l[1]; out = sine(ph[0] + o4 + 8.0f) * l[0] + o2 + o3; break;
    case 6: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + 8.0f) * l[1]; out = sine(ph[0] + 8.0f) * l[0] + o2 + o3; break;
    default: o3 = sine(ph[2] + 8.0f) * l[2]; o2 = sine(ph[1] + 8.0f) * l[1]; out = sine(ph[0] + 8.0f) * l[0] + o2 + o3 + o4; break;
    }
    for (k = 0; k < 4; k++) {
        ph[k] += v->inc[k] * dk;
        if (ph[k] >= 1.0f)
            ph[k] -= 1.0f;
    }
    return out;
}

/* the FM voices' patches: the chord's Keys (an electric piano: a body pair, and a tine pair at 14) and Bell
 * (3.5 against 1, 7 against 2); the basses: Punch (a stack, a thump in the first 90 ms), Rubber (two pairs, an
 * octave under, op 4 feeding back), Pluck (2 ops at 3, gone in 50 ms), Growl (2 ops at 1, a bite) */
static const fmpatch_t FM_KEYS = {4, 0.0f, {{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.3f, 0.35f, 0.15f}, {1.0f, 0.4f, 0.0f, 1.0f},
                                            {14.0f, 0.13f, 0.05f, 0.0f}}};
static const fmpatch_t FM_BELL = {4, 0.0f, {{1.0f, 1.0f, 0.0f, 1.0f}, {3.5f, 0.38f, 0.5f, 0.12f}, {2.0f, 0.45f, 0.0f, 1.0f},
                                            {7.07f, 0.16f, 0.18f, 0.0f}}};
static const fmpatch_t FM_PUNCH = {0, 0.12f, {{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.06f, 0.08f}, {2.0f, 0.22f, 0.12f, 0.1f},
                                             {1.0f, 0.12f, 0.2f, 0.25f}}};
static const fmpatch_t FM_RUBBER = {4, 0.3f, {{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.3f, 0.25f, 0.3f}, {0.5f, 0.6f, 0.0f, 1.0f},
                                              {1.0f, 0.18f, 0.3f, 0.4f}}};
static const fmpatch_t FM_PLUCK = {FM_ALG2, 0.0f, {{1.0f, 1.0f, 0.0f, 1.0f}, {3.0f, 0.55f, 0.035f, 0.0f}, {2.0f, 0.2f, 0.05f, 0.0f},
                                                   {1.0f, 0.1f, 0.1f, 0.0f}}};
static const fmpatch_t FM_GROWL = {FM_ALG2, 0.0f, {{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.44f, 0.07f, 0.09f}, {2.0f, 0.2f, 0.1f, 0.1f},
                                                   {1.0f, 0.1f, 0.1f, 0.0f}}};
static float fm_q(int p) { return fm_exp2f((float)par[p] * 0.02f); }           /* FM page: the upper ratios, x0.5 .. x2 */
static float fm_d(int p) { (void)p; return 1.0f; }   /* the modulators' decay: the patch's own (1.3.0: the knob is the LFO) */
static float fm_fb(int p, const fmpatch_t *pt) { return fm_clampf(pt->fb + 0.004f * (float)par[p], 0.0f, 0.7f); }   /* (1.3.0: more only) */
static int fm_alg_of(int p, const fmpatch_t *pt) { return par[p] ? par[p] - 1 : pt->alg; }

/* ------------------------------------------------- slow random motion --- */
#define NMOD 9                                  /* 0..4 the stab's, 5..8 the echo's */
static struct {
    float v, tgt, rate;
    uint32_t left;                              /* samples until a new target */
} mod[NMOD];

static void mods_step(uint32_t n)
{
    int i;
    for (i = 0; i < NMOD; i++) {
        if (mod[i].left <= n) {
            float secs = 7.0f + 33.0f * frand();
            mod[i].tgt = frand() * 2.0f - 1.0f;
            mod[i].left = (uint32_t)(secs * EN_SR);
            mod[i].rate = (float)n / (secs * EN_SR * 0.5f);
        } else {
            mod[i].left -= n;
        }
        mod[i].v += (mod[i].tgt - mod[i].v) * fm_minf(mod[i].rate, 1.0f);
    }
}
static float motion, evolve;                    /* 0..1: Motion, Evolve */
/* (1.4.0, the user's: each layer's Vary, Evolve over it) how often its part changes, times Evolve's: 1 at 50 (as Evolve
 * alone had it: exactly), 0 at 0, 3 at 100. The pings' loops, the bass's plan, the drums' (the kick, the shaker, the
 * clap), the noise's groove; ev_of: Evolve as that layer feels it */
enum { VY_PINGS, VY_BASS, VY_DRUMS, VY_NOISE, VY_N };
static float vary_l[VY_N] = {1.0f, 1.0f, 1.0f, 1.0f};
static float vary_of(int v) { return v <= 50 ? (float)v / 50.0f : 1.0f + (float)(v - 50) / 25.0f; }
#define ev_of(k) (evolve * vary_l[k])
volatile uint32_t en_vary_turns[VY_N];           /* (for tests) each layer's changes so far: a loop's note moved, a new
                                                  * plan, the shaker's steps traded, a new noise groove */
static float loose(void) { return fm_minf(evolve * 2.5f, 1.0f); }   /* (1.3.0: how loose the playing is; 0 at Evolve 0) */
static float arcm;                              /* the sweep, -1 dark .. +1 open, times Motion */

/* -------------------------------------------------------------- chord --- */
/* The stab: four voices struck together, one envelope, one filter. Saws: two a voice, 7 cents apart;
 * Keys: an FM electric piano, brighter as it's struck. Then a resonant low-pass a side that opens with
 * each stab (its own, quicker envelope) and with the sweep, and a soft clip. */
typedef struct {
    float ph[2], inc[2];
    uint8_t note, midi_note;
} stab_t;
static stab_t stab[EN_NPAD];
static float chord_vol, cut_base = 800.0f, st_dec = 0.9995f, st_fdec = 0.99f, st_t60 = 0.3f;
static float st_env, st_amp, st_fenv, st_throw, throw_dec, st_cofs, st_lc[2];
#ifndef EN_RUMBLE
#define EN_RUMBLE 1                              /* (the kick's rumble: off in 1.3.0; 1.4.0, the second try: on) */
#endif
#define RB_SR (EN_SR / 16.0f)                    /* (the rumble's rate) */
static float rb_amt, rb_g = 0.999f, rb_on = 1.0f;   /* (the kick's rumble: its level, its tail's decay a sample
                                                  * of its rate; rb_on: eased off while shedding) */
static void rumble_init(void);
static float ec_dk, ec_dd;                      /* (the classics pass) a strike ducks the echo's returns: its level,
                                                 * eased; the repeats heard in the gaps, not over the stab */
static float st_s[4], st_c[4], st_h[2];
static int32_t st_midi_left;
static int shape, root_note = 55;               /* the voicing's shape, the chord's root as voiced (MIDI) */
static const float STAB_PAN[EN_NPAD] = {-0.3f, -0.09f, 0.12f, 0.33f};   /* (1.3.0: 60% of the spread) */
static float stab_gl[EN_NPAD], stab_gr[EN_NPAD];
static int32_t gate_ctr;                        /* samples since the sixteenth began (the grid's Gate) */
static float fx_gate, gate_g, gate_dec;
/* the chord's voices: how it sounds (CV_*), how quickly it strikes (the amplitude's smoothing a sample:
 * 0.035 ~0.6 ms .. 0.00012 ~190 ms), its length (times Decay's), how far its own envelope opens the filter
 * (octaves), its modulator's ratio and index (FM voices, in turns: the index at the strike, and at rest),
 * and a level to match the others */
enum { CV_SAWS, CV_KEYS, CV_ORGAN, CV_PAD, CV_BELL, CV_WHEEL, CV_NOISE };
static const struct {
    float att, dec, fenv, ratio, idx_hi, idx_lo, gain;
} CV[EN_NCV] = {
    /* (1.3.0: each strike opens further, and closes sooner) */
    {0.035f, 1.0f, 2.5f, 0.0f, 0.0f, 0.0f, 1.0f},        /* Saws: two saws a voice, 7 cents apart */
    {0.035f, 1.3f, 1.4f, 1.0f, 0.41f, 0.06f, 1.23f},     /* Keys: FM at the carrier's pitch, an electric piano */
    {0.02f, 2.5f, 0.7f, 0.0f, 0.0f, 0.0f, 0.75f},        /* Organ: four drawbars (1, 2, 3, 4) */
    {0.00012f, 4.0f, 0.6f, 0.0f, 0.0f, 0.0f, 0.72f},     /* Pad: the saws, swelling in, long */
    {0.05f, 1.8f, 1.1f, 3.5f, 0.35f, 0.05f, 1.17f},      /* Bell: FM at 3.5, the clang dying first */
    {0.03f, 3.0f, 0.4f, 0.0f, 0.0f, 0.0f, 0.67f},        /* Tonewheel: drawbars, a percussion and a click at the
                                                          * strike, a rotary speaker turning (left against right) */
    {0.05f, 0.6f, 2.6f, 0.0f, 0.0f, 0.0f, 3.6f},         /* Noise: noise and a little saw, the filter's pitch */
};
static int cvoice;                              /* the chord's voice now */
static fmvoice_t cfm[EN_NPAD];                  /* its FM voices' operators */
static const fmpatch_t *cv_patch(int cv) { return cv == 1 ? &FM_KEYS : cv == 4 ? &FM_BELL : 0; }
static float rotor, click;                      /* Tonewheel: the rotor's turn, the key click */

/* ------------------------------------------------------------------ bass --- */
static struct {
    float ph, note, target, env;
    int32_t gate;                               /* samples left of the note */
    uint8_t midi_note;
} bass;
static float bass_vol;
/* the bass's voices: Sub (a sine and a little of its octave, softly clipped), Round (its third harmonic
 * too, rounder), Growl (FM at 1, a bite that opens with each note and closes), Pulse (a sine clipped hard
 * towards a square) */
enum { BV_SUB, BV_PUNCH, BV_RUBBER, BV_PLUCK, BV_GROWL };
static const float BV_GAIN[EN_NBV] = {1.0f, 1.1f, 1.0f, 1.0f, 1.16f};
static const fmpatch_t *const BV_PATCH[EN_NBV] = {0, &FM_PUNCH, &FM_RUBBER, &FM_PLUCK, &FM_GROWL};
static fmvoice_t bfm;
static float bass_bite, bite_dec, bass_lc;

/* ----------------------------------------------------------------- pings --- */
#define NBELL 6
typedef struct {
    float amp, adec, att, gl, gr, drop, lc;     /* (lc: the strip's Low cut's state) */
    fmvoice_t fm;
    fmpatch_t pt;                               /* its own patch: the ping's voice, as its Tone and the loop have it */
    int32_t wait;
    uint8_t on, note, loop;
} bell_t;
static bell_t bell[NBELL];
static float bell_vol, btone;
static int8_t loop_note[EN_NLOOP][EN_LOOP_MAX];
static uint8_t loop_vel[EN_NLOOP][EN_LOOP_MAX], loop_pos[EN_NLOOP];
static const uint8_t LOOP_BASE[EN_NLOOP] = {72, 67, 79, 62};     /* each loop's register */
static const float LOOP_PAN[EN_NLOOP] = {-0.36f, 0.21f, 0.42f, -0.15f};   /* (1.3.0: 60% of the spread) */
static const float RATIO[5] = {1.0f, 2.0f, 3.0f, 3.5f, 4.77f};

/* ----------------------------------------------------------------- drums --- */
typedef struct {
    float ph, inc, inc_end, amp, adec, x1, x2, att, mph[4], pend, clk, lp[2], lc;   /* (lp: a hat's low-pass; lc: the
                                                  * strip's Low cut's state) */
    int32_t wait;                               /* swing: samples until a hit that waits (pend: its velocity) */
    uint8_t on, note;
} drum_t;
static drum_t drum[EN_NDRUM];
static uint32_t drum_pat[EN_NDRUM];             /* 0 the kick's (four on the floor), 1 the shaker's, 2 the open hat's */
static float pulse_vol, swing, hat_f = 0.9f, hat_open = 0.25f;
static float step_len, pulse_left;               /* a sixteenth, in samples; to the next MIDI clock pulse */
static uint32_t noise = 22222u, cnoise = 4321u;
static const uint8_t DRUM_GM[EN_NDRUM] = {36, 42, 46, 37, 39};  /* kick, closed hat, open hat, side stick, hand clap */
static const float HAT_INC[4] = {304.4f / EN_SR, 369.6f / EN_SR, 522.7f / EN_SR, 800.0f / EN_SR};
/* the shaker's steps, the most wanted first (the off sixteenths before the downbeats; never the open
 * hat's off beats) */
static uint8_t hat_order[12];
static const uint8_t HAT_ORDER[12] = {3, 11, 7, 15, 1, 9, 5, 13, 0, 8, 4, 12};
static int hat_k, hat_kb;
static float duck_x, duck_d, duck_depth, duck_rel;
static float duck_xb, duck_db, duck_relb;       /* the bass's duck: deeper, and slower to let go (it pumps) */
/* the kits: the kick (its pitch at the strike, times its note; how quickly it falls; its length; its click;
 * its drive) and the hats (their high-pass, times Hats'; how much noise against the metal; their length) */
static const struct {
    float start, fall, t60, click, drive, hat_f, noise, hat_t, gain;
} KIT[EN_NKIT] = {
    /* (1.3.0's sound pass: each kick from lower, longer, with half the click; a soft, rolling pulse, the
     * thump down from 100-200 Hz into the sub) */
    {2.75f, 0.0016f, 0.63f, 0.25f, 1.5f, 1.0f, 0.7f, 1.0f, 1.0f},  /* Tight */
    {2.0f, 0.0008f, 0.7f, 0.1f, 1.3f, 0.92f, 0.7f, 1.2f, 0.6f},    /* Deep: long, 808-ish (shorter and softer than
                                                                    * it was: over a bass that holds now) */
    {4.0f, 0.003f, 0.38f, 0.45f, 2.5f, 1.05f, 0.6f, 0.8f, 1.35f},  /* Punch: short and hard */
    {1.8f, 0.0012f, 0.75f, 0.0f, 1.0f, 0.82f, 0.85f, 1.3f, 0.86f}, /* Soft: a muffled thump */
    {2.6f, 0.0016f, 0.6f, 0.18f, 1.8f, 0.75f, 1.0f, 1.1f, 1.0f},   /* Dusty: darker, noisier hats */
};
static int kit;

/* ------------------------------------------------------------------ space --- */
static float echo_send, rev_send, echo_len, echo_len_t, echo_fb = 0.6f, ein_gain = 1.0f, echo_t60 = 6.0f;
static float fb_cap = 2.0f;                     /* stopped, the feedback eases under 90%: the tails die */
static float fx_em = 1.0f, fx_ek = 0.001f, fx_ts = 1.0f;   /* the grid on the echo: its time's multiple, its easing; the tape stop */
static float rev_in_gain = 1.0f;                /* falls to 0 while frozen */
static float tape_lpc = 0.55f, tape_hpc = 0.08f; /* the echo's feedback band-pass */
static float crackle_vol, cloud_amt;

/* ------------------------------------------------------- the arrangement --- */
static uint32_t sixteenth, bar, chord_bars;    /* bars since the start, since the chord moved */
static uint8_t sec, sec_next, fill;
static uint8_t lay_kick = 1, lay_hats = 1, lay_open = 1, lay_bass = 1, lay_stabs = 1;
/* the kick's shape, a phrase at a time (1.3.0, REPORT2's proposal 2): none, the downbeat, 1 and 3, four with 2 and 4
 * ghosted (-10 dB), four on the floor. The Drums page's Density is where they centre; Evolve how far they wander */
enum { KS_NONE, KS_ONE, KS_HALF, KS_GHOST, KS_FOUR, KS_N };
static const uint16_t KSHAPE[KS_N] = {0x0000u, 0x0001u, 0x0101u, 0x1111u, 0x1111u};
static uint8_t kick_shape = KS_FOUR;
static uint8_t lock;                             /* (PERFORM's Lock: the phrases keep what they have) */
static uint8_t kick_next = KS_FOUR, ph_new_next; /* (the next phrase's kick and plan, decided a bar before it) */
volatile uint8_t en_change_next;                 /* the next phrase brings a change: 1 the plan, 2 the kick, 4 the section */
static int kick_draw(int next);
static uint32_t dpat(int d);
/* (the drums' and the bass's phrase plan: phrase_plan, ph_bar) */
static struct {
    uint32_t seed;
    uint8_t busy, drawn;
    int8_t oct;                                  /* (the bass's octave note: its place among the bar's, -1 none) */
    uint8_t vel[16], late[16];                   /* (the hats' feel: each step's accent, its lateness) */
} ph;
static uint32_t ph_rng = 0x2545F491u;
static uint32_t bar_k, bar_sh, bar_op, bar_bass, bar_fifth;   /* (bar_fifth: the bass's notes on the fifth) */   /* this bar's: the kick's, the shaker's, the open hat's, the bass's */
static int ph_st;                                /* (the step a drum hit is on: its lateness) */
static void noise_pattern(void);
static void nz_dens(void);
static int cr_dbars;                            /* bars till the drifts' next targets (on a barline) */
static void noise_step(int s);
static uint8_t nz_drawn;
static float arc, arc_tgt, arc_tau = 1.0f;     /* the sweep, its target, its time (samples) */
static uint8_t change_pending;

/* -------------------------------------------------------------- harmony --- */
static void scale_set(void)
{
    nsteps = en_scale(pool, steps);
}

static int bell_note(int l, int off)            /* a loop slot's note, from the chord's degree */
{
    int d = deg + off, oct = d / nsteps, s = steps[d % nsteps], base, root = par[P_ROOT], n;
    base = LOOP_BASE[l] - ((LOOP_BASE[l] - root) % 12 + 12) % 12;   /* the root at or below the register */
    n = base + s + 12 * oct + 12 * par[P_POCT];
    while (n > 103)
        n -= 12;
    while (n < 36)
        n += 12;
    return n;
}

/* the chord to the four voices: the voicing on the degree, its root moved the least from the last one,
 * in C3..B3 (as a chord memory: the shape kept, transposed whole; 1.3.0 up from F#2..F3, out of the low-mids) */
static int chord_slot = -1;                     /* the slot whose chord sounds (-1: one of the scale) */
static void voice_chord(void)
{
    int iv[4], i, pc, n;
    if (chord_slot >= 0 && par[P_CQ0 + chord_slot]) {   /* a slot's chord, as written */
        int r = par[P_CR0 + chord_slot];
        en_chord_iv(par[P_CQ0 + chord_slot], par[P_CX0 + chord_slot], iv);
        for (i = 0; i < 4; i++)
            chord_pcs[i] = (r + iv[i]) % 12;
        {                                        /* (in name order for the bass: the root, its third, its fifth) */
            int t[4];
            en_chord_iv(par[P_CQ0 + chord_slot], EN_CX_7, t);
            chord_pcs[1] = (r + t[3] - 12) % 12;
            chord_pcs[2] = (r + t[1]) % 12;
            chord_pcs[3] = (r + t[2]) % 12;
        }
        chord_nt = 4;
        deg = en_deg_of(pool, r - par[P_ROOT]);
    } else {
        chord_slot = -1;
        chord_nt = en_chord_tones(pool, par[P_ROOT], deg, chord_pcs);
        en_voicing(pool, deg, shape, iv);
    }
    en_chord_slot = (int8_t)chord_slot;
    pc = chord_pcs[0];
    n = root_note + ((pc - root_note) % 12 + 12) % 12;
    if (n - root_note > 6)
        n -= 12;
    while (n < 48)
        n += 12;
    while (n > 59)
        n -= 12;
    root_note = n;
    for (i = 0; i < EN_NPAD; i++) {
        stab[i].note = (uint8_t)(n + iv[i]);
        en_chord[i] = stab[i].note;
    }
    en_chord_root = (uint8_t)chord_pcs[0];
    en_chord_n = (uint8_t)chord_nt;
}

static int bass_note(int pc)                    /* E1..Eb2, an octave up or down (E0..Eb1: felt more than heard) */
{
    return 28 + ((pc - 4) % 12 + 12) % 12 + 12 * par[P_BASSOCT];
}

/* the chords' slots: how far the progression runs (to its last filled slot; 0: none, or Play at Random), a
 * slot's chord built from its kind and type, and a degree for a note above the root (the pings take the
 * scale's notes around it: nearest in the pool, the lower on a tie) */
volatile int8_t en_prog_pos = -1, en_chord_slot = -1;
volatile int8_t en_prog_slot[EN_NPROG][3];
volatile uint32_t en_prog_gen;
static int slot_gap(int i) { return !par[P_CQ0 + i] && !par[P_CR0 + i]; }
static int slot_walk(int i) { return !par[P_CQ0 + i] && par[P_CR0 + i]; }
static int prog_len(void)                       /* Loop: to the last slot that isn't a gap (0: none) */
{
    int i, n = 0;
    for (i = 0; i < EN_NPROG; i++)
        if (!slot_gap(i))
            n = i + 1;
    return n;
}
static void prog_publish(void)                  /* the slots, for the UI to take */
{
    int i;
    for (i = 0; i < EN_NPROG; i++) {
        en_prog_slot[i][0] = (int8_t)par[P_CR0 + i];
        en_prog_slot[i][1] = (int8_t)par[P_CQ0 + i];
        en_prog_slot[i][2] = (int8_t)par[P_CX0 + i];
    }
    en_prog_gen++;
}
void en_chord_iv(int q, int x, int *iv)
{
    static const int8_t THIRD[EN_NCQ] = {3, 4, 3, 2, 5, 3, 4}, FIFTH[EN_NCQ] = {7, 7, 7, 7, 7, 6, 8};
    int t = THIRD[(unsigned)q < EN_NCQ ? q : 0], f = FIFTH[(unsigned)q < EN_NCQ ? q : 0],
        s = q == EN_CQ_DIM ? 9 : 10, i, j;   /* the seventh: minor (a diminished seventh on Dim) */
    switch ((unsigned)x < EN_NCX ? x : 0) {
    case EN_CX_6: iv[0] = 0, iv[1] = f, iv[2] = 9, iv[3] = t + 12; break;
    case EN_CX_7: iv[0] = 0, iv[1] = f, iv[2] = s, iv[3] = t + 12; break;
    case EN_CX_MAJ7: iv[0] = 0, iv[1] = f, iv[2] = 11, iv[3] = t + 12; break;
    case EN_CX_9: iv[0] = 0, iv[1] = s, iv[2] = t + 12, iv[3] = 14; break;
    case EN_CX_11: iv[0] = 0, iv[1] = s, iv[2] = t + 12, iv[3] = 17; break;
    case EN_CX_ADD9: iv[0] = 0, iv[1] = f, iv[2] = t + 12, iv[3] = 14; break;
    default: iv[0] = 0, iv[1] = f, iv[2] = 12, iv[3] = t + 12; break;   /* the triad, open, its root doubled */
    }
    for (i = 1; i < 4; i++)                      /* the type on the sus's own note (sus2 with a 9, sus4 with an 11): */
        for (j = 0; j < i; j++)                  /* the fifth, an octave up, instead */
            if (iv[i] == iv[j])
                iv[i] = f + 12;
    for (i = 1; i < 4; i++)                      /* ascending, no two alike */
        for (j = i; j > 0 && iv[j] < iv[j - 1]; j--) {
            int y = iv[j];
            iv[j] = iv[j - 1];
            iv[j - 1] = y;
        }
    for (i = 1; i < 4; i++)
        while (iv[i] <= iv[i - 1])
            iv[i] += 12;
}
void en_walk_chord(uint32_t pool, int root, int deg, int *r, int *q, int *x)
{
    int pcs[4], n = en_chord_tones(pool, root, deg, pcs), t, f, s;
    t = (pcs[1] - pcs[0] + 12) % 12;
    f = (pcs[2 < n ? 2 : 0] - pcs[0] + 12) % 12;
    s = n >= 4 ? (pcs[3] - pcs[0] + 12) % 12 : -1;
    *r = pcs[0];
    *q = t == 2 ? EN_CQ_SUS2 : t == 5 ? EN_CQ_SUS4 : t == 3 ? (f == 6 ? EN_CQ_DIM : EN_CQ_MINOR)
       : (f == 8 ? EN_CQ_AUG : EN_CQ_MAJOR);
    *x = s == 10 || (s == 9 && *q == EN_CQ_DIM) ? EN_CX_7 : s == 11 ? EN_CX_MAJ7 : s == 9 ? EN_CX_6 : EN_CX_TRIAD;
}
const char *en_chord_label(int root, int q, int x, char *b)
{
    static const char *const QS[EN_NCQ] = {"", "", "m", "sus2", "sus4", "dim", "aug"};
    static const char *const XS[EN_NCX] = {"", "6", "7", "M7", "9", "11", "add9"};
    const char *r = EN_NOTE_NAME[(unsigned)root < 12u ? root : 0], *qs, *xs;
    int k = 0, sus;
    q = (unsigned)q < EN_NCQ ? q : 0;
    x = (unsigned)x < EN_NCX ? x : 0;
    if (!q) {
        b[0] = '-', b[1] = 0;
        return b;
    }
    qs = QS[q], xs = XS[x], sus = q == EN_CQ_SUS2 || q == EN_CQ_SUS4;
    while (*r)
        b[k++] = *r++;
    if (sus) {                                   /* the type before a sus: "C7sus4" */
        while (*xs)
            b[k++] = *xs++;
        while (*qs)
            b[k++] = *qs++;
    } else {
        while (*qs)
            b[k++] = *qs++;
        while (*xs)
            b[k++] = *xs++;
    }
    b[k] = 0;
    return b;
}
int en_deg_of(uint32_t pl, int rel)
{
    int st[12], m = en_scale(pl, st), i, best = 0, bd = 99;
    rel = ((rel % 12) + 12) % 12;
    for (i = 0; i < m; i++) {
        int d = (st[i] - rel + 12) % 12, e = d > 6 ? 12 - d : d;
        if (e < bd) {
            bd = e;
            best = i;
        }
    }
    return best;
}
static int prog_pos = -1;

/* to another degree: home most of all, then the subdominant, the seventh, the dominant; away from home,
 * home again more often than not. Never a diminished fifth (a half-diminished chord is no dub chord) */
/* the walk: the next degree after d (and before it, d2), in Play's character (0 Home, 1 Wander, 2 Free; Loop: as
 * Home). Away from home, back (Home 65%, Wander 35%, Free 15%); else a weighted choice, never the same degree,
 * never a diminished one: Home leans on the IV, then the VII and the V; Wander spreads wider; Free is even. Wander
 * and Free rarely step straight back to d2 (no ping-pong) */
const char *const EN_WALK_NAME[EN_NWALK] = {"Home", "Wander", "Free"};
#define PLAY_LOOP (par[P_PMODE] & EN_PLOOP)
static int walk_shape;                          /* the walk's voicing, now and then the other (Evolve) */
static int walk_prev;                           /* the degree before the walk's last */
static int walk_from(int d)
{
    static const float W7[3][7] = {{0.0f, 0.4f, 1.0f, 3.0f, 1.5f, 1.5f, 2.0f},
                                   {0.0f, 1.2f, 1.2f, 2.0f, 1.6f, 1.8f, 1.4f},
                                   {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}};
    static const float HOME[3] = {0.65f, 0.35f, 0.15f}, BACK[3] = {1.0f, 0.3f, 0.15f};
    const int st = (par[P_PMODE] & 3) < EN_NWALK ? par[P_PMODE] & 3 : EN_WALK_WANDER, d2 = walk_prev;
    float w[12], sum = 0.0f, r;
    int i;
    walk_prev = d;
    if (d != 0 && frand() < HOME[st] * (d2 == 0 && st ? 0.6f : 1.0f)) {   /* (just back from home: less often again) */
        i = 0;
    } else {
        for (i = 0; i < nsteps; i++) {
            int t[3], n = stack(pool, 0, i, t, 3);
            w[i] = i == d || i == 0 ? 0.0f : nsteps == 7 ? W7[st][i] : 1.0f;
            if (i == d2)
                w[i] *= BACK[st];
            if (n >= 3 && (t[2] - t[0] + 12) % 12 == 6)
                w[i] = 0.0f;
            sum += w[i];
        }
        r = frand() * sum;
        for (i = 0; i < nsteps; i++) {
            r -= w[i];
            if (r <= 0.0f && w[i] > 0.0f)
                break;
        }
        if (i >= nsteps)
            i = nsteps > 1 ? 1 : 0;
    }
    if (frand() < evolve * 0.1f)
        walk_shape ^= 1;                         /* now and then, the other voicing */
    return i;
}
static int walk_last;                           /* the degree the walk wrote last (it goes on from there) */
static void slot_walk_set(int k, int d)         /* slot k: the walk's degree d */
{
    par[P_CQ0 + k] = 0;
    par[P_CR0 + k] = (int16_t)(d + 1);
    par[P_CX0 + k] = (int16_t)walk_shape;
    walk_last = d;
}
static void walk_fill(void)                     /* Endless with every slot a gap (a fresh piece): eight from home */
{
    int k, d = 0;
    for (k = 0; k < EN_NPROG; k++)
        if (!slot_gap(k))
            return;
    walk_prev = 0;
    slot_walk_set(0, 0);
    for (k = 1; k < EN_NPROG; k++) {
        d = walk_from(d);
        slot_walk_set(k, d);
    }
    prog_publish();
}
static void slot_play(int k)                    /* slot k sounds: its chord as written, the walk's, or (a gap) held */
{
    if (par[P_CQ0 + k]) {
        chord_slot = k;
        voice_chord();
    } else if (par[P_CR0 + k]) {
        chord_slot = -1;
        deg = (par[P_CR0 + k] - 1) % (nsteps ? nsteps : 1);
        shape = par[P_CX0 + k] & 1;
        voice_chord();
    }
    chord_bars = 0;
}

static void chord_change(void)
{
    int k;
    if (PLAY_LOOP) {                            /* Loop: the slots in turn, a gap holding the chord before it */
        int n = prog_len();
        if (n) {
            prog_pos = (prog_pos + 1) % n;
            en_prog_pos = (int8_t)prog_pos;
            slot_play(prog_pos);
        }
        chord_bars = 0;
        return;
    }
    /* Endless: all up a slot (a gap stays a gap), the walk writes slot 8; slot 1 sounds */
    for (k = 0; k < EN_NPROG - 1; k++) {
        par[P_CR0 + k] = par[P_CR0 + k + 1];
        par[P_CQ0 + k] = par[P_CQ0 + k + 1];
        par[P_CX0 + k] = par[P_CX0 + k + 1];
    }
    slot_walk_set(EN_NPROG - 1, walk_from(walk_last));
    prog_pos = 0;
    en_prog_pos = 0;
    prog_publish();
    slot_play(0);
}

/* ------------------------------------------------------------- the loops --- */
static int rand_offset(void)                    /* scale steps above the chord's degree */
{
    if (frand() < 0.75f) {                      /* a chord tone, sometimes an octave up */
        int o = 2 * irand(chord_nt);
        return o + (frand() < 0.25f ? nsteps : 0);
    }
    return 1 + 2 * irand(3);                    /* a passing note: the 2nd, 4th or 6th */
}

static int loop_count(int l)
{
    int i, c = 0;
    for (i = 0; i < EN_LOOP_LEN[l]; i++)
        c += loop_note[l][i] >= 0;
    return c;
}

static int loop_target(void)                    /* 0..5 notes: Density, thinned in the dark */
{
    float d = (float)pe(P_DENSITY) * 0.05f * (1.0f + 0.5f * arcm);
    return (int)(fm_clampf(d, 0.0f, 5.0f) + frand());
}
static uint8_t loop_rest[EN_NLOOP];             /* this pass: silent (Evolve) */

static void loop_add(int l)
{
    int len = EN_LOOP_LEN[l], i, s;
    for (i = 0; i < 8; i++) {
        s = irand(len);
        if (loop_note[l][s] < 0) {
            loop_note[l][s] = (int8_t)rand_offset();
            loop_vel[l][s] = (uint8_t)(50 + irand(50));
            return;
        }
    }
}

static int loop_pick(int l)                     /* a random sounding slot, -1 none */
{
    int len = EN_LOOP_LEN[l], i, s = irand(len);
    for (i = 0; i < len; i++, s = (s + 1) % len)
        if (loop_note[l][s] >= 0)
            return s;
    return -1;
}

static void loop_publish(int l)
{
    int i;
    for (i = 0; i < EN_LOOP_MAX; i++)
        en_loop_note[l][i] = i < EN_LOOP_LEN[l] ? loop_note[l][i] : -1;
}

static void loop_regrow(int l)
{
    int i, n = loop_target();
    for (i = 0; i < EN_LOOP_MAX; i++)
        loop_note[l][i] = -1;
    for (i = 0; i < n; i++)
        loop_add(l);
    loop_publish(l);
}

/* a piece's own loops (en_pings): packed by the UI, unpacked here */
static uint8_t pings_pend[EN_PINGS_N], pings_have;
void en_pings_get(uint8_t *b)
{
    int l, i, k = 0;
    for (l = 0; l < EN_NLOOP; l++)
        for (i = 0; i < EN_LOOP_LEN[l]; i++, k++) {
            int nt = loop_note[l][i], v = (loop_vel[l][i] - 50) / 3;
            b[k] = (uint8_t)((nt < 0 || nt > 14 ? 15 : nt) | (v < 0 ? 0 : v > 15 ? 15 : v) << 4);
        }
}
void en_pings(const uint8_t *b)
{
    int i;
    for (i = 0; b && i < EN_PINGS_N; i++)
        pings_pend[i] = b[i];
    post(C_PINGS, b ? 1 : 0, 0);
}
static void pings_take(void)
{
    int l, i, k = 0;
    for (l = 0; l < EN_NLOOP; l++) {
        for (i = 0; i < EN_LOOP_MAX; i++)
            loop_note[l][i] = -1;
        for (i = 0; i < EN_LOOP_LEN[l]; i++, k++) {
            int nt = pings_pend[k] & 15;
            loop_note[l][i] = (int8_t)(nt == 15 ? -1 : nt);
            loop_vel[l][i] = (uint8_t)(50 + 3 * (pings_pend[k] >> 4));
        }
        loop_publish(l);
    }
}

static void loop_pass(int l)                    /* the end of a pass: grow, prune, evolve */
{
    int c = loop_count(l), t = loop_target(), s;
    if (ev_of(VY_PINGS) < 0.005f) {             /* Evolve 0 (or Vary): the loops as they are (Density changed: to it) */
        t = (int)(fm_clampf((float)pe(P_DENSITY) * 0.05f, 0.0f, 5.0f) + 0.5f);
        for (; c < t; c++)
            loop_add(l);
        for (; c > t && (s = loop_pick(l)) >= 0; c--)
            loop_note[l][s] = -1;
        loop_rest[l] = frand() < 0.2f * fm_maxf(-arcm, 0.0f);   /* (Motion's dark: a pass sat out, the notes kept) */
        loop_publish(l);
        return;
    }
    for (; c < t; c++)
        loop_add(l);
    for (; c > t && (s = loop_pick(l)) >= 0; c--)
        loop_note[l][s] = -1;
    loop_rest[l] = frand() < evolve * fm_minf(vary_l[VY_PINGS], 1.0f) * 0.2f + 0.2f * fm_maxf(-arcm, 0.0f);   /* (a pass
                                                  * sat out: as often up to Vary 50, as the stabs' gaps) */
    if (frand() < ev_of(VY_PINGS) * 0.7f && (s = loop_pick(l)) >= 0) {
        en_vary_turns[VY_PINGS]++;
        if (frand() < 0.5f) {
            loop_note[l][s] = (int8_t)rand_offset();             /* another note */
        } else {
            int d = irand(EN_LOOP_LEN[l]);                       /* the same note, elsewhere */
            if (loop_note[l][d] < 0) {
                loop_note[l][d] = loop_note[l][s];
                loop_vel[l][d] = loop_vel[l][s];
                loop_note[l][s] = -1;
            }
        }
    }
    loop_publish(l);
}

/* each loop its own ping: index, decay and ratio a little apart */
static const float LOOP_IDX[EN_NLOOP] = {1.0f, 0.55f, 1.5f, 0.8f}, LOOP_DEC[EN_NLOOP] = {1.0f, 1.3f, 0.7f, 1.5f};
static const int8_t LOOP_RAT[EN_NLOOP] = {0, 0, 1, -1};
/* the pings' voices: the modulator's ratio (0: by Ping tone, RATIO), the index, its decay and the ping's
 * length (times the Blip's), a falling pitch (Drop: from this many times its note), a level */
static const struct {
    float ratio, idx, idec, t60, drop, gain;
} PV[EN_NPV] = {
    {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},               /* Blip */
    {4.0f, 1.2f, 0.4f, 0.6f, 0.0f, 1.45f},              /* Marimba: a wooden bar's fourth partial */
    {3.5f, 0.8f, 2.0f, 1.7f, 0.0f, 0.79f},              /* Glass: a long, bright ring */
    {1.41f, 2.5f, 0.2f, 0.25f, 0.0f, 2.4f},             /* Wood: a knock */
    {1.0f, 0.6f, 1.0f, 0.8f, 1.0f, 1.29f},              /* Drop: the pitch falling from an octave up */
};

static void bell_voice(int l, int n, float vel, int32_t wait)
{
    int i, best = 0, ri, pv;
    float lo = 9.0f, t60, p;
    bell_t *v;
    for (i = 0; i < NBELL; i++) {               /* a free voice, or the quietest */
        float a = bell[i].on ? bell[i].amp : -1.0f;
        if (a < lo) {
            lo = a;
            best = i;
        }
    }
    v = &bell[best];
    if (v->on)
        midi(0x81, v->note, 0);
    v->on = 1;
    v->note = (uint8_t)n;
    v->loop = (uint8_t)l;
    pv = (unsigned)par[P_PVOICE] < EN_NPV ? par[P_PVOICE] : 0;
    ri = (int)(btone * 100.0f + 0.5f) * 5 / 101 + LOOP_RAT[l];
    v->drop = fm_clampf(PV[pv].drop + 0.015f * (float)par[P_PBEND], 0.0f, 3.0f);   /* Bend: from up to four times its note */
    v->amp = vel * PV[pv].gain;
    t60 = fm_clampf(0.9f - (float)(n - 60) * 0.02f, 0.25f, 1.1f) * LOOP_DEC[l] * PV[pv].t60   /* short: a ping */
          * fm_exp2f(1.5f * rel(P_PLEN));
    v->adec = fm_expf(-6.9f / (t60 * EN_SR));
    /* its patch: a carrier, the modulator (Ping tone: 0 a round blip, 100 a metallic clank: its index, in radians
     * as turns, and its decay; Ratio, the sound page's: its ratio), and two more operators for the 4-op algorithms
     * (Glass's own: two pairs) */
    {
        float idx = (0.2f + 5.0f * btone * btone) * (1.0f + 0.4f * fm_maxf(perf_sw, 0.0f)) * LOOP_IDX[l] * (0.6f + 0.4f * vel) * PV[pv].idx * (1.0f / FM_TWO_PI),
              td = (0.08f + 0.5f * btone) * PV[pv].idec * (1.0f / 6.9f);
        fmpatch_t *pt = &v->pt;
        pt->alg = pv == 2 ? 4 : FM_ALG2;
        pt->fb = 0.0f;
        pt->op[0] = (fmop_t){1.0f, 1.0f, 0.0f, 1.0f};
        pt->op[1] = (fmop_t){(PV[pv].ratio > 0.0f ? PV[pv].ratio : RATIO[ri < 0 ? 0 : ri > 4 ? 4 : ri]) * fm_exp2f(0.8f * rel(P_PRAT)),
                             idx, td, 0.0f};
        pt->op[2] = (fmop_t){2.0f, 0.45f, 0.0f, 1.0f};
        pt->op[3] = (fmop_t){pv == 2 ? 5.0f : 3.0f, idx * 0.3f, td * 0.6f, 0.0f};
        fm_note(&v->fm, pt, mtof((float)n), 1.0f, fm_q(P_PFRAT), fm_d(P_PFDEC));
        v->fm.ph[0] = v->fm.ph[1] = v->fm.ph[2] = v->fm.ph[3] = 0.0f;
        fm_ctl(&v->fm, pt, fm_alg_of(P_PALG, pt), 1.0f);
    }
    v->att = 0.0f;
    v->lc = 0.0f;
    p = (LOOP_PAN[l] + 1.0f) * (FM_PI / 4.0f);
    v->gl = fm_cosf(p);
    v->gr = fm_sinf(p);
    v->wait = wait;
    en_loop_glow[l] = 1.0f;
    hit(1, vel);
    midi(0x91, (uint32_t)n, (uint32_t)(vel * 110.0f));
}

static void bell_ring(int l, int slot)
{
    int n = bell_note(l, loop_note[l][slot]);
    float vel = (float)loop_vel[l][slot] * 0.01f;
    bell_voice(l, n, vel, irand(220));           /* a hand on the grid: up to 5 ms late */
}

/* ------------------------------------------------------------- the stabs --- */
static int stab_k, stab_kb, stab_rot;
static uint32_t stab_home;                      /* the pattern, before this bar's gaps */
/* (1.4.0, the user's: Vary, the chord's own, Evolve over it) how often the pattern turns, times Evolve's (1 at 50: as
 * Evolve alone had it; 0 at 0; 3 at 100); and its gaps (a stab left out, a bar with none), as often up to 50, none at
 * 0: at Vary 0 the pattern still, every stab played */
static float vary_t = 1.0f, vary_g = 1.0f;
volatile uint32_t en_stab_turns;

static void stabs_home(void)                    /* Stabs: one a bar .. six, off the beat */
{
    stab_kb = stab_k = 1 + (pe(P_STABS) * 5 + 50) / 100;
    stab_rot = 2;
    stab_home = en_euclid(stab_k, 16, stab_rot);
}

static void stabs_evolve(void)                  /* the pattern turns a step, or gains or loses a stab */
{
    en_stab_turns++;
    if (frand() < 0.6f) {
        stab_rot = (stab_rot + (frand() < 0.5f ? 1 : 15)) % 16;
    } else {
        int k = stab_k + (frand() < 0.5f ? 1 : -1);
        if (k >= stab_kb - 1 && k <= stab_kb + 1 && k >= 1 && k <= 8)
            stab_k = k;
    }
    stab_home = en_euclid(stab_k, 16, stab_rot);
}

static void stabs_bar(void)                     /* this bar's stabs: the pattern, with gaps */
{
    uint32_t p = stab_home, out = 0;
    int i, first = 1;
    if (!lay_stabs)
        p = 0;
    else if (sec == SEC_GROOVE)                 /* a Groove: every other stab */
        for (i = 0; i < 16; i++)
            if (p >> i & 1u) {
                if (!first)
                    p &= ~(1u << i);
                first = !first;
            }
    first = 1;
    for (i = 0; i < 16; i++)
        if (p >> i & 1u) {
            if (first || frand() >= 0.3f * evolve * vary_g)
                out |= 1u << i;
            first = 0;
        }
    if (sec != SEC_BREAK && frand() < 0.5f * evolve * vary_g)   /* (1.3.0: now and then a bar with none: the echo
                                                      * carries it; ~20% at Evolve 40, Vary 50) */
        out = 0;
    en_stab_pat = out;
}

static float cc_col(void);
static void stab_fire(float vel, int throw_it)
{
    int i;
    if (st_midi_left > 0)
        for (i = 0; i < EN_NPAD; i++)
            midi(0x80, stab[i].midi_note, 0);
    for (i = 0; i < EN_NPAD; i++) {
        float f = mtof((float)stab[i].note);
        float dt = 0.00202f * fm_exp2f(2.0f * cc_col());   /* +-3.5 cents (Colour: +-0.9 .. +-14; the LFO's, if it
                                                  * goes there, render_stabs) */
        stab[i].inc[0] = f * (1.0f - dt) * (1.0f / EN_SR);
        stab[i].inc[1] = CV[cvoice].ratio > 0.0f ? f * CV[cvoice].ratio * (1.0f / EN_SR)   /* FM: the modulator */
                                                 : f * (1.0f + dt) * (1.0f / EN_SR);
        if (cv_patch(cvoice))
            fm_note(&cfm[i], cv_patch(cvoice), f, 1.0f, fm_q(P_CFRAT), fm_d(P_CFDEC));
        stab[i].midi_note = stab[i].note;
        midi(0x90, stab[i].note, (uint32_t)(40.0f + 80.0f * vel));
    }
    st_env = vel;
    ec_dk = fm_maxf(ec_dk, vel);
    st_fenv = 1.0f;
    st_cofs = (frand() - 0.5f) * 0.8f * loose();   /* (1.3.0: each stab its own cutoff, +-0.4 octave) */
    click = cvoice == CV_WHEEL ? 0.12f * vel : 0.0f;
    en_stab_vel = vel;
    en_stab_thrown = (uint8_t)(throw_it != 0);
    en_stabs++;
    hit(0, vel);
    st_midi_left = (int32_t)(fm_maxf(st_t60 * 0.5f, 0.06f) * EN_SR);
    if (throw_it) {
        st_throw = 1.0f;
        en_throws++;
    }
}

/* ---------------------------------------------------------------- drums --- */
static void hats_publish(void)
{
    uint32_t c = 0, o = 0;
    int i, k = pe(P_HITS), ko = k < 15 ? 0 : k < 40 ? 2 : 4;
    for (i = 0; i < hat_k && i < 12; i++)
        c |= 1u << hat_order[i];
    if (ko >= 2)
        o |= 1u << 2 | 1u << 10;
    if (ko >= 4)
        o |= 1u << 6 | 1u << 14;
    drum_pat[1] = c;
    drum_pat[2] = o;
    drum_pat[0] = KSHAPE[kick_shape];
    drum_pat[3] = k >= 70 ? 0x1010u : 0u;       /* the rim, with many hits: on two and four */
    drum_pat[4] = 0x1010u;                       /* the clap: two and four, now and then (clap_auto) */
    en_drum_pat[0] = drum_pat[0];
    en_drum_pat[1] = c;
    en_drum_pat[2] = o;
    en_drum_pat[3] = drum_pat[3];
    en_drum_pat[4] = drum_pat[4];
}

static void hats_home(void)                     /* Hits: the shaker's count, its steps home */
{
    int i;
    for (i = 0; i < 12; i++)
        hat_order[i] = HAT_ORDER[i];
    hat_kb = hat_k = (pe(P_HITS) * 12 + 50) / 100;
    hats_publish();
}

static void hats_evolve(void)                   /* a shaker step trades places, or one more or less */
{
    en_vary_turns[VY_DRUMS]++;
    if (frand() < 0.6f) {
        int a = irand(11), t = hat_order[a];
        hat_order[a] = hat_order[a + 1];
        hat_order[a + 1] = (uint8_t)t;
    } else if (hat_kb) {
        int k = hat_k + (frand() < 0.5f ? 1 : -1);
        if (k >= hat_kb - 2 && k <= hat_kb + 2 && k >= 1 && k <= 12)
            hat_k = k;
    }
    hats_publish();
}

/* the kick's note: the key's root in F1..B1 (44..62 Hz); below F, its fifth (G1..B1), so the kick and the
 * sub agree. One note a piece: it doesn't follow the chords */
int en_kick_note(int root)
{
    int pc = (root % 12 + 12) % 12;
    return pc >= 5 ? 24 + pc : 24 + pc + 7;
}
#define kick_note en_kick_note

static void drum_fire(int d, float vel)
{
    drum_t *v = &drum[d];
    v->on = 1;
    v->amp = vel;
    v->att = 0.0f;
    switch (d) {
    case 0: {                                   /* kick: a sine falling fast from ~5x to its note */
        float f = mtof((float)(kick_note(par[P_ROOT]) + par[P_KTUNE])), pu = rel(P_KPUNCH);
        v->note = DRUM_GM[0];
        v->inc = f * KIT[kit].start * fm_exp2f(0.8f * pu) * (1.0f / EN_SR);
        v->inc_end = f * (1.0f / EN_SR);
        v->adec = fm_expf(-6.9f / (KIT[kit].t60 * fm_exp2f(1.2f * rel(P_KLEN)) * EN_SR));
        v->clk = KIT[kit].click * 2.0f * fm_maxf(1.0f + pu, 0.0f);   /* (Tight: 1, as before) */
        duck_x = duck_xb = 1.0f;
        break;
    }
    case 1:                                     /* the shaker: short; it chokes the open hat */
        v->note = DRUM_GM[1];
        v->adec = fm_expf(-6.9f / (0.07f * KIT[kit].hat_t * fm_exp2f(1.2f * rel(P_HLEN)) * EN_SR));   /* (1.3.0: 45 ms to 70: a
                                                                                             * shaker; the hats' length) */
        drum[2].adec = fm_expf(-6.9f / (0.03f * EN_SR));
        break;
    case 2:                                     /* open hat: the "tss" off the beat */
        v->note = DRUM_GM[2];
        v->adec = fm_expf(-6.9f / (hat_open * KIT[kit].hat_t * fm_exp2f(1.2f * rel(P_HLEN)) * EN_SR));
        break;
    case 3:                                     /* the rim: a short wooden tok, thrown into the echo */
        v->note = DRUM_GM[3];
        v->adec = fm_expf(-6.9f / (0.06f * EN_SR));
        v->clk = 1.0f;
        break;
    default:                                    /* the clap: three quick bursts and a tail, into the plate */
        v->note = DRUM_GM[4];
        v->ph = 0.0f;                            /* (its time, in samples) */
        v->adec = fm_expf(-6.9f / (0.22f * EN_SR));
        break;
    }
    en_drum_glow[d] = 1.0f;
    hit(3, d == 0 ? vel : vel * 0.6f);           /* (the kick the biggest ring) */
    midi(0x99, v->note, (uint32_t)(vel * 100.0f));
    midi(0x89, v->note, 0);
}

static void drum_hit(int d, float vel, int off)  /* (Swing delays the whole off sixteenth now: clock_pulse) */
{
    int32_t w = 0;
    (void)off;
    if ((d == 1 || d == 2) && !par[P_DMODE])     /* (1.3.0: the hats a few ms late, the phrase's feel) */
        w = (int32_t)((float)ph.late[ph_st & 15] * (1.0f / 255.0f) * 0.006f * EN_SR * loose());
    if (w > 0) {
        drum[d].wait = w;
        drum[d].pend = vel;
    } else {
        drum_fire(d, vel);
    }
}

static int32_t drums_wait(void)                 /* samples to the next hit that waits; 0: none */
{
    int32_t m = 0;
    int d;
    for (d = 0; d < EN_NDRUM; d++)
        if (drum[d].wait > 0 && (!m || drum[d].wait < m))
            m = drum[d].wait;
    return m;
}

static void drums_wait_step(int32_t k)          /* k samples went by: hits whose wait is over */
{
    int d;
    for (d = 0; d < EN_NDRUM; d++)
        if (drum[d].wait > 0 && (drum[d].wait -= k) <= 0) {
            drum[d].wait = 0;
            drum_fire(d, drum[d].pend);
        }
}

/* ----------------------------------------------------------------- bass --- */
static void bass_hit(int first, int up, int st)
{
    int pc = chord_pcs[0], n;
    (void)first;
    if (chord_nt >= 3 && bar_fifth >> st & 1u)  /* the fifth (with Drone: a whole bar of it); (1.4.0) from the phrase's
                                                  * plan, the same each A bar, not drawn at each note: the screen shows it */
        pc = chord_pcs[2];
    n = bass_note(pc);
    if (!first && up)
        n += 12;                                /* now and then, an octave up */
    if (bass.env < 0.05f)
        bass.note = (float)n;                   /* from silence: no slide */
    bass.target = (float)n;
    if (bass.gate > 0 || bass.midi_note)
        midi(0x82, bass.midi_note, 0);
    bass.gate = (int32_t)(fm_minf(GROOVE_GATE[gpat()] * fm_exp2f(rel(P_BLEN)), 15.8f) * step_len);
    bass_bite = 1.0f;
    {
        int bv = (unsigned)par[P_BVOICE] < EN_NBV ? par[P_BVOICE] : 0;
        if (BV_PATCH[bv])
            fm_note(&bfm, BV_PATCH[bv], mtof(bass.note), 1.0f, fm_q(P_BFRAT), fm_d(P_BFDEC));
    }
    bass.midi_note = (uint8_t)n;
    hit(2, first ? 1.0f : 0.75f);
    midi(0x92, (uint32_t)n, 90);
}

/* ------------------------------------------------------------- the song --- */
/* (1.4.0, the user's: build/design/SONG.md) Song structure. Endless (Song mode 0) as ever: it starts full and plays
 * on. Song: PLAY starts an intro, then the main, an outro, and silence (the grid stops, the tails ring on); Set the
 * same, the app then starting the next piece. The intro: the parts come in one by one in the piece's order (its Kind:
 * Ambient first, the noise and the chord's wash; Pulse first, the kick and the bass from the first bar), spread over
 * it, off the phrase lines, each faded in over two to four bars; the kick One, then Half, then Four; the hats last,
 * their hits growing; the chord's filter an octave under, opening; the echo a little wetter; no sections till the
 * main. The outro the mirror: the hats out first, then the pings and the bass, the kick thinning (Half, One) and
 * gone, the chord and the noise last, thrown into the echo and fading: it ends on the echo's tails and the hiss.
 * Outro now (en_outro): the outro from the next phrase, in any mode; again, in it or waiting: back to the main. The
 * knobs go on working: the song's moves are around them (the parts' faders, offsets), never on their values */
/* the parts' faders (the chord, the pings, the bass, the kick, the hats with the rim and the clap, the noise; EN_M_*'s
 * order): a gain each, 0..1, heard squared, moved to its target at most a whole fade in two bars (render_chunk): the
 * song's moves, and the dub mixer's next. All 1 in Endless: the music as it was, bit for bit */
static float fd_pos[EN_NPART] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, fd_tgt[EN_NPART] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
             fd_g[EN_NPART] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static uint32_t fd_quiet;                        /* (the parts out, EN_M_* bits: the lanes hide them, as a mute) */
volatile float en_fader[EN_NPART] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static void fader_step(uint32_t n)
{
    const float mx = (float)n / (32.0f * step_len);
    int i;
    for (i = 0; i < EN_NPART; i++) {
        const float d = fd_tgt[i] - fd_pos[i];
        if (d != 0.0f) {
            fd_pos[i] += fm_clampf(d, -mx, mx);
            fd_g[i] = fd_pos[i] * fd_pos[i];
            en_fader[i] = fd_g[i];
        }
    }
}
static void faders_full(void)                    /* (all up, at once: Endless, a new start) */
{
    int i;
    for (i = 0; i < EN_NPART; i++)
        fd_pos[i] = fd_tgt[i] = fd_g[i] = en_fader[i] = 1.0f;
    fd_quiet = 0;
}
static uint8_t song_mode, song_ph;               /* Song mode (P_SMODE); where the song is (EN_SG_*) */
static uint8_t song_bo_on, song_fixed, song_ask, song_end_now;   /* (an outro coming; its bar set by hand; asked by
                                                  * hand, waiting for its phrase; the end reached) */
static uint32_t song_b0, song_bo, song_be;       /* the bars the main, the outro and the silence start on */
static float song_ent[EN_NPART], song_fd[EN_NPART], song_cap[EN_NPART];   /* each part's entry and fade (bars); its
                                                  * fader as the outro began (a part not in yet stays out) */
static float song_co, song_wet = 1.0f, song_hits = 1.0f, song_nthr;   /* the chord's cutoff (octaves), the echo's send
                                                  * (x), the hats' share of their hits, the noise into the echo (0..1) */
static uint8_t song_thr;                         /* the outro's last bars: every stab thrown */
volatile uint8_t en_song_ph;
volatile uint32_t en_song_b0, en_song_bo, en_song_be;
/* each part's entry into the intro (a share of it) and its exit from the outro (where it starts, how long it takes,
 * shares of it): Ambient first (Fog, Haze, Chamber, Night Drive) and Pulse first (Steppers, Pressure, Concrete, Basic) */
static const float SONG_IN[2][EN_NPART] = {{0.06f, 0.60f, 0.45f, 0.30f, 0.78f, 0.0f},
                                           {0.25f, 0.45f, 0.0f, 0.0f, 0.78f, 0.58f}};
static const float SONG_OUT[EN_NPART][2] = {{0.78f, 0.22f}, {0.25f, 0.2f}, {0.4f, 0.2f}, {0.55f, 0.15f}, {0.0f, 0.15f},
                                            {0.72f, 0.28f}};
float en_song_in(int kind, int part) { return SONG_IN[kind ? 1 : 0][(unsigned)part < EN_NPART ? part : 0]; }
float en_song_out(int part) { return SONG_OUT[(unsigned)part < EN_NPART ? part : 0][0]; }
static void song_publish(void)
{
    en_song_ph = song_ph;
    en_song_b0 = song_b0, en_song_bo = song_bo_on ? song_bo : 0u, en_song_be = song_bo_on ? song_be : 0u;
}
static void song_plan(void)                      /* (the main's and the outro's lengths, as the knobs have them now) */
{
    if (song_fixed || song_ph == EN_SG_OUTRO || song_ph == EN_SG_END || !song_bo_on)
        return;
    song_bo = song_b0 + 8u * (uint32_t)par[P_SMAIN];
    if (song_bo < bar)                           /* (Main turned down past where it is: the outro from the next phrase) */
        song_bo = (bar + 7u) & ~7u;
    song_be = song_bo + 8u * (uint32_t)par[P_SOUTRO];
}
static void song_back(void)                      /* (the song's offsets off, the faders up: the main, or Endless) */
{
    int i;
    for (i = 0; i < EN_NPART; i++)
        fd_tgt[i] = 1.0f;
    song_co = 0.0f, song_wet = 1.0f, song_hits = 1.0f, song_thr = 0, song_nthr = 0.0f;
}
static void song_start(void)                     /* PLAY from the top: Endless as ever, or a song from its intro */
{
    int i;
    song_end_now = song_fixed = song_ask = 0;
    song_co = 0.0f, song_wet = 1.0f, song_hits = 1.0f, song_nthr = 0.0f, song_thr = 0;
    song_b0 = 0u;
    if (!song_mode) {
        song_ph = EN_SG_OFF, song_bo_on = 0;
        faders_full();
        song_publish();
        return;
    }
    song_b0 = 8u * (uint32_t)par[P_SINTRO];
    song_bo_on = 1;
    song_ph = song_b0 ? EN_SG_INTRO : EN_SG_MAIN;
    song_plan();
    for (i = 0; i < EN_NPART; i++) {             /* (each part's entry, off the phrase lines; its fade, 2-4 bars, done by
                                                  * the intro's end) */
        const float e = SONG_IN[par[P_SKIND] ? 1 : 0][i] * (float)song_b0;
        song_ent[i] = e > 0.0f ? fm_clampf(e + 2.0f * frand() - 1.0f, 1.0f, (float)song_b0 - 1.0f) : 0.0f;
        song_fd[i] = fm_clampf(fm_minf(2.0f + 2.0f * frand(), (float)song_b0 - song_ent[i]), 1.0f, 4.0f);
        fd_pos[i] = fd_tgt[i] = fd_g[i] = en_fader[i] = song_b0 ? 0.0f : 1.0f;
    }
    fd_quiet = song_b0 ? 0x3Fu : 0u;
    song_publish();
}
/* the kick's shape the song gives this bar (-1: the piece's own): the intro's One, Half, Four; the outro's thinning */
static int song_kick(void)
{
    const float b = (float)bar;
    if (song_ph == EN_SG_INTRO) {
        const float x = (b - song_ent[3]) / fm_maxf((float)song_b0 - song_ent[3], 1.0f);
        return x < 0.4f ? KS_ONE : x < 0.75f ? KS_HALF : KS_FOUR;
    }
    if (song_ph == EN_SG_OUTRO && song_be > song_bo) {
        const float o = (b - (float)song_bo) / (float)(song_be - song_bo);
        return o < 0.25f ? -1 : o < 0.42f ? KS_HALF : KS_ONE;
    }
    return -1;
}
static void song_bar(void)                       /* a bar's start: the song's places (before the phrase is set) */
{
    int i;
    if (song_ph == EN_SG_INTRO && bar >= song_b0)
        song_ph = EN_SG_MAIN;
    if (song_bo_on && song_ph != EN_SG_OUTRO && song_ph != EN_SG_END && bar >= song_bo) {
        song_ph = EN_SG_OUTRO, song_ask = 0;
        for (i = 0; i < EN_NPART; i++)
            song_cap[i] = fd_tgt[i];
    }
    if (song_ph == EN_SG_OUTRO && bar >= song_be) {
        song_ph = EN_SG_END;
        song_end_now = 1;
    }
    song_publish();
}
static void song_tick(void)                      /* each sixteenth: the faders' targets and the song's offsets */
{
    const float t = (float)sixteenth * (1.0f / 16.0f);
    uint32_t q = 0u;
    int i;
    if (song_ph == EN_SG_OFF)
        return;
    if (song_ph == EN_SG_INTRO) {
        const float u = t / fm_maxf((float)song_b0, 1.0f);
        for (i = 0; i < EN_NPART; i++)
            fd_tgt[i] = fm_clampf((t - song_ent[i]) / song_fd[i], 0.0f, 1.0f);
        song_co = -(1.0f - u);                   /* (the chord's filter an octave under, opening) */
        song_wet = 1.0f + 0.35f * (1.0f - u);
        song_hits = fm_clampf(0.3f + 0.7f * (t - song_ent[4]) / fm_maxf((float)song_b0 - song_ent[4], 1.0f), 0.3f, 1.0f);
    } else if (song_ph == EN_SG_MAIN) {
        song_back();
    } else if (song_ph == EN_SG_OUTRO) {
        const float o = song_be > song_bo ? (t - (float)song_bo) / (float)(song_be - song_bo) : 1.0f;
        for (i = 0; i < EN_NPART; i++)
            fd_tgt[i] = fm_minf(song_cap[i], 1.0f - fm_clampf((o - SONG_OUT[i][0]) / SONG_OUT[i][1], 0.0f, 1.0f));
        song_co = 0.0f, song_wet = 1.0f, song_hits = 1.0f;
        song_thr = (uint8_t)(o >= 0.6f);         /* (the last bars: the chord thrown, the noise into the echo) */
        song_nthr = fm_clampf((o - 0.6f) * 5.0f, 0.0f, 1.0f);
    } else {
        for (i = 0; i < EN_NPART; i++)
            fd_tgt[i] = 0.0f;
    }
    for (i = 0; i < EN_NPART; i++)
        if (fd_tgt[i] <= 0.0f && fd_pos[i] < 0.001f)
            q |= 1u << i;
    fd_quiet = q;
}
static void song_outro(void)                     /* HOME held + PLAY: the outro from the next phrase; again: the main */
{
    const uint32_t nb = (bar + 7u) & ~7u;        /* (the next phrase's first bar: the next to start, if it's one) */
    if (!en_playing || song_ph == EN_SG_END)
        return;
    if (song_ph == EN_SG_OUTRO || song_ask) {      /* (in it, or asked: back) */
        song_ask = 0;
        if (!song_mode) {
            song_ph = EN_SG_OFF, song_bo_on = 0, song_fixed = 0;
            song_back();
        } else {
            if (song_ph == EN_SG_OUTRO)
                song_ph = EN_SG_MAIN;
            song_fixed = 0;
            song_plan();
            if (song_bo < nb + 32u)              /* (four phrases of the main at least, then the outro as planned) */
                song_bo = nb + 32u, song_be = song_bo + 8u * (uint32_t)par[P_SOUTRO], song_fixed = 1;
        }
    } else {                                     /* (asked: from the next phrase, in any mode) */
        if (song_ph == EN_SG_INTRO)
            song_b0 = nb;                        /* (the intro cut short: those not in yet stay out) */
        if (song_ph == EN_SG_OFF)
            song_ph = EN_SG_MAIN;
        song_bo_on = 1, song_fixed = 1, song_ask = 1;
        song_bo = nb;
        song_be = nb + 8u * (uint32_t)par[P_SOUTRO];
    }
    song_publish();
}

/* ------------------------------------------------------- the arrangement --- */
static void section_set(int s)
{
    float bl = 16.0f * step_len;
    sec = (uint8_t)s;
    en_section = (uint8_t)s;
    lay_kick = lay_hats = lay_open = lay_bass = lay_stabs = 1;
    switch (s) {
    case SEC_BREAK:                             /* the kick and the bass out; dark, then building */
        lay_kick = lay_bass = lay_hats = 0;
        lay_open = motion < 0.5f;
        arc_tgt = -1.0f;
        arc_tau = 0.7f * bl;
        break;
    case SEC_GROOVE:                            /* fewer stabs, the drums to the front */
        arc_tgt = -0.2f + 0.5f * frand();
        arc_tau = 3.0f * bl;
        break;
    case SEC_THIN:                              /* no bass, no shaker */
        lay_bass = lay_hats = 0;
        arc_tgt = -0.5f + 0.6f * frand();
        arc_tau = 3.0f * bl;
        break;
    default:
        arc_tgt = -0.1f + 1.1f * frand();
        arc_tau = 4.0f * bl;
        break;
    }
    en_pulse_rest = (uint8_t)!lay_kick;
}

/* the next phrase (the classics pass, REPORT9: the classics hold one loop for minutes, the kick never leaving). Motion
 * in three regions: under 40 the loop holds (Full only: the change is the filter's, the echo's, the texture's); 40-70
 * a variation now and then (Groove or Thin, a phrase or two; the kick half-time at most); from 70 Breaks too (the
 * kick and the bass out, a phrase). Within a region, more often as Motion rises; at least four phrases (32 bars) in
 * Full between departures */
static uint8_t sec_age;                         /* phrases in Full since the last departure */
static uint8_t ph_age, ph_reset;                /* phrases since the kick's shape or the plan last changed */
static uint8_t kick_now;                        /* (the kick's Density turned: its shape drawn again at the next bar) */
static uint8_t nz_now;                          /* (the noise's Density turned: its groove drawn again at the next bar) */
static uint8_t kick_hand;                       /* (and from then on, the piece's opening on four left be: the knob's) */
static int section_pick(void)
{
    float r = frand(), m;
    if (motion < 0.4f)
        return SEC_FULL;
    if (sec != SEC_FULL)                         /* back, mostly; from a Break, a Thin now and then */
        return r < 0.75f ? SEC_FULL : sec == SEC_BREAK ? SEC_THIN : motion >= 0.7f && frand() < 0.5f ? SEC_BREAK : sec;
    if (sec_age < 4)
        return SEC_FULL;
    m = motion < 0.7f ? (motion - 0.4f) * (1.0f / 0.3f) : (motion - 0.7f) * (1.0f / 0.3f);
    if (r >= 0.15f + 0.35f * m)
        return SEC_FULL;
    if (motion >= 0.7f && frand() < 0.4f + 0.3f * m)
        return SEC_BREAK;
    return frand() < 0.5f ? SEC_GROOVE : SEC_THIN;
}

/* (1.3.0) the drums' and the bass's phrase plan (the user: randomness by phrase and bar, not by note): a phrase is
 * A A A B A A A B. A is the main bar, repeated exactly; B, on bars 4 and 8, a variant: sparse (breathing) or busy (a
 * fill), drawn once a phrase. Which of the hats' and the bass's notes rest (Density, Evolve's looseness), the hats'
 * feel (each step's accent and lateness) and the bass's octave note are drawn once a phrase too, from its seed, and
 * the same every bar. A new phrase keeps the plan or draws another (more often as Evolve rises; never at 0) */
static float ph_u(void)
{
    ph_rng = ph_rng * 1664525u + 1013904223u;
    return (float)(ph_rng >> 8) * (1.0f / 16777216.0f);
}
static uint32_t ph_hash(uint32_t a)
{
    a ^= a >> 16, a *= 0x7FEB352Du, a ^= a >> 15, a *= 0x846CA68Bu, a ^= a >> 16;
    return a;
}
/* of a pattern's steps (but those always kept), keep the first `keep` in an order drawn from the phrase's seed */
static uint32_t ph_thin(uint32_t pat, int keep, uint32_t always, uint32_t salt)
{
    uint32_t out = pat & always, rest = pat & ~always;
    while (keep-- > 0 && rest) {
        uint32_t best = 0, bh = 0;
        int i;
        for (i = 0; i < 16; i++)
            if (rest >> i & 1u) {
                uint32_t h = ph_hash(ph.seed ^ salt * 0x9E3779B9u ^ (uint32_t)i);
                if (!best || h < bh)
                    best = 1u << i, bh = h;
            }
        out |= best, rest &= ~best;
    }
    return out;
}
static void phrase_plan(int first)                /* (1: the first; 2: a new one, decided a bar before; 0: kept) */
{
    int i;
    if (!first && ph.drawn)
        return;                                  /* (the last phrase's plan again: a motif that comes back) */
    ph.drawn = 1;
    ph.seed = ph_rng = ph_rng * 1664525u + 1013904223u;
    ph.busy = (uint8_t)(ph_u() < 0.5f);
    ph.oct = (int8_t)(ph_u() < ev_of(VY_BASS) * 0.6f ? (int)(ph_u() * 8.0f) : -1);
    for (i = 0; i < 16; i++) {
        ph.vel[i] = (uint8_t)(255.0f * fm_minf(0.3f + 0.4f * ph_u() + (i & 1 ? 0.0f : 0.15f), 1.0f));
        ph.late[i] = (uint8_t)(255.0f * ph_u());
    }
}
static int pcount(uint32_t m) { return __builtin_popcount(m); }
/* this bar's patterns, from the plan: A, or (bars 4 and 8) B */
static void ph_bar(int pb)
{
    const int b = pb == 3 || pb == 7;
    const float lo = loose(), e = (float)mac[EN_MAC_ENERGY] * (1.0f / 50.0f), d0 = (float)par[P_BNOTES] * 0.01f;
    const float d = fm_clampf(e < 0.0f ? d0 * (1.0f + 0.6f * e) : d0 + (1.0f - d0) * e, 0.0f, 1.0f);
    uint32_t g = en_groove(gpat()), f = g & (0u - g);   /* (the groove; its first note, always kept) */
    uint32_t k = drum_pat[0], sh = drum_pat[1], op = drum_pat[2];
    bar_bass = ph_thin(g, (int)((float)pcount(g) * d + 0.5f) - (f ? 1 : 0), f, 3u);   /* Density: the same notes out */
    if (b && ph.busy && d >= 0.3f)               /* (B, busy: the whole groove and a pickup into the next bar) */
        bar_bass = g | 1u << 14;
    else if (b)                                   /* (B, sparse: every other of A's, the first kept) */
        bar_bass = ph_thin(bar_bass, pcount(bar_bass) / 2 - (f ? 1 : 0), f, 4u);
    {   /* (1.4.0) the bass's fifths, drawn from the phrase's seed (Fifth: how many), the same each A bar; each groove
         * step's note against the root's, for the screen */
        const uint32_t fst = bar_bass & (0u - bar_bass);
        const int root = bass_note(chord_pcs[0]), drone = par[P_GROOVE] == 0;
        int i, k = 0, cnt = pcount(bar_bass) > 1 ? pcount(bar_bass) : 1;
        bar_fifth = 0u;
        for (i = 0; i < 16; i++) {
            int off = 0;
            if (g >> i & 1u || bar_bass >> i & 1u) {
                if (chord_nt >= 3 && (drone || !(fst >> i & 1u)) &&
                    (float)(ph_hash(ph.seed ^ 0xF1F7u * (uint32_t)(i + 1)) >> 8) * (1.0f / 16777216.0f) < (float)par[P_FIFTH] * 0.01f)
                    bar_fifth |= 1u << i, off = bass_note(chord_pcs[2]) - root;
                if (bar_bass >> i & 1u) {
                    if (ph.oct >= 0 && k == ph.oct % cnt && i)
                        off += 12;
                    k++;
                }
            }
            en_bass_off[i] = (int8_t)off;
        }
    }
    if (par[P_DMODE]) {                          /* (Pattern: yours, each step's Chance on the grid) */
        bar_k = dpat(0), bar_sh = dpat(1), bar_op = dpat(2);
        return;
    }
    bar_sh = ph_thin(sh, pcount(sh) - (int)(0.45f * lo * (float)pcount(sh) + 0.5f), 0u, 1u);
    bar_op = ph_thin(op, pcount(op) - (int)(0.6f * lo * (float)pcount(op) + 0.5f), 0u, 2u);
    bar_k = k;
    if (b && ph.busy) {                          /* (B, busy: a ghost kick before the next bar, the hats all in) */
        if (pe(P_KDENS) < 100 && k)
            bar_k = k | 1u << 14;
        bar_sh = sh | 1u << 15;
        bar_op = op;
    } else if (b) {                              /* (B, sparse: the kick's downbeat, the shaker on the eighths) */
        if (pe(P_KDENS) < 100)
            bar_k = k & 1u;
        bar_sh &= 0x5555u;
        bar_op = 0;
    }
}

static void bar_start(void)
{
    int pb = (int)(bar & 7u), db = en_drift_bars(par[P_DRIFT]);
    song_bar();                                  /* (1.4.0: the song's places: intro, main, outro, the end) */
    en_phrase_bar = (uint8_t)pb;
    if (cr_dbars > 0)                            /* (the noise's drifts: their next targets on a barline) */
        cr_dbars--;
    if (pb == 0 && bar > 0) {
        section_set(sec_next);
        sec_age = sec != SEC_FULL ? 0 : sec_age < 255 ? sec_age + 1 : 255;
        ph_age = ph_reset ? 0 : ph_age < 255 ? ph_age + 1 : 255;   /* (phrases since the kick or the plan changed) */
        ph_reset = 0;
        if (!lock) {                             /* (Lock: the kick's shape, the noise's groove, the plan, kept) */
            kick_shape = kick_next;              /* (the phrase's kick and plan, as decided a bar before) */
            hats_publish();
            noise_pattern();                     /* (the noise's groove: new, or the last one again) */
            phrase_plan(ph_new_next ? 2 : 0);    /* (the drums' and the bass's plan: new, or the last one again) */
        }
        en_change_next = 0;
    }
    if (nz_now && !lock)                         /* (the noise's Density turned: its hits from this bar) */
        noise_pattern();
    if (kick_now && !lock) {                     /* (the kick's knob turned: its new shape from this bar) */
        kick_now = 0;
        kick_hand = 1;
        kick_shape = kick_next = (uint8_t)kick_draw(sec);
        hats_publish();
        ph_age = 0;
    }
    {   /* (1.4.0) the song's kick: the intro's One, Half, Four; the outro's thinner, never fuller */
        const int ks = song_kick();
        if (ks >= 0 && ks != kick_shape && (song_ph == EN_SG_INTRO || ks < kick_shape)) {
            kick_shape = (uint8_t)ks;
            hats_publish();
        }
    }
    if (sec == SEC_BREAK && pb == 2)            /* two bars in, the filter starts to open */
        arc_tgt = 1.0f, arc_tau = 2.2f * 16.0f * step_len;
    fill = 0;
    if (pb == 7) {
        sec_next = (uint8_t)section_pick();
        /* the last bar before the kick: a fill (the kick drops out halfway, the last stab is thrown) */
        /* (the classics pass: under Motion 40 none, the kick never leaving; then into or out of a change, or now and
         * then as Motion rises) */
        if (sec_next != SEC_BREAK && (sec == SEC_BREAK || (motion >= 0.4f && (sec_next != sec ||
                                                                             frand() < 0.5f * (motion - 0.4f)))))
            fill = 1;
        /* Energy far down: the next phrase Thin (no bass, no shaker); far up: Full, with a fill into it (after the
         * draws, which stay as they were: at Energy 0 the music is the same) */
        if (mac[EN_MAC_ENERGY] <= -25)
            sec_next = SEC_THIN;
        else if (mac[EN_MAC_ENERGY] >= 25)
            sec_next = SEC_FULL, fill = 1;
        if (song_ph != EN_SG_OFF && (bar + 1u < song_b0 || (song_bo_on && bar + 1u >= song_bo)))
            sec_next = SEC_FULL;                 /* (1.4.0: the song's intro and outro, Full: no sections) */
        /* (1.3.0) the next phrase's kick and plan, decided now, so the screen can say a change is coming */
        /* (the classics pass: both held for a few phrases, 2 at Evolve 100 to 6 at 0; a change of section draws the
         * kick anew) */
        {
            /* (1.4.0: the kick's hold the drums' Vary's, the plan's the bass's: each Evolve as its layer feels it) */
            const float ed = fm_minf(ev_of(VY_DRUMS), 1.0f), eb = ev_of(VY_BASS);
            const int hold = 2 + (int)(4.0f * (1.0f - ed)), holdb = 2 + (int)(4.0f * (1.0f - fm_minf(eb, 1.0f)));
            kick_next = lock ? kick_shape : (ph_age + 1 >= hold || sec_next != sec) ? (uint8_t)kick_draw(sec_next) : kick_shape;
            ph_new_next = (uint8_t)(!lock && eb > 0.0f && ph_age + 1 >= holdb && ph_u() < 0.2f + 0.6f * eb);
            en_vary_turns[VY_BASS] += ph_new_next;
            ph_reset = (uint8_t)(kick_next != kick_shape || ph_new_next);
        }
        en_change_next = (uint8_t)(ph_new_next | (kick_next != kick_shape) << 1 | (sec_next != sec) << 2);
    }
    if (!en_frozen && (change_pending || (db && bar > 0 && chord_bars >= (uint32_t)db))) {
        change_pending = 0;
        chord_change();
    }
    en_drift = db ? fm_clampf((float)chord_bars / (float)db, 0.0f, 1.0f) : 0.0f;
    chord_bars++;
    if ((bar & 1u) == 0u && bar > 0 && frand() < evolve * 0.6f * vary_t)   /* (1.3.0: every two bars, was four;
                                                  * 1.4.0: Vary's, the chord's own, Evolve over it) */
        stabs_evolve();
    if (pb == 0 && !lock && frand() < ev_of(VY_DRUMS) * 0.8f)   /* (1.3.0: the shaker's steps trade places at a phrase's start, not
                                                  * any bar: the A bars the same) */
        hats_evolve();
    stabs_bar();
    ph_bar(pb);                                  /* (this bar's drums and bass, from the plan) */
    if (song_hits < 1.0f) {                      /* (1.4.0: the song's intro, the hats coming in: few, growing) */
        bar_sh = ph_thin(bar_sh, (int)((float)pcount(bar_sh) * song_hits + 0.5f), 0u, 5u);
        if (song_hits < 0.7f)
            bar_op = 0u;
    }
    bar++;
}

/* the kick's shape for the next phrase: around Density (0..100 as 0..4 on the ladder; 100: four, always, as before
 * 1.3.0), each shape's weight falling with its distance (less steeply as Evolve rises), none at half; a Thin phrase
 * a step sparser. A piece starts on four (its first two phrases), then wanders down to Density over the next six;
 * and after a Break the kick comes back whole (the user: start on four on the floor, then let it evolve) */
static int kick_draw(int next)
{
    const int v = pe(P_KDENS);
    const uint32_t phr = (bar + 1u >= song_b0 ? bar + 1u - song_b0 : 0u) >> 3;   /* (the next phrase's number: 0 at
                                                  * the top, the song's main's: 1.4.0) */
    float x = (float)v * 0.04f, lam = 1.4f - fm_minf(ev_of(VY_DRUMS), 1.0f), w[KS_N], t = 0.0f, r;
    int k;
    if (!kick_hand && (phr < 2u || (sec == SEC_BREAK && next != SEC_BREAK && next != SEC_THIN)))
        return KS_FOUR;                          /* (the piece's opening on four, the kick whole after a Break: till the
                                                  * knob is turned) */
    if (v >= 90) {                               /* (the classics pass: the dub pieces' density, four in Full and in a
                                                  * Groove; a Thin, ghosted or half) */
        if (next != SEC_THIN)
            return KS_FOUR;
        return frand() < 0.5f ? KS_GHOST : KS_HALF;
    }
    if (phr < 8u && !kick_hand)
        x += (4.0f - x) * (float)(8u - phr) * (1.0f / 6.0f);
    if (next == SEC_THIN)
        x -= 1.0f;
    for (k = 0; k < KS_N; k++) {
        w[k] = fm_expf(-fm_fabsf((float)k - x) * lam) * (k == KS_NONE ? 0.5f : 1.0f);
        t += w[k];
    }
    r = frand() * t;
    for (k = 0; k < KS_N - 1; k++)
        if ((r -= w[k]) < 0.0f)
            break;
    if (k == KS_NONE && next == SEC_FULL && motion < 0.7f)   /* (the kick gone only in a Break, or from Motion 70) */
        k = KS_ONE;
    return k;
}

/* a lane's steps as they play: the pattern page's (Play: Pattern), or the generated ones */
static uint32_t dpat(int d) { return par[P_DMODE] ? (uint32_t)(uint16_t)par[P_DPAT0 + d] : drum_pat[d]; }

/* the screen's lanes, the bar as it plays: what is muted or at level 0, gone at once */
static void lanes(void)
{
    const uint32_t mu = mutes | fd_quiet;   /* (1.4.0: a part the song has out, as muted) */
    const int kick = lay_kick && !(mu & EN_M_KICK) && !(perf & EN_PF_KICKDROP) && par[P_DRUMS] > 0,
              hats = !(mu & EN_M_HATS) && par[P_DRUMS] > 0;
    en_lane[0] = (uint16_t)(mu & EN_M_CHORD || !par[P_CHORD] ? 0u : en_stab_pat);
    en_lane[1] = (uint16_t)(kick ? bar_k & (fill ? 0x00FFu : 0xFFFFu) : 0u);
    en_lane[2] = (uint16_t)(hats ? (lay_hats ? bar_sh : 0u) | (lay_open || fill ? bar_op : 0u) : 0u);
    en_lane[3] = (uint16_t)(hats && lay_hats ? dpat(3) : 0u);
    en_lane[4] = (uint16_t)(hats && (sec == SEC_FULL || sec == SEC_GROOVE || par[P_DMODE]) ? dpat(4) : 0u);
    en_lane[5] = (uint16_t)(lay_bass && !(mu & EN_M_BASS) && !(perf & EN_PF_KICKDROP) && par[P_BASS] > 0 ? bar_bass : 0u);
    en_bass_gate = fm_minf(GROOVE_GATE[gpat()] * fm_exp2f(rel(P_BLEN)), 15.8f);   /* (the bass's page: its notes' length) */
    en_bass_octn = (int8_t)(ph.oct >= 0 ? ph.oct % (pcount(bar_bass) > 1 ? pcount(bar_bass) : 1) : -1);
    en_lane_sh = (uint16_t)(hats && lay_hats ? bar_sh : 0u);   /* (the Drums page: the hats apart, and how loud) */
    en_lane_op = (uint16_t)(hats && (lay_open || fill) ? bar_op : 0u);
    {
        int i;
        for (i = 0; i < 16; i++) {
            float kv = 1.0f, hv = par[P_DMODE] ? 0.8f : (float)ph.vel[i] * (1.0f / 255.0f);
            if (bar_k >> i & 1u && i == 14 && !(KSHAPE[kick_shape] >> 14 & 1u))
                kv = 0.45f;
            if (!par[P_DMODE] && kick_shape == KS_GHOST && (i == 4 || i == 12))
                kv = 0.32f;
            if (par[P_DMODE] && !((uint32_t)(uint16_t)par[P_DACC0] >> i & 1u))
                kv = 1.0f - 0.006f * (float)par[P_DACCENT];
            if (par[P_DMODE] && !((uint32_t)(uint16_t)par[P_DACC0 + 1] >> i & 1u))
                hv = 1.0f - 0.006f * (float)par[P_DACCENT];
            en_kick_v[i] = (uint8_t)(kv * 255.0f);
            en_hat_v[i] = (uint8_t)(fm_minf(hv, 1.0f) * 255.0f);
        }
    }
}

/* a pattern's hit: its chance (Play: Pattern only, so Auto's music stays as it was), its accent */
static int dhit(int d, int st, float *vel)
{
    if (!par[P_DMODE])
        return 1;
    if (par[P_DCH0 + d] < 100 && frand() * 100.0f >= (float)par[P_DCH0 + d])
        return 0;
    if (!((uint32_t)(uint16_t)par[P_DACC0 + d] >> st & 1u))
        *vel *= 1.0f - 0.006f * (float)par[P_DACCENT];   /* (the unaccented, quieter as Accent turns up) */
    return 1;
}

static void lfo_tick(void);
static void play_stop(void);
/* the grid: one call a sixteenth (MIDI clock: 6 ticks) */
static void tick(void)
{
    int st = (int)(sixteenth & 15u), l, d;
    float r = frand();
    gate_ctr = 0;
    if (st == 0) {
        bar_start();
        en_stab_thr = 0;                         /* (the chord's page: a new bar's stabs) */
        for (l = 0; l < 16; l++)
            en_stab_v[l] = 0;
        if (song_end_now) {                      /* (1.4.0: the song's end: the grid stops, the tails ring on) */
            song_end_now = 0;
            play_stop();
            return;
        }
    }
    song_tick();
    const int kick = lay_kick && !(mutes & EN_M_KICK) && !(perf & EN_PF_KICKDROP), hats = !(mutes & EN_M_HATS);
    lanes();
    {
        const float fv = (float)ph.vel[st] * (1.0f / 255.0f);   /* (the phrase's feel: this step's accent) */
        float v0 = st ? 0.88f : 1.0f, v1 = fv, v2 = 0.55f + 0.35f * fv, v3 = 0.75f + 0.15f * r;
        ph_st = st;
        if (bar_k >> st & 1u && st == 14 && !(KSHAPE[kick_shape] >> 14 & 1u))
            v0 *= 0.45f;                         /* (the busy B bar's pickup: a ghost) */
        if (!par[P_DMODE] && kick_shape == KS_GHOST && (st == 4 || st == 12))
            v0 *= 0.32f;                         /* (two and four ghosted, -10 dB) */
        if (kick && bar_k >> st & 1u && !(fill && st >= 8) && dhit(0, st, &v0))
            drum_hit(0, v0, 0);
        else if (!(st & 3) && par[P_DRUMS] > 0) {   /* the ghost duck: the music breathes on every beat, the kick
                                                    * or not (lighter without one; lightest in a Break) */
            const float gd = lay_kick ? 0.7f : 0.45f;
            duck_x = fm_maxf(duck_x, gd);
            duck_xb = fm_maxf(duck_xb, gd);
        }
        if (hats && lay_hats && bar_sh >> st & 1u && dhit(1, st, &v1))
            drum_hit(1, v1, st & 1);
        if (hats && (lay_open || fill) && bar_op >> st & 1u && dhit(2, st, &v2))
            drum_hit(2, v2, 0);
        if (hats && lay_hats && dpat(3) >> st & 1u && dhit(3, st, &v3))
            drum_hit(3, v3, st & 1);
        if (en_lane[4] >> st & 1u) {             /* the clap: rare (Auto: 3% + 5% x Evolve, ~5% at 40; Pattern: its Chance) */
            float v4 = 0.8f + 0.2f * r;
            if (par[P_DMODE] ? dhit(4, st, &v4) : frand() < 0.03f + 0.05f * ev_of(VY_DRUMS))
                drum_hit(4, v4, 0);
        }
    }
    if (lay_bass && !(mutes & EN_M_BASS) && !(perf & EN_PF_KICKDROP) && bar_bass >> st & 1u) {
        /* (1.3.0) the bar's notes, from the plan (Density and Energy: which rest, the same each A bar); the octave
         * note, if the plan has one, the same place each bar */
        const uint32_t first = bar_bass & (0u - bar_bass), below = bar_bass & ((1u << st) - 1u);
        bass_hit(first >> st & 1u, ph.oct >= 0 && pcount(below) == ph.oct % (pcount(bar_bass) > 1 ? pcount(bar_bass) : 1) && st,
                 st);
    }
    if (en_stab_pat >> st & 1u) {               /* (the random draws made muted or not: the music goes on the same) */
        uint32_t later = en_stab_pat >> (st + 1);
        int last = !later, thr = (fill && last) || frand() < fm_minf(0.12f * loose() * (sec == SEC_BREAK ? 1.5f : 1.0f) * throw_mul, 0.6f);
        float vel = st == (int)__builtin_ctz(en_stab_pat) ? 0.9f + 0.1f * frand() : 0.65f + 0.25f * frand();
        if (!(mutes & EN_M_CHORD))
            stab_fire(vel, thr || (perf & EN_PF_THROW) || (fxm >> EN_FX_THROW & 1u) || song_thr);
        en_stab_v[st] = (uint8_t)(fm_minf(vel, 1.0f) * 255.0f);   /* (the chord's page: as struck) */
        if (en_stab_thrown)
            en_stab_thr |= (uint16_t)(1u << st);
        en_stab_t60 = st_t60;
    }
    noise_step((int)(sixteenth & 31u));         /* (the noise's groove: two bars) */
    for (d = 0; d < EN_NDRUM; d++)
        en_drum_pos[d] = (uint8_t)st;
    {   /* (1.4.0, the overview's pings row) this bar's sixteenths a ping falls on: those that rang so far, kept; then
         * the loops' notes ahead, where each loop will be (a pass's end may still change them) */
        uint32_t m = en_lane_png & ((st & 1 ? 2u : 1u) << st) - 1u;
        int q;
        if (!st)
            m = 0u;
        if ((sixteenth & 1u) == 0u) {           /* (this step's: what rings, below) */
            for (l = 0; l < EN_NLOOP; l++)
                if (loop_note[l][loop_pos[l]] >= 0 && !loop_rest[l] && !((mutes | fd_quiet) & EN_M_PINGS) && par[P_PINGS] > 0)
                    m |= 1u << st;
        }
        if (!((mutes | fd_quiet) & EN_M_PINGS) && par[P_PINGS] > 0)
            for (q = (st + 2) & ~1; q < 16; q += 2)
                for (l = 0; l < EN_NLOOP; l++) {
                    const int k = loop_pos[l] + (q - ((st + 2) & ~1)) / 2 + ((sixteenth & 1u) == 0u ? 1 : 0),
                              len = EN_LOOP_LEN[l];
                    if (loop_note[l][k % len] >= 0 && (k >= len || !loop_rest[l]))
                        m |= 1u << q;
                }
        en_lane_png = (uint16_t)m;
    }
    if ((sixteenth & 1u) == 0u) {               /* the loops step in eighths */
        for (l = 0; l < EN_NLOOP; l++) {
            int s = loop_pos[l];
            if (loop_note[l][s] >= 0 && !loop_rest[l] && !(mutes & EN_M_PINGS))
                bell_ring(l, s);
            en_loop_pos[l] = (uint8_t)s;
            if (++loop_pos[l] >= EN_LOOP_LEN[l]) {
                loop_pos[l] = 0;
                loop_pass(l);
            }
        }
    }
    sixteenth++;
    en_bars = sixteenth;
    lfo_tick();
}

/* ------------------------------------------------- reverb: Dattorro's plate --- */
/* J. Dattorro, "Effect Design, Part 1", JAES 1997 (as in FoMni): a predelay, four input diffusers,
 * then a figure-eight tank of two halves, each a modulated allpass, a delay, a damping lowpass, an
 * allpass and a delay, feeding the other half. Stereo out from seven taps a side. The paper's lengths
 * are for 29761 Hz; DS() scales them to 44.1 kHz. */
/* (the classics pass: the plate at half the rate, 22.05 kHz: a quarter of the engine's time halved, its memory too;
 * nothing of it above ~10 kHz was heard: its input band-limited, its damping) */
#define DS(n) (((n) * 7409 + 5000) / 10000)           /* 22050 / 29761 */
#define PRE_N 662                                    /* 30 ms */
#define EXC DS(16)
enum { L_PRE, L_IN1, L_IN2, L_IN3, L_IN4, L_APL, L_D1L, L_AP2L, L_D2L, L_APR, L_D1R, L_AP2R, L_D2R, L_N };
static const int DL_LEN[L_N] = {PRE_N, DS(142), DS(107), DS(379), DS(277), DS(672) + EXC + 2, DS(4453), DS(1800),
                                DS(3720), DS(908) + EXC + 2, DS(4217), DS(2656), DS(3163)};
#define DL_TOTAL (PRE_N + DS(142) + DS(107) + DS(379) + DS(277) + DS(672) + DS(4453) + DS(1800) + DS(3720) + DS(908) + \
                  DS(4217) + DS(2656) + DS(3163) + 2 * (EXC + 2))
static float pl_buf[DL_TOTAL] EN_POOL;
typedef struct {
    float *b;
    int n, i;
} dl_t;
static dl_t dl[L_N];
static float pl_bw, pl_damp_l, pl_damp_r, pl_decay = 0.5f, pl_decay_t = 0.5f, pl_dd2 = 0.5f, lfo_s, lfo_c = 1.0f;
static float pl_acc, pl_ol, pl_or;               /* (the plate at half the rate: the last input, its last output) */
static uint32_t pl_ph;

static inline float tap(const dl_t *d, int k)
{
    int j = d->i - k;
    return d->b[j < 0 ? j + d->n : j];
}
static inline void push(dl_t *d, float x)
{
    d->b[d->i] = x;
    if (++d->i >= d->n)
        d->i = 0;
}
static inline float allpass(dl_t *d, int len, float x, float g)
{
    float z = tap(d, len), v = x - g * z;
    push(d, fm_flush(v));
    return z + g * v;
}
static inline float allpass_mod(dl_t *d, float len, float x, float g)
{
    int k = (int)len;
    float f = len - (float)k, z = tap(d, k) + (tap(d, k + 1) - tap(d, k)) * f, v = x - g * z;
    push(d, fm_flush(v));
    return z + g * v;
}

static inline void reverb(float in, float *ol, float *or_)
{
    float x, a, b, fb_l = tap(&dl[L_D2R], DS(3163)), fb_r = tap(&dl[L_D2L], DS(3720));
    push(&dl[L_PRE], in);
    x = tap(&dl[L_PRE], PRE_N);
    pl_bw += 0.84f * (x - pl_bw);                /* (0.6 at the full rate) */
    x = allpass(&dl[L_IN1], DS(142), pl_bw, 0.75f);
    x = allpass(&dl[L_IN2], DS(107), x, 0.75f);
    x = allpass(&dl[L_IN3], DS(379), x, 0.625f);
    x = allpass(&dl[L_IN4], DS(277), x, 0.625f);
    {
        float s = lfo_s + 0.0002f * lfo_c, c = lfo_c - 0.0002f * s;   /* (the same speed at half the rate) */
        lfo_s = s;
        lfo_c = c;
    }
    a = allpass_mod(&dl[L_APL], (float)DS(672) + (float)EXC * lfo_s, x + pl_decay * fb_l, -0.7f);
    push(&dl[L_D1L], a);
    b = tap(&dl[L_D1L], DS(4453));
    pl_damp_l += 0.64f * (b - pl_damp_l);        /* (0.4 at the full rate) */
    b = allpass(&dl[L_AP2L], DS(1800), pl_damp_l * pl_decay, pl_dd2);
    push(&dl[L_D2L], b);
    a = allpass_mod(&dl[L_APR], (float)DS(908) + (float)EXC * lfo_c, x + pl_decay * fb_r, -0.7f);
    push(&dl[L_D1R], a);
    b = tap(&dl[L_D1R], DS(4217));
    pl_damp_r += 0.64f * (b - pl_damp_r);
    b = allpass(&dl[L_AP2R], DS(2656), pl_damp_r * pl_decay, pl_dd2);
    push(&dl[L_D2R], b);
    *ol = 0.6f * (tap(&dl[L_D1R], DS(266)) + tap(&dl[L_D1R], DS(2974)) - tap(&dl[L_AP2R], DS(1913)) +
                  tap(&dl[L_D2R], DS(1996)) - tap(&dl[L_D1L], DS(1990)) - tap(&dl[L_AP2L], DS(187)) -
                  tap(&dl[L_D2L], DS(1066)));
    *or_ = 0.6f * (tap(&dl[L_D1L], DS(353)) + tap(&dl[L_D1L], DS(3627)) - tap(&dl[L_AP2L], DS(1228)) +
                   tap(&dl[L_D2L], DS(2673)) - tap(&dl[L_D1R], DS(2111)) - tap(&dl[L_AP2R], DS(335)) -
                   tap(&dl[L_D2R], DS(121)));
}

static void plate_init(void)
{
    float *p = pl_buf;
    int k, i;
    for (k = 0; k < L_N; k++) {
        dl[k].b = p;
        dl[k].n = DL_LEN[k];
        dl[k].i = 0;
        p += DL_LEN[k];
    }
    for (i = 0; i < DL_TOTAL; i++)
        pl_buf[i] = 0.0f;
    pl_bw = pl_damp_l = pl_damp_r = 0.0f;
    lfo_s = 0.0f;
    lfo_c = 1.0f;
}

/* ------------------------------------------------- echo: tape and Echoplex --- */
/* The dub echo: a tape loop (up to ~5.6 s, synced to the tempo) at a quarter rate (11 kHz, 16-bit,
 * mono), with two heads: the first plays the loop's length, the second a part of it (3/4 at first),
 * each panned to its side. In the feedback, a band-pass (each repeat thinner and darker) that the sweep
 * and a slow drift move, and a soft clip: the feedback may pass 100% and the echo swell on itself without
 * running away. With Motion the heads wander, the feedback leans now on one, now on the other, and now
 * and then the second slides to another part of the loop (the repeats bend in pitch on the way). A
 * thrown stab pushes the feedback up a little. Frozen (dub hold), the loop stops listening and plays
 * what it holds, sample for sample, for ever. A short Echoplex (~160 ms) smears what goes in. */
#define TAPE_N 62000                             /* 5.6 s at 11.025 kHz */
#define TAPE_SR 11025.0f
static int16_t tape[TAPE_N] EN_POOL;
static int tape_w, tape_ph;
static float tape_acc, tape_lp, tape_hp, tape_p1, tape_p2, tape_c1, tape_c2, tape_p3, tape_c3;   /* (3: the right's, a
                                                 * few ms behind the left: Width) */
static const float HEAD2[5] = {0.75f, 0.6667f, 0.5f, 0.375f, 0.25f};    /* the second head's parts */
static float head2 = 0.75f, head2_t = 0.75f, head2_v;   /* its place (of the loop), target, speed */
static float fb_lean;                            /* 0..1: how much of the feedback is the second head's */
static float hg[4] = {1.0f, 0.0f, 0.0f, 1.0f};   /* the heads' pan gains: 1 left, 1 right, 2 left, 2 right */
static uint32_t head_left;                       /* samples until the second head may move */
#define PLEX_N 7100                              /* 160 ms at 44.1 kHz */
#define PLEX_TAP 2700                            /* the right's tap: ~100 ms */
static int16_t plex[PLEX_N];
static int stt_state, stt_len, stt_pos;         /* Stutter: 0 off, 1 recording a sixteenth, 2 looping it */
static float stt_mix;
static int plex_w;
static float plex_lp, plex_lp2;

static inline float tape_read(float d)           /* d quarter-rate samples ago */
{
    int k = (int)d, i0, i1;
    float f = d - (float)k, a, c;
    i0 = tape_w - k;
    i1 = i0 - 1;
    if (i0 < 0)
        i0 += TAPE_N;
    if (i1 < 0)
        i1 += TAPE_N;
    a = (float)tape[i0];
    c = (float)tape[i1];
    return (a + (c - a) * f) * (1.0f / 12000.0f);
}
static inline int16_t s16(float x)
{
    x *= 12000.0f;
    return (int16_t)(x > 32767.0f ? 32767.0f : x < -32767.0f ? -32767.0f : x);
}

static void echo_time(void)
{
    int i = par[P_ETIME];
    float beat, t;
    if (par[P_TEMPO] <= 0)
        return;                                  /* not yet (en_init applies in order) */
    beat = 60.0f / (float)par[P_TEMPO];
    t = ETIME_BEATS[i] * beat;
    if (t * TAPE_SR > (float)(TAPE_N - 8))
        t = (float)(TAPE_N - 8) / TAPE_SR;
    echo_len_t = t * TAPE_SR;
    if (echo_len <= 0.0f)
        echo_len = echo_len_t;
}

/* once a chunk: where the heads sit, how the feedback leans, the second head's slide, the band-pass */
static void heads_step(uint32_t n)
{
    float p, want, vmax, sw;
    int i;
    /* (1.3.0) the loop's gain from the tail's length: whatever the echo's time, the repeats are gone (-60 dB) in
     * Feedback's seconds; long delays vanish as quickly as short ones (Dub and the throws still push past it) */
    echo_fb = fm_exp2f(-9.966f * (echo_len_t * (1.0f / TAPE_SR)) / echo_t60);
    for (i = 0; i < 2; i++) {
        /* (1.3.0: the first head near the middle, drifting with Motion; the second, on throws, to the right; Width
         * spreads them: 0 mono, 2 as wide as before) */
        p = fm_clampf(ec_wide * ((i ? 0.4f : 0.0f) + (i ? 0.2f : 0.25f) * (0.4f + 0.6f * motion) * mod[6 + i].v), -1.0f, 1.0f);
        p = (p + 1.0f) * (FM_PI / 4.0f);
        hg[2 * i] = fm_cosf(p);
        hg[2 * i + 1] = fm_sinf(p);
    }
    fb_lean = 0.3f * motion * (1.0f + mod[8].v);
    if (head_left <= n) {
        head_left = (uint32_t)((16.0f + 32.0f * frand()) * EN_SR);
        if (en_playing && !en_frozen && frand() < motion * 0.6f)
            head2_t = HEAD2[irand(5)];
    } else {
        head_left -= n;
    }
    vmax = 0.008f / fm_maxf(echo_len, 100.0f);
    want = fm_clampf((head2_t - head2) * (1.0f / (2.0f * TAPE_SR)), -vmax, vmax);
    head2_v += (want - head2_v) * fm_minf((float)n * (1.0f / EN_SR), 1.0f);
    /* the band-pass: ~220..800 Hz below (1.3.0: from ~60..500, so the repeats thin; the classics pass: from ~165..700), ~1.1..2.6 kHz above, moving
     * up together */
    dub_up = 0.0f;                               /* (1.3.0: PERFORM's second knob is Wet now, not Dub) */
    sw = fm_clampf(0.45f + 0.5f * arcm + 0.3f * perf_sw + 0.5f * motion * mod[5].v - 0.3f * dub_up, 0.0f, 1.0f);   /* (Dub up: darker) */
    tape_hpc = 0.12f + 0.24f * sw * sw;          /* (the classics pass: ~220 Hz at the bottom, was ~165) */
    tape_lpc = 0.58f + 0.3f * sw;
}

/* one sample of both: the input (a mono send) to the tape's heads (el, er) and the Echoplex (xl, xr) */
/* 1.3.0, the echo dissolving (build/research/dubtechno/REPORT2.md, variant D): what enters the tape band-limited (Tone),
 * each repeat through five allpasses in the feedback (Blur: their gain), so a repeat comes back softer and darker
 * than the last; one head (the second only while a throw rings), the Echoplex's short taps out of the mix. (The
 * classics pass, REPORT9: the first repeat clean, a copy of the strike on its time; the blur grows with each pass,
 * the dub way. The input's allpasses moved into the feedback: they smeared even the first.) At the tape's rate: the
 * heads read where the strike comes back (the output) and, the allpasses' delay earlier, what goes round again */
#define EAP_FB 994                               /* (37 + 89 + 151 + 276 + 441: 3 to 40 ms) */
static int16_t eap[EAP_FB] EN_POOL;             /* (int16 at 1/8000: +-4) */
static int eap_i[5];
static float ec_lp, ec_hp;
static inline float eapf(int k, int off, int n, float x)
{
    int16_t *b = eap + off;
    float d = (float)b[eap_i[k]] * (1.0f / 8000.0f), y = d - ec_g * x, t = (x + ec_g * y) * 8000.0f;
    b[eap_i[k]] = (int16_t)(t > 32767.0f ? 32767.0f : t < -32767.0f ? -32767.0f : t);
    if (++eap_i[k] >= n)
        eap_i[k] = 0;
    return y;
}
static inline float ec_in(float x)
{
    ec_lp += ec_lpc * (x - ec_lp);
    ec_hp += 0.18f * (ec_lp - ec_hp);            /* (~350 Hz: the repeats above the chord's low mids) */
    return ec_lp - ec_hp;
}
static inline float ec_fb(float x)
{
    return eapf(4, 553, 441, eapf(3, 277, 276, eapf(2, 126, 151, eapf(1, 37, 89, eapf(0, 0, 37, x)))));
}

static inline void echoes(float in, float *el, float *er, float *xl, float *xr)
{
    float y = (float)plex[plex_w] * (1.0f / 12000.0f);
    int t = plex_w - PLEX_TAP;
    plex_lp += 0.3f * (y - plex_lp);
    plex_lp2 += 0.3f * ((float)plex[t < 0 ? t + PLEX_N : t] * (1.0f / 12000.0f) - plex_lp2);
    if (stt_state) {                             /* (the grid's Stutter has the Echoplex's buffer) */
        *xl = *xr = 0.0f;
    } else {
        plex[plex_w] = s16(in + 0.45f * plex_lp);
        if (++plex_w >= PLEX_N)
            plex_w = 0;
        *xl = plex_lp;
        *xr = plex_lp2;
    }
    tape_acc += in;
    if (++tape_ph >= 4) {                        /* the tape, a quarter of the time */
        float d, h1, h2, h3, f1, f2, fb, w;
        tape_ph = 0;
        echo_len += (echo_len_t * fx_em - echo_len) * fx_ek;   /* (the grid: the time doubled, the tape slowing) */
        if (echo_len > (float)(TAPE_N - 8))      /* (Tape stop slows a long echo past the tape: it stops at its end;
                                                  * the hold below reads echo_len itself. Reported by genkigenki) */
            echo_len = (float)(TAPE_N - 8);
        head2 += head2_v;
        d = echo_len * (1.0f + 0.002f * motion * mod[5].v);   /* wow */
        if (d > (float)(TAPE_N - 4))
            d = (float)(TAPE_N - 4);
        h1 = tape_read(d);                       /* (the heads: what comes out, on its time) */
        h3 = tape_read(fm_minf(d + 88.0f * fm_minf(ec_wide, 2.0f), (float)(TAPE_N - 4)));   /* (the classics pass: the
                                                  * right's 8 ms later at Width 50, 16 at 100: the repeats wide) */
        f1 = tape_read(fm_maxf(d - (float)EAP_FB, 2.0f));   /* (and what goes round, the diffusers' delay earlier) */
        if (st_throw > 1e-4f) {                  /* (the second head: only while a throw rings) */
            h2 = tape_read(d * head2);
            f2 = tape_read(fm_maxf(d * head2 - (float)EAP_FB, 2.0f));
        } else {
            h2 = f2 = 0.0f;
        }
        if (en_frozen) {                         /* dub hold: the loop, as it is */
            int k = (int)echo_len, i0 = tape_w - k;
            int16_t s = tape[i0 < 0 ? i0 + TAPE_N : i0];
            if (++tape_w >= TAPE_N)
                tape_w = 0;
            tape[tape_w] = s;
        } else {
            float g = fm_minf(echo_fb + 0.15f * st_throw + 0.12f * perf_echo + 0.1f * dub_up + 0.08f * hat_thr, fb_cap);   /* (Dub:
                                                  * past the knob; the hats' throw, a little longer, as the stabs') */
            tape_lp += tape_lpc * (f1 + fb_lean * st_throw * (f2 - f1) - tape_lp);   /* each time round: thinner, darker */
            tape_hp += tape_hpc * (tape_lp - tape_hp);
            fb = ec_fb(tape_lp - tape_hp);       /* (and more dissolved) */
            w = ec_in(tape_acc * 0.25f) + 0.7f * sat(fb * g * (1.0f / 0.7f));
            if (++tape_w >= TAPE_N)
                tape_w = 0;
            tape[tape_w] = s16(w);
        }
        tape_acc = 0.0f;
        tape_p1 = tape_c1;
        tape_p3 = tape_c3;
        tape_c3 = h3;
        tape_p2 = tape_c2;
        tape_c1 = h1;
        tape_c2 = h2;
    }
    {   /* between the tape's samples: a line from the last to the next; then to the field */
        float f = (float)tape_ph * 0.25f;
        float h1 = tape_p1 + (tape_c1 - tape_p1) * f, h2 = tape_p2 + (tape_c2 - tape_p2) * f;
        float h3 = tape_p3 + (tape_c3 - tape_p3) * f;
        *el = hg[0] * h1 + hg[2] * h2 * st_throw;   /* (the second head: only while a throw rings) */
        *er = hg[1] * (h1 + fm_minf(0.6f * ec_wide, 1.0f) * (h3 - h1)) + hg[3] * h2 * st_throw;   /* (the later head
                                                  * blended in: 60% at Width 50, all from 84) */
    }
}

/* ------------------------------------------------- cloud: a granular reverb --- */
/* After JIANT's CLOUD (github.com/juanjiant-bit/JIANT, GPL-3.0): what goes to the reverb, written at a
 * quarter rate (11 kHz, 16-bit) into a 0.74 s line, read by up to six grains born at random (denser as Cloud
 * turns up and as the filter opens), each 80..250 ms through a triangle window, from anywhere behind the
 * write, panned at random, a few cents off; now and then an octave down (rarely up). The grains go into the
 * plate, so they bloom into its tail, and a little of them back into the line: the wash builds on itself.
 * Dub hold stops the line too: the grains keep scattering over what it holds. */
#define CL_LEN 8192
#define CL_NG 6
static int16_t cl_buf[CL_LEN];                  /* (RAM, not the pool: the pool is nearly full) */
static int cl_w, cl_ph, cl_live;
static int32_t cl_next;
static float cl_acc, cl_fb, cl_pl, cl_pr, cl_cl, cl_cr, cl_dens, pl_back;
static struct {
    float pos, inc, gl, gr;
    int32_t age, len;
} cl_g[CL_NG];

static uint32_t cl_rng = 0x2545F491u;          /* its own: the Cloud knob leaves the music's choices be */
static float cl_frand(void)
{
    cl_rng = cl_rng * 1664525u + 1013904223u;
    return (float)(cl_rng >> 8) * (1.0f / 16777216.0f);
}
static int cl_irand(int n) { return (int)(cl_frand() * (float)n); }

static void cloud_birth(void)                   /* a grain, in the first voice free */
{
    int k, back;
    float r = cl_frand(), pk, p;
    for (k = 0; k < CL_NG && cl_g[k].len; k++)
        ;
    if (k >= CL_NG)
        return;
    pk = r < 0.2f ? 0.5f : r < 0.24f ? 2.0f : 1.0f;
    cl_g[k].len = 880 + cl_irand(1900);
    cl_g[k].inc = pk * (0.997f + 0.006f * cl_frand());
    back = 300 + (pk > 1.0f ? cl_g[k].len : 0) + cl_irand(CL_LEN - 3200 - (pk > 1.0f ? cl_g[k].len : 0));
    cl_g[k].pos = (float)((cl_w - back + CL_LEN) % CL_LEN);
    cl_g[k].age = 0;
    p = (cl_frand() * 1.6f - 0.8f + 1.0f) * (FM_PI / 4.0f);
    cl_g[k].gl = fm_cosf(p);
    cl_g[k].gr = fm_sinf(p);
    cl_live++;
}

/* one sample: the input (mono) in, the grains out (l, r) */
static inline void cloud(float in, float *ol, float *or_)
{
    cl_acc += in;
    if (++cl_ph >= 4) {                         /* the line and the grains, a quarter of the time */
        float l = 0.0f, r = 0.0f;
        int k;
        cl_ph = 0;
        if (!en_frozen && !(fxm >> EN_FX_FREEZE & 1u)) {   /* (the grid's Freeze: the cloud alone holds) */
            if (++cl_w >= CL_LEN)
                cl_w = 0;
            cl_buf[cl_w] = s16(cl_acc * 0.25f + (0.25f + 0.3f * cloud_amt) * cl_fb);
        }
        cl_acc = 0.0f;
        if (--cl_next <= 0) {
            int gap = 1500 - (int)(1100.0f * cl_dens);   /* 136 .. 36 ms apart, on average */
            cl_next = gap / 2 + cl_irand(gap);
            if (cl_dens > 0.0f && !SHED)
                cloud_birth();
        }
        if (cl_live) {
            for (k = 0; k < CL_NG; k++) {
                int32_t a = cl_g[k].age, L = cl_g[k].len;
                int i0;
                float f, x, w;
                if (!L)
                    continue;
                i0 = (int)cl_g[k].pos;
                f = cl_g[k].pos - (float)i0;
                x = (float)cl_buf[i0] + ((float)cl_buf[i0 + 1 < CL_LEN ? i0 + 1 : 0] - (float)cl_buf[i0]) * f;
                w = (float)(a < L - a ? a : L - a) * (2.0f / (float)L);
                x *= w * (1.0f / 12000.0f);
                l += x * cl_g[k].gl;
                r += x * cl_g[k].gr;
                cl_g[k].pos += cl_g[k].inc;
                if (cl_g[k].pos >= (float)CL_LEN)
                    cl_g[k].pos -= (float)CL_LEN;
                if (++cl_g[k].age >= L) {
                    cl_g[k].len = 0;
                    cl_live--;
                }
            }
        }
        l *= 0.55f;
        r *= 0.55f;
        cl_fb = 0.5f * (l + r);
        cl_pl = cl_cl;
        cl_pr = cl_cr;
        cl_cl = l;
        cl_cr = r;
    }
    {   /* between the quarter-rate samples: a line from the last to the next */
        float f = (float)cl_ph * 0.25f;
        *ol = cl_pl + (cl_cl - cl_pl) * f;
        *or_ = cl_pr + (cl_cr - cl_pr) * f;
    }
}

int en_voice_fm(int layer)
{
    if (layer == 0)
        return cv_patch(cvoice) != 0;
    if (layer == 2)
        return BV_PATCH[(unsigned)par[P_BVOICE] < EN_NBV ? par[P_BVOICE] : 0] != 0;
    return layer == 1;
}
int en_fm_alg(int layer)
{
    if (layer == 0)
        return cv_patch(cvoice) ? fm_alg_of(P_CALG, cv_patch(cvoice)) : 0;
    if (layer == 2) {
        const fmpatch_t *p = BV_PATCH[(unsigned)par[P_BVOICE] < EN_NBV ? par[P_BVOICE] : 0];
        return p ? fm_alg_of(P_BALG, p) : 0;
    }
    return par[P_PALG] ? par[P_PALG] - 1 : par[P_PVOICE] == 2 ? 4 : FM_ALG2;
}

/* a layer's envelope, for a sound page (computed from the values, as the render would play them) */
void en_env_shape(int layer, float *lvl, float *aux, int n, float *secs)
{
    int i;
    float T = 1.0f, a = 0.035f, t60 = 0.3f, ft = 0.1f, fe = 0.0f, rel_s = 0.02f, hold = 0.0f;
    switch (layer) {
    case 0: {                                   /* the chord: the strike (or swell), the decay; the filter's envelope */
        float xa = (float)par[P_CATT] * 0.01f;   /* (1.3.0: 0 the voice's strike, 100 a swell) */
        a = fm_clampf(CV[cvoice].att * fm_exp2f(-(xa > 0.0f ? 9.0f : 3.0f) * xa), 0.00004f, 0.3f);
        t60 = st_t60;
        ft = 0.25f * st_t60 + 0.02f;
        fe = fm_clampf(CV[cvoice].fenv * fm_maxf(1.0f + rel(P_CENV), 0.0f) / 3.4f, 0.0f, 1.0f);
        T = fm_clampf(t60 * 1.2f, 0.25f, 4.0f);
        break;
    }
    case 1: {
        int pv = (unsigned)par[P_PVOICE] < EN_NPV ? par[P_PVOICE] : 0;
        t60 = 0.8f * PV[pv].t60 * fm_exp2f(1.5f * rel(P_PLEN));
        a = 0.004f + 0.02f * fm_minf(btone * 2.0f, 1.0f);
        fe = fm_clampf(PV[pv].drop + 0.015f * (float)par[P_PBEND], 0.0f, 3.0f) / 3.0f;   /* the bend */
        ft = 0.035f;
        T = fm_clampf(t60 * 1.2f, 0.25f, 4.0f);
        break;
    }
    case 2: {                                   /* the bass: in, held for its gate, out */
        hold = fm_minf(GROOVE_GATE[gpat()] * fm_exp2f(rel(P_BLEN)), 15.8f) * step_len / EN_SR;
        a = 0.02f;
        T = fm_clampf(hold * 1.5f + 0.1f, 0.25f, 4.0f);
        break;
    }
    default: {                                  /* the kick: its decay; its pitch falling */
        t60 = KIT[kit].t60 * fm_exp2f(1.2f * rel(P_KLEN));
        a = 0.05f;
        ft = 1.0f / (KIT[kit].fall * EN_SR);
        fe = fm_clampf(KIT[kit].start * fm_exp2f(0.8f * rel(P_KPUNCH)) / 10.0f, 0.0f, 1.0f);
        T = fm_clampf(t60 * 1.2f, 0.25f, 2.0f);
        break;
    }
    }
    for (i = 0; i < n; i++) {
        float t = T * (float)i / (float)(n - 1), up = 1.0f - fm_expf(-a * EN_SR * t), v;
        if (layer == 2)
            v = t < hold ? up : up * fm_expf(-(t - hold) / rel_s);
        else
            v = up * fm_expf(-6.9f * t / t60);
        lvl[i] = fm_clampf(v, 0.0f, 1.0f);
        aux[i] = fe > 0.0f ? fe * fm_expf(-t / ft) : 0.0f;
    }
    *secs = T;
}

/* ------------------------------------------------------------ parameters --- */
static int pulse;
/* (1.3.0) an LFO synced to the bar: the chord's filter (1.4.0, the user's: deeper, +-2.5 octaves at 100, on a curve
 * so the pieces' 20-30 sweep as they did); a sine from the grid's place, eased.
 * (1.4.0, REPORT10: the records move by slow drift, rarely by a bar's sine, the bass never) the chord's and the
 * noise's only, each with its Rate: 1 to 32 bars, the sine; or Drift, a slow random wander, as a hand riding the
 * filter: a new place within the depth every 8-32 bars (anywhere in the bar), glided to over most of the wait, eased
 * at both ends; never the same path twice. (The pings' and the bass's gone: their knobs' places kept, unread) */
static float lfo_ch, lfo_n;                      /* the chord's, in octaves on its cutoff (the sine 2.5 d sqrt(d) at
                                                  * most, Drift 0.6 sqrt(d)); the noise's -d..d, a low-pass on the
                                                  * whole noise (1.4.0, the user's: open at the crest, closing up to 3.5
                                                  * octaves at 100 at the trough; it was its rings' tone, hardly heard)
                                                  * (the sines' crests: the noise's on beat 2, the chord's on beat 3).
                                                  * (1.4.0: the chord's was lfo_c, the plate's own name too: one
                                                  * variable, the chord's LFO pulling the plate's wobble about) */
/* (1.4.0, the user's: the LFO's Destination) an octave of the Cutoff's as far along another knob: Reso's and Colour's
 * -1..1 (rel), the Cutoff's 5.5 octaves their whole sweep; and the chord's Colour as it sounds, the LFO's on it or not */
#define LFO_KNOB (2.0f / 5.5f)
static float cc_col(void)
{
    return par[P_CLDEST] == EN_CLD_COLOUR ? fm_clampf(rel(P_CCOL) + LFO_KNOB * lfo_ch, -1.0f, 1.0f) : rel(P_CCOL);
}
static float lfo_cr = 1.0f, lfo_nr = 1.0f;       /* (the reach now, the wave's and the walk's together: the chord's in
                                                  * octaves, the noise's 0..2; the traces' scale) */
static struct {
    float from, to, t, len;                      /* (where it glides from and to, -1..1 of the depth; how far on and
                                                  * how long, in sixteenths) */
    uint32_t left, rng;                          /* (sixteenths till the next place; its own randomness: the music's
                                                  * choices left be) */
} lfo_dr[2];
volatile int8_t en_lfo_tr[2][EN_LFO_N] EN_POOL;   /* (in the pool: read only as far as written) */
volatile uint32_t en_lfo_w, en_lfo_picks[2];
volatile float en_lfo_at[EN_NCLDEST];
volatile float en_lfo_now, en_lfo_reach;
volatile float en_nlfo_at[EN_NNLDEST];           /* (1.4.0, for tests) what the noise's LFO puts on each destination now */
volatile float en_nlfo_now, en_nlfo_reach;       /* (the noise's LFO now and its reach, 0..2: the moving knob) */
#define NL_KNOB 0.25f                            /* (a unit of the noise's reach as far along Tone's or Air's knob: a
                                                  * quarter of its sweep; Tone's -1..1 twice Air's 0..1) */
static float dr_rand(int k)
{
    lfo_dr[k].rng = lfo_dr[k].rng * 1664525u + 1013904223u;
    return (float)(lfo_dr[k].rng >> 8) * (1.0f / 16777216.0f);
}
static float dr_at(int k, float t)               /* the drift's place, -1..1, t sixteenths into its glide */
{
    const float x = fm_clampf(t / lfo_dr[k].len, 0.0f, 1.0f);
    return lfo_dr[k].from + (lfo_dr[k].to - lfo_dr[k].from) * x * x * (3.0f - 2.0f * x);
}
static void lfo_reset(void)
{
    int k;
    for (k = 0; k < 2; k++) {
        lfo_dr[k].from = lfo_dr[k].to = lfo_dr[k].t = 0.0f;
        lfo_dr[k].len = 64.0f;
        lfo_dr[k].left = 1;                      /* (its first place on the first sixteenth) */
        lfo_dr[k].rng = k ? 0x6C8E9CF5u : 0x3C6EF372u;
    }
    lfo_ch = lfo_n = 0.0f;
}
/* a sixteenth on (tick): the drifts a step, a new place when one's due; the trace, as heard */
static void lfo_tick(void)
{
    int k;
    for (k = 0; k < 2; k++) {
        if (lfo_dr[k].left <= 1u) {
            const float f = dr_at(k, lfo_dr[k].t);
            float u = 2.0f * dr_rand(k) - 1.0f;
            if (fm_fabsf(u - f) < 0.3f)          /* (a move worth hearing) */
                u = f > 0.0f ? f - 0.6f : f + 0.6f;
            lfo_dr[k].from = f;
            lfo_dr[k].to = u;
            lfo_dr[k].t = 0.0f;
            lfo_dr[k].left = 128u + (uint32_t)(dr_rand(k) * 385.0f);   /* (8 to 32 bars) */
            lfo_dr[k].len = fm_maxf((float)lfo_dr[k].left * (0.5f + 0.4f * dr_rand(k)), 64.0f);   /* (then still) */
            en_lfo_picks[k]++;
        } else {
            lfo_dr[k].left--;
            lfo_dr[k].t += 1.0f;
        }
    }
    {
        const float c = 127.0f * lfo_ch / lfo_cr, nn = 127.0f * lfo_n / lfo_nr;   /* (the trace: its own reach)
                                                  */
        en_lfo_tr[0][en_lfo_w % EN_LFO_N] = (int8_t)fm_clampf(c + (c < 0.0f ? -0.5f : 0.5f), -127.0f, 127.0f);
        en_lfo_tr[1][en_lfo_w % EN_LFO_N] = (int8_t)fm_clampf(nn + (nn < 0.0f ? -0.5f : 0.5f), -127.0f, 127.0f);
        en_lfo_w++;
    }
}
int en_nlfo18(int old)                           /* (format 19) a Noise LFO before 19 as 19's: (1.4.0, a filter now,
                                                  * the depth as it was) */
{
    return old < 0 ? 0 : old > 100 ? 100 : old;
}
static float lfo_wave(int rate, float fr, int shape)   /* the wave's place now, -1..1, from the grid's (its shape:
                                                  * Sine, Triangle, Ramp up, Ramp down; the sine's and the triangle's
                                                  * crest a quarter in) */
{
    {
        const uint32_t n = 16u << (rate < 0 ? 0 : rate);   /* (the cycle, in sixteenths) */
        const float ph = ((float)((sixteenth - 1u) & (n - 1u)) + fr) / (float)n;   /* (0..1) */
        switch (shape) {
        case 1: return ph < 0.25f ? 4.0f * ph : ph < 0.75f ? 2.0f - 4.0f * ph : 4.0f * ph - 4.0f;
        case 2: return 2.0f * ph - 1.0f;
        case 3: return 1.0f - 2.0f * ph;
        default: return fm_sinf(6.2831853f * ph);
        }
    }
}
static void lfo_step(void)
{
    /* (where the grid is: the step playing and how far into it, smooth across the tick) */
    const float fr = (float)(pulse ? pulse - 1 : 5) * (1.0f / 6.0f);
    const float dc = (float)par[P_CFDEC] * 0.01f, dn = (float)par[P_NLFO] * 0.01f,
                wc = (float)par[P_CLWALK] * 0.01f, wn = (float)par[P_NLWALK] * 0.01f;
    /* (1.4.0, the user's: the drift its own knob, always on, as the records move: the wave, if any, over it) */
    const float sc = 2.5f * dc * fm_sqrtf(dc), rc = 0.6f * fm_sqrtf(wc);   /* (octaves: the wave's, the walk's) */
    lfo_cr = fm_maxf(sc + rc, 1e-6f);
    lfo_nr = fm_maxf(dn + wn, 1e-6f);
    lfo_ch += (sc * lfo_wave(par[P_CLRATE], fr, par[P_CLSHAPE]) + rc * dr_at(0, lfo_dr[0].t + fr) - lfo_ch) * 0.2f;
    en_lfo_now = lfo_ch;
    en_lfo_reach = sc + rc;
    lfo_n += (dn * lfo_wave(par[P_NLRATE], fr, par[P_NLSHAPE]) + wn * dr_at(1, lfo_dr[1].t + fr) - lfo_n) * 0.2f;
    en_nlfo_now = lfo_n;
    en_nlfo_reach = dn + wn;
                                                 /* (the noise's: a filter on the whole noise, render_crackle) */
}
static uint32_t ext_age;

static void tempo_set(void) { step_len = EN_SR * 60.0f / ((float)par[P_TEMPO] * 4.0f); }

static void apply(int p, int v)
{
    const en_param_t *pi = &PARAMS[p];
    if (v < pi->lo)
        v = pi->lo;
    if (v > pi->hi)
        v = pi->hi;
    par[p] = (int16_t)v;
    v = pe(p);                                   /* (as the master actions move it) */
    switch (p) {
    case P_CHORD: chord_vol = (float)(v * v) * 0.0001f; lanes(); break;
    case P_PINGS: bell_vol = (float)(v * v) * 0.0001f; break;
    case P_BASS: bass_vol = (float)(v * v) * 0.0001f; lanes(); break;
    case P_DRUMS: pulse_vol = (float)(v * v) * 0.0001f; lanes(); break;
    case P_HITS: hats_home(); break;
    case P_DMODE: case P_DPAT0: case P_DPAT1: case P_DPAT2: case P_DPAT3: case P_DPAT4: lanes(); break;
    case P_STABS: stabs_home(); break;
    case P_SWING: swing = (float)v * 0.004f; break;      /* the off sixteenths up to 0.4 of a step late */
    case P_HATS: case P_HTONE: {                 /* the piece's colour, moved by the sound page's Hats */
        float h = fm_clampf((float)(pe(P_HATS) + par[P_HTONE]), 0.0f, 100.0f);
        hat_f = 0.25f + 0.0045f * h;                      /* the high-pass: ~1.8 kHz dark .. ~5 kHz bright */
        hat_open = 0.4f - 0.0025f * h;                    /* the open hat: long and soft .. short and crisp */
        break;
    }
    case P_ECHO: echo_send = (float)(v * v) * 0.0001f; break;
    case P_ETIME: echo_time(); break;
    case P_REVERB: rev_send = (float)(v * v) * 0.0001f; break;
    case P_FEEDBACK: echo_t60 = 1.5f * fm_exp2f((float)v * (1.0f / 30.0f)); break;   /* the tail: 1.5 s .. 15 s (heads_step) */
    case P_SIZE: pl_decay_t = 0.5f + 0.0045f * (float)v; break;      /* to 0.95 */
    case P_CUTOFF: case P_CCUT:                  /* 110 Hz .. 5.4 kHz (1.4.0, the user's: the knob around the piece's
                                                  * own, P_CUTOFF the recipe's; the filter's own ends past them) */
        cut_base = 110.0f * fm_exp2f((float)(pe(P_CUTOFF) + pe(P_CCUT)) * 0.055f);
        break;
    case P_DECAY: case P_CVOICE:                 /* 60 ms .. 1.5 s (times the voice's); the filter's envelope, quicker */
        cvoice = (unsigned)par[P_CVOICE] < EN_NCV ? par[P_CVOICE] : 0;
        st_t60 = 0.06f * fm_exp2f((float)pe(P_DECAY) * 0.0464f) * CV[cvoice].dec;
        st_dec = fm_expf(-6.9f / (st_t60 * EN_SR));
        st_fdec = fm_expf(-6.9f * 32.0f / ((0.25f * st_t60 + 0.02f) * EN_SR));   /* (1.3.0: a shorter strike) */
        break;
    case P_PTONE: case P_PTON: btone = fm_clampf((float)(par[P_PTONE] + par[P_PTON]), 0.0f, 100.0f) * 0.01f; break;
    case P_MOTION: motion = (float)v * 0.01f; break;
    case P_EVOLVE: evolve = (float)v * 0.01f; break;
    case P_CVARY:                                /* (1.4.0: 50 as before, exactly: 1) */
        vary_t = vary_of(v);
        vary_g = fm_minf(vary_t, 1.0f);
        break;
    case P_PVARY: case P_BVARY: case P_DVARY: case P_NVARY:   /* (1.4.0: the layers' own, as the chord's) */
        vary_l[p - P_PVARY] = vary_of(v);
        break;
    case P_CECHO: case P_CPLATE: case P_PECHO: case P_PPLATE: case P_BECHO: case P_BPLATE: case P_DECHO: case P_DPLATE:
        sx[(p - P_CECHO) >> 1][(p - P_CECHO) & 1] = (float)v * 0.02f;   /* (1.4.0, the strips: the part's own sends times
                                                  * v / 50, as the noise's Echo and Plate were already: 50 exactly 1) */
        break;
    case P_CLOW: case P_PLOW: case P_BLOW: case P_DLOW: case P_NLOW: case P_KLOW:   /* (1.4.0, the strips) Low cut: a
                                                  * one-pole's coefficient (0: off, the filter not run), ~20 Hz at 1 ..
                                                  * ~600 Hz at 100, an octave each ~20; the noise's rings at their
                                                  * quarter rate; the kick's lcut[5] */
        {
            const float fc = 20.0f * fm_exp2f((float)(v - 1) * (4.907f / 99.0f));
            lcut[p == P_KLOW ? 5 : p - P_CLOW] = v > 0 ? 1.0f - fm_expf(-2.0f * FM_PI * fc * (1.0f / EN_SR)) : 0.0f;
            if (p == P_NLOW)
                lcut4 = v > 0 ? 1.0f - fm_expf(-2.0f * FM_PI * fc * (1.0f / TAPE_SR)) : 0.0f;
        }
        break;
    case P_CRACKLE:                              /* (1.3.0: straight, not squared: a bed at 15 too; 35 as before,
                                                  * 100 -9 dB; the palette widening as it turns up) */
        crackle_vol = (float)v * 0.0035f;
        break;
    case P_DUCK: duck_depth = (float)v * 0.0075f; break;   /* to -12 dB */
    case P_CLOUD: cloud_amt = (float)v * 0.01f; break;
    case P_CR0: case P_CR1: case P_CR2: case P_CR3: case P_CR4: case P_CR5: case P_CR6: case P_CR7:
    case P_CQ0: case P_CQ1: case P_CQ2: case P_CQ3: case P_CQ4: case P_CQ5: case P_CQ6: case P_CQ7:
    case P_CX0: case P_CX1: case P_CX2: case P_CX3: case P_CX4: case P_CX5: case P_CX6: case P_CX7:
        if (nsteps && prog_pos >= 0 && (p - P_CR0) % 8 == prog_pos)
            slot_play(prog_pos);                 /* the slot playing, edited: heard at once */
        prog_publish();
        break;
    case P_PMODE:                                /* (the walk's character, turned by hand: C_REWALK) */
        if (v & EN_PLOOP) {                      /* to Loop: the eight as they are, on from the one playing */
            if (prog_pos < 0 && prog_len())
                prog_pos = 0;
        } else {                                 /* Endless (from Loop, the UI turns them round: C_PROT) */
            walk_fill();
            if (prog_pos < 0)
                prog_pos = 0, slot_play(0);
        }
        en_prog_pos = (int8_t)prog_pos;
        prog_publish();
        break;
    case P_KIT: kit = (unsigned)v < EN_NKIT ? v : 0; break;
    case P_NCOL: nctl.colour = (float)v * 0.02f; break;   /* the noise's controls, -1..+1 (render_crackle) */
    case P_NGRAIN: nz_dens(); nz_now = 1; break;   /* (1.4.0: 0..100, its hits from the next bar) */
    case P_NROOM: nctl.room = 2.3f * (float)v * (float)v * 1e-4f; break;   /* (1.4.0, format 18: Air 0..100, 0 none;
                                                  * the hiss's gain, 2.3 at 100, on a square: as a level is heard) */
    case P_NDRIFT: nctl.drift = (float)v * 0.02f; break;
    case P_NCHAR: nctl.character = (uint8_t)((unsigned)v < EN_NNCHAR ? v : 0); nz_dens(); break;
    case P_CTHROW:                               /* the stabs' throws: x0 .. x1 (50) .. x3 */
        throw_mul = v <= 50 ? (float)v * 0.02f : 1.0f + (float)(v - 50) * 0.04f;
        break;
    case P_PGHOST:                               /* the pings' dry: 0 dB (0), -6 dB (50), -20 dB (100) */
        ping_dry = 0.5f * fm_exp2f((v <= 50 ? 6.0f * (float)(50 - v) : -14.0f * (float)(v - 50)) * 0.02f * 0.16609640f);
        break;
    case P_HATSLVL: hats_lvl = fm_exp2f((float)(v - 50) * 0.04f); break;   /* x1 at 50, -12 .. +12 dB */
    case P_KLVL: kick_lvl = v == 50 ? 1.0f : fm_exp2f((float)(v - 50) * 0.04f); break;   /* (1.4.0: the Kick strip's, as
                                                  * Hats level; 50 exactly x1) */
    case P_KECHO: case P_KPLATE:                 /* (1.4.0: the Kick strip's sends: 0 none, as before; 100 a full send,
                                                  * as the rim's into the echo, the clap's into the plate) */
        kick_snd[p - P_KECHO] = (float)v * (p == P_KECHO ? 0.008f : 0.006f);
        break;
    case P_SMODE:                                /* (1.4.0) song structure: Endless now, the song let go; a song
                                                  * while playing: its main now; from its intro at the next PLAY */
        song_mode = (uint8_t)v;
        if (v && song_ph == EN_SG_OFF && en_playing) {   /* (the user's: turned to a song while playing: its main at
                                                  * once, from this phrase, the outro after Main's bars) */
            song_b0 = bar & ~7u;
            song_bo_on = 1, song_fixed = 0, song_ask = 0;
            song_ph = EN_SG_MAIN;
            song_plan();
            song_publish();
        }
        if (!v && song_ph != EN_SG_OFF && song_ph != EN_SG_END) {
            song_ph = EN_SG_OFF, song_bo_on = 0, song_ask = 0, song_fixed = 0;
            song_back();
            song_publish();
        }
        break;
    case P_SMAIN: song_plan(); song_publish(); break;   /* (the main's length as it goes: the outro moved) */
    case P_SOUTRO:
        if (song_ph == EN_SG_OUTRO || song_fixed) {
            song_be = song_bo + 8u * (uint32_t)v;
            if (song_ph == EN_SG_OUTRO && song_be < bar)
                song_be = bar;
        } else {
            song_plan();
        }
        song_publish();
        break;
    case P_KDENS: kick_now = 1; break;           /* (1.4.0, the user's) the kick's shape turned: from the next bar, not
                                                  * the next phrase (and its hold begun again) */
    case P_RUMBLE:                               /* the kick's rumble: none (0) .. heavy (100), longer as it rises: its
                                                  * tail 2.5 .. 6 s, held across a beat, the sidechain shaping it
                                                  * (1.3.0: off, not yet to standard; 1.4.0, the second try: rumble) */
    {   /* (the user's: 45 at most in a piece, 70-75 the knob's top: its 100 the second try's 72, -7 dB) */
        const float u = 0.72f * (float)v;
        rb_amt = EN_RUMBLE ? u * 0.01f * (0.35f + 0.0065f * u) : 0.0f;   /* (-14 dB at 49, -7 at 100) */
        rb_g = fm_expf(-6.9f / ((2.5f + 3.5f * u * 0.01f) * RB_SR));   /* (the tail's -60 dB, 2.5 .. 5 s) */
    }
        break;
    case P_CGHOST:                               /* the chord's dry: 0 dB (0), -6 dB (50), -20 dB (100); its sends x1 .. x1.7 */
        chord_dry = 0.5f * fm_exp2f((v <= 50 ? 6.0f * (float)(50 - v) : -14.0f * (float)(v - 50)) * 0.02f * 0.16609640f);
        chord_snd = 1.0f + (float)v * 0.007f;
        break;
    case P_EBLUR:                                /* the diffusers' gain: 0.15 (clean copies) .. 0.6 (50) .. 0.75 */
        ec_g = v <= 50 ? 0.15f + 0.009f * (float)v : 0.6f + 0.003f * (float)(v - 50);
        break;
    case P_ETONE:                                /* what enters the tape: a low-pass ~2 kHz at 50, an octave a quarter */
        ec_lpc = 1.0f - fm_expf(-2.0f * FM_PI * fm_clampf(2000.0f * fm_exp2f((float)(v - 50) * 0.04f), 400.0f, 5000.0f) / TAPE_SR);
        break;
    case P_EWIDTH: ec_wide = (float)v * 0.02f; break;   /* the heads' spread: 0 mono, 1 (50), 2 wide */
    case P_POCT:                                 /* let it be heard now: a ping from the first loop */
        if (en_playing && nsteps) {
            int l, k;
            for (l = 0; l < EN_NLOOP; l++)
                for (k = 0; k < EN_LOOP_LEN[l]; k++)
                    if (loop_note[l][k] >= 0) {
                        bell_ring(l, k);
                        return;
                    }
        }
        break;
    case P_TUNE: tunefac = fm_exp2f((float)v * (1.0f / 1200.0f)); break;
    case P_TEMPO:
        tempo_set();
        echo_time();
        break;
    case P_ROOT:
        if (nsteps)
            voice_chord();
        break;
    case P_MIDI:                                 /* off: every channel's notes off (the chord, pings, bass, drums) */
        if (!v) {
            int c;
            for (c = 0; c < 4; c++)
                en_midi_out(0xB0 | (uint32_t)c, 123, 0);
            en_midi_out(0xB9, 123, 0);
        }
        break;
    default: break;
    }
}

static int deg_dim(int d)                       /* the scale's chord on degree d has a diminished fifth */
{
    int t[3], n = stack(pool, 0, d, t, 3);
    return n >= 3 && (t[2] - t[0] + 12) % 12 == 6;
}
static void pool_set(uint32_t p)
{
    int k, prev = 0, moved = 0;
    pool = (p | 1u) & 0xFFFu;
    scale_set();
    /* the walk's slots follow the scale, as degrees; one the new scale has no room for, or that lands on a
     * diminished chord (which the walk never plays), or repeats the slot before, is walked again from that one */
    for (k = 0; k < EN_NPROG && nsteps; k++)
        if (slot_walk(k)) {
            int raw = par[P_CR0 + k] - 1, d = raw % nsteps;
            if (raw >= nsteps || deg_dim(d) || (k > 0 && slot_walk(k - 1) && d == prev)) {
                d = walk_from(prev);
                slot_walk_set(k, d);
                moved = 1;
            }
            prev = walk_last = d;
            if (k == prog_pos)
                deg = d;
        }
    if (moved)
        prog_publish();
    if (deg >= nsteps)
        deg = 0;
    voice_chord();
}

static void regrow(void)                        /* a new piece: fresh loops, home chord, patterns home */
{
    int l, k;
    for (l = 0; l < EN_NLOOP; l++) {
        loop_regrow(l);
        loop_rest[l] = 0;
    }
    chord_slot = -1;
    deg = 0;
    shape = cvoice == CV_KEYS || cvoice == CV_PAD || cvoice == CV_BELL;   /* these like the 9th; the rest the open 7th */
    walk_shape = shape;
    if (!PLAY_LOOP)                              /* Endless: a fresh piece's eight, from the walk */
        walk_fill();
    /* the slots: Endless from slot 1, Loop from the first that isn't a gap (none: home) */
    for (prog_pos = 0; prog_pos < EN_NPROG && slot_gap(prog_pos); prog_pos++)
        ;
    if (prog_pos >= EN_NPROG || !PLAY_LOOP)
        prog_pos = prog_pos >= EN_NPROG ? -1 : 0;
    en_prog_pos = (int8_t)prog_pos;
    for (walk_last = 0, k = 0; k < EN_NPROG; k++)   /* (the walk goes on from its last) */
        if (slot_walk(k))
            walk_last = (par[P_CR0 + k] - 1) % (nsteps ? nsteps : 1);
    if (prog_pos >= 0)
        slot_play(prog_pos);
    if (chord_slot < 0 && (prog_pos < 0 || !slot_walk(prog_pos)))
        voice_chord();
    prog_publish();
    stabs_home();
    hats_home();
    section_set(SEC_FULL);
    sec_next = SEC_FULL;
    chord_bars = 0;
    change_pending = 0;
}

static void notes_off(void)
{
    int i;
    for (i = 0; i < 4; i++)
        en_midi_out(0xB0 | (uint32_t)i, 123, 0);
    en_midi_out(0xB9, 123, 0);
}

static void quiet(void)
{
    int i;
    for (i = 0; i < NBELL; i++)
        bell[i].on = 0;
    for (i = 0; i < EN_NDRUM; i++)
        drum[i].on = 0, drum[i].wait = 0;
    st_env = st_amp = 0.0f;
    bass.env = 0.0f;
    bass.gate = 0;
    notes_off();
}

void en_init(void)
{
    int i;
    for (i = 0; i <= SN; i++)
        sint[i] = fm_sinf((float)i * (FM_TWO_PI / (float)SN));
    plate_init();
    rumble_init();
    for (i = 0; i < TAPE_N; i++)
        tape[i] = 0;
    for (i = 0; i < PLEX_N; i++)
        plex[i] = 0;
    for (i = 0; i < CL_LEN; i++)
        cl_buf[i] = 0;
    for (i = 0; i < CL_NG; i++)
        cl_g[i].len = 0;
    cl_live = 0;
    tape_w = plex_w = tape_ph = 0;
    head2 = head2_t = 0.75f;
    head2_v = 0.0f;
    echo_len = 0.0f;
    q_r = q_w = 0;
    for (i = 0; i < NMOD; i++) {
        mod[i].v = mod[i].tgt = 0.0f;
        mod[i].left = 0;
    }
    for (i = 0; i < EN_NPAD; i++) {
        float p = (STAB_PAN[i] + 1.0f) * (FM_PI / 4.0f);
        stab_gl[i] = fm_cosf(p);
        stab_gr[i] = fm_sinf(p);
        stab[i].ph[0] = 0.13f * (float)i;
        stab[i].ph[1] = 0.37f + 0.21f * (float)i;
    }
    duck_rel = fm_expf(-1.0f / (0.13f * EN_SR));
    duck_relb = fm_expf(-1.0f / (0.25f * EN_SR));
    throw_dec = fm_expf(-1.0f / (0.5f * EN_SR));
    bite_dec = fm_expf(-1.0f / (0.07f * EN_SR));
    gate_dec = fm_expf(-6.9f / (1.5f * EN_SR));
    arcm = arc = 0.0f;
    lfo_reset();
    for (i = 0; i < P_NPARAMS; i++)
        apply(i, PARAMS[i].def);
    pool_set(EN_MODE_POOL[par[P_MODE]]);
    for (i = 0; i < EN_NLOOP; i++)
        loop_pos[i] = 0;
    regrow();
    bass.note = bass.target = (float)bass_note(chord_pcs[0]);
    pl_decay = pl_decay_t;
}

/* ------------------------------------------------------------ transport --- */
static int32_t tick_wait;                       /* Swing: samples until an off sixteenth plays (0: none waiting) */
static void clock_pulse(void)
{
    if (pulse == 0) {                           /* an off sixteenth: everything on it late together (the clock straight) */
        int32_t w = (sixteenth & 1u) ? (int32_t)(swing * step_len) : 0;
        if (w > 0)
            tick_wait = w;
        else
            tick();
    }
    if (!en_ext)
        midi(0xF8, 0, 0);
    if (++pulse >= 6)
        pulse = 0;
}

static void play_start(int from_top)
{
    int i;
    if (from_top) {
        pulse = 0;
        sixteenth = 0;
        en_bars = 0;
        bar = 0;
        song_start();                            /* (1.4.0: Endless as ever, or a song from its intro) */
        for (i = 0; i < EN_NLOOP; i++)
            loop_pos[i] = 0;
        section_set(SEC_FULL);
        sec_next = SEC_FULL;
        kick_hand = kick_now = 0;                /* (a new start: the opening on four again) */
        kick_shape = kick_next = (uint8_t)kick_draw(SEC_FULL);
        hats_publish();
        nz_drawn = 0;
        noise_pattern();
        phrase_plan(1);
        en_change_next = 0;
    }
    pulse_left = 0.0f;
    tick_wait = 0;
    en_playing = 1;
    if (!en_ext)
        midi(from_top ? 0xFA : 0xFB, 0, 0);
}

static void play_stop(void)
{
    int d;
    for (d = 0; d < EN_NDRUM; d++)
        drum[d].wait = 0;                        /* a swung hit still waiting stays unplayed */
    tick_wait = 0;
    en_playing = 0;
    bass.gate = 0;
    if (!en_ext)
        midi(0xFC, 0, 0);
    notes_off();
}

/* a master action turned: the parameters it moves, applied again as heard (Filter: read by render_chunk) */
static void macro(int i, int v)
{
    /* each: the parameters it moves and how far at full turn (+50), in the parameter's own units */
    static const struct { uint8_t p; int8_t at50; } MOVE[] = {
        {P_CGHOST, 50}, {P_PGHOST, 50}, {P_ECHO, 30},  /* (1.3.0) Wet: the whole music dry, or drowned in the echo (the
                                                          * chord's and the pings' Wet, the echo's send; the noise too:
                                                          * render_crackle) */
        {P_DENSITY, 85}, {P_HITS, 85}, {P_STABS, 85},    /* Energy: the notes and hits, */
        {P_PINGS, 40}, {P_HATS, 45}, {P_DECAY, 30}, {P_KDENS, 30},   /* the pings' level, the hats' colour, the stabs' length, the kick
                                                          * (and the hats' level, the phrases: render_drums, bar_start) */
    };
    static const uint8_t OF[] = {EN_MAC_DUB, EN_MAC_DUB, EN_MAC_DUB, EN_MAC_ENERGY, EN_MAC_ENERGY, EN_MAC_ENERGY,
                                 EN_MAC_ENERGY, EN_MAC_ENERGY, EN_MAC_ENERGY, EN_MAC_ENERGY};
    unsigned k;
    mac[i] = (int8_t)v;
    for (k = 0; k < sizeof OF; k++)
        if (OF[k] == i) {
            int o = MOVE[k].at50 * v / 50, before = pe(MOVE[k].p);
            pofs[MOVE[k].p] = (int8_t)o;
            if (pe(MOVE[k].p) != before)
                apply(MOVE[k].p, par[MOVE[k].p]);
        }
}

static void drain(void)
{
    while (q_r != q_w) {
        uint32_t c = q[q_r % QN];
        int a = (int)(c >> 16 & 0xFFu), b = (int)(int16_t)(c & 0xFFFFu);
        BARRIER();
        q_r++;
        switch (c >> 24) {
        case C_SET:
            if (a < P_NPARAMS)
                apply(a, b);
            break;
        case C_POOL: pool_set((uint32_t)b & 0xFFFu); break;
        case C_PLAY:
            if (a && !en_playing) {
                play_start(1);
            } else if (!a && en_playing) {
                play_stop();
                if (song_ph != EN_SG_END)        /* (stopped by hand: no song now; the next PLAY a new one) */
                    song_ph = EN_SG_OFF, song_bo_on = 0, song_ask = 0, song_publish();
            }
            break;
        case C_NEXT:
            if (en_playing)
                change_pending = 1;
            else
                chord_change();
            break;
        case C_FREEZE: en_frozen = (uint8_t)(a != 0); break;
        case C_SEED:
            rng ^= (uint32_t)(b & 0xFFFF) * 2654435761u;
            if (!rng)
                rng = 1u;
            lfo_dr[0].rng ^= (uint32_t)(b & 0xFFFF) * 2246822519u;   /* (the drifts too: each play its own path) */
            lfo_dr[1].rng ^= (uint32_t)(b & 0xFFFF) * 3266489917u;
            break;
        case C_CLOCK:
            en_ext = 1;
            ext_age = 0;
            if (a == EN_CLK_TICK) {
                if (en_playing)
                    clock_pulse();
            } else if (a == EN_CLK_START) {
                play_start(1);
            } else if (a == EN_CLK_CONTINUE) {
                if (!en_playing)
                    play_start(0);
            } else if (en_playing) {
                play_stop();
            }
            break;
        case C_PANIC: quiet(); break;
        case C_REGROW:
            regrow();
            if (pings_have)                      /* (a piece's own loops, kept) */
                pings_take();
            break;
        case C_PINGS:
            pings_have = (uint8_t)a;
            if (a)
                pings_take();
            break;
        case C_MUTE: mutes = (uint32_t)a; lanes(); break;
        case C_PERF: perf = (uint32_t)a; lanes(); break;
        case C_FX: fxm = c & 0xFFFFu; break;
        case C_REWALK: {                         /* the walk's slots walked again, all but the one playing, each from
                                                  * the slot before (yours and the gaps kept) */
            int k, prev = deg;
            for (k = 0; k < EN_NPROG && nsteps; k++) {
                if (k != prog_pos && slot_walk(k)) {
                    slot_walk_set(k, walk_from(prev));
                    prev = par[P_CR0 + k] - 1;
                } else if (slot_walk(k)) {
                    prev = (par[P_CR0 + k] - 1) % nsteps;
                } else if (par[P_CQ0 + k]) {
                    prev = en_deg_of(pool, (par[P_CR0 + k] - par[P_ROOT] + 12) % 12);
                }
            }
            prog_publish();
            break;
        }
        case C_LOCK: lock = (uint8_t)a; break;
        case C_OUTRO: song_outro(); break;
        case C_PNUDGE: {                         /* a nudge on the pings: one note of one loop, another */
            int l = irand(EN_NLOOP), k = loop_pick(l);
            if (k >= 0)
                loop_note[l][k] = (int8_t)rand_offset();
            break;
        }
        case C_PROT:                             /* Loop to Endless: the one playing up to slot 1, all turned round */
            if (prog_pos > 0) {
                int16_t t[3][EN_NPROG];
                int j, k;
                for (k = 0; k < EN_NPROG; k++)
                    for (j = 0; j < 3; j++)
                        t[j][k] = par[P_CR0 + 8 * j + (k + prog_pos) % EN_NPROG];
                for (k = 0; k < EN_NPROG; k++)
                    for (j = 0; j < 3; j++)
                        par[P_CR0 + 8 * j + k] = t[j][k];
                prog_pos = 0;
                en_prog_pos = 0;
                prog_publish();
            }
            break;
        case C_MACRO:
            if (a < EN_NMAC && (unsigned)b <= 100u)
                macro(a, b - 50);
            break;
        }
    }
}

/* --------------------------------------------------------------- render --- */
#define BLK 256
static float mixl[BLK], mixr[BLK], rsend[BLK], esend[BLK], kickb[BLK], bassb[BLK], crl[BLK], crr[BLK];
static float kickr[BLK];                         /* (1.4.0: the kick before its strip's Low cut, for the rumble's ear, while
                                                  * the cut is on: the rumble the Kick page's, never cut) */
static uint8_t kick_cut;                         /* (this block's kick went through a Low cut: kickr filled) */

/* (1.4.0, the user's: the second try) the kick's rumble: dub techno's "rumble kick", the kick into a long dark
 * reverb, its return low-passed and high-passed, ducked by the next kick, so a low bed breathes between the beats
 * (Echospace, Monolake's Cyan and Index, Intrusion). Not the kick itself ringing: 1.3.0's four short lines rang at the
 * kick's pitch (a boom against the bass's notes; its sides apart). Here the tail is the kick's level, decaying as a
 * reverb's (Rumble: 2.5 .. 6 s), carried by a band of noise ~32-80 Hz: unpitched, as the records' are (their low
 * end between the kicks noise-like, mostly 25-60 Hz), so it never fights the bass's notes; mono; not saturated (at
 * its levels a saturation did nothing; driven, it put harmonics into 150-400 Hz). All of it at a sixteenth of the
 * rate, out a line between. */
#ifndef RB_LP
#define RB_LP 80.0f                              /* (the band's top, three poles; its bottom two) */
#endif
#ifndef RB_HP
#define RB_HP 32.0f
#endif
#ifndef RB_SCD
#define RB_SCD 0.9f                              /* (the sidechain: how far down on a kick) */
#endif
#ifndef RB_SCR
#define RB_SCR 0.25f                             /* (its release, s: a swell over the beat, its top before the next) */
#endif
#ifndef RB_K
#define RB_K 50.0f                               /* (the level) */
#endif
static int rb_ph;
static uint32_t rb_rng = 0x2545F491u;
static float rb_acc, rb_e, rb_eb, rb_n1, rb_n2, rb_n3, rb_h, rb_h2, rb_pk, rb_sc, rb_p, rb_c;
static float rb_lpa, rb_hpa, rb_scr, rb_pkr;     /* (the band's, the sidechain's, the kick's peak's: rumble_init) */
static void rumble_init(void)
{
    rb_lpa = 1.0f - fm_expf(-2.0f * FM_PI * RB_LP / RB_SR);
    rb_hpa = 1.0f - fm_expf(-2.0f * FM_PI * RB_HP / RB_SR);
    rb_scr = fm_expf(-1.0f / (RB_SCR * RB_SR));
    rb_pkr = fm_expf(-1.0f / (2.0f * RB_SR));
    rb_acc = rb_e = rb_eb = rb_n1 = rb_n2 = rb_n3 = rb_h = rb_h2 = rb_pk = rb_sc = rb_p = rb_c = 0.0f;   /* (off: from
                                                  * silence when it comes back, not an old piece's tail) */
    rb_ph = 0;
}
/* a block's rumble, into the kick's bus (so it goes where the kick goes, after the music's duck and its mono lows);
 * its sidechain the kick itself, against its own recent peak (the Drums level not changing how deep it ducks) */
static void rumble_block(uint32_t n)
{
    const float g = rb_amt * rb_on;
    float p = rb_p, d = rb_c - rb_p, acc = rb_acc;
    int ph = rb_ph;
    uint32_t j;
    for (j = 0; j < n; j++) {
        const float k = kickb[j];
        acc += fm_fabsf(kick_cut ? kickr[j] : k);   /* (its ear: the kick before its strip's Low cut) */
        kickb[j] = k + (p + d * (float)ph * (1.0f / 16.0f)) * g;   /* (a line between) */
        if (++ph >= 16) {                        /* a sixteenth of the time */
            const float x = acc * (1.0f / 16.0f);
            float w, y;
            ph = 0;
            acc = 0.0f;
            rb_e = rb_e * rb_g + x * (1.0f - rb_g);   /* the tail: the kick's level, as a reverb's */
            rb_eb += (rb_e - rb_eb) * 0.01f;     /* (its build, ~35 ms: a reverb's, not the kick's attack) */
            rb_rng = rb_rng * 1664525u + 1013904223u;
            w = (float)(int32_t)rb_rng * (1.0f / 2147483648.0f);
            rb_n1 += rb_lpa * (w - rb_n1);       /* the band: three one-poles down (no mud over ~150 Hz), two up */
            rb_n2 += rb_lpa * (rb_n1 - rb_n2);
            rb_n3 += rb_lpa * (rb_n2 - rb_n3);
            rb_h += rb_hpa * (rb_n3 - rb_h);
            rb_h2 += rb_hpa * (rb_n3 - rb_h - rb_h2);
            y = (rb_n3 - rb_h - rb_h2) * rb_eb * RB_K;
            rb_pk = fm_maxf(x, rb_pk * rb_pkr);
            rb_sc = fm_maxf(rb_pk > 1e-6f ? x / rb_pk : 0.0f, rb_sc * rb_scr);   /* the sidechain: down on each kick,
                                                  * back up before the next */
            y *= 1.0f - RB_SCD * rb_sc;
            p = rb_c;
            rb_c = y;
            d = rb_c - p;
        }
    }
    rb_p = p, rb_acc = acc, rb_ph = ph;
    rb_e = fm_flush(rb_e), rb_eb = fm_flush(rb_eb), rb_n1 = fm_flush(rb_n1), rb_n2 = fm_flush(rb_n2);
    rb_n3 = fm_flush(rb_n3), rb_h = fm_flush(rb_h), rb_h2 = fm_flush(rb_h2), rb_pk = fm_flush(rb_pk);
    rb_sc = fm_flush(rb_sc), rb_c = fm_flush(rb_c), rb_p = fm_flush(rb_p);
}

/* (the classics pass) the chord's and the pings' dry, wider: each side a little of the other's, 5 and 8 ms later and
 * drifting slowly (a chorus crossed), so the two sides differ without the voices panned further; Width (0 none,
 * 50 0.4, 100 0.8) */
#define WD_N 448                                /* (to 10 ms: the pool's headroom) */
static int16_t wd_buf[2 * WD_N] EN_POOL;
static int wd_w;
static float wd_ph;
static inline float wd_read(const int16_t *b, float d)
{
    float r = (float)wd_w - d, a;
    int i0, i1;
    if (r < 0.0f)
        r += (float)WD_N;
    i0 = (int)r;
    i1 = i0 + 1 < WD_N ? i0 + 1 : 0;
    a = r - (float)i0;
    return ((float)b[i0] + a * (float)(b[i1] - b[i0])) * (1.0f / 12000.0f);
}
static float wd_c;                             /* (its amount, eased: off while shedding) */
static void widen(uint32_t n)
{
    float c, tl, tr, dl, dr;
    uint32_t j;
    wd_c += ((SHED ? 0.0f : 0.4f * fm_minf(ec_wide, 2.0f)) - wd_c) * 0.25f;
    c = wd_c;
    if (c < 0.004f)
        return;
    wd_ph += (float)n * (0.21f / EN_SR);
    if (wd_ph >= 1.0f)
        wd_ph -= 1.0f;
    tl = (0.005f + 0.001f * fm_sinf(6.2831853f * wd_ph)) * EN_SR;
    tr = (0.008f + 0.001f * fm_cosf(6.2831853f * wd_ph + 1.3f)) * EN_SR;
    for (j = 0; j < n; j++) {
        float l = mixl[j], r = mixr[j];
        wd_buf[wd_w] = s16(l), wd_buf[WD_N + wd_w] = s16(r);
        dl = wd_read(wd_buf + WD_N, tl);         /* (the left: the right's, 5 ms on) */
        dr = wd_read(wd_buf, tr);                /* (the right: the left's, 8 ms on) */
        if (++wd_w >= WD_N)
            wd_w = 0;
        mixl[j] = l + c * dl;
        mixr[j] = r + c * dr;
    }
}

static float tapb[BLK];                          /* the layer the screen listens to, alone (en_view_layer) */
/* (1.4.0) the meters: a part's peak, every 4th sample (a buffer of its own; or the mix's difference across its render,
 * against a snapshot of the mix before it) */
static float mt_l[BLK / 4] EN_POOL, mt_r[BLK / 4] EN_POOL;   /* (the pool: the RAM near full) */
static float mt_pk(const float *a, const float *b, uint32_t n)
{
    float p = 0.0f;
    uint32_t j;
    for (j = 0; j < n; j += 4)
        p = fm_maxf(p, fm_maxf(fm_fabsf(a[j]), fm_fabsf(b[j])));
    return p;
}
static void mt_snap(uint32_t n)
{
    uint32_t j;
    for (j = 0; j < n; j += 4)
        mt_l[j >> 2] = mixl[j], mt_r[j >> 2] = mixr[j];
}
static float mt_diff(uint32_t n, const float *c)  /* (c: a mono bus of the part's own, added to both sides; or 0) */
{
    float p = 0.0f;
    uint32_t j;
    for (j = 0; j < n; j += 4) {
        const float k = c ? c[j] : 0.0f;
        p = fm_maxf(p, fm_maxf(fm_fabsf(mixl[j] - mt_l[j >> 2] + k), fm_fabsf(mixr[j] - mt_r[j >> 2] + k)));
    }
    return p;
}
/* (bassb: the bass, mono, on its own so the duck takes it deeper than the rest) */

static void render_stabs(uint32_t n)
{
    uint32_t j;
    int i;
    const int cv = cvoice;
    /* the sound page: Attack (a swell up, a snap down), Colour (as each voice has it: the detune, the FM index, the
     * drawbars, the noise against the saw), Resonance, Env (how far each stab opens the filter) */
    /* (1.4.0, the user's: the LFO and the drift where its Destination is: the Cutoff, in octaves; or Reso or Colour, as
     * far along their knob as it would go along the Cutoff's, a 5.5th of its sweep an octave, within the knob's ends) */
    const int dst = par[P_CLDEST];
    const float xa = (float)par[P_CATT] * 0.01f, xc = cc_col(), up = fm_clampf(1.0f + xc, 0.0f, 2.0f), fx = fm_exp2f(1.5f * xc),
                lco = dst == EN_CLD_CUTOFF ? lfo_ch : 0.0f,
                xr = dst == EN_CLD_RESO ? fm_clampf(rel(P_CRES) + LFO_KNOB * lfo_ch, -1.0f, 1.0f) : rel(P_CRES);
    const float g = chord_vol * 0.5f * CV[cv].gain * fd_g[0],
                k = fm_clampf(1.1f - 0.6f * fm_clampf(0.5f + 0.5f * arcm, 0.0f, 1.0f) - 0.55f * xr -
                                 0.22f * fm_maxf(perf_sw, 0.0f), 0.12f, 2.0f);   /* (Open: it sings) */
    en_lfo_at[EN_CLD_CUTOFF] = lco;              /* (for tests: what the LFO puts on each, now) */
    en_lfo_at[EN_CLD_RESO] = xr - rel(P_CRES);
    en_lfo_at[EN_CLD_COLOUR] = xc - rel(P_CCOL);
    const float att = fm_clampf(CV[cv].att * fm_exp2f(-(xa > 0.0f ? 9.0f : 3.0f) * xa), 0.00004f, 0.3f),
                fe = CV[cv].fenv * fm_maxf(1.0f + rel(P_CENV), 0.0f);
    float pc = 0.0f, ck = click;
    const float dk = st_dec + (gate_dec - st_dec) * fx_gate;   /* (the grid's Gate: the chord rings on, to be chopped) */
    const fmpatch_t *pt = cv_patch(cv);
    const int alg = pt ? fm_alg_of(P_CALG, pt) : 0;
    const float cfb = pt ? fm_fb(P_CFB, pt) : 0.0f;
    float s0 = st_s[0], s1 = st_s[1], s2 = st_s[2], s3 = st_s[3], env = st_env, amp = st_amp, thr = st_throw;
    float hl = st_h[0], hr = st_h[1], ll = st_lc[0], lr = st_lc[1];
    const float lc = lcut[0], ce = 0.8f * sx[0][0], cp = 0.35f * sx[0][1];   /* (the strip: its Low cut, its sends) */
    if (st_midi_left > 0 && (st_midi_left -= (int32_t)n) <= 0)
        for (i = 0; i < EN_NPAD; i++)
            midi(0x80, stab[i].midi_note, 0);
    if ((env < 3e-4f && amp < 3e-4f) || (SHED && env < 6e-3f && amp < 6e-3f)) {   /* (1.3.0: done at -70 dB, not -100:
                                                  * past it, nothing heard; shedding, at -44 dB, as the pings) */
        st_env = st_amp = 0.0f;
        st_throw = thr * fm_expf(-(float)n / (0.5f * EN_SR));
        en_pad_level = 0.0f;
        for (i = 0; i < EN_NPAD; i++)
            en_pad_amp[i] = 0.0f;
        return;
    }
    if (dst == EN_CLD_COLOUR && (cv == CV_SAWS || cv == CV_PAD)) {   /* (1.4.0: the saws' Colour, their detune, with
                                                  * the LFO a block at a time; the others' each sample, through up and fx) */
        const float dt = 0.00202f * fm_exp2f(2.0f * xc);
        for (i = 0; i < EN_NPAD; i++) {
            const float b = 0.5f * (stab[i].inc[0] + stab[i].inc[1]);
            stab[i].inc[0] = b * (1.0f - dt);
            stab[i].inc[1] = b * (1.0f + dt);
        }
    }
    for (j = 0; j < n; j++) {
        float l = 0.0f, r = 0.0f, v1, v2, v3, yl, yr, e;
        if (!(j & 31u)) {                        /* the filter: its base, its envelope, the sweep */
            float fc = cut_base * fm_exp2f(fe * st_fenv + 1.6f * arcm + st_cofs + lco + perf_sw + 0.3f * motion * mod[0].v +
                                           song_co);   /* (the song's intro: darker, opening) */
            svf_coefs(st_c, fm_clampf(fc, 90.0f, 12000.0f), k);
            st_fenv *= st_fdec;
            pc = st_fenv * st_fenv;              /* (Tonewheel: the percussion, quicker still) */
            if (pt)
                for (i = 0; i < EN_NPAD; i++)
                    fm_ctl(&cfm[i], pt, alg, fx);   /* (Colour: the modulators' levels) */
        }
        env *= dk;
        amp += (env - amp) * att;               /* the strike, without a click (or a swell, for the Pad) */
        if (cv == CV_NOISE) {                   /* one noise for all four: the filter gives it its pitch */
            noise = noise * 1664525u + 1013904223u;
            l = r = (float)(int32_t)noise * (1.0f / 2147483648.0f) * 1.6f * (1.0f - 0.5f * xc);
        }
        for (i = 0; i < EN_NPAD; i++) {
            stab_t *v = &stab[i];
            float x;
            switch (cv) {
            case CV_KEYS: case CV_BELL:          /* the FM core */
                x = fm_sample(&cfm[i], alg, cfb, 1.0f) * 0.9f;
                break;
            case CV_ORGAN: {
                float p = v->ph[0];
                x = (sine(p) + up * (0.5f * sine(2.0f * p) + 0.33f * sine(3.0f * p) + 0.2f * sine(4.0f * p))) * 0.8f;
                break;
            }
            case CV_WHEEL: {                     /* 8', 4', 2 2/3' and 2'; the percussion (the third harmonic) dies fast */
                float p = v->ph[0];
                x = (sine(p) + up * (0.7f * sine(2.0f * p) + 0.3f * sine(4.0f * p)) + (0.35f * up + 0.9f * pc) * sine(3.0f * p)) * 0.7f;
                break;
            }
            case CV_NOISE:
                x = 0.35f * up * blep_saw(v->ph[0], v->inc[0]);
                break;
            default:                             /* Saws, Pad */
                x = blep_saw(v->ph[0], v->inc[0]) + blep_saw(v->ph[1], v->inc[1]);
                break;
            }
            v->ph[0] += v->inc[0];
            if (v->ph[0] >= 1.0f)
                v->ph[0] -= 1.0f;
            v->ph[1] += v->inc[1];
            if (v->ph[1] >= 1.0f)
                v->ph[1] -= 1.0f;
            l += x * stab_gl[i];
            r += x * stab_gr[i];
        }
        if (cv == CV_WHEEL) {                   /* the click, and the rotor: left and right in turn */
            float ro;
            noise = noise * 1664525u + 1013904223u;
            ck *= 0.996f;
            l += ck * (float)(int32_t)noise * (1.0f / 2147483648.0f);
            r += ck * (float)(int32_t)noise * (1.0f / 2147483648.0f);
            rotor += 5.6f / EN_SR;              /* the fast speed, ~5.6 turns a second */
            if (rotor >= 1.0f)
                rotor -= 1.0f;
            ro = 0.3f * sine(rotor);
            l *= 1.0f + ro;
            r *= 1.0f - ro;
        }
        v3 = l - s1;                             /* the low-pass, left */
        v1 = st_c[0] * s0 + st_c[1] * v3;
        v2 = s1 + st_c[1] * s0 + st_c[2] * v3;
        s0 = 2.0f * v1 - s0;
        s1 = 2.0f * v2 - s1;
        yl = v2;
        v3 = r - s3;                             /* and right */
        v1 = st_c[0] * s2 + st_c[1] * v3;
        v2 = s3 + st_c[1] * s2 + st_c[2] * v3;
        s2 = 2.0f * v1 - s2;
        s3 = 2.0f * v2 - s3;
        yr = v2;
        e = amp * g;
        yl = sat(yl * e * 3.0f) * 0.4f;
        yr = sat(yr * e * 3.0f) * 0.4f;
        hl += 0.0225f * (yl - hl);               /* a gentle high-pass, ~160 Hz: the stabs out of the low-mids */
        hr += 0.0225f * (yr - hr);
        yl -= hl;
        yr -= hr;
        if (lc > 0.0f) {                         /* (the strip's Low cut, after the stabs' own) */
            ll += lc * (yl - ll);
            lr += lc * (yr - lr);
            yl -= ll;
            yr -= lr;
        }
        if (fx_gate > 0.001f) {                  /* the grid's Gate: the chord in sixteenths */
            float gt = (float)(gate_ctr + j) < 0.5f * step_len ? 1.0f : 0.0f, gg;
            gate_g += (gt - gate_g) * 0.05f;
            gg = 1.0f - fx_gate * (1.0f - gate_g);
            yl *= gg;
            yr *= gg;
        }
        mixl[j] += yl * chord_dry * (1.0f - 0.5f * thr);   /* (Ghost: the dry against the sends; a throw's, -6 dB) */
        mixr[j] += yr * chord_dry * (1.0f - 0.5f * thr);
        esend[j] += (yl + yr) * (ce + 2.5f * thr) * chord_snd;   /* (a throw whatever the strip's send) */
        rsend[j] += (yl + yr) * cp;
        thr *= throw_dec;
    }
    gate_ctr += (int32_t)n;
    st_s[0] = fm_flush(s0);
    st_s[1] = fm_flush(s1);
    st_s[2] = fm_flush(s2);
    st_s[3] = fm_flush(s3);
    st_h[0] = fm_flush(hl);
    st_h[1] = fm_flush(hr);
    st_lc[0] = lc > 0.0f ? fm_flush(ll) : 0.0f;
    st_lc[1] = lc > 0.0f ? fm_flush(lr) : 0.0f;
    st_env = env;
    st_amp = amp;
    st_throw = thr;
    click = fm_flush(ck);
    en_pad_level = fm_minf(amp * 1.2f, 1.0f);
    for (i = 0; i < EN_NPAD; i++) {
        en_pad_note[i] = stab[i].note;
        en_pad_amp[i] = amp;
    }
}

static void render_bass(uint32_t n)
{
    uint32_t j;
    const int bv = (unsigned)par[P_BVOICE] < EN_NBV ? par[P_BVOICE] : 0;
    const float g = bass_vol * 0.5f * BV_GAIN[bv] * GROOVE_LVL[gpat()] * fd_g[2],
                xb = rel(P_BBITE), xs = rel(P_BSHAPE),   /* (1.4.0: no LFO on the bass) */
                drv = 1.2f * fm_exp2f(1.2f * xb), bi = fm_maxf(1.0f + xb, 0.0f);   /* Bite: the drive, the FM bite */
    const int balg = BV_PATCH[bv] ? fm_alg_of(P_BALG, BV_PATCH[bv]) : 0;
    const float bfb = BV_PATCH[bv] ? fm_fb(P_BFB, BV_PATCH[bv]) : 0.0f;
    float ph = bass.ph, env = bass.env, inc, bite = bass_bite, bl = bass_lc;
    int32_t gate = bass.gate;
    const float lc = lcut[2], be = 0.35f * sx[2][0], bp = 0.04f * sx[2][1];   /* (the strip: Low cut, sends; its
                                                  * echo none before: 0) */
    if (env < 1e-5f && gate <= 0) {
        bass.env = 0.0f;
        en_bass_level = 0.0f;
        return;
    }
    bass.note += (bass.target - bass.note) * fm_minf((float)n / (0.012f * fm_exp2f(4.4f * rel(P_BSLIDE)) * EN_SR), 1.0f);
                                                 /* a short slide (Slide: ~1 ms .. ~250 ms) */
    inc = mtof(bass.note) * (1.0f / EN_SR);
    if (BV_PATCH[bv])                            /* the FM voices follow the slide */
        fm_tune(&bfm, BV_PATCH[bv], mtof(bass.note), 1.0f, fm_q(P_BFRAT));
    for (j = 0; j < n; j++) {
        float x, tgt = gate > 0 ? 1.0f : 0.0f;
        if (gate > 0 && --gate == 0)
            midi(0x82, bass.midi_note, 0);
        if (perf & EN_PF_KICKDROP && gate > 0) {   /* PERFORM's Drop: the bass let go at once (~20 ms) */
            gate = 0;
            midi(0x82, bass.midi_note, 0);
            tgt = 0.0f;
        }
        env += (tgt - env) * (tgt > env ? 0.02f : 0.0012f);   /* ~1 ms in, ~20 ms out */
        if (BV_PATCH[bv]) {                      /* (Bite: the modulators' levels) */
            if (!(j & 31u))
                fm_ctl(&bfm, BV_PATCH[bv], balg, bi);
            x = fm_sample(&bfm, balg, bfb, 1.0f);
        } else {
            x = sine(ph) + 0.2f * sine(2.0f * ph);
        }
        if (xs > 0.0f)                           /* Shape: towards a square, or back to a sine */
            x += xs * (sat(x * 4.0f) - x);
        else if (xs < 0.0f)
            x -= xs * (sine(ph) - x);
        bite *= bite_dec;
        ph += inc;
        if (ph >= 1.0f)
            ph -= 1.0f;
        x = sat(x * drv) * env * g * (1.2f / drv + 0.0f) * (0.6f + 0.4f * fm_minf(drv / 1.2f, 1.6f));
        bassb[j] += x;
    }
    if (lc > 0.0f) {                             /* (the strip's Low cut, before its sends: bassb is the bass's alone) */
        for (j = 0; j < n; j++) {
            bl += lc * (bassb[j] - bl);
            bassb[j] -= bl;
        }
        bass_lc = fm_flush(bl);
    } else {
        bass_lc = 0.0f;
    }
    for (j = 0; j < n; j++)
        rsend[j] += bassb[j] * bp;
    if (be > 0.0f)                               /* (its echo: none in every piece, unless turned up) */
        for (j = 0; j < n; j++)
            esend[j] += bassb[j] * be;
    bass.ph = ph;
    bass_bite = fm_flush(bite);
    bass.env = fm_flush(env);
    bass.gate = gate;
    en_bass_level = env;
}

static void render_bells(uint32_t n)
{
    int i;
    const float g = bell_vol * 1.4f * fd_g[1], arate = 0.004f + 0.02f * fm_minf(btone * 2.0f, 1.0f),
                lc = lcut[1], pp = 0.4f * sx[1][1], pe1 = 1.1f * sx[1][0];   /* (the strip: Low cut, sends) */
    for (i = 0; i < NBELL; i++) {
        bell_t *v = &bell[i];
        float amp = v->amp, att = v->att, l1 = v->lc;
        const int alg = fm_alg_of(P_PALG, &v->pt);
        const float fb = fm_fb(P_PFB, &v->pt);
        uint32_t j = 0;
        if (!v->on)
            continue;
        if (v->wait > 0) {
            if ((uint32_t)v->wait >= n) {
                v->wait -= (int32_t)n;
                continue;
            }
            j = (uint32_t)v->wait;
            v->wait = 0;
        }
        for (; j < n; j++) {
            float x;
            if (!(j & 31u))
                fm_ctl(&v->fm, &v->pt, alg, 1.0f);   /* (1.4.0: no LFO on the pings) */
            x = fm_sample(&v->fm, alg, fb, 1.0f + v->drop) * amp * att * g;
            if (lc > 0.0f) {
                l1 += lc * (x - l1);
                x -= l1;
            }
            mixl[j] += x * v->gl * ping_dry;   /* (Ghost: the dry against the sends, which stay; 50: -6 dB) */
            mixr[j] += x * v->gr * ping_dry;
            rsend[j] += x * pp;
            esend[j] += x * pe1;
            v->drop *= 0.9993f;                  /* (Drop: ~35 ms to fall) */
            amp *= v->adec;
            att += (1.0f - att) * arate;
        }
        v->amp = amp;
        v->att = att;
        v->lc = lc > 0.0f ? fm_flush(l1) : 0.0f;
        if (amp < (SHED ? 0.006f : 0.0008f)) {   /* (shedding: a tail ends at -44 dB, not -62) */
            v->on = 0;
            midi(0x81, v->note, 0);
        }
    }
}

/* a hat: the metal and noise, high-passed (Chamberlin SVF, ~1.8 to 5 kHz by Hats) and low-passed (~8 kHz):
 * in the references' 2-8 kHz, not the air above (1.3.0); no pitch; into the echo (metal: the square's share,
 * at: the attack's smoothing) */
static void render_hat(drum_t *v, uint32_t n, float g, float pl, float pr, float es, float metal, float at)
{
    uint32_t j;
    int k;
    float x1, x2, amp, att, lp, lp2, l1, hf = fm_minf(hat_f * KIT[kit].hat_f, 1.2f);
    const float lc = lcut[3], rs = 0.15f * sx[3][1];   /* (the drums' strip: Low cut, the plate's send; es the echo's) */
    if (!v->on)
        return;
    x1 = v->x1, x2 = v->x2, amp = v->amp, att = v->att, lp = v->lp[0], lp2 = v->lp[1], l1 = v->lc;
    for (j = 0; j < n; j++) {
        float in = 0.0f, hp, x;
        for (k = 0; k < 4; k++) {
            v->mph[k] += HAT_INC[k];
            if (v->mph[k] >= 1.0f)
                v->mph[k] -= 1.0f;
            in += v->mph[k] < 0.5f ? 0.25f : -0.25f;
        }
        noise = noise * 1664525u + 1013904223u;
        in = metal * (1.1f - KIT[kit].noise) * in + KIT[kit].noise * (float)(int32_t)noise * (1.0f / 2147483648.0f);
        hp = in - x2 - 0.8f * x1;
        x1 += hf * hp;
        x2 += hf * x1;
        lp += 0.65f * (hp - lp);
        lp2 += 0.65f * (lp - lp2);
        x = lp2 * amp * att * g;
        if (lc > 0.0f) {
            l1 += lc * (x - l1);
            x -= l1;
        }
        mixl[j] += x * pl;
        mixr[j] += x * pr;
        rsend[j] += x * rs;
        esend[j] += x * es;
        amp *= v->adec;
        att += (1.0f - att) * at;
    }
    v->x1 = fm_flush(x1);
    v->x2 = fm_flush(x2);
    v->lp[0] = fm_flush(lp);
    v->lp[1] = fm_flush(lp2);
    v->lc = lc > 0.0f ? fm_flush(l1) : 0.0f;
    v->amp = amp;
    v->att = att;
    v->on = amp > 0.0005f;
}

static void render_drums(uint32_t n)
{
    uint32_t j;
    /* (1.4.0, the user's: the kick and the hats apart) the Hats strip's: its Low cut, its sends' shares, the throw
     * (the hats, the rim and the clap into the echo while held, whatever their send: ht, added to each one's) */
    const float g = pulse_vol * KIT[kit].gain, fall = KIT[kit].fall, drive = KIT[kit].drive,
                lc = lcut[3], de = sx[3][0], dp = sx[3][1], ht = hat_thr, gh = g * fd_g[4];   /* (gh: the hats' part,
                                                  * its fader) */
    drum_t *v = &drum[0];
    kick_cut = 0;
    if (v->on) {                                 /* the kick: its own bus, ducked by nothing; the Kick strip's level,
                                                  * Low cut and sends (none unless turned up) */
        float ph = v->ph, inc = v->inc, amp = v->amp, att = v->att, clk = v->clk, cl = v->x1, l1 = v->lc;
        const float gk = g * kick_lvl * fd_g[3], kc = lcut[5], ke = kick_snd[0], kp = kick_snd[1];
        kick_cut = kc > 0.0f;
        for (j = 0; j < n; j++) {
            float x;
            noise = noise * 1664525u + 1013904223u;
            cl += 0.5f * ((float)(int32_t)noise * (1.0f / 2147483648.0f) - cl);   /* the click: noise, softened */
            x = (sine(ph) * att + 0.5f * cl * clk) * amp;
            x = sat(x * drive) * gk * (0.89f / drive);   /* (-2 dB from 1.2.0: -5 in 1.3.0, the drums as spice; +3 back, the user: hear the drums) */
            if (kc > 0.0f) {                     /* (the strip's Low cut: the kick thinned; its rumble its own) */
                kickr[j] = x;
                l1 += kc * (x - l1);
                x -= l1;
            }
            kickb[j] += x;
            if (ke > 0.0f || kp > 0.0f) {        /* (its sends: none before 1.4.0's Kick strip) */
                esend[j] += x * ke;
                rsend[j] += x * kp;
            }
            ph += inc;
            if (ph >= 1.0f)
                ph -= 1.0f;
            inc += (v->inc_end - inc) * fall;
            amp *= v->adec;
            att += (1.0f - att) * 0.05f;          /* (in over ~0.5 ms: softer) */
            clk *= 0.985f;
        }
        v->ph = ph;
        v->inc = inc;
        v->amp = amp;
        v->att = att;
        v->clk = clk;
        v->x1 = fm_flush(cl);
        v->lc = kc > 0.0f ? fm_flush(l1) : 0.0f;
        v->on = amp > 0.0005f;
    }
    {   /* (Energy: the hats -6 dB down to +6 dB up) */
        const float hl = fm_exp2f((float)mac[EN_MAC_ENERGY] * (1.0f / 50.0f)) * hats_lvl;   /* (and Hats: +-12 dB) */
        render_hat(&drum[1], n, gh * 0.16f * hl, 0.6f, 1.0f, 0.2f * de + 0.9f * ht, 0.6f, 0.02f);   /* shaker: a little right, soft, into the echo */
        render_hat(&drum[2], n, gh * 0.17f * hl, 1.0f, 0.7f, 0.2f * de + 0.9f * ht, 1.0f, 0.3f);   /* open: a little left, some echo */
    }
    v = &drum[3];                                /* the rim: two tones (~520 Hz, ~1.7 kHz) and a click of noise */
    if (v->on) {
        float amp = v->amp, att = v->att, clk = v->clk, l1 = v->lc;
        const float re = 1.2f * de + 1.8f * ht, rp = 0.5f * dp;
        for (j = 0; j < n; j++) {
            float x;
            v->ph += 520.0f / EN_SR;
            if (v->ph >= 1.0f)
                v->ph -= 1.0f;
            v->mph[0] += 1710.0f / EN_SR;
            if (v->mph[0] >= 1.0f)
                v->mph[0] -= 1.0f;
            noise = noise * 1664525u + 1013904223u;
            x = (sine(v->ph) * 0.55f + sine(v->mph[0]) * 0.45f + (float)(int32_t)noise * (1.0f / 2147483648.0f) * clk) *
                amp * att * gh * 0.3f;
            if (lc > 0.0f) {
                l1 += lc * (x - l1);
                x -= l1;
            }
            mixl[j] += x * 0.85f;
            mixr[j] += x;
            esend[j] += x * re;                  /* (mostly into the echo: a dub rim) */
            rsend[j] += x * rp;
            amp *= v->adec;
            att += (1.0f - att) * 0.4f;
            clk *= 0.9f;
        }
        v->amp = amp;
        v->att = att;
        v->clk = clk;
        v->lc = lc > 0.0f ? fm_flush(l1) : 0.0f;
        v->on = amp > 0.0005f;
    }
    v = &drum[4];                                /* the clap: noise through a band-pass (~1.1 kHz), its envelope */
    if (v->on) {                                 /* three bursts 9 ms apart, then the tail */
        float amp = v->amp, t = v->ph, x1 = v->x1, x2 = v->x2, l1 = v->lc;
        const float ke = 0.5f * de + 1.2f * ht, kp = 1.4f * dp;
        for (j = 0; j < n; j++) {
            float w, hp, bp, e, x, tb = t < 1200.0f ? (t - 400.0f * (float)(int)(t * (1.0f / 400.0f))) : 1e9f;
            noise = noise * 1664525u + 1013904223u;
            w = (float)(int32_t)noise * (1.0f / 2147483648.0f);
            hp = w - x2 - 0.7f * x1;            /* (a Chamberlin SVF: the band) */
            x1 += 0.16f * hp;
            x2 += 0.16f * x1;
            bp = x1;
            /* a burst: ~3 ms, in over ~0.3 ms (from nothing at once, the ringing band stepped: a click each burst,
             * which the plate and the echo repeated) */
            e = tb < 1e8f ? (1.0f - fm_expf(-tb * (1.0f / 6.0f))) * fm_expf(-tb * (1.0f / 130.0f)) : 0.0f;
            if (t >= 1200.0f)
                e = amp * 0.9f;
            x = bp * e * gh * 1.1f;
            if (lc > 0.0f) {
                l1 += lc * (x - l1);
                x -= l1;
            }
            mixl[j] += x;
            mixr[j] += x * 0.8f;
            rsend[j] += x * kp;                  /* (into the plate, most of it) */
            esend[j] += x * ke;
            if (t >= 1200.0f)
                amp *= v->adec;
            t += 1.0f;
        }
        v->amp = amp;
        v->ph = t;
        v->x1 = fm_flush(x1);
        v->x2 = fm_flush(x2);
        v->lc = lc > 0.0f ? fm_flush(l1) : 0.0f;
        v->on = amp > 0.0005f;
    }
}

/* Crackle: a small palette of noises, each mono, each drifting on its own over minutes (its level, and the
 * hiss its colour), so the bed never settles into one static noise (1.3.0; it was a garnish of hiss and clicks):
 *   the hiss: the tape's floor, flat over 2-8 kHz and dark above, its band moving slowly (an auto-filter on a
 *     random LFO);
 *   the vinyl: clicks at random, mostly small, now and then a pop, each its own colour (dull to sharp) and its
 *     own side; how many drifts, so the transients slow down and space out now and then;
 *   the dust: soft grains of band-passed noise (1.5-6 kHz, 10-50 ms), a few a beat at random, partly into the
 *     echo: a texture between the hits;
 *   the rumble: a room's, a deck's, under 60 Hz and mono, very quiet;
 *   the wind: a breath of noise through a narrow band (~250 Hz-2 kHz), swept slowly by the drift and Motion's arc.
 * The Crackle knob is the level of all of it; turned up, more of the palette comes in (the hiss and the vinyl
 * from the start, the dust from ~20, the rumble from ~25, the wind from ~35). In a Break or Thin, the hiss and
 * the dust come forward (+2.5 dB), to carry the groove. It plays while the music plays, fading in and out over
 * a second; a little of it into the plate.
 * Four controls shape it, each -1..+1 around the piece's own (0), and a character (a preset of the mix): ready
 * on ARP's Noise page (P_NCOL .. P_NCHAR, into nctl) */
const char *const EN_NCHAR_NAME[EN_NNCHAR] = {"Vinyl", "Tape", "Dust", "Room", "Machine"};
/* 1.3.0's noise machine (build/research/dubtechno/REPORT4-noise.md, after Bahadirhan Kocer's "Using Resonators in
 * Live"): not a broadband bed but a voice. A crackle groove, hits on a 2-bar sixteenth pattern (Density; redrawn by
 * phrase, now and then kept: a motif that comes back), each a click and a short noise burst, rings three combs tuned
 * to the chord's root, third and fifth in octave 1 (Decay: how long), through a band-pass (Tone, ~250 Hz-2 kHz:
 * the harmonics as a chord, nothing up top); into the plate, a little into the echo (more on a throw). Under it the
 * Air, a quiet hiss that breathes with the duck, and now and then a dry vinyl click. The characters (ALGORITHM) are
 * the same engine set otherwise. The combs and the band-pass at a quarter rate (all of it under 2.5 kHz) */
static const struct {
    float res, cont, clicks, dens, dec, tone, air;  /* resonance (0 dry), the hiss into the combs (a pad), the dry
                                                     * clicks, and offsets: hits, Decay, Tone, Air */
    float loose;                                    /* the dry clicks off the grid (the rest on its sixteenths) */
    uint8_t key, inh;                               /* on the key's triad (not the chord's); inharmonic ratios */
} NCHAR[EN_NNCHAR] = {
    {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.6f, 0, 0},     /* Vinyl: the resonated crackle groove, on the chord */
    {1.0f, 1.0f, 0.4f, -0.6f, 0.6f, -0.2f, -0.3f, 0.3f, 0, 0},  /* Tape: the hiss through the combs, a pad that swells between
                                                           * the beats (the duck), few hits */
    {0.0f, 0.0f, 2.0f, 0.3f, 0.0f, 0.3f, 0.0f, 0.5f, 0, 0},     /* Dust: dry clicks and bursts, no resonance */
    {1.0f, 0.3f, 0.7f, -0.3f, 0.5f, -0.3f, 0.2f, 0.25f, 1, 0},   /* Room: on the key's tonic triad, long, more of the plate */
    {1.0f, 0.0f, 1.2f, 0.2f, 0.2f, 0.15f, -0.2f, 0.0f, 0, 1},   /* Machine: inharmonic (1 : 1.41 : 2.76), metallic */
};
#define CR_ND 4                                 /* the drifts: the air's level, the tone, the clicks' density, the decay */
static float cr_lp, cr_lp2, cr_hp, cr_cl, cr_cr, cr_fl, cr_fr, cr_cc = 0.3f, cr_gate, cr_sg = 1.0f;
static float cr_dv[CR_ND] = {1.0f, 1.0f, 1.0f, 1.0f}, cr_dt[CR_ND] = {1.0f, 1.0f, 1.0f, 1.0f};
static const float CR_DRANGE[CR_ND] = {0.66f, 0.7f, 2.0f, 0.5f};   /* (octaves of each drift's span) */
#define NZ_L 280                                /* a comb's line, at the quarter rate (E1: 269 samples) */
static int16_t nz_buf[3 * NZ_L];
static int nz_w, nz_ph;
static float nz_D[3] = {200.0f, 160.0f, 133.0f}, nz_lp[3], nz_acc, nz_b[4], nz_p[2], nz_c[2];
static uint32_t nz_pat;                         /* this phrase's hits, 32 sixteenths (two bars) */
static uint8_t nz_vel[32];
static float nz_ex, nz_exd = 0.99f, nz_click;   /* a hit: its burst's level and fall, its click */
/* (a hit struck early: the combs speak a period after the burst and swell after, so the next step's hit is struck
 * that much before its tick, its ring heard on it; REPORT6 9.1) */
static int32_t nz_wait = -1;                    /* samples till the next hit strikes (-1: none) */
static float nz_wv, nz_wd, nz_wD[3];            /* its burst's level, fall, the combs' tuning */
static uint8_t nz_due;                          /* (the hit struck for this tick: its screen event on it) */
static float nz_gck;                            /* a dry click on this tick (its level; 0: none) */
static float cr_u(void)                          /* the noises' own randomness: the music's choices left be */
{
    cnoise = cnoise * 1664525u + 1013904223u;
    return (float)(cnoise >> 8) * (1.0f / 16777216.0f);
}
static unsigned nz_char(void) { return nctl.character < EN_NNCHAR ? nctl.character : 0u; }
/* (1.4.0, format 18) Density 0..100: the hits in two bars, 16 at 100, none at 0; the clicks' density as before 18
 * (where -50..50 and the character's offset made 2..16 hits: en_nz_dens18 the way across) */
static void nz_dens(void)
{
    const int v = par[P_NGRAIN] < 0 ? 0 : par[P_NGRAIN] > 100 ? 100 : par[P_NGRAIN];
    nctl.hits = (uint8_t)((v * 16 + 50) / 100);
    nctl.grain = fm_clampf(((float)v * 0.16f - 9.0f) * (1.0f / 7.0f) - 0.5f * NCHAR[nz_char()].dens, -1.0f, 1.0f);
}
int en_nz_air18(int old, int character)          /* an Air before 18 (-50..50, the character's lean on it) as 0..100 */
{
    const float nr = fm_clampf((float)old * 0.02f + NCHAR[(unsigned)character < EN_NNCHAR ? character : 0].air, -1.0f, 1.0f);
    return (int)(100.0f * fm_sqrtf(fm_exp2f(1.2f * nr) * (1.0f / 2.3f)) + 0.5f);
}
int en_nz_dens18(int old, int character)         /* a value before 18 (-50..50) as 18's (0..100) */
{
    const float d = fm_clampf((float)old * 0.02f + 0.5f * NCHAR[(unsigned)character < EN_NNCHAR ? character : 0].dens,
                              -1.0f, 1.0f);
    const int k = (int)(9.0f + 7.0f * d + 0.5f);
    return (k * 100 + 8) / 16;
}
/* the phrase's groove: Density's hits (2..16 in two bars), Euclidean, turned, a couple nudged a step off; now and then
 * the last one kept (more often as Evolve falls): a motif that comes back */
static void noise_pattern(void)
{
    int k = nctl.hits, i, m;
    uint32_t p;
    if (nz_drawn && !nz_now && cr_u() > (0.35f + 0.5f * evolve + 0.3f * nctl.drift) * vary_l[VY_NOISE])   /* (Change:
                                                  * more often new; 1.4.0: Vary over it, the recipe's Change and Evolve
                                                  * its 50) */
        return;
    en_vary_turns[VY_NOISE] += nz_drawn && !nz_now;   /* (a new groove: Vary's, not a knob's) */
    nz_now = 0;
    nz_drawn = 1;
    if (sec == SEC_BREAK || sec == SEC_THIN)
        k = k * 7 / 10;
    k = k < 0 ? 0 : k > 16 ? 16 : k;
    p = en_euclid(k, 32, (int)(cr_u() * 32.0f));
    for (m = 0; m < 2; m++) {                   /* (a hit a step later or earlier, where it's free) */
        int s = (int)(cr_u() * 32.0f) & 31, t = (s + (cr_u() < 0.5f ? 1 : 31)) & 31;
        if (p >> s & 1u && !(p >> t & 1u))
            p = (p & ~(1u << s)) | 1u << t;
    }
    nz_pat = p;
    for (i = 0; i < 32; i++)
        nz_vel[i] = (uint8_t)(cr_u() < 0.15f ? 64 : 180 + (int)(cr_u() * 75.0f));   /* (a ghost now and then) */
    en_nz_pat = p;                               /* (the Noise page's lane) */
    for (i = 0; i < 32; i++)
        en_nz_vel[i] = nz_vel[i];
}
static float nz_note(int pc) { return mtof((float)(28 + (pc + 8) % 12)); }   /* octave 1: E1 .. Eb2 */
/* a sixteenth (tick): the hit struck for it told; the next step's, if the groove has one, struck ahead of its tick by
 * the combs' latency (the shortest one's period, and a little of the swell), the combs retuned as it strikes (never
 * mid-ring); a dry click on the grid now and then (the character's loose ones at random: render_crackle) */
static void noise_step(int s)
{
    en_nz_step = (uint8_t)(s & 31);              /* (the Noise page: the step playing) */
    const unsigned c = nz_char();
    const int nx = (s + 1) & 31;
    float f[3], v, gap, lead = 0.0f;
    int k;
    if (nz_due) {
        nz_due = 0;
        v = (float)nz_vel[s & 31] * (1.0f / 255.0f);
        nev(v > 0.5f ? 1 : 0, v * crackle_vol * 9.0f, 0.5f);
    }
    if ((mutes & EN_M_NOISE) || crackle_vol <= 0.0f || !nctl.hits)   /* (Density 0: no hits, no clicks) */
        return;
    {   /* the grid's dry clicks: of the clicks a second, those not loose, a sixteenth at a time */
        const float rate = (3.0f + 12.0f * crackle_vol) * cr_dv[2] * fm_exp2f(fm_clampf(nctl.grain, -1.0f, 1.0f)) *
                           (1.0f - NCHAR[c].loose);
        if (cr_u() < fm_minf(rate * step_len * (1.0f / EN_SR), 0.9f)) {
            float a = cr_u();
            nz_gck = (0.03f + 0.4f * a * a * a);
        }
    }
    if (!(nz_pat >> nx & 1u))
        return;
    if (NCHAR[c].key) {                          /* (Room: the key's tonic triad) */
        int r = par[P_ROOT];
        f[0] = nz_note(r), f[1] = nz_note(r + (pool >> 3 & 1u ? 3 : 4)), f[2] = nz_note(r + 7);
    } else {
        f[0] = nz_note(chord_pcs[0]), f[1] = nz_note(chord_pcs[1]), f[2] = nz_note(chord_pcs[chord_nt >= 3 ? 2 : 0]);
    }
    if (NCHAR[c].inh)                            /* (Machine: 1 : 1.41 : 2.76) */
        f[1] = f[0] * 1.41f, f[2] = f[0] * 2.76f;
    for (k = 0; k < 3; k++) {
        float d = TAPE_SR / f[k];
        while (d > (float)(NZ_L - 2))
            d *= 0.5f;
        nz_wD[k] = d;
    }
    if (NCHAR[c].res > 0.0f)                     /* (the shortest comb's period, at the full rate, and 6 ms) */
        lead = 4.0f * fm_minf(fm_minf(nz_wD[0], nz_wD[1]), nz_wD[2]) + 0.006f * EN_SR;
    gap = step_len * (1.0f + (s & 1 ? -swing : swing));   /* (to the next tick: the odd ones late by Swing) */
    v = (float)nz_vel[nx] * (1.0f / 255.0f);
    nz_wv = v;
    nz_wd = fm_expf(-6.9f / ((0.005f + 0.008f * cr_u()) * EN_SR));   /* (a 5-13 ms burst: struck, staccato) */
    if (SHED)                                    /* (shedding: no new hits; the rings die, and the combs rest) */
        return;
    nz_wait = (int32_t)fm_maxf(gap - lead, 0.0f);
    nz_due = 1;
}
static float nzl_s[4];                           /* (the strip's Low cut's state: the rings', the clicks') */
static float nzf_s[2];                           /* (the noise's LFO filter's state: the rings', l and r) */
static inline float svf_lp(const float *c, float *s, float x, float *bp);
static void render_crackle(uint32_t n)
{
    uint32_t j, thr;
    int i;
    float g, k, sg, nc, ng, nd, gh, lpc, vg, gres, cg, lpk, bf, cont, dry, nzla = 1.0f, nzl4 = 1.0f, room;
    const unsigned c = nz_char();
    cr_gate += ((en_playing ? 1.0f : 0.0f) - cr_gate) * fm_minf((float)n / (0.5f * EN_SR), 1.0f);
    if (cr_gate < 1e-4f)
        cr_gate = 0.0f;
    g = mutes & EN_M_NOISE ? 0.0f : crackle_vol * cr_gate * fd_g[5];   /* (muted: its level kept; the song's fader) */
    if (g <= 0.0f) {
        for (j = 0; j < n; j++)
            crl[j] = crr[j] = 0.0f;
        return;
    }
    {   /* (1.4.0, the user's: the noise LFO's Destination) its Tone or its Air moved, within their knobs' ranges, a
         * quarter of the knob's sweep a unit of its reach (Depth and Drift together, 0..2); the Filter below */
        const int nld = par[P_NLDEST];
        const float ta = nld == EN_NLD_TONE ? fm_clampf(nctl.colour + NL_KNOB * 2.0f * lfo_n, -1.0f, 1.0f) : nctl.colour,
                    aa = nld == EN_NLD_AIR ? fm_clampf((float)par[P_NROOM] * 0.01f + NL_KNOB * lfo_n, 0.0f, 1.0f) : 0.0f;
        en_nlfo_at[EN_NLD_FILTER] = nld == EN_NLD_FILTER ? lfo_n : 0.0f;   /* (for tests: what it puts on each, now) */
        en_nlfo_at[EN_NLD_TONE] = ta - nctl.colour;
        en_nlfo_at[EN_NLD_AIR] = nld == EN_NLD_AIR ? aa - (float)par[P_NROOM] * 0.01f : 0.0f;
        nc = fm_clampf(ta + NCHAR[c].tone, -1.0f, 1.0f);   /* Tone */
        room = nld == EN_NLD_AIR ? 2.3f * aa * aa : nctl.room;   /* (as P_NROOM's curve) */
    }
    ng = fm_clampf(nctl.grain, -1.0f, 1.0f);                     /* Density (the clicks with it) */
    nd = fm_clampf(nctl.drift, -1.0f, 1.0f);                     /* the drifts' speed (the recipes') */
    if (cr_dbars <= 0) {                         /* the drifts: new targets on a barline, every 1-3 bars, eased over
                                                  * a bar (REPORT6 9.3: they crossed the barlines at random) */
        float sp = fm_exp2f(nd);
        for (i = 0; i < CR_ND; i++)
            cr_dt[i] = fm_exp2f((cr_u() - 0.5f) * CR_DRANGE[i] * sp);
        cr_dbars = (int)fm_maxf((1.0f + 3.0f * cr_u()) / sp, 1.0f);
    }
    k = fm_minf((float)n * fm_exp2f(nd) / (16.0f * step_len), 1.0f);
    for (i = 0; i < CR_ND; i++)
        cr_dv[i] += (cr_dt[i] - cr_dv[i]) * k;
    cr_sg += ((sec == SEC_BREAK || sec == SEC_THIN ? 1.0f : 0.0f) - cr_sg) * fm_minf((float)n / (2.0f * EN_SR), 1.0f);
    sg = cr_sg;                                  /* (Break and Thin: the combs ring longer, the noise carries it) */
    {   /* Decay: the combs' gain, 0.75 .. 0.97 (-60 dB in ~70 ms .. ~1.2 s); Tone: the band-pass ~250 Hz-2 kHz, the
         * damping with it; both moved by the drifts and Motion's arc */
        float dec = fm_clampf((float)par[P_NTUNE] * 0.01f + 0.3f * NCHAR[c].dec + 0.15f * sg +
                              0.3f * (cr_dv[3] - 1.0f), 0.0f, 1.0f), t;
        cg = 0.8f + 0.18f * dec;
        en_nz_t60 = NCHAR[c].res > 0.0f ? 0.07f * fm_exp2f(4.1f * dec) : 0.02f;   /* (the Noise page: ~70 ms .. ~1.2 s;
                                                  * Dust's dry bursts short) */
        t = fm_clampf(nc + 0.7f * (cr_dv[1] - 1.0f) + 0.4f * arcm, -1.0f, 1.0f);
        bf = 2.0f * fm_sinf(FM_PI * fm_minf(700.0f * fm_exp2f(1.5f * t), 2400.0f) / TAPE_SR);
        lpk = fm_clampf(0.72f + 0.22f * t, 0.3f, 0.97f);
    }
    gres = (20.0f * NCHAR[c].res + 5.0f * (1.0f - NCHAR[c].res)) * g;   /* (the band's level; the knob's folded in) */
    dry = 1.0f - NCHAR[c].res;                   /* (Dust: the bursts dry, into the band-pass) */
    cont = NCHAR[c].cont;
    gh = 0.03f * g * cr_dv[0] * room;           /* the Air: quiet (it breathes with the duck: render_chunk) */
    lpc = fm_clampf(0.4f * fm_exp2f(0.5f * nc), 0.15f, 0.8f);   /* (its band: ~2-4 kHz at the top) */
    if (par[P_NLDEST] == EN_NLD_FILTER && (par[P_NLFO] > 0 || par[P_NLWALK] > 0)) {   /* (1.4.0, the user's) the LFO
                                                  * and the drift on the Filter (the Destination's first): a low-pass on the
                                                  * whole noise, its hits and its hiss; open at the top of their
                                                  * reach, closing towards the bottom, 3.5 octaves at 100 (~20 kHz ..
                                                  * ~1.4 kHz). In the noise's own filters, not one of its own after
                                                  * them (the FM-1's CPU: on in every piece, the drift's): the hiss's
                                                  * and the clicks' low-passes no higher than it, the rings' through a
                                                  * one-pole at their quarter rate */
        const float fc = 20000.0f * fm_exp2f(-1.9f * fm_minf((float)(par[P_NLFO] + par[P_NLWALK]) * 0.01f - lfo_n, 2.0f));
        nzla = 1.0f - fm_expf(-2.0f * FM_PI * (1.0f / EN_SR) * fc);
        nzl4 = 1.0f - fm_expf(-2.0f * FM_PI * (1.0f / TAPE_SR) * fc);
        lpc = fm_minf(lpc, nzla);
    }
    vg = 0.5f * g * NCHAR[c].clicks * fm_exp2f(0.7f * ng);   /* (the dry clicks, quiet) */
    thr = !nctl.hits ? 0u : (uint32_t)((3.0f + 12.0f * g) * cr_dv[2] * fm_exp2f(ng) * NCHAR[c].loose * (4294967296.0f / EN_SR));   /* dry
                                                  * clicks a second, the loose ones (the rest on the grid: noise_step) */
    {   /* (the state in locals: the compiler keeps it in registers, past the sends' stores) */
        uint32_t cn = cnoise;
        float lp1 = cr_lp, lp2 = cr_lp2, hp1 = cr_hp, cl = cr_cl, cr = cr_cr, fl = cr_fl, fr = cr_fr, cc = cr_cc;
        float ex = nz_ex, ck = nz_click, acc = nz_acc, b0 = nz_b[0], b1 = nz_b[1], b2 = nz_b[2], b3 = nz_b[3];
        float pl = nz_p[0], pr = nz_p[1], ql = nz_c[0], qr = nz_c[1];
        float exd = nz_exd, rl = nzf_s[0], rr = nzf_s[1], k0 = nzl_s[0], k1 = nzl_s[1], k2 = nzl_s[2], k3 = nzl_s[3];
        const float lc = lcut[4], lc4 = lcut4;   /* (the strip's Low cut: the rings at their rate, the clicks at the
                                                  * full; the hiss is above it, high-passed ~1 kHz) */
        const float sw = 0.05f * cont * (1.0f - 0.8f * duck_d);   /* (Tape: the hiss in, between beats) */
        const int32_t sj = nz_wait >= 0 && nz_wait < (int32_t)n ? nz_wait : -1;   /* (a hit strikes in this chunk) */
        const int rest = SHED && ex < 1e-4f && cont <= 0.0f &&
                         fm_fabsf(nz_lp[0]) + fm_fabsf(nz_lp[1]) + fm_fabsf(nz_lp[2]) < 2e-4f;
        /* (1.3.0: the noise under the duck, deeper than the music, its sends too: it dips on each beat and swells
         * between, the kick or not; the duck as it is at this chunk's start) */
        const float dn = 1.0f - fm_minf(1.7f * duck_depth, 0.85f) * duck_d;
        /* (the combs at the music's own depth: ducked deeper, a hit on the beat swelled late, REPORT6 9.1) */
        const float dnr = 1.0f - fm_minf(duck_depth, 0.85f) * duck_d;
        /* (the second page: Width, Plate, Echo; PERFORM's Wet, -50..+50: the dry x2 .. x0.5, the sends x0.5 .. x2) */
        const float wv = (float)par[P_NWIDTH] * 0.02f, wa = 0.6f + 0.25f * wv, wb = 0.6f - 0.25f * wv,
                    wm = (float)mac[EN_MAC_DUB] * (1.0f / 50.0f), nw = fm_exp2f(-wm),
                    np = (float)par[P_NPLATE] * 0.02f * fm_exp2f(wm), ne = (float)par[P_NECHO] * 0.02f * fm_exp2f(wm) * (1.0f + 3.0f * song_nthr);
        int ph = nz_ph, w = nz_w;
#define CRU() (cn = cn * 1664525u + 1013904223u, (float)(cn >> 8) * (1.0f / 16777216.0f))
        for (j = 0; j < n; j++) {
            float wn, h, x, l, r, f;
            cn = cn * 1664525u + 1013904223u;
            wn = (float)(int32_t)cn * (1.0f / 2147483648.0f);
            lp1 += lpc * (wn - lp1);             /* the Air */
            lp2 += lpc * (lp1 - lp2);
            hp1 += 0.15f * (lp2 - hp1);
            h = lp2 - hp1;
            if ((int32_t)j == sj) {              /* the next step's hit strikes, ahead of its tick */
                ex = nz_wv, exd = nz_wd;
                nz_D[0] = nz_wD[0], nz_D[1] = nz_wD[1], nz_D[2] = nz_wD[2];
            }
            if (j == 0 && nz_gck > 0.0f) {       /* a dry click on the tick */
                float s1 = CRU() < 0.5f ? -1.0f : 1.0f, pn = 0.3f + 0.4f * CRU(), a = nz_gck;
                cc = 0.05f + 0.3f * CRU();
                a *= 1.8f - 0.8f * cc;
                cl += a * s1 * (1.0f - pn);
                cr += a * s1 * pn;
                nev(0, a * vg * 3.0f, pn);
                nz_gck = 0.0f;
            }
            if (cn < thr) {                      /* a dry vinyl click, now and then (dull, near the middle) */
                float a = CRU(), s1 = CRU() < 0.5f ? -1.0f : 1.0f, pn = 0.3f + 0.4f * CRU();
                cc = 0.05f + 0.3f * CRU();
                a = (0.03f + 0.4f * a * a * a) * (1.8f - 0.8f * cc);
                cl += a * s1 * (1.0f - pn);
                cr += a * s1 * pn;
                nev(0, a * vg * 3.0f, pn);
            }
            {
                const float cq = fm_minf(cc, nzla);   /* (the LFO's low-pass: above) */
                fl += cq * (cl - fl);
                fr += cq * (cr - fr);
            }
            cl *= 0.55f;
            cr *= 0.55f;
            x = wn * ex + h * sw + 0.3f * (fl + fr);   /* what rings the combs: the hit's burst (no crack), the hiss
                                                         * (Tape), the clicks */
            ck = 0.0f;
            ex *= exd;
            acc += x;
            if (++ph >= 4) {                     /* the combs and the band-pass, a quarter of the time */
                float y[3], o[3], m, sl, sr2;
                int q;
                ph = 0;
                m = acc * 0.25f;
                acc = 0.0f;
                if (dry >= 1.0f || rest)         /* (Dust: no resonance, the combs not run; shedding, rings quiet) */
                    o[0] = o[1] = o[2] = 0.0f;
                else
                for (q = 0; q < 3; q++) {        /* y = x + g LP(y[n - D]), read between two samples */
                    float d = nz_D[q], rd = (float)w - d, a, fb;
                    int i0, i1;
                    if (rd < 0.0f)
                        rd += (float)NZ_L;
                    i0 = (int)rd;
                    i1 = i0 + 1 < NZ_L ? i0 + 1 : 0;
                    a = rd - (float)i0;
                    fb = ((float)nz_buf[q * NZ_L + i0] + a * (float)(nz_buf[q * NZ_L + i1] - nz_buf[q * NZ_L + i0])) * (1.0f / 12000.0f);
                    nz_lp[q] += lpk * (fb - nz_lp[q]);
                    o[q] = cg * nz_lp[q];        /* (out: the ring alone, not the burst that struck it) */
                    y[q] = m + o[q];
                    nz_buf[q * NZ_L + w] = s16(y[q]);
                }
                if (++w >= NZ_L)
                    w = 0;
                sl = o[0] + wa * o[1] + wb * o[2] + dry * m;   /* (the root in the middle, the third left, the fifth
                                                                  * right: Width) */
                sr2 = o[0] + wb * o[1] + wa * o[2] + dry * m;
                b1 += bf * (sl - b0 - 0.9f * b1);    /* (Chamberlin band-passes, Q ~1.1: b1 the band, b0 the low) */
                b0 += bf * b1;
                b3 += bf * (sr2 - b2 - 0.9f * b3);
                b2 += bf * b3;
                pl = ql, pr = qr;
                rl += nzl4 * (b1 * gres - rl);   /* (the LFO's low-pass: above; off, 1: as they were) */
                rr += nzl4 * (b3 * gres - rr);
                ql = rl, qr = rr;
                if (lc4 > 0.0f) {
                    k0 += lc4 * (ql - k0);
                    k1 += lc4 * (qr - k1);
                    ql -= k0;
                    qr -= k1;
                }
            }
            f = (float)ph * 0.25f;               /* (back to the full rate: a line from the last to the next) */
            l = (pl + (ql - pl) * f) * dnr;
            r = (pr + (qr - pr) * f) * dnr;
            esend[j] += (l + r) * (0.15f * ne + 0.8f * st_throw);   /* (a little into the echo, Echo; a throw, more) */
            rsend[j] += (l + r) * 0.3f * np + h * gh * 0.4f;   /* (the plate, Plate) */
            l *= 0.75f * nw;                     /* (the dry -2.5 dB, less into the plate than before: further back,
                                                  * its breathing heard, not smoothed into the plate's wash; PERFORM's
                                                  * Wet: the dry down as it turns up) */
            r *= 0.75f * nw;
            if (lc > 0.0f) {
                float cl1, cr1;
                k2 += lc * (fl - k2);
                k3 += lc * (fr - k3);
                cl1 = fl - k2;
                cr1 = fr - k3;
                l += (h * gh + vg * cl1) * dn;
                r += (0.9f * h * gh + vg * cr1) * dn;
            } else {
                l += (h * gh + vg * fl) * dn;
                r += (0.9f * h * gh + vg * fr) * dn;
            }
            crl[j] = l;
            crr[j] = r;
        }
#undef CRU
        nzf_s[0] = fm_flush(rl), nzf_s[1] = fm_flush(rr);
        if (lc > 0.0f)
            nzl_s[0] = fm_flush(k0), nzl_s[1] = fm_flush(k1), nzl_s[2] = fm_flush(k2), nzl_s[3] = fm_flush(k3);
        else
            nzl_s[0] = nzl_s[1] = nzl_s[2] = nzl_s[3] = 0.0f;
        cnoise = cn;
        cr_lp = fm_flush(lp1), cr_lp2 = fm_flush(lp2), cr_hp = fm_flush(hp1);
        cr_cl = fm_flush(cl), cr_cr = fm_flush(cr), cr_fl = fm_flush(fl), cr_fr = fm_flush(fr), cr_cc = cc;
        nz_wait = sj >= 0 ? -1 : nz_wait >= 0 ? nz_wait - (int32_t)n : -1;
        nz_exd = exd;
        nz_ex = fm_flush(ex), nz_click = ck, nz_acc = acc, nz_ph = ph, nz_w = w;
        nz_b[0] = fm_flush(b0), nz_b[1] = fm_flush(b1), nz_b[2] = fm_flush(b2), nz_b[3] = fm_flush(b3);
        nz_p[0] = pl, nz_p[1] = pr, nz_c[0] = ql, nz_c[1] = qr;
        for (i = 0; i < 3; i++)
            nz_lp[i] = fm_flush(nz_lp[i]);
    }
}

/* -------------------------------------------------------- the effects grid --- */
/* REC held, the keys: each effect eased in while its key is held and out when let go, costing nothing until
 * then. The echo ones (Throw, Time, Tape stop, Swell) are in the echo; Gate in the chord; Freeze in the cloud;
 * the rest here: a master filter on everything (Low-pass and High-pass sweeping, Wah, Phone), the plate thrown
 * (Verb, Crash), Wash (everything into the echo and the plate, the dry down), Stutter (a sixteenth caught and
 * looped), a dub Siren and a noise Riser */
static float mf_lp = 14.2f, mf_hp = 4.3f, mf_wah, mf_ph, mf_lfo;   /* the master filter: log2 of its cutoffs */
static float mf_c[3][4], mf_s[12];
static float crash_lp, crash_att;                /* (the Crash: darker, eased in) */
static float fx_vt, fx_wash, fx_cf, crash, sir_amp, sir_ph, sir_lfo, ris_amp, ris_oct, ris_s[2], ris_c[4];
static uint32_t fx_noise = 9871u;

static void fx_ctl(uint32_t n)                   /* once a chunk: the effects' easing, the filters' coefficients */
{
    const float t = (float)n / EN_SR;
    uint32_t on = fxm, rise = fxm & ~fxm_was;
    float lpt, hpt, ec;
    fxm_was = fxm;
    /* the echo: Time doubles it (halves it, if the tape is too short), Tape stop slows the tape to a quarter */
    fx_ts += ((on >> EN_FX_TAPESTOP & 1u ? 4.0f : 1.0f) - fx_ts) * fm_minf(t / (on >> EN_FX_TAPESTOP & 1u ? 0.7f : 0.25f), 1.0f);
    ec = on >> EN_FX_TIME & 1u ? (echo_len_t * 2.0f <= (float)(TAPE_N - 8) ? 2.0f : 0.5f) : 1.0f;
    fx_em = ec * fx_ts;
    fx_ek = fx_ts > 1.01f ? 0.004f : 0.001f;
    /* the master filter's targets: Low-pass down to 350 Hz, High-pass up to 1.3 kHz, Phone a band 450..3 kHz */
    lpt = on >> EN_FX_LOWPASS & 1u ? 8.45f : on >> EN_FX_PHONE & 1u ? 11.55f : 14.2f;
    hpt = on >> EN_FX_HIGHPASS & 1u ? 10.35f : on >> EN_FX_PHONE & 1u ? 8.8f : 4.3f;
    mf_lp += (lpt - mf_lp) * fm_minf(t / (lpt < mf_lp ? 0.5f : 0.3f), 1.0f);
    mf_hp += (hpt - mf_hp) * fm_minf(t / (hpt > mf_hp ? 0.5f : 0.3f), 1.0f);
    mf_wah += ((on >> EN_FX_WAH & 1u ? 1.0f : 0.0f) - mf_wah) * fm_minf(t / 0.08f, 1.0f);
    mf_ph += ((on >> EN_FX_PHONE & 1u ? 1.0f : 0.0f) - mf_ph) * fm_minf(t / 0.05f, 1.0f);
    if (mf_lp < 14.15f || mf_hp > 4.35f || mf_wah > 0.001f) {
        float wf;
        mf_lfo += t * 1.3f;                      /* (the wah: 1.3 sweeps a second) */
        if (mf_lfo >= 1.0f)
            mf_lfo -= 1.0f;
        wf = 350.0f * fm_exp2f(2.2f * (0.5f + 0.5f * sine(mf_lfo)));
        svf_coefs(mf_c[0], fm_exp2f(mf_lp), 1.0f);
        svf_coefs(mf_c[1], fm_exp2f(mf_hp), 1.2f);
        svf_coefs(mf_c[2], wf, 0.35f);
    }
    fx_gate += ((on >> EN_FX_GATE & 1u ? 1.0f : 0.0f) - fx_gate) * fm_minf(t / 0.03f, 1.0f);
    fx_vt += ((on >> EN_FX_VERB & 1u ? 1.0f : 0.0f) - fx_vt) * fm_minf(t / 0.03f, 1.0f);
    fx_cf += ((on >> EN_FX_FREEZE & 1u ? 1.0f : 0.0f) - fx_cf) * fm_minf(t / 0.1f, 1.0f);
    fx_wash += ((on >> EN_FX_WASH & 1u ? 1.0f : 0.0f) - fx_wash) * fm_minf(t / (on >> EN_FX_WASH & 1u ? 0.4f : 0.8f), 1.0f);
    if (rise >> EN_FX_CRASH & 1u)
        crash = 1.0f, crash_att = 0.0f;
    if (rise >> EN_FX_STUTTER & 1u) {            /* a sixteenth, caught from now */
        stt_state = 1;
        stt_pos = 0;
        stt_len = (int)fm_clampf(step_len, 1000.0f, (float)PLEX_N);
    } else if (!(on >> EN_FX_STUTTER & 1u) && stt_mix < 0.001f) {
        stt_state = 0;
    }
    sir_amp += ((on >> EN_FX_SIREN & 1u ? 1.0f : 0.0f) - sir_amp) * fm_minf(t / (on >> EN_FX_SIREN & 1u ? 0.02f : 0.15f), 1.0f);
    if (on >> EN_FX_RISER & 1u) {                /* the riser: up an octave and a half a second, louder as it goes */
        ris_oct = fm_minf(ris_oct + t * 1.5f, 5.2f);
        ris_amp += (fm_minf(0.25f + ris_oct * 0.15f, 1.0f) - ris_amp) * fm_minf(t / 0.05f, 1.0f);
    } else {
        ris_amp *= fm_expf(-t / 0.15f);
        if (ris_amp < 0.001f)
            ris_oct = 0.0f;
    }
    if (ris_amp > 0.0005f)
        svf_coefs(ris_c, 250.0f * fm_exp2f(ris_oct), 0.5f);
}

static void render_fx(uint32_t n)                /* the Siren and the Riser: two more voices, into the mix */
{
    uint32_t j;
    if (sir_amp > 0.0005f)
        for (j = 0; j < n; j++) {               /* a square-ish tone, its pitch swung by a triangle, ~0.8 a second */
            float tri, x;
            sir_lfo += 0.8f / EN_SR;
            if (sir_lfo >= 1.0f)
                sir_lfo -= 1.0f;
            tri = sir_lfo < 0.5f ? 4.0f * sir_lfo - 1.0f : 3.0f - 4.0f * sir_lfo;
            sir_ph += 480.0f * fm_exp2f(1.3f * tri) * (1.0f / EN_SR);
            if (sir_ph >= 1.0f)
                sir_ph -= 1.0f;
            x = sat(sine(sir_ph) * 3.0f) * 0.075f * sir_amp;   /* (1.3.0: -4 dB, it stood 6 dB over the music) */
            mixl[j] += x;
            mixr[j] += x;
            esend[j] += x * 2.0f;
        }
    if (ris_amp > 0.0005f)
        for (j = 0; j < n; j++) {               /* noise through a rising band-pass */
            float w, v1, v2, v3, x;
            fx_noise = fx_noise * 1664525u + 1013904223u;
            w = (float)(int32_t)fx_noise * (1.0f / 2147483648.0f);
            v3 = w - ris_s[1];
            v1 = ris_c[0] * ris_s[0] + ris_c[1] * v3;
            v2 = ris_s[1] + ris_c[1] * ris_s[0] + ris_c[2] * v3;
            ris_s[0] = 2.0f * v1 - ris_s[0];
            ris_s[1] = 2.0f * v2 - ris_s[1];
            x = v1 * 0.22f * ris_amp;            /* (1.3.0: -4 dB, it stood 6 dB over the music) */
            mixl[j] += x;
            mixr[j] += x;
            rsend[j] += x * 1.2f;
            esend[j] += x * 0.5f;
        }
    ris_s[0] = fm_flush(ris_s[0]);
    ris_s[1] = fm_flush(ris_s[1]);
}

static inline float svf_lp(const float *c, float *s, float x, float *bp)   /* one TPT SVF step: low-pass (bp: its band) */
{
    float v3 = x - s[1], v1 = c[0] * s[0] + c[1] * v3, v2 = s[1] + c[1] * s[0] + c[2] * v3;
    s[0] = 2.0f * v1 - s[0];
    s[1] = 2.0f * v2 - s[1];
    *bp = v1;
    return v2;
}

/* the master filter on one side (s: its six states): the low-pass, then the high-pass, the wah's band, the phone */
static inline float mfx(float x, float *s)
{
    float bp, lp;
    x = svf_lp(mf_c[0], s, x, &bp);
    lp = svf_lp(mf_c[1], s + 2, x, &bp);
    x = x - mf_c[1][3] * bp - lp;
    if (mf_wah > 0.001f) {
        (void)svf_lp(mf_c[2], s + 4, x, &bp);
        x += mf_wah * (bp * 1.8f - x);
    }
    if (mf_ph > 0.001f)
        x += mf_ph * (sat(x * 2.2f) * 0.6f - x);
    return x;
}

static float ms_lp1, ms_lp2;                     /* the side's lows (mono below ~150 Hz) */
static void render_chunk(int32_t *out, uint32_t n, float gain)
{
    uint32_t j;
    float epk, mpk[EN_NMETER] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, dlo = 1.0f, dlob = 1.0f;
    const float rg = rev_send * rev_in_gain * (1.0f - 0.2f * arcm);
    /* (the cloud listens as if Reverb were at 63 at least, so Cloud works with little reverb too) */
    const float cl_in = rev_send > 0.0001f ? fm_maxf(rev_send, 0.4f) / rev_send : 0.0f;
    const float push = perf_echo;
    float eg = (echo_send + (1.0f - echo_send) * push) * ein_gain * song_wet, ca = cloud_amt * (sec == SEC_BREAK ? 1.4f : 1.0f);   /* more in a Break */
    /* PERFORM's Open and Shut on the music, the kick left out on top: Shut a low-pass down to ~230 Hz, Open a
     * resonant high-pass up to ~400 Hz (thin and bright), both swept with perf_sw */
    const float shut = fm_maxf(-perf_sw, 0.0f), open = fm_maxf(perf_sw, 0.0f);
    if (shut > 0.01f)
        svf_coefs(mb_c, 14000.0f * fm_exp2f(-1.9f * shut), 0.55f);
    else if (open > 0.01f)
        svf_coefs(mb_c, 25.0f * fm_exp2f(1.33f * open), 0.5f);
    else
        mb_s[0] = mb_s[1] = mb_s[2] = mb_s[3] = 0.0f;
    for (j = 0; j < n; j++)
        mixl[j] = mixr[j] = rsend[j] = esend[j] = kickb[j] = bassb[j] = 0.0f;
    fader_step(n);                               /* (1.4.0: the parts' faders, the song's) */
    lfo_step();
    {   /* (a layer's own part of the dry mix: what its render adds; the bass, the kick and the noise their own) */
        const int tl = en_view_layer, mo = en_meter_on;
        render_stabs(n);
        if (mo)                                  /* (the meters: the mix is the chord's alone here) */
            mpk[EN_MT_CHORD] = mt_pk(mixl, mixr, n), mt_snap(n);
        if (tl == 1)
            for (j = 0; j < n; j++)
                tapb[j] = mixl[j] + mixr[j];
        render_bass(n);
        if (tl == 2)
            for (j = 0; j < n; j++)
                tapb[j] = -(mixl[j] + mixr[j]);
        render_bells(n);
        if (mo)
            mpk[EN_MT_BASS] = mt_pk(bassb, bassb, n), mpk[EN_MT_PINGS] = mt_diff(n, 0);
        widen(n);                                /* (the chord's and the pings' dry, wider) */
        if (tl == 2)
            for (j = 0; j < n; j++)
                tapb[j] += mixl[j] + mixr[j];
        if (tl == 4)
            for (j = 0; j < n; j++)
                tapb[j] = -(mixl[j] + mixr[j]);
        if (mo)
            mt_snap(n);
        render_drums(n);
        if (mo)
            mpk[EN_MT_KICK] = mt_pk(kickb, kickb, n), mpk[EN_MT_HATS] = mt_diff(n, 0);   /* (1.4.0: the kick its bus, the
                                                  * hats, the rim and the clap the mix's rest) */
        if (tl == 4)
            for (j = 0; j < n; j++)
                tapb[j] += mixl[j] + mixr[j] + 2.0f * kickb[j];
        if (tl == 3)
            for (j = 0; j < n; j++)
                tapb[j] = 2.0f * bassb[j];
        render_crackle(n);
        if (tl == 5)
            for (j = 0; j < n; j++)
                tapb[j] = crl[j] + crr[j];
        if (mo)
            mpk[EN_MT_NOISE] = mt_pk(crl, crr, n);
    }
    if (fxm || sir_amp > 0.0005f || ris_amp > 0.0005f)
        render_fx(n);
    eg += (1.0f - eg) * fx_wash;                 /* Wash: everything into the echo */
    epk = en_echo_pk;
    rb_on += ((SHED ? 0.0f : 1.0f) - rb_on) * 0.25f;
    if (rb_amt > 0.0f && rb_on > 0.004f)       /* (the kick's rumble, mono, ducked by the kick) */
        rumble_block(n);
    else if (rb_c != 0.0f || rb_e != 0.0f)
        rumble_init();
    for (j = 0; j < n; j++) {
        float wl, wr, l, r, el, er, xl, xr, dg, dgb, rin, cl, cr, sd, dry = 1.0f - 0.75f * fx_wash;
        duck_d += (duck_x - duck_d) * 0.08f;
        duck_x *= duck_rel;
        dg = 1.0f - 0.3f * duck_depth * duck_d;  /* the duck split: the wash ducked lightly, the bass deep (it pumps) */
        duck_db += (duck_xb - duck_db) * 0.08f;
        duck_xb *= duck_relb;
        dgb = 1.0f - fm_minf(2.0f * duck_depth, 0.9f) * duck_db;
        dlo = fm_minf(dlo, dg), dlob = fm_minf(dlob, dgb);   /* (the meters' GR) */
        echoes(esend[j] * eg, &el, &er, &xl, &xr);
        {   /* (a strike: the returns down by up to 6 dB, back over ~150 ms; not a throw's, which rides over) */
            float ge;
            ec_dd += (ec_dk - ec_dd) * 0.05f;
            ec_dk *= 0.99985f;
            ge = 1.0f - 0.5f * fm_minf(ec_dd, 1.0f) * (1.0f - st_throw);
            el *= ge;
            er *= ge;
        }
        epk = fm_maxf(epk, fm_fabsf(el + er));   /* (the screen's ripples: the echo as it comes out) */
        rin = (rsend[j] * rg + (el + er) * 0.12f * rev_send) * 0.8f;   /* (the classics pass: less of the echo into
                                                  * the plate, 0.25 before: the repeats stay distinct) */
        if (fx_vt + fx_wash > 0.001f || crash > 0.001f) {   /* Verb, Wash: the dry into the plate; Crash: a burst */
            rin += (mixl[j] + mixr[j] + 2.0f * bassb[j]) * (0.35f * fx_vt + 0.3f * fx_wash);
            if (crash > 0.001f) {
                fx_noise = fx_noise * 1664525u + 1013904223u;
                /* (1.3.0: it was white noise at x2, 15 dB over the music: now a darker wash, eased in over ~8 ms,
                 * at the music's own level) */
                crash_lp += 0.3f * ((float)(int32_t)fx_noise * (1.0f / 2147483648.0f) - crash_lp);
                crash_att += (1.0f - crash_att) * 0.003f;
                rin += crash_lp * crash * crash_att * 0.6f;
                crash *= 0.99971f;               /* (~80 ms) */
            }
        }
        cloud(rin * ca * 2.5f * cl_in + pl_back * cloud_amt * 0.22f, &cl, &cr);   /* the grains, into the plate: they bloom;
                                                 * the plate's tail back into the line: the wash grows on itself */
        {   /* the plate, every other sample (its input the pair's mean, its output a line between) */
            const float pin = rin * (1.0f - 0.5f * cloud_amt) + (cl + cr) * 0.8f;
            if ((pl_ph ^= 1u) != 0u) {
                pl_acc = pin;
                wl = pl_ol, wr = pl_or;
            } else {
                float nl, nr;
                reverb(0.5f * (pl_acc + pin), &nl, &nr);
                wl = 0.5f * (pl_ol + nl), wr = 0.5f * (pl_or + nr);
                pl_ol = nl, pl_or = nr;
            }
        }
        pl_back = 0.5f * (wl + wr);
        cl *= 1.0f + 9.0f * fx_cf;               /* (Freeze: the held cloud brought forward) */
        cr *= 1.0f + 9.0f * fx_cf;
        l = mixl[j] * dry + 1.2f * el + 1.1f * wl + 1.4f * cl;   /* (1.3.0: the Echoplex's taps out) */
        r = mixr[j] * dry + 1.2f * er + 1.1f * wr + 1.4f * cr;
        sd = 0.5f * (l - r);                     /* mono below ~80 Hz (the classics pass; was ~150): the side's lows
                                                  * taken out (two poles), so the
                                                  * stabs' and pings' low partials, the echo's and the plate's, sit in
                                                  * the middle with the bass */
        ms_lp1 += 0.0113f * (sd - ms_lp1);
        ms_lp2 += 0.0113f * (sd - ms_lp1 - ms_lp2);
        sd = ms_lp1 + ms_lp2;
        l = (l - sd) * dg + bassb[j] * dry * dgb;
        r = (r + sd) * dg + bassb[j] * dry * dgb;
        if (shut > 0.01f) {
            float bp;
            l = svf_lp(mb_c, mb_s, l, &bp);
            r = svf_lp(mb_c, mb_s + 2, r, &bp);
        } else if (open > 0.01f) {               /* (the high-pass: what the low-pass and its band leave) */
            float bp, lp = svf_lp(mb_c, mb_s, l, &bp);
            l -= lp + mb_c[3] * bp;
            lp = svf_lp(mb_c, mb_s + 2, r, &bp);
            r -= lp + mb_c[3] * bp;
        }
        l = (l + kickb[j] * (1.0f - 0.5f * fx_wash) + crl[j]) * 0.75f * gain;   /* (1.3.0: +1.5 dB, what the
                                                                                  * re-balance took, from the headroom;
                                                                                  * the classics pass: +2 dB more, the
                                                                                  * user's ear; the loop held, the
                                                                                  * level steadier) */
        r = (r + kickb[j] * (1.0f - 0.5f * fx_wash) + crr[j]) * 0.75f * gain;
        if (mf_lp < 14.15f || mf_hp > 4.35f || mf_wah > 0.001f) {   /* the master filter */
            l = mfx(l, mf_s);
            r = mfx(r, mf_s + 6);
        }
        if (stt_state) {                         /* Stutter: catch a sixteenth (mono), then loop it */
            float m = 0.5f * (l + r), v = 0.0f, tg = 0.0f;
            if (stt_state == 1) {
                plex[stt_pos] = s16(m * 0.5f);
                if (++stt_pos >= stt_len)
                    stt_state = 2, stt_pos = 0;
            } else {
                int e = stt_pos < stt_len - stt_pos ? stt_pos : stt_len - stt_pos;
                v = (float)plex[stt_pos] * (2.0f / 12000.0f) * fm_minf((float)e * (1.0f / 64.0f), 1.0f);
                if (++stt_pos >= stt_len)
                    stt_pos = 0;
                tg = fxm >> EN_FX_STUTTER & 1u ? 1.0f : 0.0f;
            }
            stt_mix += (tg - stt_mix) * 0.005f;
            l += stt_mix * (v - l);
            r += stt_mix * (v - r);
        }

        mpk[EN_MT_MASTER] = fm_maxf(mpk[EN_MT_MASTER], fm_maxf(fm_fabsf(l), fm_fabsf(r)));   /* (the master meter, the
                                                  * clip light: going into the soft clip) */
        l = soft(l);                             /* (1.3.0: a rational curve, not tanh: the same below the clip,
                                                  * a fraction of the time) */
        r = soft(r);
        out[2 * j] = (int32_t)(l * 8300000.0f);
        out[2 * j + 1] = (int32_t)(r * 8300000.0f);
        {   /* (the screen's scope and spectrum: the output, or a layer alone) */
            const float v = en_view_layer ? tapb[j] * 0.75f * gain : l + r;
            scope_acc += v;
            spec_acc += v;
        }
        if (scope_ph & 1u) {
            en_spec[en_spec_w & (EN_SPEC_N - 1u)] = (int16_t)(fm_clampf(spec_acc, -4.0f, 4.0f) * 8000.0f);
            en_spec_w++;
            spec_acc = 0.0f;
        }
        if (++scope_ph >= 8u) {                  /* (an eighth of the rate, each the mean of its eight) */
            scope_ph = 0;
            en_scope[en_scope_w & (EN_SCOPE_N - 1u)] = (int16_t)(fm_clampf(scope_acc, -16.0f, 16.0f) * 2000.0f);
            scope_acc = 0.0f;
            en_scope_w++;
        }
    }
    en_echo_pk = epk;
    {   /* (the meters: in the output's units, the parts' with the master's gain; each the highest since taken) */
        const float og = 0.75f * gain;
        int i;
        for (i = 0; i < EN_MT_MASTER; i++)
            if (mpk[i] * og > en_meter[i])
                en_meter[i] = mpk[i] * og;
        if (mpk[EN_MT_MASTER] > en_meter[EN_MT_MASTER])
            en_meter[EN_MT_MASTER] = mpk[EN_MT_MASTER];
        if (dlo < en_duck_lo[0])
            en_duck_lo[0] = dlo;
        if (dlob < en_duck_lo[1])
            en_duck_lo[1] = dlob;
    }
    duck_x = fm_flush(duck_x);
    duck_d = fm_flush(duck_d);
    duck_xb = fm_flush(duck_xb);
    duck_db = fm_flush(duck_db);
    ms_lp1 = fm_flush(ms_lp1);
    ms_lp2 = fm_flush(ms_lp2);
}

static void control(uint32_t n)                 /* once a chunk: the slow things */
{
    int i;
    mods_step(n);
    fx_ctl(n);
    heads_step(n);
    if (en_playing)
        arc += (arc_tgt - arc) * fm_minf((float)n / arc_tau, 1.0f);
    arcm = arc * motion;
    en_arc = arcm;
    pl_decay += ((en_frozen ? 0.9995f : pl_decay_t) - pl_decay) * 0.002f;
    pl_dd2 = fm_clampf(pl_decay + 0.15f, 0.25f, 0.5f);
    rev_in_gain += ((en_frozen ? 0.0f : 1.0f) - rev_in_gain) * 0.01f;
    ein_gain += ((en_frozen ? 0.0f : 1.0f) - ein_gain) * 0.05f;
    fb_cap += ((en_playing ? 2.0f : 0.9f) - fb_cap) * fm_minf((float)n / (2.0f * EN_SR), 1.0f);
    {   /* the performance's pushes, eased in and out (the filter swept over ~400 ms, three octaves; the echo ~40 ms) */
        /* (the Filter knob: the same sweep, quicker to follow a hand; the keys on top of it, to the same ends) */
        /* (the knob reaches further than the keys: Shut down to ~120 Hz, Open up to ~1.2 kHz) */
        float t = fm_clampf((float)mac[EN_MAC_FILTER] * (mac[EN_MAC_FILTER] < 0 ? 0.072f : 0.084f) +
                            (perf & EN_PF_OPEN ? 3.0f : 0.0f) - (perf & EN_PF_CLOSE ? 3.0f : 0.0f), -3.6f, 4.2f);
        perf_sw += (t - perf_sw) * fm_minf((float)n / ((perf & (EN_PF_OPEN | EN_PF_CLOSE) ? 0.4f : 0.12f) * EN_SR), 1.0f);
        perf_echo += ((perf & EN_PF_ECHO || fxm >> EN_FX_SWELL & 1u ? 1.0f : 0.0f) - perf_echo) * fm_minf((float)n / (0.04f * EN_SR), 1.0f);
        /* (1.4.0, the user's) the hats thrown: in over ~15 ms, out over ~60 ms (the send let go, the echo's tails left
         * to ring); off, 0 exactly (the hats' sends as before, to the bit) */
        hat_thr += ((perf & EN_PF_HATTHROW ? 1.0f : 0.0f) - hat_thr) *
                   fm_minf((float)n / ((perf & EN_PF_HATTHROW ? 0.015f : 0.06f) * EN_SR), 1.0f);
        if (!(perf & EN_PF_HATTHROW) && hat_thr < 1e-4f)
            hat_thr = 0.0f;
    }
    if (echo_len > 0.0f)                        /* for the screen's ripples */
        en_echo_ms = echo_len * (1000.0f / TAPE_SR);
    en_echo_g = en_frozen ? 1.0f : fm_minf(echo_fb + 0.12f * perf_echo, fb_cap) * 0.82f;
    en_echo_in = echo_send + (1.0f - echo_send) * perf_echo;
    en_duck = duck_d * (duck_depth > 0.0f ? 1.0f : 0.0f);
    cl_dens = cloud_amt > 0.0f ? fm_clampf(0.25f + 0.5f * cloud_amt + 0.3f * arcm, 0.05f, 1.0f) : 0.0f;
    if (fx_cf > 0.01f)                          /* (Freeze: the grains thick, even at Cloud 0) */
        cl_dens = fm_maxf(cl_dens, fx_cf);
    for (i = 0; i < EN_NLOOP; i++)
        en_loop_glow[i] *= 0.985f;
    for (i = 0; i < EN_NDRUM; i++)
        en_drum_glow[i] *= 0.93f;
}

void en_render(int32_t *out, uint32_t n, uint32_t gain_q12)
{
    const float gain = (float)gain_q12 * (1.0f / 4096.0f) * 2.0f;
    if (en_load > 75u)                           /* (the last block ran heavy: shed for half a second; the classics
                                                  * pass: from 75, was 80, and shedding more) */
        shed_left = (int32_t)(0.5f * EN_SR);
    else if (shed_left > 0)
        shed_left -= (int32_t)n;
    drain();
    if (en_ext && ++ext_age > 100u)
        en_ext = 0;
    while (n) {
        uint32_t k = n > BLK ? BLK : n;
        int timed = en_playing && !en_ext;
        if (timed) {
            if (pulse_left <= 0.0f) {
                clock_pulse();
                pulse_left += step_len * (1.0f / 6.0f);
            }
            if ((float)k > pulse_left)
                k = (uint32_t)pulse_left + 1u;
        }
        if (k > n)
            k = n;
        {
            int32_t w = drums_wait();            /* a swung hit lands on its sample */
            if (w > 0 && (uint32_t)w < k)
                k = (uint32_t)w;
            if (tick_wait > 0 && (uint32_t)tick_wait < k)   /* (and a swung sixteenth) */
                k = (uint32_t)tick_wait;
        }
        if (timed)
            pulse_left -= (float)k;
        control(k);
        render_chunk(out, k, gain);
        drums_wait_step((int32_t)k);
        if (tick_wait > 0 && (tick_wait -= (int32_t)k) <= 0) {
            tick_wait = 0;
            if (en_playing)
                tick();
        }
        out += 2 * k;
        n -= k;
    }
}
