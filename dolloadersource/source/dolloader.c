#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <unistd.h>

#include "loader.h"

/** After calling this function, the DVD drive can be reseted (needed for disc change). */
void wii_dvd_reset_unlock(void)
{
	volatile unsigned long *pi_cmd = (unsigned long *) 0xCC003024;
	volatile unsigned long *ios_magic = (unsigned long *) 0x800030F8;
	volatile unsigned long *ios_sync_base = (unsigned long *) 0x800030E0;
	volatile unsigned long *phys_ios_magic = (unsigned long *) 0xC00030F8;

	/* I assume this will synchronize with Starlet/MIOS. */
	*pi_cmd |= 7;

	*ios_magic = 0xDEAEBEEF;
	DCFlushRange((void *) ios_sync_base, 32);

	while(*phys_ios_magic != 0) {
		/* I assume this waits for Starlet. */
	}
}

int main(int argc, char **argv)
{
	wii_dvd_reset_unlock();
	
	load_dol();

	return 0;
}
