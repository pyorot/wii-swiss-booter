#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gccore.h>
#include <malloc.h>

#include <fat.h>
#include <sdcard/gcsd.h>
#include <sdcard/wiisd_io.h>
#include <ogc/usbstorage.h>

#include "dolloader_dol.h"

#define BC 0x0000000100000100ULL

char filepath[3][32] = {
	{"fat:/apps/Swiss/swiss.dol"},
	{"fat:/apps/swiss.dol"},
	{"fat:/swiss.dol"}
};

const DISC_INTERFACE* devices(int index) {
	switch (index) {
		case 0: return &__io_gcsdb;
		case 1:	return &__io_gcsda;
		case 2:	default: return &__io_wiisd;
		case 3:	return &__io_usbstorage;
	}
};

const char* deviceNames(int index) {
	switch (index) {
		case 0: return "<gcsdb>";
		case 1:	return "<gcsda>";
		case 2:	default: return "<wiisd>";
		case 3:	return "<usbstorage>";
	}
}

static tikview view ATTRIBUTE_ALIGN(32);

// Prevent IOS36 loading at startup
s32 __IOS_LoadStartupIOS() { return 0; }

// setup basic terminal. commentated example:
// https://github.com/devkitPro/gamecube-examples/blob/master/devices/dvd/readsector/source/dvd.c
void initVideo() {
	VIDEO_Init();
	GXRModeObj* vMode = VIDEO_GetPreferredMode(NULL);
	VIDEO_Configure(vMode);
	void* fb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(vMode));
	CON_Init(fb, 20, 64, vMode->fbWidth, vMode->xfbHeight, vMode->fbWidth * 2);
	VIDEO_ClearFrameBuffer(vMode, fb, COLOR_BLACK);
	VIDEO_SetNextFramebuffer(fb);
	VIDEO_SetBlack(false);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(vMode->viTVMode & VI_NON_INTERLACE) { VIDEO_WaitVSync (); }
}

void storageShutdown(int index) {
	fatUnmount("fat");
	devices(index)->shutdown();
}

int storageInit(int index) {
	//storageShutdown(index);
	const DISC_INTERFACE* storage = devices(index);
	if(!storage->startup()) { return -1; }
	if(!fatMountSimple("fat", storage)) { storageShutdown(index); return -2; }
	return 0;
}

int loadGCDol(FILE* fp) {
	u32 filesize = 0;
	u8 *offset;
	u32 buffer;
	
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (filesize <= 0x100) { return -5; }
	fseek(fp, 0xe0, SEEK_SET);
	fread(&buffer, 4, 1, fp);
	
	if (buffer > 0x80700000) {
		offset = (u8 *)0x80100000;
	} else {
		offset = (u8 *)0x80A00000;
		memset((void *)0x80100000, 0, 32);
		DCFlushRange((char *)0x80100000, 32);
		ICInvalidateRange((char *)0x80100000, 32);
	}
	
	fseek(fp, 0, SEEK_SET);
	fread(offset, filesize, 1, fp);
	DCFlushRange(offset, filesize);
	ICInvalidateRange(offset, filesize);				
	return 0;
};

int findandLoadSwiss() {
	s32 err = 0;
	static char buf[128];
	FILE* fp = NULL;
	bool dol_found = false;

	err = !fatInitDefault(); // seems to be required now; must unmount "usb" and "sd" at end
	if (err != 0) { return -3; }
	for (int i = 0; i < 4; i++) {
		err = storageInit(i);
		if (err == 0) {
			printf("Device mounted: %s.\n", deviceNames(i));
			for (int j = 0; j < 3; j++) {
				snprintf(buf, 128, filepath[j]);
				fp = fopen(buf, "rb");
				if (fp != NULL)	{
					printf("- File opened: %s%s.\n", deviceNames(i), filepath[j]+3);
					err = loadGCDol(fp);
					fclose(fp);
					if (err != 0) { return err; }
					printf("Loading Swiss from %s%s\n", deviceNames(i), filepath[j]+3);
					dol_found = true;
					break;
				} else {
					printf("- File not read: %s%s.\n", deviceNames(i), filepath[j]+3);
				}
			}
			storageShutdown(i);
			if (dol_found) { break; }
		} else {
			printf("Device not read: %s [%d].\n", deviceNames(i), err);
		}
	}
	fatUnmount("sd");
	fatUnmount("usb");
	
	if (dol_found == false) { return -4; }
	return 0;
}

int main(int argc, char* argv[]) {
	char* error;
	int ret;

	initVideo();
	printf("Wii Swiss Booter\n");
	printf("================\n\n");

	ret = findandLoadSwiss();
	if (ret != 0) { error = "ERROR | (findandLoadSwiss)"; goto fail; }; 
	
	printf("* O *\n");

	// Copy the integrated DolLoader
	memcpy((void *)0x80800000, dolloader_dol, dolloader_dol_size);
	DCFlushRange((char *)0x80800000, dolloader_dol_size);
	ICInvalidateRange((char *)0x80800000, dolloader_dol_size);

	// Tell the cMIOS to load the DolLoader
	memset((void *)0x807FFFE0, 0, 32);
	strcpy((char *)0x807FFFE0, "gchomebrew dol");
	DCFlushRange((char *)0x807FFFE0, 32);
	ICInvalidateRange((char *)0x807FFFE0, 32);

	ret = ES_GetTicketViews(BC, &view, 1);
	if (ret != 0) {	error = "ERROR | (ES_GetTicketViews)"; goto fail; }
	
	*(volatile unsigned int *)0xCC003024 |= 7;
	
	printf("Starting...\n");
	sleep(1);

	VIDEO_SetBlack(true);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	ret = ES_LaunchTitle(BC, &view);
	if (ret != 0) {	error = "ERROR | (ES_LaunchTitle)"; goto fail; }

	return 0;

fail:
	printf("%s %d\n", error, ret);
	printf("Press any button to exit.\n");
	PAD_Init();
	while(true) {
		VIDEO_WaitVSync();
		PAD_ScanPads();
		if (PAD_ButtonsDown(0)) { break; }
	}
	printf("Exiting...\n");
	return 0;
}
