#include <fat.h>
#include "booter.h"

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
