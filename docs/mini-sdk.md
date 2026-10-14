# Source-built EX-word mini SDK experiment

The demo now builds the HEXED scratch-RAM hex editor. This experiment removes the dependency on **MaxSignal's precompiled DevKitPRO
archive**. It does **not yet remove libdataplus APIs**: instead it recompiles
libdataplus from its original source with Hollyhock's current SH4A GCC.

| Component | Source |
| --- | --- |
| Compiler/assembler/linker | `ghcr.io/classpaddev/hollyhock-3:v2.2.2` |
| Graphics, keyboard, syscall and utility libraries | [TheRainbowPhoenix/libdataplus-ex](https://github.com/TheRainbowPhoenix/libdataplus-ex), latest `master` by default; ref override supported |
| Converter | [brijohn/elf2d01](https://github.com/brijohn/elf2d01) at pinned SHA, compiled locally |
| CRT0 and memory map | `exword/exword_crt0.s` and `exword/exword.ld` |
| EX-word dictionary packaging | `Makefile.minisdk` and existing `html/` metadata |

No DevKitPRO / MaxSignal tarball is downloaded in the mini-SDK workflow.

## Build

```sh
docker run --rm --platform linux/amd64 -v "$PWD:/work" -w /work \
 --entrypoint /bin/bash ghcr.io/classpaddev/hollyhock-3:v2.2.2 \
 -ec 'export PATH=/toolchain/bin:$PATH; bash scripts/build-mini-sdk.sh'
```

Outputs: `.mini-sdk/lib`, `.mini-sdk/include`, `.mini-sdk/bin/elf2d01`,
and `build-minisdk/{ja,cn}/HEXED/`.

## Develop quickly using libdataplus-ex

The build now pulls **[TheRainbowPhoenix/libdataplus-ex](https://github.com/TheRainbowPhoenix/libdataplus-ex)** rather than Brian Johnson's upstream repository. By default, it resolves your fork's current `master` branch **at each CI run**, so library changes do not require changing a pinned commit in this app repository.

- Push a change to `libdataplus-ex/master`, then open the most recent successful [mini-SDK CI run](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/mini-exword-sdk.yml) in `exword-xd-a8500-TEST` and select **Re-run all jobs**. The fetch happens on every run, including re-runs; no dummy commit is needed. Artifact uploads use `overwrite: true` so re-runs can replace earlier ZIPs. Alternatively, push to the EX-word experimental branch.
- For a specific library branch or SHA, use **Actions → EXPERIMENT - Source-built EX-word mini SDK → Run workflow** and fill `libdataplus_ref` (once the workflow is available from the repository default branch), or set `LIBDATAPLUS_REF=my-branch` when running the build locally.
- Each artifact includes `.mini-sdk/source-provenance.txt` recording the source repository and resolved commit SHA. Use that SHA for exact builds later.

A push to the **other repository** does **not** automatically trigger this workflow: cross-repo triggering needs an explicit dispatch integration or CI job on the library fork.

**Note:** Since `master` is followed dynamically, two runs with identical EX-word repository commits may build different versions of the library if its branch advances. The provenance record is the authoritative revision for each run.

## Future development

1. Test the ELF/D01 against a known-good toolchain and validate hardware boot
   before trusting the generated binaries.
2. Isolate OS syscall bindings from generic C and device-specific code.
3. Replace LCD, text, input, timers and memory APIs one subsystem at a time,
   with tests and backwards-compatible functions where useful.
4. Borrow **architecture**, not ClassPad-specific register addresses, from
   [Hollyhock 3](https://github.com/ClasspadDev/hollyhock-3),
   [Hollyhock 2](https://github.com/SnailMath/hollyhock-2) and
   [gint](https://git.planet-casio.com/Lephenixnoir/gint).
5. Create an independent packaged SDK image once toolchain behavior is proven.

**Important:** A successful CI build alone cannot establish runtime
compatibility or hardware safety. Do not assume ClassPad MMIO/OS APIs apply
to the EX-word. Source copyrights and licenses must be preserved.

## What we can reuse from gint, Hollyhock 2 and 3

- **Reusable immediately:** freestanding modern GCC toolchain, assembler/linker,
  CMake or Make build patterns, static library organization, image/fonts asset
  converters, HAL interface patterns, test suites and build artifacts.
- **Good next abstractions:** buffered display drawing, RGB565 primitives,
  event-driven input with key repeats, a timing API, memory allocation and
  EX-word filesystem wrappers. Borrow the design, not device addresses.
- **Needs an EX-word-specific backend:** keyboard matrix mapping, LCD controller
  register addresses, syscall entry points, timer setup, interrupt controller,
  ROM/RAM layout, loader/startup, power and OS return behavior.
- **Not directly portable:** the ClassPad Hollyhock `.hh3` loader or `.hh3`
  image format, ClassPad touchscreen implementation, OS patches, and platform
  firmware assumptions. EX-word needs `.d01` under a dictionary metadata
  wrapper, with its existing per-device limitations.

gint's ability to temporarily take control of hardware and restore OS
state is especially valuable architecturally, but dangerous to copy
unmodified. Start by calling the EX-word OS and progressively isolate
hardware drivers. A future real 'gint-like' runtime needs separate
interrupt context, clock/power, peripheral state and restoration tests.

## License/provenance note

`elf2d01` is GPL-2.0. The source-only prototype builds libraries from
[TheRainbowPhoenix/libdataplus-ex](https://github.com/TheRainbowPhoenix/libdataplus-ex) (forked from [brijohn/libdataplus](https://github.com/brijohn/libdataplus)), but GitHub
does not identify an SPDX license or a root COPYING/LICENSE in that
repository. **Confirm the libdataplus reuse and binary redistribution
permissions with its maintainer before distributing a public standalone
SDK package.** The experimental CI artifacts exist for compatibility
testing only; they do not imply permission to relicense that code.

The Hollyhock compiler is invoked via a separately distributed image,
not vendored as part of our SDK. No Hollyhock-2/3 or gint source code
is copied into this repository. Keep copyright attributions for the
EX-word startup/linker files inherited from the historic toolchain.
