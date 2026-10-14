# HEXED — minimal EX-word hex editor

Experimental **HEXED** editor for CASIO EX-word dictionaries, built using
[libdataplus-ex](https://github.com/TheRainbowPhoenix/libdataplus-ex)
and the modern Hollyhock SH4A GCC. The hex/ASCII grid is loosely inspired
by [CPHexEditor for HHK3](https://github.com/PyCSharp/CPHexEditor-For-HHK3),
but this is a new EX-word-native C implementation; it does not require
Hollyhock 3 ClassPad symbols.

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
