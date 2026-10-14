# Source-built EX-word mini SDK experiment

This experiment removes the dependency on **MaxSignal's precompiled DevKitPRO
archive**. It does **not yet remove libdataplus APIs**: instead it recompiles
libdataplus from its original source with Hollyhock's current SH4A GCC.

| Component | Source |
| --- | --- |
| Compiler/assembler/linker | `ghcr.io/classpaddev/hollyhock-3:v2.2.2` |
| Graphics, keyboard, syscall, libc-compatible libraries | [brijohn/libdataplus](https://github.com/brijohn/libdataplus) at pinned SHA |
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
and `build-minisdk/{ja,cn}/XDTST/`.

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
