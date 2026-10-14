#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
source sdk-exword/sources.env

# LIBDATAPLUS_REF can come from the environment (CI dispatch or local override).
# Fetch the requested ref *every time*, so the fork's master updates are picked up.
LIBDATAPLUS_REF="${LIBDATAPLUS_REF:-$LIBDATAPLUS_DEFAULT_REF}"
mkdir -p .deps .mini-sdk/bin

fetch() {
  local repo="$1" ref="$2" dir="$3"
  if [[ ! -d "$dir/.git" ]]; then
    git init -q "$dir"
  fi
  git -C "$dir" fetch -q --no-tags --depth 1 "$repo" "$ref"
  git -C "$dir" -c advice.detachedHead=false checkout -q --detach FETCH_HEAD
}

fetch "$LIBDATAPLUS_URL" "$LIBDATAPLUS_REF" .deps/libdataplus
LIBDATAPLUS_RESOLVED_SHA="$(git -C .deps/libdataplus rev-parse HEAD)"
echo "Building libdataplus from $LIBDATAPLUS_URL"
echo "Requested ref: $LIBDATAPLUS_REF"
echo "Resolved commit: $LIBDATAPLUS_RESOLVED_SHA"
printf 'libdataplus_repository=%s\nlibdataplus_requested_ref=%s\nlibdataplus_commit=%s\n' \
  "$LIBDATAPLUS_URL" "$LIBDATAPLUS_REF" "$LIBDATAPLUS_RESOLVED_SHA" \
  > .mini-sdk/source-provenance.txt

fetch https://github.com/brijohn/elf2d01.git "$ELF2D01_COMMIT" .deps/elf2d01
printf 'elf2d01_commit=%s\n' "$(git -C .deps/elf2d01 rev-parse HEAD)" \
  >> .mini-sdk/source-provenance.txt

# Native converter from upstream C source, built on the CI host.
if [ ! -x .mini-sdk/bin/elf2d01 ]; then
  command -v cc >/dev/null || { echo "Missing native .mini-sdk/bin/elf2d01; compile with host cc first" >&2; exit 1; }
  cc -O2 -Wall -Wextra .deps/elf2d01/elf2d01.c -o .mini-sdk/bin/elf2d01
fi

# Rebuild from scratch so incremental local testing also reflects changed fork sources.
make -f sdk-exword/Makefile LIBDATAPLUS_SRC=.deps/libdataplus SDK_DIR=.mini-sdk clean
make -f sdk-exword/Makefile LIBDATAPLUS_SRC=.deps/libdataplus SDK_DIR=.mini-sdk -j2 all
make -f Makefile.minisdk SDK_DIR=.mini-sdk clean
make -f Makefile.minisdk SDK_DIR=.mini-sdk all
