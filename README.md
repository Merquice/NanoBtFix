# NanoBtFix

WILL REQUIRE LINUX OS; I RECOMMEND UNBUNTU 24.XX.

NanoBtFix is a NanoApps application for the **7th-generation iPod nano** that applies a RAM-only Bluetooth A2DP compatibility patch.

It was created to improve Bluetooth audio compatibility with newer headphones and earbuds that can pair with the iPod nano but fail to play audio correctly.

## What it does

NanoBtFix patches the iPod nano's Bluetooth A2DP RTP timestamp calculation in memory.

The original firmware advances the RTP timestamp using a timing value derived from milliseconds.

NanoBtFix changes the timestamp update to:

    timestamp += frame_count * 128

For SBC audio, each SBC frame represents 128 audio samples.

The patch is applied only in RAM and is removed automatically when the iPod is rebooted.

After patch, iPod should play audio, and have support for play, pause, skip, reconnecting in range.

## Compatibility

Currently tested with:

- iPod nano 7th generation
- iPod firmware 1.1.2 environment, installed through ipod_sun
- NanoApps
- AirPods Pro 2

I have only tested Airpod Pro 2's, other headphones should benefit. 

## Installation

First install NanoApps:

https://github.com/nfzerox/NanoApps

Then clone NanoBtFix (install through terminal on Unbuntu, copy and paste these lines):

    git clone https://github.com/Merquice/NanoBtFix.git

Copy it into the NanoApps app directory:

    cp -r NanoBtFix NanoApps/apps/nanobtfix

Enter NanoApps:

    cd NanoApps

Build NanoBtFix:

    ./start build nanobtfix

Then run:

    ./start

Choose:

    Install one app

and select:

    nanobtfix

## Usage

After rebooting the iPod, launch NanoBtFix once.

A successful patch displays:

    PATCHED
    RTP TS = 128/frame

If the patch is already active:

    ALREADY PATCHED
    RTP TS = 128/frame

If NanoBtFix displays:

    MISMATCH
    Patch not applied

the firmware bytes do not match the expected version and the patch will not be applied.

This check is intentional and prevents NanoBtFix from blindly modifying an unknown firmware version.

## Technical details

Patch address:

    0x082B2BD6

Original instruction block:

    01 EE 10 CA
    91 ED 01 0A
    B8 EE 40 0B
    B8 EE 41 1B
    08 EE 01 0B
    BC EE C0 0B
    81 ED 01 0A

Replacement logic:

    ldr     r0, [r1, #4]
    add.w   r0, r0, r12, lsl #7
    str     r0, [r1, #4]

Equivalent operation:

    timestamp += frame_count * 128

NanoBtFix verifies the original bytes before writing anything and invalidates the instruction cache after patching.

## Safety

NanoBtFix modifies executable firmware code in RAM.

Although the patch is temporary and disappears after reboot, low-level firmware modification always carries some risk.

Use at your own risk! I am not responsible if you damage your Nano.

## Credits

Made By Merquice

Built using:

- NanoApps by nfzerox on github, couldn't have been made without it!
- iPod nano reverse-engineering work from the iPod homebrew community
- ipod_sun by CUB3D on github.

Special thanks to everyone contributing tools and research for the iPod nano 7th generation.

If you have any questions, i'd be happy to help.

## License

See `LICENSE`.
