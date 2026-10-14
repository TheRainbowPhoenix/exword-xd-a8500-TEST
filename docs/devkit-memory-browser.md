# MEMBR — unbounded EX-word memory browser

This is a **read-only, unbounded 32-bit memory inspector**. It uses the
**original devkitSH4 GCC + original prebuilt libdataplus**, the combination
whose Hello and grid probes both worked on the target device. It does not
use the Hollyhock GCC that rebooted the device during the Hello probe.

**Unbounded does not mean every address is readable.** The program will
attempt a volatile byte read at every address of the currently viewed
128-byte page. Reading unmapped or privileged memory, hardware registers,
or certain ROM/IO ranges may immediately crash, hang or reboot the device.
This code has no exception handler, MPU mapping or fault recovery. There is
no safe way to guarantee arbitrary reads in user mode, and a browser reboot
is evidence to stop testing that region. Don't sweep the address space.

## Features

- Bright white theme with dark hex/ASCII text and pale-blue selection.
- Eight hex bytes per row, sixteen rows per page, full **8-digit 32-bit addresses**.
- Startup view points to a **512-byte aligned buffer owned by the program**;
  there is no initial arbitrary address probing.
- **Jump** key opens an 8-nibble address input. No address allowlist, clamping,
  forced region, or address validation is applied before the read.
- **Read only:** no memory writes, file read/write, dump, search, networking or
  persistent storage.
- Navigation wraps modulo 2^32; a jump to an invalid address can reboot
  immediately when Enter is pressed.

## Controls

| Key | Action |
| --- | --- |
| Arrows | Move byte or row within the full 32-bit address space |
| Page Up / Page Down | Move 128 bytes |
| Jump key | Edit any 8-digit hexadecimal address |
| Jump: Q W E R T Y U I O P | Type hex digits 0 1 2 3 4 5 6 7 8 9 |
| Jump: A B C D E F | Type A B C D E F |
| Jump: Left / Right | Select one of eight nibbles |
| Jump: Up / Down | Increase/decrease selected nibble |
| Jump: Backspace | Clear current nibble to 0 |
| Jump: Enter | Attempt to show chosen address; can reboot |
| Jump: Back | Cancel without reading the new address |
| Back / Power (browser) | Exit the add-on |

The unusual Q-to-P numeric mapping is necessary because the current
EX-word keypad header defines QWERTY alpha keys but no direct 0–9 codes.

## Download and install

Use the separate [devkit memory browser workflow](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/devkit-memory-browser.yml).
The successful run exposes **exword-membr-devkit-ja** and
**exword-membr-devkit-cn** ZIP artifacts (not GitHub Releases).

Extract the appropriate region's **MEMBR/** folder into your PC's
libexword working directory. Connect and authenticate with existing
credentials, then run:

```text
setpath mem:///
dict install MEMBR
```

Do not run `dict reset`; it removes installed add-on dictionaries.
No SD card is necessary. The existing HEXED add-on remains a separate ID.

## Scope

The core address cursor and jump-nibble logic are host-unit-tested.
The D01 build and metadata are checked in CI. Nothing in CI can determine
whether a given *physical* device has a readable mapping at an address.
No code is copied from the HHK3 hex editor; it only inspired the grid
layout and jumping workflow.
