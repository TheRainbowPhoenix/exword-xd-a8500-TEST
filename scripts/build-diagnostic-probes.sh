#!/bin/bash
# Run inside Hollyhock container after scripts/build-mini-sdk.sh.
# Requires source-built .mini-sdk/libs and .mini-sdk/bin/elf2d01.
set -euo pipefail
cd "$(dirname "$0")/.."
make -f Makefile.minisdk   APP_MAIN=diagnostics/probe.c   TARGET=hxhel APPID=HXHEL APPTITLE="EX-WORD HELLO PROBE"   BUILDDIR=build-probes/hello EXTRA_CFLAGS=-DHEXED_PROBE_STAGE=1 all
make -f Makefile.minisdk   APP_MAIN=diagnostics/probe.c   TARGET=hxgrd APPID=HXGRD APPTITLE="EX-WORD GRID PROBE"   BUILDDIR=build-probes/grid EXTRA_CFLAGS=-DHEXED_PROBE_STAGE=2 all
