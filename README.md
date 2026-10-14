# NORSD — small EX-word NOR-to-SD dumper

**Updated NORSD diagnostic:** The original binary could report DUMP
COMPLETED without readback verification. The latest NORSD now has an
optional **T → Enter** 16-byte SD file test; test that first if a full
dump wasn't visible through libexword. It reopens saved files, checks
their actual sizes and reads back the initial bytes.
[SD diagnosis and full notes](docs/nor-to-sd.md).


**NEW:** [NORSD](docs/nor-to-sd.md) writes a read-only image of the mapped
NOR window to an ordinary file on the **removable SD/TF card (crd0)**.
It's compiled with the original devkitSH4 GCC and original precompiled
libdataplus. This version doesn't require a CASIODICS header because the
target model didn't show the signature at the first expected location.

The add-on **NORSD** lets you select **16 MiB (Enter)** or an
**experimental 32 MiB (Page Down)** and requires a second confirmation.
It checks SD free space, writes in 4 KiB chunks, avoids overwriting
prior files (NOR16A/B.BIN or NOR32A/B.BIN), and never writes to flash.

**Caution:** The 16/32 MiB lengths are chosen by the operator, not
automatically validated NOR sizes. Reading beyond mapped ROM may
hang or reboot the device. Test on a disposable SD card and preserve
existing data. There is no hardware test yet.

[Download Japanese/Chinese packages from NORSD GitHub Actions](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/nor-to-sd.yml).

---

# MEMBR — EX-word memory browser with OS/NOR view

MEMBR runs using **the original devkitSH4 GCC and precompiled
libdataplus** (your on-device test confirmed this toolchain works).

The white-themed memory browser now supports read-only hex/ASCII views,
8-digit Jump-to-address, and **S (own RAM), O (firmware/NOR),
U (raw/unrestricted)** modes. The ordinary keyboard letter **J**
opens Jump, in addition to the physical **ジャンプ / Jump Search** key
(`KEY_JUMP = 74`).

