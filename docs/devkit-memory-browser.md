# MEMBR — EX-word live memory browser (devkitSH4)

**Hardware feedback:** MEMBR runs on the device in the original devkitSH4
configuration. Reading an address outside usable RAM caused a reboot; that
is expected when a raw pointer hits an unmapped or protected address.

The updated browser keeps the white hex/ASCII UI, read-only values,
page navigation and 8-digit Jump editor, but now has three explicit views:

| View | How to enter | Reads attempted |
| --- | --- | --- |
| **RAM** (default) | **S** | Only its own known 512-byte scratch RAM; other addresses display `--` |
| **OS / NOR** | **O**, then **Enter** to confirm the header probe | The `0x80000000` NOR window, **only if** a `CASIODICS` firmware header is detected with a plausible `nor_size` |
| **RAW** | **U**, then **Enter** to confirm the risk | Any 32-bit address as before; an invalid or side-effectful MMIO read may reboot |

**The NOR probe itself can still crash.** It reads firmware-header
addresses `0x8000FF80` and, if necessary, `0x8001FF80` before it can
verify the magic string. Those addresses are documented by
[dictscript/NOR_dumper](https://github.com/dictscript/dictscript/blob/main/softwares/run-on-dictionary/NOR_dumper/src/nor_dumper.c)
on supported Ex-word hardware; that does **not** prove they're mapped on
every model. The opt-in confirmation page is shown *before* these reads.

The `CASIODICS` header's `nor_size` determines which addresses the
browser is allowed to display in OS mode. Reads outside that exact
window show `--` without dereferencing a pointer. This guards against
accidental scrolling beyond the firmware image, but is **not** an
MMU-level memory protection mechanism, and a recognised header does
not guarantee every NOR access works on every hardware revision.

## What is the Jump key?

The legacy libdataplus keyboard map defines `KEY_JUMP = 74`. This is
the EX-word's dedicated **ジャンプ / Jump Search** key, not an on-screen
button. Different models may place or label it differently.

You can now press ordinary **J** as a reliable alternative, then
edit the 8-digit address: Q W E R T Y U I O P = hex 0–9, A B C D E F =
hex A–F, Left/Right changes the active digit, Up/Down increments/
decrements it, Backspace zeros it, Enter navigates, and Back cancels.
For a region that's not enabled, Jump still moves the cursor but the
grid shows `--`; it does not silently switch to dangerous RAW mode.

## Built with the working compiler

The [separate MEMBR GitHub Actions workflow](https://github.com/TheRainbowPhoenix/exword-xd-a8500-TEST/actions/workflows/devkit-memory-browser.yml)
still uses the **original patched devkitSH4 GCC plus original
precompiled libdataplus**—the combination that successfully launched
your Hello, hex grid and MEMBR builds. The crashing Hollyhock GCC isn't
used in this build.

The workflow runs host tests of cursor arithmetic, Jump-field editing,
firmware signature/size validation, and allowed-address checks; it
produces downloadable `exword-membr-devkit-ja` and
`exword-membr-devkit-cn` ZIPs. CI verifies compilation, not
device-specific NOR mappings.

## Installation

Extract the appropriate region's `MEMBR` folder in the PC working
directory where you launch libexword. Using your existing login/key:

```text
setpath mem:///
dict install MEMBR
```

If MEMBR is already installed, use the usual authenticated dictionary
remove/install procedure to replace it. **Do not use `dict reset`**:
that deletes installed add-on dictionaries. No SD card is required.

This experiment performs **no memory write and no file dump**. Note
that some memory-mapped hardware registers have read side effects;
a C `const volatile` access is not a guarantee against them.

## Next research steps

- Test **S** mode first; no new addresses are probed automatically.
- If desired, try **O** and confirm once, understanding it can reboot if
  the model does not map the dictscript firmware-header addresses.
- If the header is recognised and OS bytes render, compare the
  displayed `CASIODICS` bytes to those in the upstream NOR dumper.
- Leave **U/RAW** for deliberate hardware research; do not sweep
  addresses or repeatedly access an address that reboots the device.

See also [dictscript's firmware documentation](https://github.com/dictscript/dictscript/blob/main/docs/nand-and-nor.md)
and the [RAM scanner](https://github.com/ToTo-40417/exword-ram-scanner#english).
These are model-dependent observations, not a universal readable map.
