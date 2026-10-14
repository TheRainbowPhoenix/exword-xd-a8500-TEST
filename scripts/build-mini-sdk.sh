#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
source sdk-exword/sources.env
mkdir -p .deps .mini-sdk/bin
fetch() {
  repo="$1"; sha="$2"; dir="$3"
  if [[ ! -d "$dir/.git" ]]; then git init -q "$dir"; fi
  if [[ "$(git -C "$dir" rev-parse HEAD 2>/dev/null || true)" != "$sha" ]]; then
    git -C "$dir" fetch -q --depth 1 "$repo" "$sha"
    git -C "$dir" checkout -q --detach FETCH_HEAD
  fi
}
fetch https://github.com/brijohn/libdataplus.git "$LIBDATAPLUS_COMMIT" .deps/libdataplus
fetch https://github.com/brijohn/elf2d01.git "$ELF2D01_COMMIT" .deps/elf2d01
# Native converter from upstream C source, not from MaxSignal.
cc -O2 -Wall -Wextra .deps/elf2d01/elf2d01.c -o .mini-sdk/bin/elf2d01
make -f sdk-exword/Makefile LIBDATAPLUS_SRC=.deps/libdataplus SDK_DIR=.mini-sdk -j2 all
make -f Makefile.minisdk SDK_DIR=.mini-sdk all
