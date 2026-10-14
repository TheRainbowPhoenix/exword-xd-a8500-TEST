#!/usr/bin/env bash
# Compare identical EX-word application source with two original-library builds.
# TOOLCHAIN=hollyhock -> modern SH4A GCC + original libdataplus binaries.
# TOOLCHAIN=devkit    -> patched devkitSH4 GCC + original libdataplus binaries.
set -euo pipefail
cd "$(dirname "$0")/.."
: "$TOOLCHAIN"
: "$DEVKITPRO"

case "$TOOLCHAIN" in
    hollyhock)
        command -v sh4a_nofpueb-elf-gcc >/dev/null
        ;;
    devkit)
        DEVKITSH4="$DEVKITPRO/devkitSH4"
        export DEVKITSH4
        export PATH="$DEVKITPRO/tools/bin:$DEVKITSH4/bin:$PATH"
        command -v sh-elf-gcc >/dev/null
        ;;
    *)
        echo "Unsupported toolchain: $TOOLCHAIN" >&2
        exit 2
        ;;
esac

test -f "$DEVKITPRO/libdataplus/lib/libgraphics.a"
test -f "$DEVKITPRO/libdataplus/lib/libsh4a.a"
test -f "$DEVKITPRO/libdataplus/include/graphics/drawing.h"
test -x "$DEVKITPRO/tools/bin/elf2d01"

# Same source set and stage selectors for both compilers.
printf -v cfiles '%s ' diagnostics/probe.c src/libc/*.c
printf -v sfiles '%s ' src/libc/*.s
mkdir -p "comparison/$TOOLCHAIN"
{
    echo "Compiler/library isolation for EX-word reboot"
    echo "source=diagnostics/probe.c"
    echo "library=original precompiled libdataplus from MaxSignal/devkitPro archive"
    echo "toolchain=$TOOLCHAIN"
    echo "stage 1=basic text display; stage 2=16-row text grid"
    echo "Not validated on physical hardware. Stop testing if it reboots."
    echo "Install one five-letter add-on ID at a time."
    echo "Use your existing libexword authentication: do NOT dict reset."
} > "comparison/$TOOLCHAIN/README-EXPERIMENT.txt"

for stage in 1 2; do
    if [ "$TOOLCHAIN" = hollyhock ]; then
        if [ "$stage" -eq 1 ]; then
            id=HHLLO
            module=hhllo
            title="HH GCC HELLO TEST"
            variant=hello
        else
            id=HHGRD
            module=hhgrd
            title="HH GCC GRID TEST"
            variant=grid
        fi
        build="comparison-build/hollyhock/$variant"
        make -f Makefile.hollyhock \
            DEVKITPRO="$DEVKITPRO" \
            CFILES="$cfiles" SFILES="$sfiles" \
            TARGET="$module" APPID="$id" APPTITLE="$title" \
            BUILDDIR="$build" EXTRA_CFLAGS="-DHEXED_PROBE_STAGE=$stage" all
        for region in ja cn; do
            mkdir -p "comparison/$TOOLCHAIN/$region/$id"
            cp -a "$build/$region/$id/." "comparison/$TOOLCHAIN/$region/$id/"
        done
        cp "$build/elf-header.txt" "comparison/$TOOLCHAIN/$module-elf-header.txt"
    else
        if [ "$stage" -eq 1 ]; then
            id=DVLLO
            module=dvllo
            title="DEVKIT HELLO TEST"
        else
            id=DVGRD
            module=dvgrd
            title="DEVKIT GRID TEST"
        fi
        # The legacy make rules leave objects under src/. Clean when changing
        # the compile-time stage to avoid accidentally recycling stage-1 code.
        make -f Makefile \
            CFILES="$cfiles" SFILES="$sfiles" \
            TARGET="$module" MODNAME="$module" APPID="$id" \
            APPTITLE="$title" CC_OPTS="-DHEXED_PROBE_STAGE=$stage" clean
        make -f Makefile \
            CFILES="$cfiles" SFILES="$sfiles" \
            TARGET="$module" MODNAME="$module" APPID="$id" \
            APPTITLE="$title" CC_OPTS="-DHEXED_PROBE_STAGE=$stage" app
        for region in ja cn; do
            mkdir -p "comparison/$TOOLCHAIN/$region/$id"
            cp -a "build/$region/$id/." "comparison/$TOOLCHAIN/$region/$id/"
        done
        make -f Makefile \
            CFILES="$cfiles" SFILES="$sfiles" \
            TARGET="$module" MODNAME="$module" APPID="$id" \
            APPTITLE="$title" CC_OPTS="-DHEXED_PROBE_STAGE=$stage" clean
    fi
done

for region in ja cn; do
    if [ "$TOOLCHAIN" = hollyhock ]; then
        test -s "comparison/$TOOLCHAIN/$region/HHLLO/hhllo.d01"
        test -s "comparison/$TOOLCHAIN/$region/HHGRD/hhgrd.d01"
    else
        test -s "comparison/$TOOLCHAIN/$region/DVLLO/dvllo.d01"
        test -s "comparison/$TOOLCHAIN/$region/DVGRD/dvgrd.d01"
    fi
done

echo "Ready: comparison/$TOOLCHAIN (Japanese + Chinese packages, 2 variants)"
