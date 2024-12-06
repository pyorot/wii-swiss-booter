#include <stdio.h>
#include <errno.h>
#include <fat.h>

// special values and addresses
#define MIOS_DIR		"/title/00000001/00000101/content"	// location of MIOS
#define BC		0x0000000100000100ULL	// title ID of GameCube bootloader
#define PI_CMD_REG      0xCC003024		// memory address set for GameCube mode
#define ADDR_ENTRYPOINT 0xE0			// memory address of entrypoint in Dol header

// error handling (errno is also defined, from <errno.h>)
extern char* errStr;
extern int ret;
enum OurErrors {
	ERR_DENIED = -1,			ERR_MIOS_LOOKUP = -2,	// common
	ERR_FATINIT = -3,			ERR_NOTFOUND = -4,		// common
	ERR_MIOS_NOTFOUND = -11,	ERR_MIOS_SIZE = -12,	// rare
	ERR_DOL_SIZE = -13,			ERR_DOL_ENTRY = -14,	// rare
};

// == device.c: removable storage devices ==
typedef struct {
	const char* name;					// for printing
	const DISC_INTERFACE* interface;	// for interacting
} device;
int devicesInit();						// newly-required init routine (calls fatInitDefault)
void deviceStop(device dev);			// dismount then shut-down of device
int deviceStart(device dev);			// startup then mount of device

//  == check.c ==					
int testMIOS();							// tests if cMIOS is installed

// == main.c ==
int loadGCDol(FILE* fp);				// loads the Dol file into memory
int findandLoadGCDol();					// finds the Dol file on a device and loads it into memory
int bootGCDol();						// boots the Dol from memory
int go();								// tries to start the Dol file
int fail();								// handles failure and asks the user what to do
int main(int argc, char* argv[]);		// entrypoint
