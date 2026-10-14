# NORSD: experimental NOR-to-crd0 dump

This is an independent five-character EX-word add-on **NORSD**, built using
the **original patched devkitSH4 GCC and original precompiled libdataplus**.
That compiler/library combination is the one that successfully ran your
DVLLO, DVGRD and MEMBR experiments. It does not use Hollyhock GCC.

## What it does

- Starts on a confirmation screen and does not write automatically.
- **Enter** selects a 16 MiB image from 0x80000000 through 0x80FFFFFF.
- **Page Down** selects an experimental 32 MiB image from 0x80000000
  through 0x81FFFFFF.
- Requires **Enter once more** to confirm; Back/Power cancels.
- Requires a writable SD/TF card exposed by the EX-word as **crd0**.
- Checks free space on crd0; requires image size plus at least 1 MiB.
- Creates a new file in the CRD0 root, using the first unoccupied slot:
  NOR16A.BIN or NOR16B.BIN, or NOR32A.BIN or NOR32B.BIN.
- Never calls sys_delete, sys_sdformat, or any firmware-write syscall.
  It does **write an ordinary file to the removable SD card**.
- Passes the mapped ROM address directly to sys_write, 4 KiB at a time.
  No CASIODICS check; that header wasn't found at the first header
  address on the user's device.
- Reports hexadecimal byte progress; Back/Power can stop after a
  chunk. Interrupted or failed dumps are kept and never overwritten.

## Cautions

16 MiB is a conservative **operator-selected amount**, based on
observations of memory that MEMBR displayed on a particular device.
It is not proof that every byte is readable or that 16 MiB is the
entire NOR firmware. The 32 MiB range is even less certain.
If an address is invalid, a ROM read can reboot or hang the device.
Stop if that happens. Do not repeatedly access a region that crashes.

The software does not modify firmware, but unexpected SD filesystem
behavior, insufficient card capacity or a reboot while writing could
leave an incomplete file. Use a disposable, backed-up SD card first.
This program does not hash or reread the saved image. Verify the size
of the resulting BIN on your computer: 16 MiB = 16,777,216 bytes;
32 MiB = 33,554,432 bytes.

The implementation adapts the CRD0 path, 0x80000000 source window and
the EX-word file syscall conventions from dictscript's NOR dumper:
https://github.com/dictscript/dictscript/blob/main/softwares/run-on-dictionary/NOR_dumper/src/nor_dumper.c

## Install and run

1. Download the Japanese artifact from
   https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/nor-to-sd.yml
   after a successful Actions run.
2. Extract the NORSD folder into the PC working directory used by libexword.
3. Connect the EX-word in Library mode, select internal memory
   with setpath mem:///, and use your existing authentication if necessary.
4. Run **dict install NORSD** (do not reset authentication).
5. Disconnect USB, insert a compatible SD/TF card, and launch NORSD
   from the add-on dictionary list.
6. Choose 16 MiB with Enter or 32 MiB with Page Down; confirm with Enter.
7. After completion, use your PC to copy the BIN file from the SD card.

No existing app is changed or replaced, and the build remains on a
separate experimental branch. CI tests building and packaging only;
SD writing and ROM mapping have **not** been verified on the device.
