#include <stdio.h>
#include <fat.h>

// == video.c: a basic terminal ==
void videoInit();           // on program startup (configure registers, video mode, frame buffer)
void videoClear();          // clear console
void videoShow(bool show);  // toggle showing video vs black screen (hides transitions)

// == device.c: removable storage devices ==
typedef struct {
	const char* name;					// for printing
	const DISC_INTERFACE* interface;	// for interacting
} device;
int devicesInit();						// newly-required init routine (calls fatInitDefault)
void deviceStop(device dev);			// dismount then shut-down of device
int deviceStart(device dev);			// startup then mount of device

// == main.c ==
// special values and addresses
#define BC		0x0000000100000100ULL	// title ID of GameCube bootloader
#define PI_CMD_REG      0xCC003024		// memory address set for GameCube mode
#define ADDR_ENTRYPOINT 0xE0			// memory address of entrypoint in Dol header
// functions
int loadGCDol(FILE* fp);				// loads the Dol file into memory
int findandLoadGCDol();					// finds the Dol file on a device and loads it into memory
int bootGCDol();						// boots the Dol from memory
int go();								// tries to start the Dol file
int fail();								// handles failure and asks the user what to do
int main(int argc, char* argv[]);		// entrypoint
