# Experimental Hollyhock SH4A toolchain test

[Launch the experimental GitHub Action](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/hollyhock-sh4.yml).

This is **not** the default `Makefile` or [existing build](../.github/workflows/build.yml).
The purpose is to answer whether the modern
`sh4a_nofpueb-elf-gcc` from [hollyhock-3](https://github.com/ClasspadDev/hollyhock-3)
can compile and link this EX-word DATAPLUS add-on.

## What is being reused?

- Compiler, assembler, linker: **`sh4a_nofpueb-elf-gcc`** from the image
  `ghcr.io/classpaddev/hollyhock-3:v2.2.2` (as used by
  [HHK3template](https://github.com/QBos07/HHK3template)).
- EX-word-specific headers and static libraries: `libdataplus` from the
  historical [devkitSH4 distribution](https://github.com/MaxSignal/buildscripts).
- EX-word module packaging: historical `elf2d01`. A ClassPad `.hh3`
  is **not** an EX-word `.d01`.
- Source: unmodified `src/` and `src/libc/` from this repository.

The original devkitSH4 builds with the custom
`sh-elf-gcc -mb -m3 -mexword -mrenesas`; hollyhock's SH4A big-endian
toolchain is *not* guaranteed ABI-compatible with these older libraries.
A successful ELF link does **not** necessarily mean it is safe to run
on the EX-word. Treat even successful artifacts as experimental until
validated for ELF machine/endianness, static relocation, conversion and
hardware behavior.

## Locally (Linux with Docker)

```bash
curl -fsSL https://github.com/MaxSignal/buildscripts/releases/download/Linux/devkitPro.tar.gz | tar xzf -
docker run --rm -v "$PWD:/work" -v "$PWD/devkitPro:/legacy/devkitPro:ro" \
  -w /work -e DEVKITPRO=/legacy/devkitPro --entrypoint /bin/sh \
  ghcr.io/classpaddev/hollyhock-3:v2.2.2 \
  -ec 'export PATH=/toolchain/bin:$PATH; make -f Makefile.hollyhock'
```

Do not install or test on hardware until the image, ABI and output format
have been independently validated.

## CI

The experimental action is independently triggered and cannot break the
stable [EX-word build](../.github/workflows/build.yml). On success, it
uploads Japanese and Chinese packages as a GitHub Actions artifact.
On failure, inspect the logs for the exact compilation/link/conversion error.
No GitHub Release is created.
