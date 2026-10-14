# Runtime reboot: compiler × original-lib comparison

**Status:** HXHEL and HXGRD, compiled using the source-built mini-SDK with
Hollyhock's GCC, both caused an immediate hardware reboot. CI success only
shows an ELF/D01 was built, not that the EX-word can execute it.

This experiment builds **exactly the same** `diagnostics/probe.c` source
(two stages) with two compiler/library combinations. Both download the
**same original** `devkitPro.tar.gz`, including its original, prebuilt
`libdataplus` static libraries and `elf2d01` tool. This isolates the
modern compiler/toolchain path from our newly recompiled `libdataplus-ex`.

| Combination | Compiler | Libraries | Stage 1 ID | Stage 2 ID |
| --- | --- | --- | --- | --- |
| Previously tested; **reboots** | Hollyhock `sh4a_nofpueb-elf-gcc` | rebuilt `libdataplus-ex` | `HXHEL` | `HXGRD` |
| **New experiment A** | Hollyhock `sh4a_nofpueb-elf-gcc` | original *prebuilt* libdataplus | `HHLLO` | `HHGRD` |
| **New experiment B** | original `sh-elf-gcc` / devkitSH4 | original *prebuilt* libdataplus | `DVLLO` | `DVGRD` |

All programs have **read-only UI logic**, no file access, no RAM scanning,
no firmware writes, and no direct arbitrary-address dereferencing beyond the
existing library's graphics/keypad interfaces. They are **not** hardware-proven
and can still crash; a reboot is evidence to **stop testing that build**.

## Downloading builds

See the [compiler comparison workflow](../.github/workflows/compare-original-lib.yml)
or [its Actions runs](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/compare-original-lib.yml).
It publishes two ZIP artifacts (no GitHub Release):

- `probes-hollyhock-original-lib`: `ja/HHLLO`, `ja/HHGRD`, and Chinese equivalents
- `probes-devkit-original-lib`: `ja/DVLLO`, `ja/DVGRD`, and Chinese equivalents

Choose **only one** Japanese (JA) add-on at a time. Extract the chosen
five-character directory into your libexword working directory, and after
connecting and authenticating with your *existing* account, run
`setpath mem:///` followed by `dict install DVLLO` (or the selected ID).
Do **not** run `dict reset` or `format`.

**Suggested testing sequence:** try **DVLLO** (original compiler and
original libraries) first. If it boots and responds normally, try **HHLLO**
(modern compiler and original libraries). Test grid variants only if the
corresponding minimal variant runs normally. Stop if any build reboots.

## Interpreting the hardware results

| Result | What to investigate next |
| --- | --- |
| DVLLO works; HHLLO reboots | Hollyhock compiler flags, ABI, runtime startup, linker script or D01 layout. The *library itself* is the same. |
| Both DVLLO and HHLLO work; HXHEL reboots | Recompiled libdataplus-ex and/or mini SDK linkage/packaging rather than basic device API. |
| Both DVLLO and HHLLO reboot | Device/model compatibility, original SDK assumptions, packaging or this probe. Compare to a known-working **original** EX-word hello-world add-on before further tests. |
| DVLLO reboots; HHLLO works | Unexpected toolchain-specific difference; investigate legacy build flags, D01 packaging, and hardware compatibility. |

This is a **two-factor test, not an exhaustive factorial design**. The original
compiler uses its built-in EX-word startup path and `-m3 -mexword -mrenesas`
flags; the modern compiler uses an explicitly linked EX-word CRT0 and
linker script. Therefore "compiler" also encompasses the corresponding
startup/linker/ABI path. We cannot attribute a reboot to the code generator
alone without further isolation.

For local reproduction, set `TOOLCHAIN=devkit` with `DEVKITPRO` pointing
to an extracted original devkitSH4, or run `TOOLCHAIN=hollyhock` inside
the Hollyhock 3 container with `DEVKITPRO` pointing to the same archive.
Both invoke `scripts/build-original-lib-comparison.sh`.

Useful known EX-word examples:
[EXplorer](https://github.com/brain-hackers/EXplorer),
[faketerminal](https://github.com/yukkuri-Dev/faketerminal), and
[RAM Scanner](https://github.com/ToTo-40417/exword-ram-scanner#english).
The RAM scanner's validated address region applies only to the hardware
model actually tested by its author, not automatically to other EX-word
models.
