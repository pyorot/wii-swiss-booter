// setup basic terminal. commentated example:
// https://github.com/devkitPro/gamecube-examples/blob/master/devices/dvd/readsector/source/dvd.c
#include <gccore.h>
GXRModeObj* _vMode;
void* _fb;

void videoInit() {
	VIDEO_Init();
	_vMode = VIDEO_GetPreferredMode(NULL);
	VIDEO_Configure(_vMode);
	_fb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(_vMode));
}

void videoClear() {
	CON_Init(_fb, 20, 64, _vMode->fbWidth, _vMode->xfbHeight, _vMode->fbWidth * 2);
	VIDEO_ClearFrameBuffer(_vMode, _fb, COLOR_BLACK);
	VIDEO_SetNextFramebuffer(_fb);
}

void videoShow(bool show) {
	VIDEO_SetBlack(!show);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(_vMode->viTVMode & VI_NON_INTERLACE) { VIDEO_WaitVSync (); }
}
