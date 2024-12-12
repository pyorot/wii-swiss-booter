#include <stdio.h>
#include <errno.h>
#include <fat.h>

// error handling (errno is also defined, from <errno.h>)
extern char* errStr;
extern int ret;
enum OurErrors {
	ERR_DENIED = -1,			ERR_MIOS_LOOKUP = -2,		ERR_NOTFOUND = -4,		// common
	ERR_MIOS_NOTFOUND = -11,	ERR_MIOS_CORRUPT = -12,								// rare
	ERR_DOL_SIZE = -13,			ERR_DOL_ENTRY = -14,								// rare
};

// == device.c: removable storage devices ==
typedef struct {
	const char* name;					// for printing
	const DISC_INTERFACE* interface;	// for interacting
} device;
void deviceStop(device dev);			// dismount then shut-down of device
int deviceStart(device dev);			// startup then mount of device

//  == check.c ==					
int testMIOS(void);						// tests if cMIOS is installed

// == main.c ==
int findandLoadGCDol(void);				// finds the Dol file on a device and loads it into memory
int go(void);							// tries to start the Dol file
int fail(void);							// handles failure and asks the user what to do
int main(int argc, char* argv[]);		// entrypoint