**Important:** The firmware/NOR mode checks `CASIODICS` headers at
`0x8000FF80` and `0x8001FF80`, following
[dictscript's NOR dumper](https://github.com/dictscript/dictscript/blob/main/softwares/run-on-dictionary/NOR_dumper/src/nor_dumper.c).
That first *header check itself* can still reboot an incompatible
device. It requires explicit confirmation. In RAM and verified NOR
views, addresses outside the enabled region show `--`; RAW deliberately
retains the older unbounded reads and their crash risk. No writes or
file dumps are performed.

[Download the Japanese/Chinese devkitSH4 builds](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/devkit-memory-browser.yml) ·
[Full controls, firmware reference and safety notes](docs/devkit-memory-browser.md)

---

# HEXED — minimal EX-word hex editor

Experimental **HEXED** editor for CASIO EX-word dictionaries, built using
[libdataplus-ex](https://github.com/TheRainbowPhoenix/libdataplus-ex)
and the modern Hollyhock SH4A GCC. The hex/ASCII grid is loosely inspired
by [CPHexEditor for HHK3](https://github.com/PyCSharp/CPHexEditor-For-HHK3),
but this is a new EX-word-native C implementation; it does not require
Hollyhock 3 ClassPad symbols.

## Original toolchain crash-isolation packages

**New: both comparison builds pass CI.** We now compile the same diagnostic
Hello and grid sources against the **original precompiled libdataplus**
using either the Hollyhock SH4A GCC or the original patched devkitSH4 GCC.

See [compiler/lib compatibility matrix, download instructions and test order](docs/compiler-crash-matrix.md).
The [comparison workflow](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/compare-original-lib.yml)
publishes two ZIPs: `probes-devkit-original-lib` with `DVLLO`/`DVGRD`,
and `probes-hollyhock-original-lib` with `HHLLO`/`HHGRD`.
**Test DVLLO first**, and stop if it reboots. These builds have not yet
been tested on physical hardware; do not rerun the known-crashing HEXED.

## Crash investigation (hardware feedback)

**A user reports that the current HEXED binary makes their EX-word reboot.**
A green Actions build does not mean the binary is safe to run.
**Do not launch the main HEXED app again until the crash is isolated.**

The mini-SDK workflow now builds two separate diagnostic apps:

- **HXHEL** (hello probe): only clears/draws a few lines and polls
  Back/Power with `get_key_state`, matching common EX-word examples.
- **HXGRD** (grid probe): draws 16 rows of hexadecimal-like text, without
  touching the editor buffer or calling `get_key_pressed`.

Do not install both at once if the device has limited add-on space. Start
with HXHEL, and proceed to HXGRD only if HXHEL launches and exits normally.
If HXHEL also triggers a reboot, suspect startup/ABI/MMIO/device compatibility
rather than the HEXED editing logic. Do not keep re-running a crashing probe.

These are **not guaranteed safe** just because they are smaller. Report
whether the crash happens immediately, before any text appears, after
text appears, or only after pressing a key. Such information will help
locate the failing subsystem.

For device-specific work, see
[EX-word RAM Scanner](https://github.com/ToTo-40417/exword-ram-scanner#english)
(its tested RAM range applies to XD-N6500 only), and
[EXplorer](https://github.com/brain-hackers/EXplorer) /
[faketerminal](https://github.com/yukkuri-Dev/faketerminal), which demonstrate
EX-word UI and file operations on supported hardware.

## Safe first prototype

The editor contains a **512-byte scratch-RAM buffer** initialized with a
byte pattern and some example ASCII text. You can inspect and edit the
bytes in memory. **It does not access arbitrary device addresses, write
firmware, load/save files, search, or dump to storage.**
Edits are discarded when you exit.

| EX-word key | Action |
| --- | --- |
| Left / Right | Move one byte |
| Up / Down | Move one row (8 bytes) |
| Page Up / Page Down | Move one screen (128 bytes) |
| Enter | Increase the selected byte by 1 |
| Backspace | Decrease the selected byte by 1 |
| Shift + Enter / Backspace | Increase / decrease by 16 |
| Back / Power | Exit the editor |

The display shows byte offsets, a hexadecimal grid, printable ASCII (or
`.` for nonprintable bytes), and the current selected value. The offsets
are indices into its private scratch array, *not* physical memory addresses.

**Storage status:** `libdataplus-ex` exposes OS syscall wrappers for file
operations such as `sys_open`, `sys_read` and `sys_write`. We have not
verified that those calls allow safe file storage on your specific model,
so this program doesn't call any of them.

## CI builds and installation

- [Source-built mini SDK workflow](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/mini-exword-sdk.yml): builds with modern SH4A GCC and your `libdataplus-ex` fork, without prebuilt DevKitPRO; uploads **exword-hex-editor-minisdk**.
- [Legacy devkitSH4 workflow](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/build.yml): builds the same HEXED program with the historic compiler for comparison; uploads Japanese and Chinese ZIPs.
- [Hollyhock compiler comparison](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/hollyhock-sh4.yml): experimental compatibility path.

Each package contains `HEXED/` with `hexed.d01`, `diction.htm`
and the add-on metadata. The five-character dictionary ID **HEXED**
is distinct from the old XDTST test package.

Extract the appropriate `HEXED/` directory into the *PC working
directory* from which libexword is launched, connect your EX-word in
Library communication mode, then (using your **existing** credentials):

```text
connect
setpath mem:///
dict auth YOUR_USER 0xYOUR_EXISTING_KEY
dict install HEXED
```

If you are already connected and authenticated, skip those commands.
Internal memory works without an SD card. **Do not run `dict reset`
just to install this program**: it removes previously installed add-ons.

## Development

- `src/main.c` — UI, graphics, button handling using libdataplus-ex
- `src/hexedit.[ch]` — pure, bounded editor logic (512-byte scratch RAM)
- `tests/test_hexedit.c` — host-side unit tests for movement and edits
- `Makefile.minisdk` — self-contained source-built Exword mini-SDK build
- `Makefile.hollyhock` — older Hollyhock compiler + legacy-lib comparison
- `Makefile` — original devkitSH4 comparison
- `docs/mini-sdk.md` — SDK architecture and migration roadmap

The mini SDK fetches your fork's latest `master` on each run; its
artifacts include the exact Git revision used. CI tests scratch-buffer
logic, builds EX-word `.d01` files, and checks the linker entry point.
**Hardware compatibility has not yet been established.**

## Provenance and license

This app is GPL-2.0-based; see [COPYING](COPYING). The inspiration
[CPHexEditor-For-HHK3](https://github.com/PyCSharp/CPHexEditor-For-HHK3)
is public-domain licensed; no original program code or ClassPad MMIO
addresses are copied. The distribution terms of historical libdataplus
source still need clarification before publishing a standalone reusable SDK.
