# Wii Swiss Booter

Finds and boots [Swiss](https://github.com/emukidid/swiss-gc/) on a Wii in native GameCube mode using a cMIOS, and can be easily adapted to general GameCube software. It is needed because the Homebrew Channel cannot boot into GameCube mode by itself.

Original program by WiiPower (version 0.3 from 2011/01/15). Icon by Cisz. Check out the [program's thread](https://gbatemp.net/threads/wii-swiss-booter.277350/).

## Setup
Extract the latest release .7z file into the root of your portable SD card or USB stick. The folder structure should look like `A:/apps/Swiss/boot.dol` (same for `icon.png` and `meta.xml`). If so, it will appear and be startable from the Homebrew Channel.

This app requires a cMIOS. The installer of the standard cMIOS is available [here](https://www.mediafire.com/file/4utullykvbe2xlu/cMIOSWiiGator.7z/file). Run it from the Homebrew Channel as described in the previous paragraph. This in turn requires a cIOS, installed as per [here](https://wii.hacks.guide/cios).

This app also requires a copy of Swiss of course, which can be downloaded from [here](https://github.com/emukidid/swiss-gc/releases). Download `swiss_r0000.7z`, where `0000` is replaced by the version you want, extract it, and navigate to the file `swiss_r0000\DOL\swiss_r0000.dol`. Rename it to `swiss.dol`, and place it on a device to go into your Wii.

## Usage
When you run Swiss Booter, it will look for `swiss.dol` and start the first copy it finds, searching in the order:
1. SD Gecko (in left GC memory card slot)
2. SD Gecko (in right GC memory card slot)
3. SD Card (in front Wii slot)
4. USB Drive (in either Wii slot)

Within each drive, it searches in the order:
1. `A:/apps/Swiss/swiss.dol`
2. `A:/apps/swiss.dol`
3. `A:/swiss.dol`

You can hold A for debug mode. This:
- tests your MIOS for compatibility with Swiss;
- stalls the app to show where it found Swiss.

## Compilation
It is recommended to use Linux or WSL, since the build scripts are Linux commands (so they fail if you have a space in your repository path 😤). To set up:
- **libogc** is required; install it via [these instructions](https://devkitpro.org/wiki/Getting_Started) – for the Unix-like instructions, select the `wii-dev` group.
- Ensure you have also [cloned the libruntimeiospatch submodule](https://stackoverflow.com/questions/3796927/how-do-i-git-clone-a-repo-including-its-submodules), which should appear in a `librip` folder within the root folder.

To compile, package and clean up, run commands in the repository root in a bash shell:
1. **make/dl.sh** compiles the DolLoader binary to `_/data/dolloader.dol`.
2. **make/librip.sh** compiles the libruntimeiospatch (librip) library to `_/lib/libruntimeiospatch.a`.
3. With these two files generated, run **make** to compile the end product to `_/build/boot.dol` and `_/build/boot.elf`. This generates intermediate files in `_/cache`.
4. With `boot.dol` generated, run **make/release.sh** to bundle it with the `meta` folder into a complete Homebrew Channel app package at `_/wii-swiss-booter.7z` (this requires p7zip: run `sudo apt install p7zip-full`).
5. To avoid stale data, rebuild everything from scratch now and again by running **make/all.sh**.
6. the `_` folder or any of its contents can be deleted at any time.
