# Wii Swiss Booter

> [!IMPORTANT]
> **Swiss r1761 onwards includes a Wii booter with swiss.dol built in, which should be used instead of this**. However, both options suffer from an undiagnosed [memory corruption bug](https://github.com/pyorot/wii-swiss-booter/issues/4), likely in the cMIOS, that causes Swiss to fail to boot on some Wiis, either sometimes or always, despite all checks passing in this booter. Newer Wii hardware revisions are believed to be stable, else you can try to use this app with an uncompressed swiss.dol, which you can construct from `DOL/swiss_rxxxx.elf` from a Swiss release using [elf2dol](https://wiibrew.org/wiki/ELF_to_DOL_Converter).

This program finds and boots [Swiss](https://github.com/emukidid/swiss-gc/) on a Wii in native GameCube mode using a cMIOS, and can be easily adapted to general GameCube software. It's needed because the Homebrew Channel cannot boot into GameCube mode by itself.

Original program by WiiPower (version 0.3 from 2011/01/15). Icon by Cisz. Check out the [program's thread](https://gbatemp.net/threads/wii-swiss-booter.277350/).

## Setup
Extract the latest release .7z file into the root of your portable SD card or USB stick. The folder structure should look like `A:/apps/Swiss/boot.dol` (same for `icon.png` and `meta.xml`). If so, it will appear and be startable from the Homebrew Channel.

This app requires a cMIOS. The installer of the standard cMIOS is available [here](https://www.mediafire.com/file/4utullykvbe2xlu/cMIOSWiiGator.7z/file). Run it from the Homebrew Channel as described in the previous paragraph. This in turn requires a cIOS, installed as per [here](https://wii.hacks.guide/cios).

This app also requires a copy of Swiss of course, which can be downloaded from [here](https://github.com/emukidid/swiss-gc/releases). Download `swiss_r0000.7z`, where `0000` is replaced by the version you want, extract it, and navigate to the file `DOL/swiss_r0000.dol`. Rename it to `swiss.dol` and place it on a device to go into your Wii.

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
Follow the instructions [here](https://github.com/systemwii/template#quick-start), using this repo's URL at the `git clone` step. This project requires **libfat-ogc (aka libdvm) v2.0.1+**, released in Nov 2024.

This project uses the [systemwii Make setup](https://github.com/systemwii/make), whose readme explains build configuration. Compilation instructions are at the [Make Arguments](https://github.com/systemwii/make#make-arguments) section. In addition to `make build`, `make clean` and `make run`, use `make dist` to package the compiled program into a .7z archive of a Homebrew Channel app, to be extracted to the root of removable media (this requires p7zip: run `sudo apt install p7zip-full`). The build folder is `_` (likewise for all submodules), and its contents can be deleted at any time.
