// defines search priorities
#include "device.h"
#include <sdcard/gcsd.h>
#include <sdcard/wiisd_io.h>
#include <ogc/usbstorage.h>

const device devices[4] = {
	{ "<sd gecko (right)>", &__io_gcsdb      },
	{ "<sd gecko (left)>",  &__io_gcsda      },
	{ "<wii sd (front)>",   &__io_wiisd      },
	{ "<wii usb (any)>",    &__io_usbstorage }
};

// defines filepath priority
const char filepaths[3][32] = {
	{"fat:/apps/Swiss/swiss.dol"},
	{"fat:/apps/swiss.dol"},
	{"fat:/swiss.dol"}
};
