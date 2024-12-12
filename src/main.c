#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gccore.h>

#include "booter.h"
#include "config.h"
#include "execgcdol.h"
#include "console.h"

char* errStr = "";
int ret = 0;

int findandLoadGCDol() {
	static char filepath[128];
	FILE* fp = NULL;
	bool dolLoaded = false;

	for (int i = 0; i < sizeof(devices)/sizeof(device); i++) {
		ret = deviceStart(devices[i]);
		if (ret == 0) {
			printf(CON_MAGENTA("%s") ": device mounted.\n", devices[i].name);
			for (int j = 0; j < sizeof(filepaths)/sizeof(char*); j++) {
				snprintf(filepath, 5, "fat:");
				snprintf(filepath+4, 124, filepaths[j]);
				fp = fopen(filepath, "rb");
				if (fp != NULL)	{
					printf(CON_GREEN("o | File opened: %s:%s.\n"), devices[i].name, filepaths[j]);
					printf("Loading file to RAM...\n");
					ret = EXECGCDOL_LoadFile(fp);
					fclose(fp);
					if (ret == -1) { errStr = "Dol file's header is too small (<256B); file corrupted?"; return ERR_DOL_SIZE; }
					if (ret == -2) { errStr = "Dol file's entrypoint isn't a valid pointer; file corrupted?"; return ERR_DOL_ENTRY; }
					dolLoaded = true;
					break;
				} else { // non-fatal error (intended behaviour)
					printf("x | %s: " CON_MAGENTA("%s") ":" CON_CYAN("%s") ".\n", strerror(errno), devices[i].name, filepaths[j]);
				}
			}
			deviceStop(devices[i]);
			if (dolLoaded) { break; }
		} else { // non-fatal error (intended behaviour)
			printf(CON_MAGENTA("%s") ": device didn't %s (probably unplugged).\n", devices[i].name, ret == -1 ? "start" : "mount");
		}
	}
	if (dolLoaded == false) { errStr = "Dol file not found; consult the readme for allowed locations."; return ERR_NOTFOUND; }
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
	videoShow(false);
	ret = EXECGCDOL_BootLoaded();
	if (ret != 0) { errStr = "(EXECGCDOL_BootLoaded)"; return fail(); };
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
