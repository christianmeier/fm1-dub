// SPDX-License-Identifier: GPL-3.0-only
// The SMK-37 browser build (web/emu/build.sh smk37) in Node: boot it, play the pads and the keybed, expect sound,
// screen frames and the pads lit; PERFORM (SEL) on the pads: pad 4 mutes the kick (its light dark orange), pad 6
// dark; KNOB BANK: the picker.
import fs from "fs";
const wasm = fs.readFileSync(process.argv[2]);
let mem;
const wasi = new Proxy({ clock_time_get: (id, p, out) => { new DataView(mem.buffer).setBigUint64(out, 0n, true); return 0; },
  fd_write: (fd, iov, n, out) => { new DataView(mem.buffer).setUint32(out, 0, true); return 0; } }, { get: (t, k) => k in t ? t[k] : () => 0 });
const { instance } = await WebAssembly.instantiate(wasm, { wasi_snapshot_preview1: wasi, env: new Proxy({}, { get: () => () => 0 }) });
const ex = instance.exports; mem = ex.memory;
const fail = (m) => { console.log("FAIL", m); process.exit(1); };
for (const f of ["web_keybed", "web_pads", "web_smk_buttons", "web_fader", "web_wheels", "web_pad_rgb", "web_smk_lit", "web_pad_layout"])
  if (typeof ex[f] !== "function") fail("no export " + f);
if (ex._initialize) ex._initialize();
ex.web_master(2800); ex.web_boot();
let peak = 0;
const run = (blocks) => {
  for (let i = 0; i < blocks; i++) {
    ex.web_render(128);
    for (const v of new Float32Array(mem.buffer, ex.web_out_l(), 128)) peak = Math.max(peak, Math.abs(v));
  }
};
const rgb = (p) => Array.from(new Uint8Array(mem.buffer, ex.web_pad_rgb() + 3 * p, 3));
run(1100);                                    // ~3 s: playing
for (let i = 0; i < 8; i++) { ex.web_pads(1 << (8 + i)); run(20); ex.web_pads(0); run(20); }   // the bottom row: the scale
ex.web_keybed(1 << 0, 0); run(20); ex.web_keybed(0, 0); run(20);                               // C3, an outer key
ex.web_fader(0, 64); run(10); ex.web_fader(0, 100); run(10);
ex.web_wheels(60, 40); run(20); ex.web_wheels(0, 0); run(20);
if (ex.web_pad_layout() !== 0) fail("pad layout " + ex.web_pad_layout());
const lit = [...Array(16).keys()].filter((p) => Math.max(...rgb(p)) > 20).length;
if (lit < 8) fail("pads lit: " + lit);
ex.web_buttons(1 << 1); run(20); ex.web_buttons(0); run(20);       // SEL: PERFORM
if (ex.web_pad_layout() !== 1) fail("PERFORM: pad layout " + ex.web_pad_layout());
ex.web_pads(1 << 3); run(20); ex.web_pads(0); run(40);             // pad 4: the kick off
const k = rgb(3);
if (!(k[0] > 90 && k[2] < 20)) fail("kick pad " + k);
const dark = rgb(5);
if (Math.max(...dark) > 4) fail("pad 6 lit " + dark);
ex.web_buttons(1 << 1); run(20); ex.web_buttons(0); run(20);       // SEL: PERFORM off
ex.web_smk_buttons(1); run(20); ex.web_smk_buttons(0); run(20);   // KNOB BANK: the picker
if (ex.web_pad_layout() !== 2 || !(ex.web_smk_lit() & 1)) fail("picker");
console.log("peak", peak.toFixed(3), "blits", ex.web_blits(), "pads lit", lit);
process.exit(peak > 0.05 && peak < 1 && ex.web_blits() > 10 ? 0 : 1);
