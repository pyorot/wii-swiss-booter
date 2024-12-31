# Wii Swiss Booter

> [!IMPORTANT]
> **Older Wiis may often crash when booting Swiss from a file** owing to a [memory corruption bug](https://github.com/pyorot/debug-cmios/). This can be fixed by using this app in direct-boot mode (holding Y) with a [cMIOS with Swiss built-in](https://github.com/pyorot/debug-cmios/releases) installed to your Wii. For newer Wiis that boot Swiss reliably from a file, you may use a [recent Swiss release](https://github.com/emukidid/swiss-gc/releases) (r1788 onwards includes a Wii booter) instead of this app.

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

If you hold Y, Swiss Booter will not load anything to memory and instead directly boot into the MIOS. This is useful if your cMIOS has Swiss built-in for example. 

You can hold A for debug mode. This:
- tests your MIOS for compatibility with Swiss;
- stalls the app to show where it found Swiss.

## Compilation
Follow the instructions [here](https://github.com/systemwii/template#quick-start), using this repo's URL at the `git clone` step. This project requires **libfat-ogc (aka libdvm) v2.0.1+**, released in Nov 2024.

This project uses the [systemwii Make setup](https://github.com/systemwii/make), whose readme explains build configuration. Compilation instructions are at the [Make Arguments](https://github.com/systemwii/make#make-arguments) section. In addition to `make build`, `make clean` and `make run`, use `make dist` to package the compiled program into a .7z archive of a Homebrew Channel app, to be extracted to the root of removable media (this requires p7zip: run `sudo apt install p7zip-full`). The build folder is `_` (likewise for all submodules), and its contents can be deleted at any time.
