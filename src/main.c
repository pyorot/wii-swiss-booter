#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <malloc.h>

#include <fat.h>
#include <ogc/usbstorage.h>
#include <sdcard/wiisd_io.h>
#include <sdcard/gcsd.h>

#include "dolloader_dol.h"

#define BC 0x0000000100000100ULL

char filepath[2][32] = 
{{"fat:/apps/Swiss/swiss.dol"},
{"fat:/swiss.dol"}
};

// Prevent IOS36 loading at startup
s32 __IOS_LoadStartupIOS()
{
	return 0;
}

static tikview view ATTRIBUTE_ALIGN(32);

DISC_INTERFACE storage;

s32 storage_init(u32 index)
{
	int ret;
	
	switch (index)
	{
		case 0:
			storage = __io_gcsdb;
		break;

		case 1:
			storage = __io_gcsda;
		break;

		case 2:
			storage = __io_wiisd;
		break;

		case 3:
			storage = __io_usbstorage;
		break;	
	}

	ret = storage.startup();
	if (ret < 0) 
	{
		// ...
		return ret;
	}
	ret = fatMountSimple("fat", &storage);

	if (ret < 0) 
	{
		storage.shutdown();
		// ...
		return ret;
	}

	return 0;
}

void storage_shutdown()
{
	fatUnmount("fat");
	storage.shutdown();
}

int load_gamecube_dol()
{
	u8 *offset;
	u32 buffer;
	
	static char buf[128];
	FILE *fp = NULL;
	u32 filesize = 0;
	bool dol_loaded = false;
	int ret;

	int i = 0;
	while (i < 4 && dol_loaded == false)
	{
		ret = storage_init(i);
		
		if (ret == 0)
		{
			int j = 0;
			while (j < 2 && fp == NULL)
			{
				snprintf(buf, 128, filepath[j]);
				fp = fopen(buf, "rb");
				j++;
			}
			if (fp != NULL)
			{
				fseek(fp, 0, SEEK_END);
				filesize = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				if (filesize > 0x100)
				{
					fseek(fp, 0xe0, SEEK_SET);
					fread(&buffer, 4, 1, fp);
					
					if (buffer > 0x80700000)
					{
						offset = (u8 *)0x80100000;
					} else
					{
						offset = (u8 *)0x80A00000;
						memset((void *)0x80100000, 0, 32);
						DCFlushRange((char *)0x80100000, 32);
						ICInvalidateRange((char *)0x80100000, 32);
					}
					
					fseek(fp, 0, SEEK_SET);
					fread(offset, filesize, 1, fp);
					
					DCFlushRange(offset, filesize);
					ICInvalidateRange(offset, filesize);				
					
					dol_loaded = true;
				}

				fclose(fp);
				
			}			
			storage_shutdown();
		}	
		i++;
	}
	
	if (dol_loaded == false)
	{
		// ...
		return -1;
	}
	
	return 0;
}


int main(int argc, char* argv[])
{
	int ret;

	ret = load_gamecube_dol();

	if (ret == 0)
	{
		// Copy the integrated dolloader
		memcpy((void *)0x80800000, dolloader_dol, dolloader_dol_size);
		DCFlushRange((char *)0x80800000, dolloader_dol_size);
		ICInvalidateRange((char *)0x80800000, dolloader_dol_size);

		// Tell the cMIOS to load the dolloader
		memset((void *)0x807FFFE0, 0, 32);
		strcpy((char *)0x807FFFE0, "gchomebrew dol");
		DCFlushRange((char *)0x807FFFE0, 32);
		ICInvalidateRange((char *)0x807FFFE0, 32);
	} else
	{
		//  ...
	}

	ret = ES_GetTicketViews(BC, &view, 1);
	if (ret != 0)
	{
		// ...
	}
	
	*(volatile unsigned int *)0xCC003024 |= 7;
	
	ES_LaunchTitle(BC, &view);

	return 0;
}
