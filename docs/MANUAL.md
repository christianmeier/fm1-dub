# DUB-1

DUB-1 (a working name) turns the M-VAVE FM-1 into a generative dub techno machine. It plays itself
from the moment it's switched on: chord stabs through a resonant low-pass into a tape echo, short
FM pings on loops of unequal length, a sub bass on one of five grooves, a kick that ducks the rest
(four on the floor, or sparser, a phrase at a time), a shaker and an open hat, and a crackle that
rings on the chord. An eight-bar arrangement takes the kick out for a break, thins the layers and
brings them back with a fill, and now and then a stab is thrown into the echo. The drums and the
bass play in phrases, a main bar three times and a variant on the fourth. You don't play it, you
steer it: pick a piece and a scale, take notes out, open the filter, push the feedback, hold the
echo, lock the groove.

DUB-1 is a fork of AMB-1, Charles Vestal's generative ambient firmware for the FM-1.

![A drawing of the FM-1 running DUB-1, each control labelled with what it does: the Controls section below says the same in words](img/fm1-panel.svg "The FM-1 (a drawing), running DUB-1, each control labelled")

---

## Contents

1. Install
2. Getting started
3. Controls
4. The screen
5. The layers
6. The arrangement
7. The pieces
8. The pages
9. MIDI
10. Saving
11. Troubleshooting
12. Parameters
13. Credits

---

## 1. Install

**Try it first** in the browser, with no FM-1: <https://sbegueria.github.io/fm1-dub/emu/>. It's the
same code the FM-1 runs, with sound.

**Install from the browser:** open <https://sbegueria.github.io/fm1-dub/install/> in Chrome or Edge
(they have Web MIDI), connect the FM-1 with a USB data cable, switch it on and press Install. Nothing
to install on the computer. Don't unplug it while it's writing; when it finishes, the FM-1 restarts
into DUB-1.

