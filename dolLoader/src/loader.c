#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <network.h>
#include <sys/errno.h>
#include <ogcsys.h>
#include <fat.h>

#include "dol.h"
#include "asm.h"
#include "processor.h"

#include <ogc/lwp_watchdog.h>
//#include <time.h>

extern void __exception_closeall(void);

int load_dol(void)
{
	void (*ep)();
	unsigned long level = 0;
	
	u32 data;
	if (*(u32 *)0x80100000 == 0x00000100)
	{
		data = 0x80100000;
	} else
	{
		data = 0x80A00000;
	}

	ep = (void(*)())load_dol_image((void *)data);

	settime(secs_to_ticks(time(NULL) - 946684800));

	_CPU_ISR_Disable(level);
	__exception_closeall();

	ep();
	return 3;
}
