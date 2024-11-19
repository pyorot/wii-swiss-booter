// removable storage devices
#include <fat.h>

typedef struct {
	const char* name;
	const DISC_INTERFACE* interface;
} device;

int devicesInit() { // fatInitDefault seems to be required now
    return fatInitDefault() ? 0 : -1;
}

void devicesClear() { // clean up mounts auto-generated by fatInitDefault
    fatUnmount("sd");
	fatUnmount("usb");
}

void deviceStop(device dev) {
	fatUnmount("fat");
	dev.interface->shutdown();
}

int deviceStart(device dev) {
	const DISC_INTERFACE* storage = dev.interface;
	if(!storage->startup()) { return -1; } // a mount must follow a startup
	if(!fatMountSimple("fat", storage)) { deviceStop(dev); return -2; }
	return 0;
}