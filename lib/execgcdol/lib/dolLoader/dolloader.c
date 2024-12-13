// this compiles to a dol that's injected as a blob into another dol
// when executed, it starts a different dol in place of booting the disc

#include <stdio.h>
#include <string.h>
#include <ogcsys.h>
#include <ogc/machine/processor.h>
#include <ogc/lwp_watchdog.h>
#include <unistd.h>

extern void __exception_closeall(void);

typedef struct _dolheader {
	u32 text_data_pos[18]; // text[7] + data[11]
	u32 text_data_start[18];
	u32 text_data_size[18];
	u32 bss_start;
	u32 bss_size;
	u32 entry_point;
} dolheader;

// loads the data of the dol at *dolstart into current execution
// this code was contributed by shagkur of the devkitpro team, thx!
u32 load_dol_image (void *dolstart) {
	u32 i;
	dolheader *dolfile;

	if (dolstart) {
		dolfile = (dolheader *) dolstart;
		// set bss to 0
		DCInvalidateRange((void *) dolfile->bss_start, dolfile->bss_size);
		memset ((void *) dolfile->bss_start, 0, dolfile->bss_size);
		DCFlushRange((void *) dolfile->bss_start, dolfile->bss_size);
		ICInvalidateRange((void *) dolfile->bss_start, dolfile->bss_size);
		// copy text + data
		for (i = 0; i < 18; i++) {
			if (!dolfile->text_data_size[i] || dolfile->text_data_start[i] < 0x100) { continue; }
			DCInvalidateRange((void *) dolfile->text_data_start[i], dolfile->text_data_size[i]);
			memcpy ((void *) dolfile->text_data_start[i], dolstart+dolfile->text_data_pos[i], dolfile->text_data_size[i]);
			DCFlushRange((void *) dolfile->text_data_start[i], dolfile->text_data_size[i]);
			ICInvalidateRange((void *) dolfile->text_data_start[i], dolfile->text_data_size[i]);
		}
		return dolfile->entry_point;
	}
	return 0;
}

// loads dol from memory buffer into execution memory, and executes it
int load_dol(void) {
	u32 data;
	void (*entry_point)();
	unsigned long level = 0;

	sleep(3);
	data = (*(u32*)0x80100000 == 0x100) ? 0x80100000 : 0x80A00000;
	entry_point = (void(*)())load_dol_image((void *)data);
	sleep(3);

	settime(secs_to_ticks(time(NULL) - 946684800));
	_CPU_ISR_Disable(level);
	__exception_closeall();
	entry_point();
	return 3;
}

// after calling this function, the DVD drive can be reset (needed for disc change)
void wii_dvd_reset_unlock(void) {
	volatile unsigned long *pi_cmd = (unsigned long *) 0xCC003024;
	volatile unsigned long *ios_magic = (unsigned long *) 0x800030F8;
	volatile unsigned long *ios_sync_base = (unsigned long *) 0x800030E0;
	volatile unsigned long *phys_ios_magic = (unsigned long *) 0xC00030F8;

	*pi_cmd |= 7; // I assume this will synchronize with Starlet/MIOS.
	*ios_magic = 0xDEADBEEF;
	DCFlushRange((void *) ios_sync_base, 32);
	while (*phys_ios_magic != 0); // I assume this waits for Starlet.
}

int main(int argc, char **argv) {
	wii_dvd_reset_unlock();
	load_dol();
	return 0;
}
