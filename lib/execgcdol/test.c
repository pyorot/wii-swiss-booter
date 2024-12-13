#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <gccore.h>
#include <fat.h>

#include "execgcdol.h"
#include "console.h"

#define BC      0x0000000100000100ULL   // title ID of GameCube bootloader

char* errStr = "";
int ret = 0;
int fail();

int go() {
	videoShow(true);
	printf("== ExecGCDol ==\n\n");

    if (!fatInitDefault()) { errStr = "(fatInitDefault)"; return fail(); }; 
    FILE* fp = fopen("usb:/swiss.dol", "rb");
    if (!fp) { errStr = strerror(errno); return fail(); };
    ret = EXECGCDOL_LoadFile(fp);
    fclose(fp);
    if (ret != 0) { errStr = "(EXECGCDOL_LoadFile)"; return fail(); };
	printf("Booting Dol...\n");
    EXECGCDOL_BootLoaded(); sleep(3);
	while (padScanOnNextFrame()) { // while controller connected, stall if A held 
		if ((~PAD_ButtonsHeld(0)) & PAD_BUTTON_A) { break; }
	}
    printf("Launching...\n");
	ret = WII_LaunchTitle(BC);
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