**Or from the command line:** download `dub-1.3.0.fwsc` from the
[releases](https://github.com/sbegueria/fm1-dub/releases), install Python 3 with
`pip3 install mido python-rtmidi`, and run, in a checkout of the source:

```
python3 tools/fm1_install.py dub-1.3.0.fwsc
```

**Go back:** the installer page's "Back to the stock firmware", or M-VAVE's own updater, M-UPGRADE,
puts the stock firmware back.

> Installing other firmware is at your own risk. If the FM-1 ever won't start, see
> [Recovering an FM-1 that won't start](#recovering-an-fm-1-that-won-t-start).

---

## 2. Getting started

DUB-1 starts playing as soon as it's on. A few minutes with it:

1. **Listen.** The first piece, Basic, is playing: F Dorian at 122, saw stabs into a dotted-eighth
   echo, an offbeat sub, the kick, the shaker and the open hat. Turn MASTER for the volume.
2. **Try another piece.** Turn PRESETS. Each of the eight pieces has its own key, tempo, sounds,
   groove and amount of movement.
3. **Change the scale.** Press a key whose note is lit: it leaves the scale, and nothing plays it
   any more. Press it again to bring it back. Any key that isn't lit adds its note.
4. **Move on.** Press EDIT: the chord moves to the next one at the start of the next bar.
5. **Hold the echo.** Press REC: dub hold. The echo stops listening and repeats what it holds,
   over and over, while the stabs, pings and drums go on dry above it. Press REC again to let it go.
6. **Shape a layer.** Each layer has a button: ENV the chord, ARP the pings, LFO the bass, SEQ the
   drums, SAVE the noise, FX the echo and the plate (the Dub page; again, the Echo page), HOME the whole
   (the Form page; again,
   the Key page: tempo, root, mode, swing), GLO the setup. The page's name is at the top right.
   KNOB 1–4 set the four values at the bottom of the screen; while one turns, the line above them
   says what it does. SELECT is the DJ filter, on every page.
   Try Cutoff on the Chord voice page (ENV twice), or
   Feedback on the Dub page. Or turn SELECT (the Filter) down, and
   bring it back. On a layer's page, OCT− and OCT+ turn that layer down and up; both together, a
   quick press, mute it (and bring it back).
7. **Rest.** Press PLAY: the music stops and the echo and reverb ring away. PLAY again and it
   starts from the top of a phrase.

What you change is kept with the piece. DUB-1 saves it by itself while resting, or hold GLO for half a second (or turn Setup's KNOB 4).

Want a new colour? Hold a layer's button (ENV, ARP, LFO or SEQ) for half a second: its sound takes a small step somewhere new.

Like the groove as it is? Hold SEL for half a second: **Lock**. Each new phrase keeps the kick, the
drums' and the bass's patterns and the noise's groove until you hold SEL again.

---

## 3. Controls

The drawing at the top shows them all; here they are in more detail.

| Control | What it does |
|---|---|
| Keys | The scale. Each key adds its note to the scale or takes it out; the keys light up for the notes in it. Every octave of a note is the same key. The root stays ("THE ROOT STAYS"), and the scale keeps at least three notes ("3 NOTES AT LEAST"). |
| PLAY | Play / rest. DUB-1 starts playing by itself when it's switched on. At rest, the echo's feedback eases under 90% so its tails die away, the reverb rings out and the noise fades. Playing again starts from the first bar of a phrase. Lit while playing. In Song or Set mode (GLO again: [Songs](#songs-an-intro-the-main-an-outro)) PLAY starts a song from its intro, and the song stops PLAY by itself at its end. **HOME held + PLAY:** the **outro**, from the next phrase, in any mode (Endless too: a way to land a piece by hand); again, in it or while it waits: back to the main. |
| REC | A tap: **dub hold**: the echo stops listening and repeats what it holds, sample for sample, for ever; the reverb stops listening too and rings on; the chord doesn't move. The stabs, pings and drums go on, dry, above it. Tap again to let go. Lit while held. Held: the **effects grid** (below). |
| EDIT | A tap: the **Chords page** (below): the eight chords coming, to see and change (again, it stays). Held (half a second): the next chord, at the start of the next bar (at once while resting; in dub hold, once you let go); a card says NEXT CHORD until the chord changes. Held on (1.5 s): **new chords**, the walk's all walked again (yours, the gaps and the one playing kept); a red card, NEW CHORDS. |
| SEL | A tap: the **Arrangement page** (PERFORM's until 1.4.0): the screen shows each part's level on a desk's meters, the knobs are Width, Mixer (the dub mixer), Wet and Duck, and the keys stop being the scale: they mute and solo the parts (white 1–6 held: a part's channel strip), make the dub moves and throw the hats (see below). Again, it stays; a page's button or SELECT leaves it. Lit while on. **Held** (half a second), on any page: **Lock** on or off (below). |
| HOME | The **Form** page (Dub until 1.4.0): how the piece unfolds (Energy, Evolve, Motion). Again: the **Song** page (its intro, main and outro, 1.4.0); again: the Key page (tempo, root, mode, swing), and again back to Form; HOME blinks on the Song and Key pages. Held on for 1.5 s (nothing turned meanwhile): **a shake of everything**, the chord, the pings, the bass, the drums and the noise at once ("SHAKE ALL"), the page kept; again and again, to explore until something sounds right. Held with SELECT turned: the tempo. |
| ENV · ARP · LFO · SEQ | The layers' pages: Chord, Pings, Bass, Drums, each its part (see [The pages](#8-the-pages)). Again: the layer's voice page (**Chord voice**, **Pings voice**, **Bass voice**; the drums' **Kick**), then its FM page or, for the drums, the **Hats** page and then the Pattern page, then back. The chord has one more: after its FM page (for every chord voice: its Colour is there), the **Chord LFO** page, then back. Held (half a second): a **nudge** to the layer's sound; held on (1.5 s): a **shake** (below). |
| FX · GLO | The **Dub** page, the echo and the plate (Space until 1.4.0; FX again: the **Echo page**, the echo's blur and tone, the plate's size and its cloud, FX blinking; again, back); the Setup page, for what holds for every piece (GLO again: **Song mode**, Endless, Song or Set; again, back). The lit button shows the page. FX held: a nudge to the space; held on: a shake (a new echo time). **GLO held** (half a second, then let go): **saves** all eight pieces (a card, SAVED); held on (1.5 s): the piece back to its recipe (a red card, RESET). |
| SELECT | **The DJ filter**, on every page (its only place since 1.4.0's Chord pages; the Arrangement page's Open and Shut keys push on top of it): left shuts it (a low-pass, under water), right opens it (a high-pass, thin and bright), Off in the middle, where it stops on its way through (three clicks to leave it). While it turns, a big ring in the middle shows it (FILTER over it, the amount inside, Shut or Open under it); the page stays. Hold HOME and turn it for the tempo, from any page. The pages are on their buttons; the page's name, top right, says where you are. |
| ALGORITHM | On a layer's page (Chord, Pings, Bass, Drums): that layer's **voice** (see [The voices](#the-voices)). On the Noise pages: its character. On the LFO pages: where the LFO goes, its **Destination** (the Chord LFO's: Cutoff, Reso, Colour; the Noise LFO's: Filter, Tone, Air). On the Chords page: the walk's character (Home, Wander, Free). Anywhere else: the middle of the screen: ripples, a scope, a spectrum, the echo trail, the fifths or the tape (see [The screen](#4-the-screen)). The mode, a piece's choice, is on the Key page (HOME again). |
| PRESETS | The piece (see [The pieces](#7-the-pieces)). |
| KNOB 1–4 | The four values on the screen. A few clicks are always single steps; a quick turn speeds up, so half a turn sweeps the range. While a knob turns, its value shows large in the middle of the screen, in a big ring with its name and what it does under it (on the Chords and Pattern pages, which show the change themselves, the line above the knobs says what it does). A **centred** knob (0 in the middle: the voice pages' −50…+50 ones, the chord's Cutoff, the Noise voice's Tone, the Filter, Energy (the Form and the Arrangement pages') and the Arrangement page's Wet) has a **notch** at 0: turning through, it stops there, and to leave it takes three clicks the same way (the first two are held back). A switch shows its positions as dots, up to ten (Change, Mode, Algo). A grey knob does nothing there. |
| OCT− / OCT+ | On a layer's page (Chord, Pings, Bass, Drums or Noise, and their voice, FM, Hats and Pattern pages, and the Noise pages): that layer's **level**, 5 down or up a press (a big ring in the middle, in the layer's colour, BASS over it, the level inside, until a second after the last press); held, it keeps going. **Both together**, let go within a second: the layer **muted** ("BASS MUTED"), its level kept; again, back ("BASS ON"). The drums mute the kick and the hats together. These are the Arrangement page's mutes (white 1–6): a mute made here shows there, and the other way round. Held together for five seconds: the update mode (see [Troubleshooting](#11-troubleshooting)); a long press never mutes. On the other pages they do nothing. (The pings' octave is on the Pings voice page, KNOB 1.) |
| MASTER | Volume. |
| SAVE | The **Noise page** (below: its part); again, the **Noise voice** (its Air, Tone, Decay and Width; SAVE blinks); again, **Noise LFO** (its wave and drift, on its filter, its Tone or its Air); again, back to the Noise page. (Its sends are its channel strip's, on the Arrangement page.) Held: a nudge to the noise; held on (1.5 s): a shake (another noise character). Saving is on GLO. |

**One rule for the buttons:** a button with one page stays on it when you press it again (EDIT,
SEL); a button with several steps through them (HOME: Form, Song, Key; GLO: Setup, Song mode; FX: Dub, Echo; SAVE: Noise,
Noise voice, Noise LFO; a layer's: its page, its voice page, its FM page; the drums': Drums, Kick, Hats,
Pattern; the chord's, Chord, Chord voice, Chord FM and Chord LFO). To leave a page, press
another page's button.

**The Arrangement page** (SEL; PERFORM until 1.4.0). The mixing desk: the screen shows every part's
level, and the knobs act on the whole music. Wet is 0 in the middle (the piece as it is), with the
notch there, so it's easy to find:

| Knob | Down | Up |
|---|---|---|
| KNOB 1 · **Width** | the whole mix narrower, to mono at 0 (the lows are always mono) | wider: the echo's right side a few milliseconds behind its left, a slow crossed chorus on the stabs and the pings; at 100 as wide as the widest records (Fog and Night Drive have 85) |
| KNOB 2 · **Mixer** | 0: off, the desk as you set it | up to 100: the desk rides itself, more and further: the parts' levels and sends, the chord's filter, the echo's feedback (below) |
| KNOB 3 · **Wet** | the whole music drier: the chord and the pings up front, less into the echo, the noise dry | the whole music drowned: the chord and the pings almost all echo, more into the echo, the noise into the echo and the plate |
| KNOB 4 · **Duck** | 0: the music still, the kick on top of it | up to 100: each beat pushes the rest down, to −12 dB, the music pumping |

**Width** is the desk's master stereo field, the piece's own, saved with it (the Echo page's until
1.4.0's space pages). **Energy**, here until then too, is on the Form page alone (HOME): one value, in
one place. The Filter isn't here any more:
it's on SELECT, on every page. The Open and Shut keys push on top of it. **Wet** moves the chord's
and the pings' dry against their sends (each piece's own balance, on no knob) and the echo's send
together, and the noise with them (its dry down, its plate and echo up): one knob for the whole
music, dry and close or lost in the echo. It's an offset on top of everything else, the channel
strips' sends included (below), which stay where they are. Wet is for playing: it isn't saved, and it
stays where it is when you change piece, back in the middle when the FM-1 starts.

**Mixer**, the virtual dub mixer, is how much the desk rides itself, as a dub engineer's hand on the
faders and the sends. It's the piece's own, saved with it: Basic 25, Steppers 20, Pressure 30, Concrete 35,
Chamber 45, Night Drive 55, Fog and Haze 60 (the source's held loops little, the US records' played desk
most). At 0 it's off: the desk stays where you set it.

- **The rides:** each part's level, its echo send and its plate send, the chord's filter and the
  echo's feedback move slowly, each to a new place every 10 to 60 seconds, gliding there over most of
  the wait. They start anywhere in the bar, not on the phrases' lines: about one in eight lands on one,
  as chance would have it (the records' changes about a quarter, DUB-1's own arrangement nearly half).
  How far they go is Mixer's: at 100 a level from −1.5 to −7 dB down and +1 to +3 dB up, never more;
  a send −8 to +6 dB; the filter half an octave either way. The kick moves least (its level only,
  ±1.5 dB): it never leaves the classics.
- **The moves:** now and then (about once a minute at 100, every two at 50), one thing more: a part
  ridden right out and back over a few bars (the pings, the hats or the noise; the chord or the bass
  down 12 dB; the kick only above 75, rarely), or a part's echo send swelled for a bar or two, the
  feedback with it, the repeats left to ring. Half of them start on a phrase's line, half anywhere.
- **Around your settings:** the rides move offsets around the knobs; your values stay as you set them.
  Turn a Level and the ride goes on around the new one.
- **When it rests:** not in a Break or a Thin (Motion's own moves), not in a song's intro or outro
  (the song's; its rides are half as deep there, no moves); **Lock** and dub hold rest it (nothing
  new; what's under way ends). Turned to 0, every ride glides home over four bars.
- **On the screen:** the meters' white notches move with the rides, where each part's level is now; a
  held strip's Level, Echo and Plate knobs show the ride as the LFO's moving knob does: a band from
  your setting to where the ride has it, a white dot there.

![The Arrangement page with Mixer at 100 after a minute: the white notches off their settings, where the rides have the levels now; MIXER 100](img/screen-mixer.png "The dub mixer riding")
![The Chord strip held while the dub mixer rides: LEVEL, ECHO and PLATE each with a light band from the knob's value to a white dot, where the ride has it](img/screen-strip-ride.png "A strip's knobs showing the ride")

**Duck** is the piece's own (it was the Drums page's KNOB 4 until 1.3.0), saved with it: how far
each beat pushes the rest down, letting it back in over a little more than a tenth of a second. A
kick ducks it fully; a beat without one a little less, and less again while the arrangement has the
kick out (a Break), so the music keeps breathing (the ghost duck). The noise is ducked deeper than
the music.

![The Arrangement page: seven rows of green LED segments, CHD, PNG, BAS, KCK, HAT, NOI and MST, each with a white fader marker, the kick's and the master's reaching yellow; under the master, DUCK, its orange segments from the right; WIDTH 40, MIXER 25, WET 0, DUCK 80](img/screen-perform.png "The Arrangement page")

**The meters.** A row a part, top to bottom, as the white keys 1–6: the chord (CHD), the pings (PNG),
the bass (BAS), the kick (KCK), the hats (HAT: the shaker and the open hat, with the rim and the clap),
the noise (NOI), and under them the master (MST). Each row is a meter of LED segments, left to right, from −48 dB to 0 dB, 2 dB a segment:

- **The lit segments:** the part's level as it plays, its peaks: up at once, falling back at about
  20 dB a second. The parts are measured dry, as each goes into the mix (their echo and plate are
  in the master, not in their rows), before the duck; the master as it goes out, after MASTER.
- **A lone segment above them:** the peak held, for a second; then it falls back.
- **The white notch** through a row: the part's level setting (its strip's Level, below; OCT− / OCT+ on
  its page), as a fader's position, 0 at the left, 100 at the right. Turn a level and it moves; the
  dub mixer moves it too, where its ride has the level now. The
  kick's and the hats' are their own faders, 50 in the middle as the piece has them; the drums' level
  (OCT− / OCT+ on the Drums pages) moves both.
- **Colours** (two builds to try): *classic*, green, yellow from −12 dB, red in the top 4 dB, on
  every row; or *the layers'*, each row in its layer's colour (the master white), its top two
  segments amber and red.
- **A muted part's row is dark** (its notch kept, grey); with a **solo** on, the rows soloed out are
  dimmed, the one you hear bright. (The noise has no solo key: a solo quiets it with the rest.)
- **DUCK**, under the master, a row as the others: the duck, as a compressor's gain reduction, eating segments from the
  right with each kick (2 dB a segment): on the bass while it plays (the deepest, the pump you
  hear), else on the rest of the music. Duck at 0, it stays dark.
- **Red at the top of MST** means the output is at full scale: the soft clip is squashing it, and
  a red **!** lights in the header, on every page, for a second (turn MASTER or a level down).

On the Arrangement page, each key says what it did on a card in the middle for a moment (under a second, so the
meters under it show the effect): a mute ("PINGS muted", in the part's colour), a solo, a move as you press it
("THROW: every stab into the echo"). A page's button **leaves the Arrangement page**: the keys are the scale again
(the mutes and solos stay), and SEL brings it back.

**Hold white 1–6** for a part's **channel strip**, as on a mixing desk: KNOB 1–4 that part's
**Level**, **Echo** send, **Plate** send and **Low cut**, until you let go. White 1 the chord, 2 the
pings, 3 the bass, 4 the kick, 5 the hats (with the rim and the clap), 6 the noise: the meters' rows,
top to bottom. The strip comes up after a third of a second held, or at once if you turn a knob; its
name shows at the top right ("KICK STRIP"), its row on the meters is lit behind, its knobs are in
its colour, and the big ring names what you turn ("HATS ECHO"). Let go and you're back on the
Arrangement page. A **tap** still mutes the part: the mute comes as you let go (a hold mutes nothing).
(Until 1.4.0's last builds, white 6 held was the Mix page, the four levels on the knobs: each part's
Level is its strip's now, the noise's too.)

![A channel strip: white 1 held, the chord's: its meter row lit, CHORD ECHO 60 in the big ring, LEVEL 52, ECHO 60, PLATE 50, LOW CUT 0](img/screen-strip.png "A channel strip")

| Knob | What it does |
|---|---|
| KNOB 1 · **Level** | the part's level: the same as OCT− / OCT+ on its page (the noise's too). The kick's and the hats' are their own: 50 as the piece has them (×1), about 12 dB down at 0 and 12 dB up at 100; the hats' is the Hats page's Level (the shaker and the open hat; the rim and the clap go with the drums' level, OCT− / OCT+ on the Drums pages, which moves the kick and the hats too) |
| KNOB 2 · **Echo** | how much it sends into the echo: 50 as the piece sends it, 0 none, 100 twice as much (+6 dB). The bass sends none in every piece (0): turn it up for a dub bass throw. The hats' is the hats', the rim's and the clap's. The kick sends none in every piece (0): turn it up and it goes into the echo too (100 a full send) |
| KNOB 3 · **Plate** | how much it sends into the plate, the same way: 50 the piece's, 0 none, 100 twice (the kick's from 0, as its Echo) |
| KNOB 4 · **Low cut** | a gentle high-pass (6 dB an octave) on the part, before its sends: 0 off, then from about 20 Hz up to about 600 Hz at 100, an octave every 20 or so. The chord keeps its own gentle cut at about 160 Hz under it; the kick's leaves the rumble be (its own knob, on the Kick page) |

![The Kick strip: white 4 held, its meter row lit, KICK LOW CUT 20 in the big ring, LEVEL 50, ECHO 0, PLATE 0, LOW CUT 20](img/screen-strip-kick.png "The Kick strip")

![The Hats strip: white 5 held, HATS ECHO 60 in the big ring, LEVEL 40, ECHO 60, PLATE 50, LOW CUT 0](img/screen-strip-hats.png "The Hats strip")

![The Noise strip: white 6 held, NOISE PLATE 60 in the big ring, LEVEL 8, ECHO 50, PLATE 60, LOW CUT 0](img/screen-strip-noise.png "The Noise strip")

A thrown stab still goes into the echo whatever the chord's Echo, and the Arrangement page's Wet
moves everything on top. The strips are the piece's, saved with it.
A page's button goes to its page (HOME: the Form page). The keys, from the lowest up,
are the parts, an empty white key, then the moves. Each key that does something is lit:

| | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| **Black keys:** solo, the hats' throw | Chord | Pings | Bass | Kick | Hats | Hats throw (held) | | | | | | |
| **White keys:** parts, moves | Chord | Pings | Bass | Kick | Hats | Noise | — | Throw | Open | Shut | Drop | Echo |

The first five black keys (F♯ G♯ A♯, C♯ D♯) are the solos; the sixth (F♯, the next octave's) sits
just above Throw, the hats' throw beside the stabs'. White 7 (E) is dark and does nothing: the gap
between the parts and the moves, so you can find them without looking.

- **White 1–6, the chord, pings, bass, kick, hats and noise:** a tap mutes the part, another brings it
  back (the mute as you let go: held, the key is the part's channel strip, above); lit while it plays.
  The levels stay as they are (a true mute: the strips' Levels don't move). They
  are the same mutes as OCT− + OCT+ on a layer's page (there, the drums' mute is the kick's and the
  hats' together): a mute made either way shows on the other. The mutes stay when you leave the Arrangement page.
  The music goes on underneath: a muted part comes back where it would have been.
- **Black 1–5, solos:** a press solos the layer: the others go quiet, the noise too (their echoes and
  reverb ring out). Solo more than one and they play together; press
  again to let one go. A soloed key blinks; the white keys show what you hear. SOLO shows top right,
  beside the page's name. Solos stay when you leave the Arrangement page, as the mutes do.
- **Black 6, the hats thrown, held** (lit; dark while held): the hats, the rim and the clap go into the
  echo, whatever their strip's Echo, and the echo feeds back a little more; let go and their repeats
  ring on. The kick is never thrown with them.
- **White 8–12, moves, held** (lit; dark while held):
  - **Throw:** every stab is thrown into the echo.
  - **Open:** the music thins and brightens over the kick: a resonant high-pass on everything but
    the kick sweeps up to about 400 Hz, and the chords' filter opens three octaves and sings, the
    echo's band-pass and the pings brighter with it.
  - **Shut:** the music goes under water while the kick keeps pumping: a low-pass on everything but
    the kick sweeps down to about 230 Hz, the chords' filter and the echo's band-pass closing with it.
  - **Drop:** the floor falls away: the kick and the bass out while held (the bass let go at once),
    both back on the next beat when you let go. The kick's mute (white 4) takes it out until you
    press again; Drop is the move you hold for a bar or two and release on the downbeat.
  - **Echo:** the echo pushed: everything into it, feeding back more.
  - Each sweeps in over about half a second; letting go eases back (to where the Filter knob is).
- The other keys do nothing on the Arrangement page; MIDI notes in still change the scale.

**Lock** (SEL held for half a second, on any page; held again, off). The groove stays as it is:
each new phrase keeps the kick's shape, the noise's groove, the drums' and the bass's phrase plan
(its B bars still B: see [The arrangement](#6-the-arrangement)) and the shaker's pattern, and
Evolve's own nudges and moves of the bass's groove rest. The sections still come and go, the
chords still move, and the stabs and the pings play on as Evolve has them. "lock" shows in red
above the page's name, the phrase's eight dots all alike, and a card says it ("the groove kept",
"the groove free"). A tap on SEL is still the Arrangement page. Lock isn't saved: the FM-1 starts with it off.

**The Chords page** (EDIT tapped): the chord machine, in view. The progression is eight slots. Each
slot is one of three:
- a chord from **the walk**, in teal: the random walk over the scale's degrees (see
  [The arrangement](#6-the-arrangement)). It's a degree of the scale, so it follows the key and the
  mode when you change them;
- a chord **you wrote**, in white: as written, whatever the key;
- a **gap**, "(HOLDS)": the chord before it goes on.

| | |
|---|---|
| KNOB 1 · Slot | the slot you're editing (1–8), outlined on the screen |
| Keys | its root, any note (lit on the keys). A walk's chord becomes yours, keeping its kind and type; a gap becomes a minor chord on it |
| KNOB 2 · Chord | its kind: Empty (a gap), Major, Minor, Sus2, Sus4, Dim, Aug. Turning a walk's chord makes it yours |
| KNOB 3 · Type | Triad, 6, 7, Maj7, 9, 11, Add9 |
| KNOB 4 · Play | **Endless** or **Loop** |
| ALGORITHM | the walk's character: **Home**, **Wander** or **Free**. Its chords (the teal ones) are walked again at once, all but the one playing; yours and the gaps stay. The character shows at the left of the knob row |

**Home, Wander, Free** (ALGORITHM; Wander is the default) set how the walk moves, from the most settled to the
most varied. Turning ALGORITHM rewrites the walk's chords on the page straight away, so you see and hear the
change; in Loop it rolls a new loop from the teal slots (turn until you like it, then leave it). **Home** goes back to the home chord often (about two times in three) and leans on the IV, the
way DUB-1 has always moved. **Wander** goes home less, spreads over more of the scale's chords and rarely
bounces straight back to the chord before last. **Free** goes home least and treats the other chords evenly.

**Endless:** a new piece starts with eight chords from the walk, from home. The chord
playing is always slot 1. When it moves on (every Change bars, the Chord page's, or EDIT held), every slot moves up one:
a gap stays a gap, your chords stay yours. The walk then writes a new chord into slot 8, so you see
the chords coming and can change them before they arrive.

**Loop:** the eight in turn, from the one playing, a chord every Change bars, round again after the
last that isn't a gap. Nothing new is written. Gaps at the end make the loop shorter. Switching from
Endless to Loop keeps the eight you see, to catch a progression you like as it goes by. Back to
Endless, the one playing moves up to slot 1 and the walk goes on.

Editing the slot that's playing is heard at once. The slot playing is lit; in Loop the other pages
show "2/4" under the chord's name. EDIT again stays; any page's button leaves the page.
**EDIT held** (half a second), anywhere, moves to the next chord. The slots are kept with the piece.

**The effects grid** (REC held). After a moment the whole screen shows sixteen effects, four rows of four (the knobs do nothing while it's up);
the white keys from the lowest play them, each as long as its key is held (even after you let go of
REC). The first key of each row is lit, to find the rows by touch.

| Row | Key 1 | 2 | 3 | 4 |
|---|---|---|---|---|
| Echo (white 1–4) | **Throw**: every stab thrown into the echo | **Time x2**: the echo's time doubled (halved if the tape is too short), gliding there and back | **Tape stop**: the echo's tape slowing to a quarter, its pitch diving | **Swell**: everything into the echo, feeding back more |
| Filter (5–8) | **Low**: a low-pass on everything, sweeping down to 350 Hz | **High**: a high-pass, up to 1.3 kHz (thin, a radio) | **Wah**: a resonant band sweeping, 1.3 times a second | **Phone**: a narrow band, 450 Hz to 3 kHz, overdriven |
| Space (9–12) | **Verb**: everything into the plate | **Crash**: a dark burst of noise into the plate, eased in, a splash at the music's own level | **Freeze**: the cloud stops listening, its grains thick and brought forward | **Wash**: the dry down, everything into the echo and the plate |
| Rhythm (13–16) | **Stutter**: a sixteenth of the mix caught and looped | **Gate**: the chord ringing on, chopped in sixteenths | **Siren**: the dub siren, into the echo | **Riser**: noise rising through a band-pass, louder as it climbs |

A tap on REC alone is still dub hold; held long enough for the grid (or with a key played), its
release does nothing. Each effect eases in and out. All sixteen at once is the heaviest thing DUB-1
can do: in a heavy piece it can approach the FM-1's limit (watch the Setup page's CPU).

**The nudge** (ENV, ARP, LFO or SEQ held for half a second). The layer's sound takes a step somewhere new:
three or four of its sound values move, each up to an eighth of its range (up to 12 on a −50…+50 value), like a
random walk. The walk is pulled back to the piece's recipe: the further a value has gone, the likelier its next
step goes back (a quarter of the range away, always), so nudging again and again explores round the piece's
sound and never wanders off. A card in the middle, outlined in amber, says the biggest change
("CHORD NUDGE", "Resonance −7"), and the page with that knob comes up, the knob lit (the layer's voice
page, or its FM page, the Hats page, the Noise page: wherever it moved most; on the Arrangement page,
you stay where you are).

**The shake** (keep holding, 1.5 s): the wild one. The layer gets **another voice** (the drums another kit;
the chord's Noise is left to ALGORITHM) and every one of its sound values takes a big step. A red card says the
new voice ("CHORD SHAKE", "Keys").

**Shake everything:** hold **HOME** for 1.5 s (without turning anything) and the chord, the pings, the
bass, the drums and the noise all take a shake at once, one card for all ("SHAKE ALL"); you stay on the
page you're on. Do it again and again to explore the piece's variations until one sounds right, then
save it (GLO held) or keep it with Lock (SEL held).

| Held | Nudges |
|---|---|
| ENV | the chord: Attack, Colour, Reso, its Decay and Cutoff a little; on an FM voice, its ratio and feedback (the page shown: Chord voice, or Chord FM when Colour or the FM moved most) |
| ARP | the pings: Tone, Ratio, Length, Bend (an FM voice: its upper ratio and feedback); now and then one note of one loop (the page shown: Pings voice, or Pings FM) |
| LFO | the bass: Bite, Shape, Gate, Glide, the Fifth (an FM voice: its ratio and feedback; the page shown: Bass voice, or Bass FM, or the Bass page for the Fifth); its shake also moves the **groove** to a neighbour (Drone, Offbeat, Dotted, Sync, Roller, in that row) |
| SEQ | the drums: the kick's tune, length and punch, the hats' tone and level (the page shown: Kick, or Hats) |
| SAVE | the noise: Tone, Density, Decay, Air (the page shown: Noise voice, or the Noise page for Density); its shake: another noise character |
| FX | the Dub page's: the echo's send and feedback (smaller steps), the reverb, the plate's size, the cloud; its shake: a new echo time |

Saving (GLO held) keeps what you like; GLO held on (1.5 s) puts the piece back to its recipe. Evolve nudges by itself too (the Form
page, below), but never shakes.

**The cards.** Every long press says what it did on a card in the middle of the screen, for about a
second: a nudge (amber) or a shake (red), the layer and what changed; EDIT held, NEXT
CHORD (gone the moment the chord changes); EDIT held on, NEW CHORDS (red); GLO held, SAVED (teal);
GLO held on, RESET (red); SEL held, LOCK. Evolve's own nudges show no card.

![A nudge: an amber card in the middle, CHORD NUDGE, Resonance −7; behind it the Chord voice page, RESO lit at −7](img/screen-nudge.png "A nudge")
![Lock: a card in the middle, LOCK, the groove kept; "lock" in red above FORM, top right](img/screen-lock.png "Lock")

---

## 4. The screen

![The main screen on the Form page: Basic, F Dorian, tempo 122; FULL and the phrase's eight bar cells top left, bars 4 and 8 ticked, the line to the next chord under them; FORM top right; the ripples with the chord's name in the middle, the seven lanes below, each with its level meter at the left; ENERGY 0, EVOLVE 55, MOTION 30 (its ring marked at 40 and 70), the fourth knob free](img/screen-main.png "The Form page (HOME)")
![A Groove phrase: GROOVE lit top left, fewer stabs in the top lane; MOTION 80](img/screen-groove.png "A Groove")

- **Top line:** the piece in a tag, always (its key is on the Key page, HOME again). On the right, a beat light (a play triangle
  while resting), the tempo (EXT when it follows MIDI clock), a dot when there are unsaved
  changes, and the battery; left of the beat light, a red **!** for a second when the master clips
  (the output at full scale: the Arrangement page's meters show where; it is not Setup's grey "!n",
  the audio's dropouts, beside the CPU's load); three bars (the outline red when it's nearly empty), or the bars filling
  in turn while USB powers and charges it. Messages ("ADDED F#", "NEXT CHORD ON THE BAR", "DUB HOLD ON", "KICK OFF") show here for
  a moment; a knob turning or a level changing doesn't: its big ring names it.
- **Top left:** the section playing (FULL in grey; BREAK, GROOVE and THIN lit up; in a song, INTRO and OUTRO) and the phrase's
  eight bars, a cell each, lit up to the bar playing, the one playing in teal. Bars 4 and 8, the B
  bars of the phrase plan (see [The arrangement](#6-the-arrangement)), have a tick over them and are
  brighter, ahead and played; under Lock all eight are alike. On the phrase's last bar, cell 8 blinks
  when the next phrase brings a change: a new plan, another kick shape or another section. Under the
  cells, a thin line fills up on the way to the next chord (none when the Chord page's Change is Hold, or in dub hold).
  Under that, while a song plays (or an outro is coming), the **song line**: the whole song as a thin line, the intro
  and the outro brighter at its ends, a tick where the main starts and where the outro starts, and a dot where it is.
- **Top right:** the page's name, in teal (ARRANGEMENT on SEL's page). SOLO, in yellow, to its left
  while a solo is on. Under it, in grey, its place among its button's pages (**2/3**: the second of three), and to
  its left, in red: "hold" in dub hold, "lock" under Lock.
- **The ripples:** each stab drops a ring in the middle that spreads out to the screen's sides (since
  1.4.0: the whole width, behind the section's word, the page's name and the chord, above the lanes)
  and fades. Its outline is the
  sound itself at that moment, its wave wrapped round the circle: a soft pad a smooth ring, a
  bright stab a jagged one. The echo's repeats follow it, a ring each echo time, each as strong as
  the echo's output really is then (measured, not worked out), and a little smoother each time
  round, as the tape darkens them. So a long Feedback makes long trains of rings, a short one a
  single ring, and a filtered, ducked or dropped echo shows as it sounds. A stab thrown into the
  echo makes a thicker, brighter ring. In dub hold the rings turn red, and a new stab makes a
  single ring (the echo plays what it holds). Each bass note drops a gold ring too (no repeats:
  the bass hardly goes into the echo), so with the chord muted the bass still shows. The chord's
  name is in the middle, a soft grey that flashes white with each kick, as strong as it hits, on every page that shows it (red in
  dub hold). A new chord comes in big and teal, bouncing big, small, big, small, and settles to white
  over about a second. Around it the four ping loops each circle as a blue dot, one step a turn's step: its
  place is where the loop is, and it lights up when the loop plays a ping (dim, a ghost, between).
  The loops have different lengths, so the dots drift apart and meet again.
  Under it all, the noise: a grey speck for each of its hits you can hear, and for each dry click
  (on its side), as bright as it is loud, gone in a moment; a strong hit a brighter one. More
  Level or Density (the Noise page), more specks. The specks show on the whole piece's ripples
  and on the Noise pages (there alone), not on a layer's page.
- **ALGORITHM** (on any page but a layer's, the Noise page or the Chords page) changes what the middle shows: the ripples, a **scope**, a
  **spectrum**, and then three more, new in 1.4.0: the **echo trail**, the **fifths**
  and the **tape**; then the ripples again (turned back, the other way round). Each piece keeps its own view, saved with it: Haze on the spectrum, say, and Pressure on the scope. With the
  scope, the spectrum, the echo trail or the tape showing, the chord's name moves to the top.
- On a layer's own page (Chord, Pings, Bass, Drums) and its voice pages, the middle shows **that layer alone**: the ripples its notes, in
  its colour (the stabs on the Chord page, the pings with their loops' dots on the Pings page, a ring a bass note, a
  ring a drum hit), and the scope or the spectrum its sound, dry. Below it, that layer's own lane, large (below), with the
  seven meters at its left, the layer's own bright and the others dimmed. The Noise pages
  follow it too, with the noise alone: its specks, or its scope or spectrum. Form, Dub, Echo and the other pages show the whole piece (the Arrangement page shows its meters).
  - The **scope** draws the output (left and right mixed) as a thick line, nearly the screen's width. Each trace starts on a
    rising zero crossing so the picture stands still, and the two traces before it fade behind.
    Its height follows the level, so a quiet passage still fills it.
  - The **spectrum** splits the output into 18 bands, from the sub bass (43 Hz, left) to the hats
    (11 kHz, right). Each band is a column of segments: teal, then brass, amber and red at the
    top. The columns jump up with the sound and fall back slowly. A white cap stays at each
    column's peak for a moment before it drops. The columns span nearly the screen's width.
  - The **echo trail** draws the echo as tape running from the head at the right
    edge to the left: each note a bar as tall as it struck, its repeats the bars after it, an echo time
    apart (the dotted columns), each as tall as the echo really is when it comes back (the ripples'
    own measure). A long Feedback leaves a long train, a short one a single bar; a throw is in the
    accent colour.
  - The **fifths** draws the twelve notes round the circle of fifths, C at the top:
    the key's notes a dot, the others a point. The chord's notes are lit and joined, so you see its
    shape; its root is the biggest, white with each kick. The last few roots leave a trail behind it
    (where the walk has been), and the dots round the outside fill up on the way to the next chord
    (Change).
  - The **tape** draws two reels, the tape running from the left one to the right past
    three heads: the record head lit with the output, the play head with the echo coming back. The
    reels turn with the tempo, the smaller one faster. Through a song the tape winds from the left reel
    to the right one, so the reels show where the song is; in Endless it winds back and forth over 128
    bars. Its counter shows what no other screen does: at the left, where the song is (INTRO, MAIN,
    OUTRO, or ENDLESS) and the bars left of that part; at the right, the time since PLAY (minutes and
    seconds) and, in a song, its whole length at this tempo ("OF 7:20").
- The levels in full and the duck are on the Arrangement page (SEL), on its meters; the other pages
  show a short meter at each lane's left (since 1.4.0; the duck meter is the Arrangement page's).
- **The lanes** (Form, Dub, Echo, Setup, Key): the bar as it plays, sixteen steps, seven rows, top to
  bottom: **CHD** the stabs (teal), **PNG** the pings (blue: the sixteenths a loop's note falls on
  this bar, those played and those to come), **BAS** the bass (brass), **KCK** the kick (amber),
  **HAT** the hats (white: the shaker and the open hat), **R/C** the rim (pink) and the clap
  (yellow), side by side when both hit one step, and **NOI** the noise's hits this bar (grey, a
  ghost dimmer). A hit that only sometimes comes (the clap in Auto, a lane's Chance under 50%) is a
  ring, filled when it plays. The step playing is lit. A layer that's out (in a Break, muted on the
  Arrangement page, or at level 0) has an empty row at once. In the stabs' row, rings show where the
  echo's repeats land, fainter for each, as the echo's time and Feedback have them; each flashes white
  as the playhead reaches it.
- **The meters:** at each row's left, a short level meter in the row's colour, over a dim track:
  the chord, the pings, the bass and the noise as the Arrangement page's meters measure them (−48 to
  0 dB), the kick, the hats and the rim and clap from their own hits, jumping up with each and
  falling back (the R/C meter yellow when the clap was the last). A glance at the mix. A muted part's
  meter is dark; soloed out, dimmed (the kick and the hats each their own: the rim and clap follow the
  hats). On a layer's pages and the Noise pages the same meters stand in the same place, without the
  notes, the page's own bright (the Drums pages: KCK, HAT and R/C) and the others dimmed; its large
  lane to their right.
- **Bottom:** a rule (on a layer's page, the voice's name at its left: the Noise page's character,
  the Chords page's walk, the Chord LFO page's Destination, TO CUTOFF), then the page's four values, one for each knob: a dial (or, for a
  switch, its positions as dots) and the value. The one you're turning lights up, and while it
  turns, the rule says what it does ("HOW LONG THE REPEATS GO ON"); a moment after, the rule is
  back. A grey knob does nothing here (on an FM page, the voice not FM, the Chord FM page's Colour
  still turning; or Ratio on a two-operator algorithm).
- **The cards:** a long press's card, in the middle (see [The cards](#3-controls)).
- **The big ring:** a knob turning, in the middle: its value large inside, its name over it in large
  capitals, in its layer's colour (teal elsewhere; the page's name steps aside for it), and under the
  ring, in one or two lines, what it does; a 0–100 knob fills from the start, a centred one from the top, a
  switch shows its choice by name; Motion's ring has its regions marked at 40 and 70. SELECT's filter
  shows there too (the amount, Shut or Open), and OCT− / OCT+'s level (see [Controls](#3-controls)),
  and ALGORITHM's choice (a voice, a kit, the noise's character, the chord's LFO's Destination, the screen's
  view). One ring at a time,
  the last one turned; it fades a second after; a new page clears it.
- **The Bass page's lane:** the bass alone, large, under the ring: this bar's notes as bars as long
  as they hold (an octave note higher), the groove's other notes (those Density leaves out, or a B
  bar's) as outlines; at its left, A and B, the phrase's two bars (B: bars 4 and 8), the one playing lit. Turning Groove or
  Density shows there at once, under the big ring.
- **The Chord page's lane:** the chord's stabs alone, large: each a bar as tall as it was struck (those
  still to come this bar at a middle height), its tail fading for as long as it rings, the echo's
  repeats as rings where they land, as faint as they are; a thrown stab the tallest, in a white frame,
  its repeats brighter.
  Turning Density shows there at once.
- **The Pings page's lane:** the four loops, a row each (13, 17, 23 and 29 eighths, each across the lane,
  so you see them drift against each other), each note a dot at its pitch, the step each loop is on lit,
  brighter as it sounds. Turning Density shows there at once.
- **The Drums page's lane:** what its knobs move: the kick, a row (orange: a ghost dimmer), and the
  hats, a row (white: the shaker filled, the open hat an outline), all the same height; A and B at the
  left. Turning Density or Hits shows there at once. (The whole kit is on the Pattern page.)
- **The Noise pages' lane:** its groove, two bars of sixteenths, a row each (1 above 2), each hit a block
  as strong as it is (a ghost lower), its ring a tail fading as long as it rings (Decay); 1 and 2 at the
  left, the bar playing lit. A new groove (Density, a phrase's start) shows when it's drawn; the dry
  clicks come at random and aren't shown. (The noise's LFO and drift show on the Noise LFO page.)
- **The LFO pages' middle:** the LFO's path as you heard it, the last 16 bars, scrolling from right to
  left: the present at the right with a dot (white while playing), the older part fainter. The faint
  line in the middle is the knob's own value (the chord's: its Destination's knob, Cutoff, Reso or
  Colour; the noise's filter half open), with the bars ticked on it; the dotted lines above and below
  are the reach of the wave and the drift together. What moves it is written over it ("4 BARS",
  "4 BARS + DRIFT", "DRIFT"), and, the chord's, where it goes ("4 BARS + DRIFT ON CUTOFF").

![The Dub page with FEEDBACK lit as it turns, set to 6.7 s, the big ring in the middle saying HOW LONG THE REPEATS GO ON; ECHO 55, ECHO TIME 1/8., REVERB 35](img/screen-turning.png "Turning Feedback on the Dub page")
![The scope: the output as a thick teal line over a dotted centre line, the two traces before it fading behind; Fm7 at the top](img/screen-scope.png "ALGORITHM: the scope")
![The spectrum: eighteen columns of teal and brass segments, a white cap over each; Fm7 at the top](img/screen-spectrum.png "ALGORITHM: the spectrum")

![The echo trail: the stabs as bars from the head at the right edge, their repeats an echo time apart, smaller as the echo dies; Fm7 at the top](img/screen-echo-trail.png "ALGORITHM: the echo trail") ![The fifths: the circle of fifths, the key's notes as dots, the chord's notes lit and joined, its root the biggest, Fm7 in the middle](img/screen-fifths.png "ALGORITHM: the fifths") ![The tape: two reels, the tape from the left one past three heads to the right one; Fm7 at the top](img/screen-tape.png "ALGORITHM: the tape")
![Dub hold: red rings, the chord's name red, and "hold" in red above FORM, top right](img/screen-hold.png "Dub hold")
![The Arrangement page with a mute and a solo: PNG dark (muted), the bass soloed (SOLO top right), the other rows dimmed but the noise, which a solo leaves playing](img/screen-arrangement-solo.png "The Arrangement page: a mute, a solo")

---

## 5. The layers

DUB-1 plays four layers from one scale and one chord, on a grid of sixteenths.

- **Chord:** the stab. Four voices struck together, each two saws a few cents apart, or an FM
  electric piano (**Keys**), through a resonant low-pass that opens with each stab and closes
  again, then a soft clip. The stabs fall on a Euclidean pattern over the bar, off the beat. One
  voicing (an open minor 7th, root, fifth, seventh and tenth; or a ninth without its fifth) is
  shifted whole to each new root, the way a chord memory does, its root kept between F#2 and F3. Much of
  the stab goes to the echo (its Wet, the piece's, sets the dry against it). Every so often (**Change**, on the Chord page) the chord moves to another degree of the
  scale: in a seven-note scale, most often the fourth, then the seventh, the fifth and the sixth; away from home, back
  home about two times in three. It never moves to a half-diminished chord.
- **Pings:** short FM blips played by four loops 13, 17, 23 and 29 eighths long. Each loop holds a
  few notes of the chord (now and then a passing note, or a note an octave up), each in its own
  register, place in the stereo field and shade of tone, so the loops slide against each other.
  They go mostly into the echo.
- **Bass:** a sub on the chord's root, sometimes its fifth, in the sub's band (E1 to Eb2 at home),
  on one of five grooves: Drone (one long note a bar), Offbeat (the four off-beat eighths), Dotted,
  Sync and Roller, all its notes or fewer (**Density**), in the phrase plan (see [The arrangement](#6-the-arrangement)). It slides briefly from note to note. It stays dry, out of the echo.
- **Drums:** a kick (a sine falling fast onto the key's root, F1 to B1, 44 to 62 Hz; below F its fifth; with a click), a
  shaker on the sixteenths, an open hat off the beat, and a **rim** (two short tones and a click,
  mostly sent into the echo, so it echoes away: the dub snare), on two and four once Hits is 70 or
  more; and a **clap** (three quick bursts of noise and a tail, into the plate), rarely: on two or
  four now and then, about one chance in twenty (a little more with Evolve), in Full and Groove phrases.
  The kick plays a **shape** a phrase at a time: none, the downbeat alone, one and three, four with
  two and four ghosted (10 dB down), or four on the floor. **Density** (the Drums page) is where the
  shapes centre; at 100 it's four on the floor, always. The kick ducks everything else (**Duck**,
  the Arrangement page's KNOB 4), and stays out of the echo; the hats send a little to it. On a beat with no kick the music still
  breathes, ducked a little less (less again while the kick is out, as in a Break): the **ghost
  duck**, so the pumping goes on whatever the kick does. The drums play in the phrase plan too.

Then the space: the tape echo (up to 5.6 seconds, synced to the tempo), a granular **cloud** and
Dattorro's plate reverb. The cloud records what goes to the reverb (the last 0.74
seconds, at a quarter of the sample rate) and plays it back in grains: up to six at a time, each
80 to 250 ms from anywhere in what it holds, at a random place in the stereo field, now and then
an octave down (rarely up). The grains go into the plate, so they bloom into its tail, and the
plate's tail goes back into the cloud: with **Cloud** (the Echo page) up the reverb turns into a moving wash, longer
with a bigger plate. The grains come thicker as the filter opens, and in a Break there is more of
the cloud. Dub hold freezes the cloud as it does the echo.

**The echo dissolves.** What goes into the tape is band-limited first (**Tone**, on the Echo page),
and the first repeat comes back clean, a copy of the stab on its time. Each repeat after it goes
through a chain of allpass filters on its way round (**Blur**): every repeat comes back softer and
darker than the one before, until it melts into a wash. Each new stab pushes the returns down
(by up to 6 dB, back over about 150 ms), so the repeats are heard in the gaps between the stabs,
not over them; a thrown stab's repeats ride over. In the feedback, a band-pass also makes each repeat thinner and
darker, and moves with the arrangement. **Feedback** (the Dub page) is how long the repeats go
on, in seconds: the loop's gain follows the echo's time, so a long echo dies away as soon as a
short one. A throw, the Arrangement page's Echo key and the grid's Swell push it further; a soft clip lets the
feedback pass 100% then, so the echo swells on itself without running away. The echo plays from
one head, its right side a few milliseconds behind its left (**Width**, the Arrangement page's: 8 ms at 50), so the
repeats spread wide without changing their rhythm; a second head, at three quarters of the loop,
joins in only while a thrown stab rings, to the right. With **Motion** up,
the heads wander across the field, and every so often the second head slides to another part of
the loop (two thirds, a half, three eighths, a quarter), bending its repeats in pitch on the way.
The stabs and the pings are widened too, each side taking a little of the other's sound a few
milliseconds later and slowly drifting (with Width), so the two sides differ without the voices
being panned further. Under about 80 Hz the mix stays mono.

Under it all, **the noise** (the Noise page): a crackle groove that rings on the chord, and a quiet
hiss (see [SAVE: Noise](#save-noise)). It plays only while the music plays, fading in and out over
about half a second.

**Dub throws:** now and then a stab is thrown hard into the echo, which pushes the feedback up
for a moment: the repeats come back louder, and the stab's dry is 6 dB down, so it's the echo you
hear. **Throws** (the Chord page) sets how often: at 50, about one stab in eight; at 100, about one
in three; at 0, none but the last stab of a fill, which is always thrown. There are more in a
Break. Evolve lets them in: at 0 there are none, and from 40 up Throws has them all.

**Evolve** also keeps the patterns moving and the playing loose: every two bars the stab pattern
may turn a step or gain or lose a stab; stabs drop out of a bar, and now and then a whole bar has
none, the echo carrying it (about one bar in five at Evolve 40, never in a Break); each stab has
its own cutoff, a little darker or brighter; at a phrase's start the shaker's steps may trade
places or gain or lose one, and the drums' and the bass's plan is drawn again more often (in Auto,
more of the hats rest, and they lean later; the bass gets an octave note more often); a ping
changes note or moves at the end of a pass, a ping loop sits a pass out, and when the chord moves
it may change voicing. It also widens how far the kick's shape wanders from Density. As Evolve
goes to 0 all of this tightens: at 0, every stab plays every bar, the same cutoff, no throws, and
the phrase plan stays as it is.

---

## 6. The arrangement

DUB-1 plays in phrases of eight bars. Each phrase is one of four sections:

| Section | What plays |
|---|---|
| **Full** | Everything. The filter sweeps somewhere between a little dark and open. |
| **Break** | The kick, the bass and the shaker out (the open hat stays only while Motion is under 50); the music still breathes on the beat, lightly. The filter goes dark, then from the third bar opens up over two bars or so. Throws are more likely. The noise's hits fewer, ringing longer. |
| **Groove** | Every other stab of the pattern; the filter in the middle. |
| **Thin** | No bass, no shaker; the kick's shape a step sparser. The filter a little darker. The noise's hits fewer, ringing longer. |

In the last bar of each phrase, the next one is chosen. **Motion** has three regions (marked on its
ring at 40 and 70):

- **Under 40: the loop holds.** Full only, phrase after phrase, the kick never leaving; the music
  moves by its filter, its echo and its texture, as the classic records do.
- **40 to 70: variations.** Now and then a Groove or a Thin for a phrase or two, more often as
  Motion rises; the kick half-time at most, never gone.
- **70 and over: Breaks too.** A Break now and then (the kick and the bass out for a phrase).

Between two departures there are always at least four phrases (32 bars) of Full. From any other
section it goes back to Full three times in four. Energy (the Form page) and the Arrangement page's Drop break it by hand at
any Motion.

**Fills:** the last bar before the kick comes back from a Break is always a fill; from Motion 40 a
phrase ends with one when the section changes, and now and then otherwise (more often with Motion
up); under 40, never. In a fill
the kick (if it's playing) drops out for the second half of the bar, the open hat plays (if Hits
gives it any steps), and the last stab is thrown into the echo.

**The sweep:** the arrangement moves a slow filter sweep, from dark to open, and Motion sets how
far it goes (at 0 it stays put). It opens the stabs' filter (up to an octave and a half either
way) and its resonance, moves the echo's band-pass, thins the pings in the dark, moves the noise's
tone and takes a little off the reverb when it's open.

**The kick, a few phrases at a time:** the kick takes a shape (none, the downbeat, one and three,
four with two and four ghosted, four on the floor) and keeps it for two to six phrases (fewer as
Evolve rises), drawn round the Drums page's Kick: the nearer a shape is to it, the likelier. On
Four it's four on the floor in every Full or Groove phrase, ghosted or half in a Thin. Turning Kick
takes effect from the next bar. Under Motion 70 it never goes silent in Full. A piece starts on four on the floor
for its first three phrases, and after a Break the kick comes back whole. Evolve lets the shape
wander further; Energy (the Form page) moves it. With the Pattern page on Pattern, your kick lane plays instead. The
shape is chosen in the phrase's last bar (dot 8 blinks if it changes), and Lock keeps it.

**The phrase plan** (the drums and the bass): a phrase plays **A A A B A A A B**. A is the main
bar, the same every time; B, on bars 4 and 8, is a variant, drawn once a phrase, either
**sparse** (the kick on the downbeat alone, the shaker on the eighths, no open hat, the bass on
every other of its notes) or **busy** (a ghost kick just before the next bar, the hats all in with
a pickup, the bass's whole groove and a pickup into the next bar). At Density 100 the kick stays
four on the floor in a B bar too. Which of the hats' and the bass's notes rest (Hits, Density and
Evolve's looseness), the hats' feel (each step's accent and how late it comes) and, now and then,
one bass note an octave up are drawn once a phrase too, the same every bar. At a phrase's start
the plan is kept (a motif that comes back) or drawn again: more often as Evolve rises (at 55,
about half the time; at 100, four times in five), never at Evolve 0. Lock keeps it. The screen
marks the B bars (dots 4 and 8, brighter) and blinks dot 8 when the next phrase brings a new plan.

### Songs: an intro, the main, an outro

![A song's intro on the Form page: INTRO top left, and under the bar cells the song line, its dot near the start](img/screen-song-intro.png "A song's intro")

DUB-1 plays for ever, starting full, as it always has: that's **Endless**, the Song mode it starts in.
Set **Song** (GLO again, the Song mode page) and a piece becomes a song: PLAY starts its **intro**, the
parts coming in one by one; then the **main**, the piece as Endless plays it; then the **outro**, the
parts leaving; then silence, the last echoes ringing out, and PLAY goes off by itself. **Set** plays
the same, then the next piece starts from its intro as the tails die (about six seconds on), an album
from the piece you're on to the last (Haze); after it, DUB-1 rests. Song mode is the player's, as
Tune: the same for every piece, and saved.

How long each part is, and how the intro goes, is the piece's own, on the Song page (HOME again):
the **Intro**, the **Main** and the **Outro** in bars, a phrase (8 bars) a click, and its **Kind**:

- **Ambient** first (Fog, Haze, Chamber, Night Drive): the noise from the first bar, then the chord's
  wash, then the kick, sparse, then the bass, the pings, and the hats last.
- **Pulse** first (Steppers, Pressure, Concrete, Basic): the kick, sparse, and the bass from the
  first bar, then the chord, the pings, the noise, and the hats last.

**In the intro** each part comes in at its place in that order, spread over the intro and not on the
phrase lines (a bar or so either way), each fading in over two to four bars. The kick plays the
downbeat alone, then one and three, then four on the floor by the intro's end; the hats come in last
with a few of their hits, more each bar, to the piece's own. The chord's filter starts about an
octave darker and opens to its Cutoff, and the echo is a little wetter at first, so the wash leads.
There are no Breaks, Grooves or Thins in the intro: the arrangement starts with the main, the kick on
four for its first phrases as a piece's opening always is.

**In the outro**, the mirror: the hats leave first, then the pings and the bass; the kick thins (one
and three, then the downbeat alone) and stops; the chord and the noise stay longest, and for the last
bars every stab is thrown into the echo and the noise goes into it too, fading, so the song ends on
the echo's tails and the hiss, then silence. No sections in the outro either.

**Outro now:** hold HOME and press PLAY: the outro starts from the next phrase, in any mode. In Endless
too: a way to land any piece by hand (it plays the Song page's Outro, then rests). Again, in the outro
or while it waits: back to the main (in a song, for four phrases at least, then the outro as planned;
in Endless, on as ever). A card says which.

The knobs work as ever during a song: what you turn is kept, and the song's moves are around it (each
part's fader, the filter's and the echo's offsets), never on its values. Turning Main as it plays moves
the outro (if it's already past it, to the next phrase); turning the Song mode to Endless lets the song
go, the parts back in, playing on. A song's lengths with the defaults (32, 160 and 32 bars) make
about 7 minutes at 122 BPM, the intro and the outro about 29% of it (the records' run from 3% to 37%).

---

## 7. The pieces

PRESETS chooses one of eight pieces. Each is a recipe for the whole machine: the layers, the
sounds, tempo, key and mode, the groove, the space, how busy it is and how far it moves. Each
remembers how you left it. Hold GLO for 1.5 s to put the piece you're on back to its recipe.
Tune, MIDI out, Lights and the Song mode are the same for all.

| Piece | What it is |
|---|---|
| **Basic** | F Dorian, 122, after Basic Channel's *BCD*: held, saw stabs, short, into a clean dotted-eighth echo, the dry forward, almost no hats, narrow and dark, the kick on the floor; a new chord every 64 bars. |
| **Chamber** | A minor, 128, after Fluxion's *Vibrant Forms*: one chord that never moves (Change on Hold), warm organ stabs up front, a long half-note echo, a drone bass, an even four, the steadiest loop, the noise ringing on the key (Room). |
| **Steppers** | D Dorian, 126, after Maurizio's *M-Series*: the most frozen loop, one chord held, short saw stabs, a dotted bass, the open hat on the four "ands", real swing, clean. |
| **Fog** | G minor, 120, after Echospace's *The Coldest Season*: pad stabs lost in a quarter-note echo and a big plate, very wide, hiss (Tape), offbeat hats, a drone bass, a kick that comes and goes, a rare Break; the chord moves every 64 bars. |
| **Concrete** | C pentatonic minor, 136, after Monolake's *Hongkong*: the fastest. Chords stacked in fourths, short bright noise stabs, glassy pings over them, a rolling bass, loud hats, a clean dotted-eighth echo, deep filter sweeps, a vinyl crackle. |
| **Night Drive** | Bb Dorian, 120, after Intrusion's *The Seduction of Silence*: FM keys, more pings, an offbeat bass with a deep sub, a dotted-quarter echo, very wide, a filter that's played; variations now and then; a new chord every 16 bars. |
| **Pressure** | E Phrygian, 126, after Porter Ricks' *Biokinetics*: it never breaks; the kick and the bass forward, one chord held, a wide tape-noise bed under it, wide. |
| **Haze** | Eb major, 116: the sparse intro. Pad stabs, the longest, the most echo, pings an octave up and the most of them, a kick that comes and goes, dry and dusty (Dust), wide; variations, no Breaks. |

The pieces' echo tails run from about 4 to 8 seconds.

---

## 8. The pages

One page a part, each on its own button, with four values on KNOB 1–4. The buttons sit on the panel
as Dub (FX), Chord (ENV), Bass (LFO), Setup (GLO), then Form (HOME), Noise (SAVE), Pings (ARP),
Drums (SEQ). This chapter takes them by layer.

Each layer has the same pages, its button pressed in turn (DUB-1 1.4.0): first **its part**, what it
plays and how that changes (its Density, how many stabs, pings, bass notes, kicks or noise hits, and
its **Vary**, how often its part changes); then **its voice**, how it sounds; then, for the chord,
the pings and the bass, its **FM page**; then the chord's and the noise's LFO. A knob the layer
doesn't have is left empty, so the same thing sits in the same place on every layer.

| Layer | Its part | Its voice | Then |
|---|---|---|---|
| Chord (ENV) | Density, Throws, Change, Vary | Chord voice: Cutoff, Reso, Attack, Decay | Chord FM, Chord LFO |
| Pings (ARP) | Density, Vary, —, — | Pings voice: Octave, Tone, Length, Bend | Pings FM |
| Bass (LFO) | Groove, Vary, Density, Fifth | Bass voice: Octave, Shape, Gate, Glide | Bass FM (its Bite) |
| Drums (SEQ) | Kick, Hits, Vary, — | Kick: Tune, Length, Punch, Rumble | Hats: Level, Tone, Length; Pattern |
| Noise (SAVE) | Density, Vary, —, — | Noise voice: Air, Tone, Decay, Width | Noise LFO |

**Vary**, on every layer's part page, is how often that layer's part changes, with Evolve (the Form
page) over all of them. At 50, every piece's, each changes as Evolve alone changed it before; down to
0 less and less, and at 0 never, at any Evolve (the part held as it is); up to 100 about three times
as often. What changes is each layer's own: the chord's stab pattern, the pings' loops (a note moved,
a pass sat out), the bass's plan (its phrase's figure, the octave note), the drums' (how long the
kick holds its shape and how far it wanders, the shaker's steps trading places, the clap), the
noise's groove (drawn anew, or kept).

The layers' **levels** aren't on their pages: OCT− / OCT+ set them on each layer's pages (the
noise's on the Noise pages), 5 a press, and both together mute it (see
[Controls](#3-controls)); the Arrangement page's meters show them all, and each part's level, its sends and its
Low cut are its **channel strip** there (white 1–6 held: the kick and the hats each their own).

### The voices

On a layer's page, ALGORITHM steps through that layer's voices (after the last, the first again).
The voice's name shows at the left of the knob strip, and a message says it ("VOICE Organ").
Each piece keeps its own.

| Page | Voices |
|---|---|
| Chord (ENV) | **Saws** (two detuned saws a voice), **Keys** (an FM electric piano), **Organ** (four drawbars, longer), **Pad** (the saws swelling in, long), **Bell** (FM at 3.5, the clang dying first), **Tonewheel** (a second organ: drawbars, a quick percussion and a key click at each strike, a rotary speaker turning left and right), **Noise** (noise through the resonant filter, a little saw under it) |
| Pings (ARP) | **Blip**, **Marimba** (a wooden bar), **Glass** (long and bright), **Wood** (a knock), **Drop** (the pitch falling from an octave up) |
| Bass (LFO) | **Sub** (a sine and a little of its octave), **Punch** (FM, four operators stacked: a thump in the first 90 ms), **Rubber** (FM, two pairs, one an octave under, op 4 feeding back), **Pluck** (FM at 3, gone in 50 ms), **Growl** (FM at 1: a bite at each note) |
| Drums (SEQ), the kit | **Tight**, **Deep** (long, 808-ish), **Punch** (short and hard), **Soft** (a muffled thump, no click), **Dusty** (darker, noisier hats) |

The voices are matched in level, so changing one keeps the mix.

### The FM pages

Keys and Bell (chord), every ping voice, and Punch, Rubber, Pluck and Growl (bass) are **FM
voices**: sine operators modulating each other, as in the FM-1's own firmware and the 4-operator FM
synths. The layer's button pressed a third time (after its voice page) opens its **FM page**. On
a voice that isn't FM (Saws, Organ, Pad, Tonewheel, Noise; Sub) the page says so and names the FM
ones: turn ALGORITHM there to pick one. Until you do, Algo, Ratio and Feedback are grey and do
nothing when turned (what you set on an FM voice is kept for when you come back to one). The
fourth knob is the chord's **Colour** on the Chord FM page and the bass's **Bite** on the Bass FM
page: every voice's, so they turn whatever the voice ("IS NOT FM: ITS COLOUR ALONE", "ITS BITE
ALONE"). On the Pings FM page (every ping voice is FM) the second is the pings' **Ratio** (the
modulator's, from harmonic to clangy) and the fourth their **Upper** (operators 3 and 4's pitch, the
other pages' Ratio).

![The Chord FM page on Saws: SAWS IS NOT FM: ITS COLOUR ALONE, ALGORITHM, FOR ONE: KEYS, BELL; ALGO, RATIO and FEEDBACK grey, COLOUR 0](img/screen-chord-fm.png "The Chord FM page, a voice not FM")

- **Algo:** which operators modulate which. *Voice* (the voice's own), or one of the classic eight
  4-operator algorithms (1: a stack of four; 5: two pairs; 8: four sines side by side), or 2-op
  (one modulator into one carrier). The screen draws it: the carriers (heard) on the line, each
  modulator above the one it modulates, operator 4's feedback as a loop. Its ten positions show
  as dots.
- **Ratio** (the Pings FM page's **Upper**): operators 3 and 4's pitch against the note, from half
  to twice the voice's. On a two-operator algorithm (2-op, or a voice's own: Pluck, Growl, and every
  ping voice but Glass) there are no operators 3 and 4, so it's grey and rests. (The Pings FM page's
  own **Ratio** is its modulator's: the ping from harmonic to clangy, a step of the ratio at a
  time.)
- **Feedback:** operator 4 modulating itself, more than the voice's own (0) towards gritty, and
  noise.
- **Colour** (the chord's): as each voice has it: the saws' detune, the FM voices' brightness (the
  modulators' level), the organs' upper drawbars, the noise against its saw; −50 to +50 around the
  voice's own (0). The Chord LFO can move it (its Destination, below).
- **Bite** (the bass's): the drive, clean to growling, on every voice, and on the FM ones their bite
  (the modulators' level); −50 to +50 around the voice's own.

Algo and Ratio are relative to the voice (Voice, 0); Feedback starts from the voice as it is at 0.
Each stays when the voice changes, and is kept by the piece.

### The LFO pages

![The Chord LFO page: TO CUTOFF, DEPTH 60, RATE 4 bars, SHAPE Sine, DRIFT 30, 4 BARS + DRIFT ON CUTOFF over the filter's sine riding its drift over the last 16 bars in the middle](img/screen-chord-lfo.png "The Chord LFO page")

Two layers move by themselves, slowly: the chord's filter (or, by its Destination, its resonance or
its Colour) and a filter on the whole noise, its hits and its hiss (or, by its Destination, its Tone
or its Air). Each moves two ways at once, a
**wave** locked to the bars and a **drift**, a slow random wander; either can be off. Each has an
**LFO page**: ENV a fourth time (Chord, Chord voice, Chord FM, **Chord LFO**, then back to Chord), and SAVE
a third time (Noise, Noise voice, **Noise LFO**, then back to Noise). The button blinks while you're
there. ALGORITHM on either picks where its LFO goes, its **Destination** (below); the noise's
character is on its other pages. The middle shows the LFO's path,
the last 16 bars of it (see [The screen](#4-the-screen)); the lane under it is the layer's, as on its
page.

- **Depth:** how deep the wave, 0 (off) to 100. The chord's filter: up to two and a half octaves
  either way (on Reso or Colour, as far along their knob: below) (gently low on the knob, deep near the top: the pieces' 20 to 45 a gentle sweep). The
  noise's filter: open at the wave's top, closing towards its bottom, up to three and a half octaves
  (from 20 kHz down to about 1.4 kHz) with the drift (on Tone or Air: below).
- **Rate:** how long the wave, a switch, one position a click: **1 bar** (one redonda (whole note)),
  **2, 4, 8, 16 or 32 bars**, locked to the bars.
- **Shape:** the wave's shape: **Sine**, **Triangle**, **Ramp up** (a slow rise, then back at once),
  **Ramp down**. The sine's and the triangle's top comes a quarter of the way into the cycle.
- **Drift:** how far the slow random wander goes, 0 (off) to 100: up to about 0.6 octave either way on
  the chord's filter. Like a hand riding the filter: every 8 to 32 bars it picks a new place somewhere
  within its reach (a real move, not a twitch), and glides there, smoothly, easing in and out, over
  most of the time until the next; then it rests there a little. Its moves don't wait for a barline,
  it never repeats a path, and the chord's and the noise's wander each on their own. It runs under
  the wave, so the wave rides on it. This is how the records move (the study behind it: they drift
  far more than they cycle).
- **Destination** (ALGORITHM, on the Chord LFO page; round, a click a step): what the chord's wave and
  drift move: **Cutoff** (the filter, as ever), **Reso** (the filter's resonance) or **Colour** (the
  voice's colour, the Chord FM page's KNOB 4). On Reso and Colour they go as far along that knob as
  they would along the Cutoff's (an octave a 5.5th of its sweep: about ±45 of its 100 at Depth 100,
  ±11 with the drift at 100), never past the knob's own ends: Depth 100 sweeps most of the
  resonance, never past what the knob itself reaches. The filter then stays where its Cutoff is. The
  big ring names it as it turns (DESTINATION over it), the rule over the knobs says TO CUTOFF, TO RESO,
  TO COLOUR, and the moving knob (see [ENV again: Chord voice](#env-again-chord-voice)) follows it to
  its page. Each piece keeps its own; every recipe's is Cutoff.
- **The noise's Destination** (ALGORITHM, on the Noise LFO page; round): **Filter** (the low-pass on
  the whole noise, as ever), **Tone** (the Noise voice's Tone: its rings and its hiss darker and
  brighter) or **Air** (the hiss's level, rising and falling under the hits). On Tone and Air they go a
  quarter of the knob's sweep for each 100 of Depth or Drift (Depth 100 and Drift 100 together: half
  of it either way), never past the knob's own ends; the filter then stays open. The big ring names it
  (DESTINATION), the rule says TO FILTER, TO TONE or TO AIR, and on the Noise voice page the Tone or
  Air knob shows it moving, as the chord's do (the Filter has no knob: the LFO page's path shows it).
  Every recipe's is Filter.

![The Noise LFO page, ALGORITHM turned: the big ring, DESTINATION over it, Air in it, What the LFO moves under it; TO AIR on the rule](img/screen-noise-lfo-dest.png "The Noise LFO page: the Destination")

![The Chord LFO page, ALGORITHM turned: the big ring, DESTINATION over it, Reso in it, What the LFO moves under it; TO RESO on the rule](img/screen-chord-lfo-dest.png "The Chord LFO page: the Destination")

Every piece drifts (its chord 25 to 45, its noise 20 to 35). Chamber's, Fog's and Haze's chords, the
slow, ambient ones, drift alone (the wave off); Fog's and Haze's noise too. The others add a wave:
Basic, Pressure and Concrete's chords on 4 bars, Steppers' on 2, Night Drive's on 8 (each piece's in
[Parameters](#12-parameters)). The defaults: a sine, 4 bars.

The bass and the pings have no LFO (DUB-1 1.4.0: the records never move a dub bass that way, and the
pings' was barely heard). The bass moves with the arrangement and the hand: Energy, Motion, a
nudge.

### The voice pages

Press a layer's button again (ENV on the Chord page, ARP on Pings, LFO on Bass, SEQ on Drums, SAVE
on Noise) for its **voice page**: four more knobs that shape its sound. The button blinks while
you're there; press it again for the next page, or go anywhere else as usual. ALGORITHM still changes
the voice.

Most knobs are **relative to the voice as it is**: 0, the middle, is the preset; turning moves
away from it, to −50 or +50 (with the notch at 0). Two are **one-sided**, 0 to 100 from the voice's
own, since the voice can't go the other way: the chord's **Attack** (0 the voice's strike, slower
from there) and the pings' **Bend** (0 the voice's own fall, more from there). The chord's
**Cutoff** is around the piece's own, −50 to +50 (0 the piece's); its Decay is its own, 0–100. The
registers (the pings' and the bass's Octave) come first. Your turns stay when you change the voice,
so a chord made slower and brighter stays slower and brighter on the next voice. Holding GLO 1.5 s
(the piece back to its recipe) puts them back to 0. Each piece keeps its own.

The middle of the screen shows the layer live, as its page does: its own ripples (or, with
ALGORITHM's view, the scope or the spectrum of its sound), with the chord's name, and its lane under
it.

| Voice page | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|---|
| Chord voice | **Cutoff**: the low-pass, darker or brighter than the piece's own (see [ENV again: Chord voice](#env-again-chord-voice)) | **Reso**: the filter's resonance | **Attack**: 0 the voice's strike, up to a slow swell | **Decay**: how long a stab rings, 0–100, from 60 ms to 1.5 s (times the voice's own length) |
| Pings voice | **Octave**: two octaves down to two up | **Tone**: round to metallic (moves the piece's Ping tone) | **Length** | **Bend**: the pitch falling at the strike, 0 the voice's own, up to from four times its note |
| Bass voice | **Octave**: −1, 0, +1 | **Shape**: up, towards a square; down, towards a pure sine | **Gate**: how long each note holds, against its groove's | **Glide**: how long it slides to the next note, ~1 ms to ~250 ms |
| Kick | **Tune**: semitones from the key's note, −12 to +12 | **Length**: how long it booms | **Punch**: the kick's start pitch and its click | **Rumble**: a dark roar under the kick, none at 0 |
| Hats (SEQ a third time) | **Level**: against the kick | **Tone**: dark to bright | **Length**: how long they ring | — |
| Noise voice | **Air**: the hiss under it | **Tone**: the rings darker or brighter | **Decay**: how long each hit rings | — |

The chord's Decay is the voice's length times its own: an Organ or a Pad rings longer than Saws at
the same Decay. The filter's own envelope is quicker. How far each stab opens the filter comes with
the voice and the piece. The bass's Bite and the pings' Ratio are on their FM pages (see
[The FM pages](#the-fm-pages)).

### HOME: Form

![The Form page: ENERGY 0, EVOLVE 55, MOTION 30 (its ring marked at 40 and 70), the fourth knob free](img/screen-main.png "The Form page")

How the piece unfolds (the Dub page until 1.4.0). **ALGORITHM** (here and on any page but a layer's, the Noise page or the Chords page): the middle of the screen as ripples, scope,
spectrum, echo trail, fifths or tape (see [The screen](#4-the-screen)). **HOME** again: the Song page (below), then the Key page.

- **Energy:** busier or sparser, now: 0 in the middle, the notch there. Down, sparser and drier:
  fewer and quieter pings, fewer and quieter hats, fewer and shorter stabs, a thinner bass line, a
  sparser kick; past halfway, the next phrases go Thin (no bass, no shaker). Up, busier and brighter:
  more of each, louder hats, longer stabs, a thin bass filled back in, the kick nearer four on the
  floor; past halfway, the phrases stay Full, with a fill into each. For playing: not saved, and it
  stays where it is when you change piece. (The Arrangement page's KNOB 1 too until 1.4.0's space
  pages, now there the Width; the DJ filter, KNOB 1 here until 1.4.0's Chord pages, is on SELECT, on
  every page.)
- **Evolve:** how restless and loose it is: the stab pattern turning and gaining or losing stabs,
  stabs and whole bars dropping out (as far as each layer's Vary lets it: its page), each stab's own cutoff, the shaker's pattern and looser hats,
  the pings' notes, loops sitting out, octave jumps in the bass, how far the kick's shape wanders,
  and whether there are dub throws at all (see [The layers](#5-the-layers)). At 0, with Motion at 0
  and the Drums page's Kick on Four too, it's a strict loop: every stab every bar, the kick four
  on the floor, no throws, no fills, the phrase plan as it was drawn when the music started (good
  for playing along with other gear). To keep the groove at any Evolve, hold SEL: Lock. Above 40
  it also nudges the sound by itself: now and then, on a phrase's first bar, one or two of one
  layer's sound values take a step (half a nudge by hand's), so over a long run the piece slowly
  changes colour (at 100, about every third phrase; at 40 and under, never; under Lock, never). It never shakes.
  Above 60 it also moves the **bass's groove**, rarely, to a neighbour (Offbeat to Dotted, say), and
  back towards the piece's own later: at 100, every few minutes; at 60 and under, never. The groove
  away from home usually lasts a few minutes, so a long run's bass line slowly breathes.
- **Motion:** the arrangement, in three regions (marks on the ring at 40 and 70): under 40 the loop
  holds, 40 to 70 a variation now and then, from 70 Breaks too (see [The arrangement](#6-the-arrangement)).
  It also sets how far the filter sweeps and how much the echo's heads, the filter and the
  band-pass wander.
- KNOB 4 is free for now: Drift, how long a chord lasts, went to the Chord page (as Change).

### HOME again: Song

![The Song page: INTRO 32 bars, MAIN 160 bars, OUTRO 32 bars, KIND Pulse; the song's shape in the middle, a row a part from where it comes in to where it leaves](img/screen-song.png "The Song page")

The piece's song (1.4.0): how long its intro, its main and its outro are, and which parts come first
(see [Songs](#songs-an-intro-the-main-an-outro)). It plays when the Song mode (GLO again) is Song or
Set; in Endless the page shows the song it would be, dimmed. HOME blinks while you're here; HOME again
goes to the Key page.

- **Intro:** 0 to 128 bars, a phrase (8 bars) a click; None: the song starts full.
- **Main:** 32 to 512 bars, a phrase a click: the piece as Endless plays it.
- **Outro:** 0 to 128 bars; None: it stops at the main's end.
- **Kind:** Ambient (the noise and the chord's wash first) or Pulse (the kick and the bass first).

In the middle, the song's shape: a band over the rows, the intro and the outro in teal at its ends,
the main between; under it a row a part (the lanes' rows, in their colours), each from where it
comes in to where it leaves, dimmer as it fades in and out; while a song plays, a white line where it is.

### HOME a third time: Key

![The Key page: TEMPO 122, ROOT F, MODE Dorian, SWING 51%](img/screen-key.png "The Key page")

The piece's key and time, which a dub track seldom changes. HOME blinks while you're here; HOME
again goes back to the Form page.

- **Tempo:** 90 to 140. Also HOME + SELECT, from any page.
- **Root:** the key's root note. The scale moves with it, and the chord at once.
- **Mode:** **Minor** (Aeolian), **Major** (Ionian), Dorian, Mixolydian, pentatonic minor and major, Phrygian, Lydian (the most common first; the screen shows Aeolian as Minor and Ionian as Major); it
  loads the mode's scale. Editing the scale with the keys
  makes it Custom.
- **Swing:** the off sixteenths late, from straight (50%) to a heavy lilt (70%): everything on them, the stabs, the bass, the pings and the drums, together.

### ENV: Chord

![The Chord page: SAWS, DENSITY 30, THROWS 70, CHANGE 64 bars, VARY 50](img/screen-chord.png "The Chord page")

The chord has four pages, ENV pressed in turn: **Chord**, its part (what it plays and how that
changes); **Chord voice**, its sound; **Chord FM**, the FM voice and its Colour (for every voice; see
[The FM pages](#the-fm-pages)); **Chord LFO**, its slow movement (see [The LFO pages](#the-lfo-pages));
then back to Chord.

- **Density:** how many stabs a bar, from one (0 to 9) to six (90 and up), on a Euclidean pattern
  off the beat. Evolve may add or take one now and then, leave one out, or rest a whole bar (as far as Vary lets it).
- **Throws:** how often a stab is thrown into the echo: at 0, never (but the last stab of a fill);
  at 50, about one stab in eight; at 100, about one in three. More in a Break; none at Evolve 0. A
  thrown stab's dry is 6 dB down. The Arrangement page's Throw and the grid's throw every stab, whatever it is.
- **Change:** how long a chord lasts before it moves on: 2, 4, 8, 16, 32 or 64 bars, or Hold (it
  moves only when you press EDIT); a switch, its seven positions as dots. The chord moves at the
  start of a bar. (The Form page's Drift until 1.4.0's Chord pages: the same value, kept.)
- **Vary:** how often the stabs' pattern turns (a step round, or a stab gained or lost, within one of
  Density's), Evolve over it. At 50, every piece's, it turns as Evolve alone turned it before; down
  to 0 less and less, and at 0 never, at any Evolve: the pattern still, every stab of it played (no
  stab left out, no bar without one; the arrangement's Groove and Break still thin it). Up to 100
  about three times as often: at Evolve 40, about every seventh bar at 50, every third at 100. At
  Evolve 0 it's still whatever Vary.

The chord's **Wet** (the stabs dry against their echo) left this page in 1.4.0's Chord pages. Its
sends are now the chord's **channel strip** on the Arrangement page (white 1 held: Level, Echo,
Plate, Low cut); the dry against them stays each piece's own, on no knob, and the Arrangement page's
Wet moves it on top. The stab's sound, the voice, comes with the piece (ALGORITHM changes it).

### ENV again: Chord voice

![The Chord voice page: SAWS, CUTOFF 0, RESO −7 (the LFO on it: its reach two pale lines, a white dot where it is now), ATTACK 0, DECAY 27](img/screen-chord-voice.png "The Chord voice page")

The chord's sound (the Chord sound page until 1.4.0's Chord pages; the voice's Colour is on the Chord
FM page, ENV again). ALGORITHM still changes the voice.

- **Cutoff:** where the low-pass sits, around the piece's own: 0, the middle (the notch there), is
  the piece's brightness; −50 darker, +50 brighter, each step about an 18th of an octave (±2.75
  octaves at the ends), as the main filter turns around its middle. (The piece's own, its recipe's,
  is 110 Hz to 5.4 kHz; the filter goes from 90 Hz to 12 kHz.) Each stab opens it further for a
  moment, the arrangement's sweep moves it (with Motion), and with Evolve each stab sits a little
  higher or lower (up to about half an octave). (A 0–100 knob until 1.4.0's layers' pages: what you
  had set is kept, as its offset from the piece's own.)
- **Reso:** the filter's resonance, −50 to +50 around the voice's own.
- **Attack:** 0 the voice's strike, up to a slow swell.
- **Decay:** how long a stab rings, 0–100, from 60 ms to 1.5 s (times the voice's own length).

**The moving knob.** The Chord LFO (its wave and its drift) moves the knob its Destination names:
the Cutoff (every recipe's), Reso, here, or Colour, on the Chord FM page. That knob shows it: two
pale lines, just outside and just inside its ring, span how far they reach either way, and a white
dot on the ring is where it is now, moving as you hear it. The knob's own place, the middle they
swing around, is where its teal ends (its own dot left out while the LFO moves it).

### ARP: Pings

![The Pings page: BLIP, DENSITY 30, VARY 50, the third and fourth knobs empty](img/screen-pings.png "The Pings page")

The pings' part. ARP again: the **Pings voice** (below); again, the **Pings FM** page (see
[The FM pages](#the-fm-pages)); again, back.

- **Density:** how many notes each loop holds, none at 0 to five at the top, a little fewer while
  the sweep is dark. A change shows up as each loop comes round.
- **Vary:** how often the loops change (a note moved, or one sat out for a pass), Evolve over it:
  50 as Evolve alone, 0 never, 100 about three times as often (see [The pages](#8-the-pages)).
- KNOB 3 is empty: the pings' Wet (their dry against the echo) left the page in 1.4.0's layers'
  pages. Their sends are their **channel strip** on the Arrangement page (white 2 held); the dry
  against them stays each piece's own, and the Arrangement page's Wet moves it on top.
- KNOB 4 is empty: the Cloud, the plate's grains, is on the Echo page with the space (FX twice) since
  1.4.0's space pages.

**ARP again: Pings voice**

![The Pings voice page: BLIP, OCTAVE 0, TONE 0, LENGTH 0, BEND 0](img/screen-pings-voice.png "The Pings voice page")

- **Octave:** the pings two octaves down to two up (KNOB 1, as on the Bass voice page). While
  playing, a ping sounds at once in the new octave.
- **Tone:** round to metallic (it moves the piece's Ping tone), −50 to +50.
- **Length:** how long each ping rings, −50 to +50 around the voice's.
- **Bend:** the pitch falling at the strike, 0 the voice's own, up to from four times its note.

### SAVE: Noise

![The Noise page: VINYL, DENSITY 56, VARY 50, the noise's groove under the chord](img/screen-noise.png "The Noise page")

SAVE, a tap: the Noise page, its part (Density, Vary); again, the **Noise voice** (Air, Tone, Decay,
Width); again, the **Noise LFO**; and again back. Its sends, the plate and the echo, are its
**channel strip** on the Arrangement page (white 5 held).
The noise isn't a bed of hiss but a voice of its own, played like the drums:

- **The crackle groove:** hits on a two-bar pattern of sixteenths, spread evenly (a Euclidean
  pattern), turned and with a hit or two nudged a step off, now and then one a quiet ghost. When a
  phrase starts the pattern is drawn again, or now and then kept, so a motif comes back (kept more
  often as Evolve falls). Break and Thin phrases have fewer hits.
- **Rung on the chord:** each hit is a short burst of noise that strikes three resonators tuned to
  the chord's root, third and fifth, low (E1 to Eb2), the root in the middle, the third left, the
  fifth right. They're retuned at each hit, so the crackle follows the chord. Then a band-pass
  (Tone) takes the top off: almost nothing above 2 kHz. It goes into the plate, a little into the
  echo, more while a stab is thrown. In a Break or Thin phrase it rings longer, to carry the music.
- **The Air:** a quiet hiss under it, and now and then a dry vinyl click.
- **Under the duck:** the noise dips on every beat, deeper than the music, and swells between.

The Noise page, its part:

- **Density:** how many hits ring in the two bars, from none at 0 to 16 at 100 (8 at 50); the dry
  clicks come with it, and at 0 they're gone too (the hiss stays: Air). A turn is heard from the
  next bar.
- **Vary:** how often its groove is drawn anew at a phrase's start, rather than kept: 50 as each
  piece has it (its own pace, and Evolve over it), 0 never (the groove kept), 100 about three times
  as often (see [The pages](#8-the-pages)).

![The Noise voice page: VINYL, AIR 66, TONE 0, DECAY 50, WIDTH 50](img/screen-noise-voice.png "The Noise voice page")

SAVE again, the **Noise voice**: Air runs from nothing at 0; Tone is around the piece's own noise,
0 in the middle; Decay 50 is the middle:

- **Air:** the hiss under it, from none at 0 to the loudest at 100.
- **Tone:** the band-pass, the rings darker (about 250 Hz) or brighter (up to about 2 kHz). The
  arrangement's sweep moves it too.
- **Decay:** how long each hit rings on the chord, from a knock (about 70 ms) to a hum (over a second).
- **Width:** the rings mono (0) to wide (100): the third further left, the fifth further right; 50
  as the pieces have it. (From the Noise space page, gone in 1.4.0's channel strips.)
- The **level** is on OCT− / OCT+, as the layers' are, 5 a press; both together mute the noise, its
  level kept.
- **ALGORITHM:** (here and on the Noise page; on the Noise LFO page it is the LFO's Destination) the noise's character, the same machine set otherwise: **Vinyl** (the default: the
  crackle groove rung on the chord), **Tape** (the hiss through the resonators too, a pad that swells
  between the beats; fewer hits, longer, darker), **Dust** (dry clicks and bursts, no ringing; more
  of them, brighter), **Room** (on the key's tonic triad instead of the chord, long, more air),
  **Machine** (inharmonic, metallic). Its name shows at the left of the knob strip, and a message says it
  ("NOISE Tape").

The middle of the screen shows the spectrum of the noise alone, so you can see its colour as it
moves. Each piece has its own: Basic and Steppers on Vinyl, Fog and Night Drive on Tape, Chamber on
Room, Concrete on Vinyl too, Haze on Dust, Pressure a quieter, darker Vinyl.

**Where the noise sits.** Its Width is on the Noise voice (above). How much of it rings in the
plate and how much goes into the echo are its **channel strip**'s (the Arrangement page, white 5
held: Plate and Echo, 50 as the pieces have it, none at 0, twice at 100; a thrown stab still sends
more), with its Level and a Low cut (its rings and its clicks; the hiss is above it). The Noise space
page that held them (SAVE a third time, until 1.4.0's channel strips) is gone.

How fast its levels, tone and decay drift is each piece's own (its recipe), and so is the pace its
groove changes at, under the Noise page's Vary; the groove is drawn anew at a phrase's start now and
then, more often as Evolve rises, and Lock keeps it. Its drifts move on the barlines.

The Arrangement page's Wet moves the noise too: its dry down and its plate and echo up, or the other way.

### LFO: Bass

![The Bass page: SUB, GROOVE Offbeat, VARY 50, DENSITY 90, FIFTH 20](img/screen-bass.png "The Bass page")

The bass's part (its structure all here: the groove first). LFO again: the **Bass voice** (Octave,
Shape, Gate, Glide: see [The voice pages](#the-voice-pages)); again, the **Bass FM** page, its Bite
there (see [The FM pages](#the-fm-pages)); again, back.

- **Groove:** the pattern, from sparse to busy: Drone (one note a bar, held almost to the next),
  Offbeat (the four off-beat eighths), Dotted (two notes a dotted eighth apart, twice a bar), Sync
  (three notes, syncopated) and Roller (six notes a bar, rolling). Density thins any of them.
- **Vary:** how often the phrase plan is drawn anew (which notes play, its octave note: below),
  Evolve over it: 50 as Evolve alone, 0 never (the plan kept as it was first drawn), 100 about three
  times as often (see [The pages](#8-the-pages)).
- **Density:** how many of the groove's notes play. At 100, every one; lower, some are left out
  (at 50 about half), the bar's first note always kept. Which ones is drawn once a phrase (the
  phrase plan): the same every A bar, every other one in a sparse B bar, the whole groove in a busy
  one. **Energy** (the Form page) thins it further, or fills it back towards every note.
- **Fifth:** how often a note is the chord's fifth instead of its root. A note on the bar's first
  sixteenth is the root, except with Drone: there Fifth sets how often a whole bar is the fifth.

On the Bass voice page, **Octave** is −1, 0 or +1. At 0 the bass sits between E1 and Eb2 (41 to
78 Hz); at +1 an octave higher; at −1 an octave lower, E0 to Eb1 (21 to 39 Hz): felt more than
heard, and lost on small speakers.

![The Bass FM page on Sub: SUB IS NOT FM: ITS BITE ALONE; ALGO, RATIO and FEEDBACK grey, BITE 0](img/screen-bass-fm.png "The Bass FM page, a voice not FM")

### SEQ: Drums

![The Drums page: TIGHT, KICK Four, HITS 10, VARY 50, the fourth knob empty](img/screen-drums.png "The Drums page")

The drums' part. SEQ again: the **Kick** page (its sound); again, the **Hats** page; again, the
**Pattern** page (below); again, back.

- **Kick:** the kick's shape, a switch, from the fullest to the sparest: **Four** (four on the floor),
  **Ghost** (four, with two and four ghosted, about 10 dB softer), **Half** (one and three), **One**
  (the downbeat alone). One click is one position. It takes effect from the next bar, even at a
  piece's opening, and holds for a few phrases; with Evolve up it wanders now and then to a
  neighbouring shape (on Four, a Thin phrase ghosted or half). Evolve lets it wander
  further, a Thin phrase takes it a step sparser, and Energy (the Form page) moves it. The kick is out in
  a Break and the second half of a fill; with the Pattern page on Pattern, your kick lane plays
  instead.
- **Hits:** how busy the hats are. The shaker plays from none to all twelve sixteenths that aren't
  the open hat's, the ones before the beats first. The open hat comes in at 15, on the off-beat of
  beats 1 and 3, and plays all four off-beats from 40. At 0, the kick only. Which of them rest
  (with Evolve) and their feel are drawn once a phrase: the phrase plan (see
  [The arrangement](#6-the-arrangement)).
- **Vary:** how often the drums' part changes, Evolve over it: how long the kick holds its shape and
  how far it wanders, the shaker's steps trading places, the clap now and then. 50 as Evolve alone,
  0 the kick settled on Kick's own shape and the shaker's pattern kept, 100 about three times as
  often (see [The pages](#8-the-pages)).

Duck, how far each beat pushes the rest down, is the Arrangement page's KNOB 4 now (see [Controls](#3-controls)).
Swing is on the Key page (HOME, then HOME again).

**SEQ again: Kick**

![The Kick page: TIGHT, TUNE 0, LENGTH −4, PUNCH −5, RUMBLE 21](img/screen-kick.png "The Kick page")

- **Tune:** semitones from the key's note, −12 to +12.
- **Length:** how long the kick booms, −50 to +50 around the kit's.
- **Punch:** the kick's start pitch and its click, −50 to +50.
- **Rumble:** dub techno's rumble: a dark, unpitched bed under the kick (around 30–80 Hz, mono), the
  kick's tail held over the beat, ducked by each kick and swelling before the next; none at 0, deeper
  and longer as it rises (Steppers' 14, Basic's 21, Fog's 62). Most pieces sit between 15 and 60; 100
  is heavy, for when you want it.

**SEQ a third time: Hats**

![The Hats page: TIGHT, LEVEL 40, TONE 0, LENGTH 0, the fourth knob empty](img/screen-hats.png "The Hats page")

- **Level:** the shaker and the open hat against the kick: at 50, as the pieces have them; about
  12 dB down at 0 and 12 dB up at 100. The same as the Hats strip's Level (white 5 held on the
  Arrangement page); the kick's is the Kick strip's (white 4). The drums' level (OCT− / OCT+ on the
  Drums pages) moves the kick, the hats, the rim and the clap together.
- **Tone:** how dark or bright the hats are, around the piece's own colour (−50 to +50).
- **Length:** how long the shaker and the open hat ring, from about half the kit's to twice.

**The Pattern page** (SEQ a fourth time, after the Hats page): the drums' five lanes,
sixteen steps each, to write yourself.

| | |
|---|---|
| KNOB 1 · Lane | Kick, Shaker, Open (hat), Rim, Clap |
| White keys 1–16 | the lane's sixteen steps: a key turns its step on or off (lit when on) |
| A black key held + a white key | that step accented (on, and drawn taller) |
| KNOB 2 · Accent | how much quieter the unaccented steps are, 0 to 100 |
| KNOB 3 · Chance | how likely the lane's steps are to play, 0 to 100%: loose, breathing hats (the clap starts at 5%: rare, as it shines) |
| KNOB 4 · Play | Auto (the patterns generated from Density and Hits) or Pattern (yours) |

The first step you change takes the pattern over from what Auto was playing, and switches Play to
Pattern. The arrangement still takes lanes out over yours (a Break, the second half of a fill,
the Arrangement page's mutes), and Swing still delays the off sixteenths. Your patterns play as written, each
step by its lane's Chance: the kick's shapes, the phrase plan's B bars and the hats' feel are
Auto's (the bass still plays its plan). The patterns are kept with the piece.

### FX: Dub

![The Dub page: ECHO 55, ECHO TIME 1/8., FEEDBACK 6.3 s, REVERB 35](img/screen-space.png "The Dub page")

The echo and the plate, dub's own instrument (the Space page until 1.4.0).

- **Echo:** how much goes to the echo.
- **Echo time:** the tape loop's length: an eighth, a dotted eighth, a quarter, a dotted quarter, a
  half or one bar. The second head, heard while a throw rings, repeats at three quarters of it, or
  with Motion up, wherever it has slid to.
- **Feedback:** how long the repeats go on, shown as the time they take to die away (to −60 dB):
  1.5 s to 15 s. The loop's gain follows the echo time, so a long echo fades as soon as a short
  one. Throws, the Arrangement page's Echo key and the grid's Swell still push it further; past 100% the soft clip
  holds it, and the echo swells and smears on itself. While resting it's kept under 90%, so the
  tails always die out.
- **Reverb:** how much goes to the plate.

### FX again: Echo

![The Echo page: BLUR 25, TONE 50, SIZE 55, CLOUD 25](img/screen-echo.png "The Echo page")

FX on the Dub page opens the Echo page (FX blinks while you're here); FX again goes back. The
space's colour, the echo's and the plate's, together. The four are 0 to 100; 50 is the echo as the
pieces have it.

- **Blur:** how much each repeat dissolves: at 0, clean copies; at 50, each a little softer and
  more smeared than the last; at 100, a wash.
- **Tone:** what goes into the echo, darker or brighter: a low-pass at about 2 kHz at 50, an octave
  down or up for every 25 (from about 500 Hz to 5 kHz).
- **Size:** how long the plate rings.
- **Cloud:** how much of the reverb goes through the granular cloud: at 0 the plate alone, at the
  top a thick wash of grains. (On the Pings page until 1.4.0's space pages.)

The echo's **Width**, the whole mix's stereo field, is the Arrangement page's KNOB 1 since 1.4.0's
space pages (SEL).

### GLO: Setup

![The Setup page: TUNE 0, MIDI OUT On, LIGHTS On, SAVE](img/screen-setup.png "The Setup page")

What holds for every piece and the whole machine (the root is the piece's: on the Key page). GLO
again: the Song mode page (below).

- **Tune:** fine tuning, ±50 cents.
- **MIDI out:** on or off.
- **Lights:** On: the keys light the scale, and the unlit buttons glow dimly so they can be read
  on the black FM-1. Keys: the key lights only. Off.
- **Save:** a turn saves all eight pieces. It shows **Save**, its dot lit, while there are unsaved
  changes, and **Saved** after. (GLO held does the same.)

On this page the top right shows the machine instead of the tempo. **CPU 35/61** is how much of
the FM-1's time the sound takes: 35% now (smoothed), and 61% at the heaviest moment of the last
two seconds. Under 100% the sound never breaks up; if it ever did, a **!** and a count of the
dropouts would follow. When a moment runs heavy (past 75%), DUB-1 sheds for half a second what is
least heard: the quietest tails of the stabs and the pings, the cloud's new grains, the noise's new
hits and the width, eased back as the load falls, so a crowded moment doesn't break
up. Then, next to the battery icon, a rough charge in % (**67%**), or **USB**
when a computer powers (and charges) the FM-1.

### GLO again: Song mode

![The Song mode page: MODE Song; the song's shape in the middle](img/screen-song-mode.png "The Song mode page")

How DUB-1 plays a piece (1.4.0; see [Songs](#songs-an-intro-the-main-an-outro)): the player's, the
same for every piece, and saved. GLO again goes back to Setup.

- **Mode:** **Endless** (as ever: it starts full and plays on), **Song** (PLAY: an intro, the main,
  an outro, then silence, PLAY off), **Set** (the same, then the next piece from its intro, to the
  last). Turned to Song or Set while playing, the song starts at once from its main (the section word
  as ever, the song line under the bar cells, the outro after Main's bars); PLAY again starts it from
  its intro. Turned to Endless, at once (the song let go, playing on).

---

## 9. MIDI

MIDI comes in over USB and the TRS jack, and goes out over USB.

- **In, channel 1:** a note adds or removes its note from the scale, like a key.
- **In, clock:** the grid follows MIDI clock (six clocks a sixteenth), and start, continue and stop
  start, continue and stop the music. The screen shows EXT for the tempo. When the clock stops
  coming, DUB-1 goes back to its own tempo.
- **Out** (on by default): the chord on channel 1, the pings on 2, the bass on 3 and the drums on
  10 (kick 36, shaker 42, open hat 46), and, while it plays on its own tempo, clock with
  start / stop. Turning MIDI out off sends All Notes Off on channels 1, 2, 3 and 10.

### Screen reader

DUB-1 can tell a screen reader what its screen shows. Connect the FM-1 by USB, open the
[FM-1 Screen Reader](https://charlesvestal.github.io/fm1-reader/) in Chrome or Edge, and click
Connect. It reads out the page you're on and its knobs, a knob's value as you turn it, the piece
and its key, the notes you add or take out, every message, and the chord each time it
moves. Press W to hear the whole screen again: the page, the piece, the chord, the section and its
bar, and the scale. The FM-1 says nothing until the page connects, so a DAW on the same cable
never sees it.

DUB-1's word list (how the reader says what's written on the screen) is published with DUB-1's
site, and the FM-1 tells the reader where to find it.

---

## 10. Saving

DUB-1 keeps all eight pieces in the FM-1's memory, each as you left it: the knobs, the scale, the
mode, the root, the tempo and the pings' octave, each piece's song, plus Tune, MIDI out, Lights and the Song mode.

- **By itself:** a few seconds after a change, but only while resting and once the stabs have
  died away. Writing to the memory interrupts the sound for a moment, so it never happens while
  the music plays. A dot at the top of the screen means there are unsaved changes.
- **GLO held** for half a second, then let go: saves now, playing or not ("SAVED").
- **Setup's KNOB 4 (Save):** shows **Save**, its dot lit, while there are unsaved changes, and
  **Saved** once there aren't; a turn saves.
- **Hold GLO** on for 1.5 s: the piece goes back to its recipe ("RESET"). That isn't saved until the
  next save, so switching off before then brings your version back.

Lock, the Filter, Energy (the Form page's) and the Arrangement page's Wet aren't saved; its Width and its Duck are the piece's, and are.

Projects saved by an older DUB-1 are read and converted. **A piece you never changed takes its
new recipe**, so it sounds as this version tunes it: DUB-1 recognises it by comparing it with that
piece's recipes as they have been since 1.0.0, and with its scale. **A piece you changed keeps what
you set**, and the values that are new (the chord's Wet, the kick's Density, the echo's Blur, Tone
and Width, the noise's Decay, Width, Plate and Echo, the hats' length) come from its recipe. A
value saved past a knob's range as it is now (the chord's Attack, the pings' Bend and the FM
pages' Feedback are 0–100 from the voice's own now; the FM pages' Mod dec is now the LFO) is
brought to that range's nearest end, the rest kept. From 1.4.0, the LFOs' Rates, Shapes and
Drifts come from the recipe (a 1.4.0 test build's Rate on Drift becomes the Drift knob, the wave off);
the pings' and the bass's LFO values are kept but do nothing. From 1.4.0's Chord pages (format 21),
the chord's Vary and its LFO's Destination come from the recipe (Vary 50, Cutoff: as it played before),
and the Form page's Drift is the Chord page's Change, the same value kept. From 1.4.0's layers' pages
(format 22), the pings', the bass's, the drums' and the noise's Vary come from the recipe (50: as they
played before), and a chord Cutoff you had set (then a 0–100 knob) is kept as the new knob's offset from
the piece's own (as far as the knob goes, ±50). From 1.4.0's layers' pages (format 22), the channel
strips' sends and Low cuts come from the recipe (the sends 50, as they sent before; the bass's Echo
0; the Low cuts off), and the noise's Width, Plate and Echo (the Noise space page's) are kept, now
on the Noise voice and the noise's strip. From 1.4.0's channel strips (format 23, the drums one strip),
the drums' strip is the Hats strip, its sends and Low cut kept, and the Kick strip comes in at its level
×1 with no sends, its Low cut the drums' (as it sounded: that one cut the kick too). From 1.4.0's six
strips (format 24), the noise LFO's Destination comes from the recipe (Filter, as it played before);
the Cloud and the Width are kept, on their new pages. From 1.4.0's noise LFO Destination (format 25),
each piece's song comes from its recipe (Intro 32, Main 160, Outro 32 bars, its Kind), and the Song mode is
Endless, as it played before. From 1.4.0's song structure (format 26), each piece's Mixer comes from its
recipe. To get a changed piece's
whole new recipe, hold GLO for 1.5 s on it.
Projects saved by AMB-1 aren't read by DUB-1: the first start after installing it begins with the
eight recipes.

---

## 11. Troubleshooting

**Update mode:** hold OCT- and OCT+ together for five seconds. A countdown appears; let go
before it ends to cancel. (A quick press of both, let go within a second, is a layer's mute on its
page; holding them longer never mutes anything.)

**Calibration:** if a button or knob does the wrong thing, hold OCT- and OCT+ while you switch
the FM-1 on. The screen then asks you to press each button and turn each knob in turn.

**After a crash:** DUB-1 shows a red CRASH screen and restarts by itself a few seconds later. The
next start shows RESTARTED AFTER A CRASH and an address (PC ...) for a moment. If you report the
problem, include that address and what you were doing.

**Safe mode:** if DUB-1 crashes twice in a row while starting, it starts in SAFE MODE instead.
There's no sound, but USB works, so the installer can reach it: reinstall with
`tools/fm1_install.py`, or put the stock firmware back with M-UPGRADE. Press PLAY to try starting
DUB-1 normally again. If even safe mode fails twice, the FM-1 drops into the chip's own update
mode, and only the rescue below can reach it.


### Recovering an FM-1 that won't start

An FM-1 in the chip's own update mode shows a blank screen, and neither the installer nor M-VAVE's
updater can see it. A script can put M-VAVE's firmware back, on a Mac, with the USB cable you
already have.

Open Terminal in a checkout of the DUB-1 source and run:

```
bash tools/fm1_rescue.sh
```

1. If a window asks to install the command line developer tools, click Install. When it has
   finished, run the line again.
2. If it asks for a password, type your Mac password (nothing shows as you type) and press Return.
3. When it says it's waiting, plug the FM-1 in and switch it on. The script catches it, checks the
   chip, saves a backup of the FM-1's memory in the `fm1-rescue` folder in your home folder, and
   asks before it writes anything.
4. Type **yes** and press Return. Don't unplug anything until it says it's restarting the FM-1.
   The FM-1 then starts into its original firmware, and you can install DUB-1 again from there.

The script downloads its rescue tool from X0X's repository, M-VAVE's FM-1 V15 firmware and JieLi's
flash loader, and checks the firmware against a known fingerprint. It writes only the firmware
area of the memory, never the bootloader or the FM-1's own settings, and checks everything it
writes. Running it again is always safe. If the FM-1 never shows up at all, Felucca's
[FM-1-transporter](https://github.com/kurogedelic/FM-1-transporter) (a small RP2040 board wired
to the USB lines) can still reach it.

---

## 12. Parameters

The defaults are Basic's; each piece has its own.

| Page | Value | Range | Basic |
|---|---|---|---|
| Form | Energy | −100 – 0 – +100 (for playing, not saved) | 0 |
| Form | Evolve | 0–100 | 55 |
| Form | Motion | 0–100 (under 40 held, 40–70 variations, 70– Breaks) | 30 |
| Song | Intro | None, 8 – 128 bars, a phrase a click | 32 bars |
| Song | Main | 32 – 512 bars, a phrase a click | 160 bars |
| Song | Outro | None, 8 – 128 bars, a phrase a click | 32 bars |
| Song | Kind | Ambient, Pulse (the piece's own) | Pulse |
| Chord | Density | 0–100 (1 to 6 stabs a bar) | 30 |
| Chord | Throws | 0–100 (none – about 1 stab in 8 at 50 – about 1 in 3) | 70 |
| Chord | Change | 2, 4, 8, 16, 32, 64 bars, Hold | 64 bars |
| Chord | Vary | 0–100 (0 the stabs' pattern still – 50 as Evolve turns it – 100 three times as often) | 50 |
| (no knob: the piece's own) | the chord's dry against its sends | 0–100 (the dry 0 dB – −6 dB at 50 – −20 dB) | 45 |
| Pings | Density | 0–100 (0 to 5 notes a loop) | 30 |
| Pings | Vary | 0–100 (0 the loops still – 50 as Evolve changes them – 100 three times as often) | 50 |
| (no knob: the piece's own) | the pings' dry against their sends | 0–100 (the dry 0 dB – −6 dB at 50 – −20 dB) | 70 |
| Pings voice | Octave | −2 – +2 | 0 |
| Pings voice | Tone, Length | −50 – +50 | 0 |
| Pings voice | Bend | 0–100 (from the voice's own) | 0 |
| Noise | Density | 0–100 (hits in two bars: none – 16) | the piece's |
| Noise | Vary | 0–100 (0 the groove kept – 50 the piece's own pace – 100 three times as often) | 50 |
| Noise voice | Air | 0–100 (the hiss: none – the loudest) | the piece's |
| Noise voice | Tone | −50 – +50 | 0 |
| Noise voice | Decay | 0–100 (about 70 ms – over a second) | 50 |
| Noise voice | Width | 0–100 (50: as the pieces have it) | 50 |
| Noise pages | Character (ALGORITHM) | Vinyl, Tape, Dust, Room, Machine | Vinyl |
| Noise LFO | Depth | 0–100 (on its Destination: a filter on the whole noise, closing up to 3.5 octaves with the drift; or Tone, or Air) | 25 |
| Noise LFO | Rate | 1, 2, 4, 8, 16, 32 bars | 4 bars |
| Noise LFO | Shape | Sine, Triangle, Ramp up, Ramp down | Sine |
| Noise LFO | Drift | 0–100 (the slow random wander) | 20 |
| Bass | Groove | Drone, Offbeat, Dotted, Sync, Roller | Offbeat |
| Bass | Vary | 0–100 (0 the plan kept – 50 as Evolve redraws it – 100 three times as often) | 50 |
| Bass | Density | 0–100 (100: every note of the groove) | 90 |
| Bass | Fifth | 0–100 | 20 |
| Bass voice | Octave | −1 – +1 | 0 |
| Bass voice | Shape, Gate, Glide | −50 – +50 | 0 |
| Key | Tempo | 90–140 | 122 |
| Key | Root | C – B | F |
| Key | Mode | 8 modes, or Custom | Dorian |
| Key | Swing | 50%–70% | 51% |
| Drums | Kick (its shape) | Four, Ghost, Half, One | Four |
| Drums | Hits | 0–100 | 10 |
| Drums | Vary | 0–100 (0 the kick settled, the shaker kept – 50 as Evolve – 100 three times as often) | 50 |
| Kick | Tune | −12 – +12 semitones | 0 |
| Kick | Length, Punch | −50 – +50 | 0 |
| Kick | Rumble | 0–100 (none – a deep roar) | 21 |
| Hats | Level | 0–100 (−12 dB – ×1 at 50 – +12 dB) | 40 |
| Hats | Tone, Length | −50 – +50 | 0 |
| Dub | Echo | 0–100 | 55 |
| Dub | Echo time | 1/8, 1/8., 1/4, 1/4., 1/2, 1 bar | 1/8. |
| Dub | Feedback | 1.5 s – 15 s (the tail, to −60 dB) | 6.3 s |
| Dub | Reverb | 0–100 | 35 |
| Echo | Blur | 0–100 | 25 |
| Echo | Tone | 0–100 | 50 |
| Echo | Size | 0–100 | 55 |
| Echo | Cloud | 0–100 | 30 |
| Setup | Tune | −50 – +50 cents | 0 |
| Setup | MIDI out | Off, On | On |
| Setup | Lights | Off, On, Keys | On |
| Song mode | Mode | Endless, Song, Set (for every piece) | Endless |
| SELECT | Filter | −100 – 0 – +100 (Shut, Off, Open); for playing, not saved | Off |
| Arrangement | Width | 0–100 (the whole mix: mono – wide) | 40 |
| Arrangement | Wet | −100 – 0 – +100; for playing, not saved | 0 |
| Arrangement | Mixer | 0–100 (off – the desk riding itself most); the piece's, saved | 25, 45, 20, 60, 35, 55, 30, 60 |
| Arrangement | Duck | 0–100 (to −12 dB); the piece's, saved | 50 |
| Strip (white 1–6 held) | Level | the part's level (as OCT− / OCT+; the kick's and the hats' their own, ×1 at 50) | 52, 30, 68, 50, 40, 8 |
| Strip | Echo, Plate | 0–100 (0 none – 50 as the piece sends – 100 twice; the kick's 0 none – 100 a full send) | 50 (the bass's Echo 0, the kick's 0) |
| Strip | Low cut | 0–100 (off – about 20 Hz at 1 – about 600 Hz) | 0 |
| SEL held | Lock | Off, On; not saved | Off |
| ALGORITHM | Screen (all pages but a layer's, the Noise and LFO pages and the Chords page) | Ripples, Scope, Spectrum, Echo trail, Fifths, Tape | Ripples |
| ALGORITHM | The walk (Chords page) | Home, Wander, Free | Wander |
| ALGORITHM | Chord voice (Chord page) | 7 voices | Saws |
| ALGORITHM | Ping voice (Pings page) | 5 voices | Blip |
| ALGORITHM | Bass voice (Bass page) | 5 voices | Sub |
| ALGORITHM | Kit (Drums page) | 5 kits | Tight |
| ALGORITHM | Destination (Chord LFO page) | Cutoff, Reso, Colour | Cutoff |
| ALGORITHM | Destination (Noise LFO page) | Filter, Tone, Air | Filter |
| Chord voice | Cutoff | −50 – +50 around the piece's own (its recipe's 0–100: 110 Hz – 5.4 kHz; Basic's 40) | 0 |
| Chord voice | Reso | −50 – +50 | 0 |
| Chord voice | Attack | 0–100 (from the voice's own strike) | 0 |
| Chord voice | Decay | 0–100 (60 ms – 1.5 s, times the voice's) | 25 |
| Chord, Pings, Bass FM | Algo | Voice, 1–8, 2-op | Voice |
| Chord, Bass FM | Ratio (operators 3 and 4) | −50 – +50 | 0 |
| Pings FM | Ratio (the modulator), Upper (operators 3 and 4) | −50 – +50 | 0 |
| Chord, Pings, Bass FM | Feedback (from the voice's own) | 0–100 | 0 |
| Chord FM | Colour (every chord voice) | −50 – +50 | 0 |
| Bass FM | Bite (every bass voice) | −50 – +50 | 0 |
| Chord LFO | Depth | 0–100 (the filter's wave: to ±2.5 octaves; on Reso, Colour as far along their knob) | 25 |
| Chord LFO | Rate | 1, 2, 4, 8, 16, 32 bars | 4 bars |
| Chord LFO | Shape | Sine, Triangle, Ramp up, Ramp down | Sine |
| Chord LFO | Drift | 0–100 (the slow random wander: to ±0.6 octave) | 30 |
| OCT− / OCT+ (a layer's page), a strip's Level (Arrangement, white 1–6 held) | Levels: chord, pings, bass, drums, noise (a strip: its part's) | 0–100, 5 a press | 52, 30, 68, 80 (noise 8) |

Set by the piece (no knob of its own): the pings' tone (from a round blip to a metallic clank;
the Pings voice page's Tone moves it), the hats' colour (the Hats page's Tone moves it) and how far
each stab opens the filter. Each piece also has its own voices.

| Piece | Chord | Pings | Bass | Kit | Ping tone | Hats |
|---|---|---|---|---|---|---|
| Basic | Saws | Blip | Sub | Tight | 40 | 50 |
| Chamber | Organ | Glass | Sub | Deep | 30 | 40 |
| Steppers | Saws | Marimba | Punch | Punch | 50 | 60 |
| Fog | Keys | Blip | Sub | Soft | 25 | 30 |
| Concrete | Noise | Wood | Pluck | Punch | 60 | 75 |
| Night Drive | Keys | Drop | Growl | Tight | 45 | 55 |
| Pressure | Tonewheel | Wood | Growl | Deep | 35 | 65 |
| Haze | Pad | Glass | Sub | Dusty | 25 | 35 |

The LFOs, each piece's (all sines):

| Piece | Chord wave (depth, rate) | Chord drift | Noise wave (depth, rate) | Noise drift |
|---|---|---|---|---|
| Basic | 25, 4 bars | 30 | 25, 4 bars | 20 |
| Chamber | off | 45 | 30, 8 bars | 25 |
| Steppers | 20, 2 bars | 25 | 30, 4 bars | 20 |
| Fog | off | 45 | off | 35 |
| Concrete | 45, 4 bars | 35 | 45, 2 bars | 25 |
| Night Drive | 40, 8 bars | 40 | 40, 8 bars | 25 |
| Pressure | 20, 4 bars | 25 | 20, 4 bars | 20 |
| Haze | off | 45 | off | 30 |

---

## 13. Credits

DUB-1 is free software under the GNU General Public License, version 3. If you share the
firmware, you must also share its source code.

- DUB-1 is a fork of [AMB-1](https://github.com/charlesvestal/fm1-amb) by Charles Vestal
  (GPL-3.0-only), whose loops, echo chain, pieces, UI and platform it keeps.
- The idea comes from AMB-1 and from dub techno in general: chords dissolving in a tape echo over
  a steady kick.
- The reverb is Jon Dattorro's plate ("Effect Design, Part 1", JAES 1997), by way of
  [FoMni-1](https://github.com/charlesvestal/fm1-fomni).
- The granular cloud follows the design of CLOUD in [JIANT](https://github.com/juanjiant-bit/JIANT)
  (GPL-3.0), another FM-1 firmware.
- The Euclidean rhythms are Godfried Toussaint's reading of Bjorklund's algorithm.
- The noise machine (a crackle groove ringing resonators tuned to the chord) follows Bahadirhan
  Kocer's "Using Resonators in Live".
- The random chords are a weighted random walk over the scale's degrees, adapted from AMB-1's
  (see [The layers](#5-the-layers)). They are voiced as dub techno's chord-memory stabs
  (Basic Channel, Maurizio): one shape, moved whole to each new root.
- The platform (hardware layer, USB, update loader, installer and storage) is from
  **Felucca** by Leo Kuroshita, Hügelton Instruments, by way of
  [X0X](https://github.com/charlesvestal/fm1-x0x), FoMni-1 and AMB-1.
- The idea of running FM-1 firmware in the browser comes from [Groove OS](https://www.groove-os.com/).
- The fonts are Barlow Semi Condensed and Terminus (SIL Open Font License).

M-VAVE and FM-1 are trademarks of their owners. DUB-1 is not connected with M-VAVE or Hügelton
Instruments.
