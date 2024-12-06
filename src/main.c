#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gccore.h>

#include "booter.h"
#include "config.h"
#include "dolloader_dol.h" // generated in /_lib
#include "console.h"

char* errStr = "";
int ret = 0;

// override to block IOS36 from loading at startup
s32 __IOS_LoadStartupIOS() { return 0; }

int loadGCDol(FILE* fp) {
	u32 filesize = 0;	// size of Dol
	u32 entrypoint;     // address of main() in Dol
	u8* baseAddress;	// address to load Dol to in RAM
	// set filesize
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	if (filesize <= 0x100) { errStr = "Dol file's header is too small (<256B); file corrupted?"; return ERR_DOL_SIZE; }
	// set entrypoint
	fseek(fp, ADDR_ENTRYPOINT, SEEK_SET);
	fread(&entrypoint, 4, 1, fp);
	if (entrypoint < 0x80000000 || entrypoint >= 0x81800000) { errStr = "Dol file's entrypoint isn't a valid pointer; file corrupted?"; return ERR_DOL_ENTRY; }
	// set baseAddress
	if (entrypoint > 0x80700000) {
		baseAddress = (u8*)0x80100000;
	} else {
		baseAddress = (u8*)0x80A00000;
		memset((void*)0x80100000, 0, 32);
		DCFlushRange((char*)0x80100000, 32);
		ICInvalidateRange((char*)0x80100000, 32);
	}
	// read full Dol file into memory at baseAddress
	fseek(fp, 0, SEEK_SET);
	fread(baseAddress, filesize, 1, fp);
	DCFlushRange(baseAddress, filesize);
	ICInvalidateRange(baseAddress, filesize);				
	return 0;
};

int findandLoadGCDol() {
	static char buf[128];
	FILE* fp = NULL;
	bool dolLoaded = false;

	ret = devicesInit();
	if (ret != 0) { errStr = "Failed to start file driver; ensure there's at least 1 device, remove any dodgy ones, and try again (you might need to exit the app)."; return ERR_FATINIT; }
	for (int i = 0; i < sizeof(devices)/sizeof(device); i++) {
		ret = deviceStart(devices[i]);
		if (ret == 0) {
			printf(CON_MAGENTA("%s") ": device mounted.\n", devices[i].name);
			for (int j = 0; j < sizeof(filepaths)/sizeof(char*); j++) {
				snprintf(buf, 5, "fat:");
				snprintf(buf+4, 124, filepaths[j]);
				fp = fopen(buf, "rb");
				if (fp != NULL)	{
					printf(CON_GREEN("o | File opened: %s:%s.\n"), devices[i].name, filepaths[j]);
					printf("Loading file to RAM...\n");
					ret = loadGCDol(fp);
					fclose(fp);
					if (ret != 0) { return ret; }
					dolLoaded = true;
					break;
				} else {
					printf("x | %s: " CON_MAGENTA("%s") ":" CON_CYAN("%s") ".\n", strerror(errno), devices[i].name, filepaths[j]);
				}
			}
			deviceStop(devices[i]);
			if (dolLoaded) { break; }
		} else {
			// non-fatal error (intended behaviour)
			errStr = ret == -1 ? "device didn't start (probably unplugged)" : CON_YELLOW("device didn't mount");
			printf(CON_MAGENTA("%s") ": %s.\n", devices[i].name, errStr);
		}
	}
	if (dolLoaded == false) { errStr = "Dol file not found; consult the readme for allowed locations."; return ERR_NOTFOUND; }
	return 0;
}

// boots a Dol from memory
static tikview view ATTRIBUTE_ALIGN(32);   // requires alignment, so static
int bootGCDol() {
	videoShow(false);
	// copy the integrated DolLoader into RAM
	memcpy((void*)0x80800000, dolloader_dol, dolloader_dol_size);
	DCFlushRange((char*)0x80800000, dolloader_dol_size);
	ICInvalidateRange((char*)0x80800000, dolloader_dol_size);
	// tell the cMIOS to load the DolLoader
	memset((void*)0x807FFFE0, 0, 32);
	strcpy((char*)0x807FFFE0, "gchomebrew dol");
	DCFlushRange((char*)0x807FFFE0, 32);
	ICInvalidateRange((char*)0x807FFFE0, 32);
	// get ticket
	ret = ES_GetTicketViews(BC, &view, 1);
	if (ret != 0) {	errStr = "(ES_GetTicketViews)"; return ret; }
	// trigger gc mode
	*(volatile unsigned int *)PI_CMD_REG |= 7;
	// launch
	ret = ES_LaunchTitle(BC, &view);
	if (ret != 0) {	errStr = "(ES_LaunchTitle)"; return ret; } // probably only triggered if BC is broken
	return 0;
}

int go() {
	videoShow(true);
	printf(WELCOME_TEXT);
	if (padScanOnNextFrame() && PAD_ButtonsHeld(0) & PAD_BUTTON_A) {	// run setup tests if A held
		printf("Running tests...\n");
		ret = testMIOS();
		if (ret != 0) { return fail(); }
	}
	ret = findandLoadGCDol();
	if (ret != 0) { return fail(); }; 
	printf("Booting Dol...\n");
	while (padScanOnNextFrame()) { // while controller connected, stall if A held 
		if ((~PAD_ButtonsHeld(0)) & PAD_BUTTON_A) { break; }
	}
	ret = bootGCDol();
	if (ret != 0) { return fail(); };
	return 0;
}

int fail() {
	videoShow(true);
	printf(CON_RED("ERROR | %s [%d]\n"), errStr, ret);
	printf("Press A to retry or B to exit.\n");
	while (padScanOnNextFrame()) { // while controller connected, await input 
		if (PAD_ButtonsDown(0) & PAD_BUTTON_A) { consoleClear(); usleep(300000); return go(); }
		if (PAD_ButtonsDown(0) & PAD_BUTTON_B) { break; }
	}
	printf("Exiting...\n");
	return 0;
}

int main(int argc, char* argv[]) {
	consoleInit();
	return go();
}
