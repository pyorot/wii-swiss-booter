// defines search priorities and custom console messages
// the rest of the code is not specific to Swiss
#include <sdcard/gcsd.h>
#include <sdcard/wiisd_io.h>
#include <ogc/usbstorage.h>

const device devices[4] = {
	{ "<sd gecko (right)>", &__io_gcsdb      },
	{ "<sd gecko (left) >", &__io_gcsda      },
	{ "< wii sd  (front)>", &__io_wiisd      },
	{ "< wii usb (any)  >", &__io_usbstorage }
};

const char filepaths[3][32] = {
	{"fat:/apps/Swiss/swiss.dol"},
	{"fat:/apps/swiss.dol"},
	{"fat:/swiss.dol"}
};

#define WELCOME_TEXT	"Wii Swiss Booter (v1.0)\n"\
						"=======================\n"\
						"\n"\
						"This program will find and load the Dol file swiss.dol.\n"
