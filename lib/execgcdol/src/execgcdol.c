#include <string.h>
#include <gccore.h>
#include "execgcdol.h"
#include "dolloader_dol.h" // generated in _/cache/data
#include "sha1.h"

typedef struct {
	u32 magic;
	u32 size;
	u8 hash[20];
	u8* payloads[9];
} __attribute__((packed)) CMIOSTag;

static void* cmiosTagLoc = (void*)0x80001800;
static CMIOSTag cmiosTag = {
	.magic = 0x1d0110ad,
	.size = 0xa8ea8,
    .payloads = {(u8*)0x80300000, (u8*)0x80500020, (u8*)0x80700060, (u8*)0x80a00080, (u8*)0x80c00040},
};

#define ADDR_ENTRYPOINT 0xE0            // memory address of entrypoint in Dol header

int EXECGCDOL_LoadFile(FILE* fp) {
    u32 filesize = 0;   // size of Dol
    u32 entrypoint;     // address of main() in Dol
    u8* baseAddress;    // address to load Dol to in RAM
    // set filesize
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    printf("filesize:    %d\n", filesize);
    if (filesize < 0x100) { return -1; }
    // set entrypoint
    fseek(fp, ADDR_ENTRYPOINT, SEEK_SET);
    fread(&entrypoint, 4, 1, fp);
    printf("entrypoint:  0x%8x\n", entrypoint);
    if (entrypoint < 0x80000000 || entrypoint >= 0x81800000) { return -2; }
    // read full Dol file into memory at cmiosTag.payloads
    for (int i=0; i<sizeof(cmiosTag.payloads)/sizeof(cmiosTag.payloads[0]); i++) {
        if ((u32)cmiosTag.payloads[i] < 0x80000000 || (u32)cmiosTag.payloads[i] >= 0x81800000) { continue; }
        printf("swiss.dol  : %p -- %p\n", cmiosTag.payloads[i], cmiosTag.payloads[i]+filesize);
        fseek(fp, 0, SEEK_SET);
        fread(cmiosTag.payloads[i], filesize, 1, fp);
        //*(volatile u32*)(0x803a8010) = 0;
        DCFlushRange(cmiosTag.payloads[i], filesize);
        ICInvalidateRange(cmiosTag.payloads[i], filesize);
    }
    // hash
    printf("hash");
    cmiosTag.size = filesize;
    for (int i=sizeof(cmiosTag.payloads)/sizeof(cmiosTag.payloads[0])-1; i>=0; i--) {
        if ((u32)cmiosTag.payloads[i] < 0x80000000 || (u32)cmiosTag.payloads[i] >= 0x81800000) { continue; }
        SHA1(cmiosTag.payloads[i], cmiosTag.size, cmiosTag.hash);
        printf(" %02x%02x", cmiosTag.hash[0], cmiosTag.hash[1]);
    }
    printf("\n");
    return 0;
};

// boots a Dol from memory
int EXECGCDOL_BootLoaded() {
    /*// tell the cMIOS to load the DolLoader
    memset((void*)0x807FFFE0, 0, 32);
    strcpy((char*)0x807FFFE0, "gchomebrew dol");
    DCFlushRange((char*)0x807FFFE0, 32);
    ICInvalidateRange((char*)0x807FFFE0, 32);*/
    memcpy(cmiosTagLoc, &cmiosTag, sizeof(cmiosTag));
    DCFlushRange(cmiosTagLoc, sizeof(cmiosTag));
    ICInvalidateRange(cmiosTagLoc, sizeof(cmiosTag));
    return 0;
}
