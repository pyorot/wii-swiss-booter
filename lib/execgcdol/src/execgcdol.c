#include <string.h>
#include <gccore.h>
#include "execgcdol.h"
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
    .payloads = {(u8*)0x80300000, (u8*)0x80500020, (u8*)0x80700060, (u8*)0x80a00080, (u8*)0x80c00040},
};

int EXECGCDOL_LoadFile(FILE* fp) {
    // set filesize
    fseek(fp, 0, SEEK_END);
    cmiosTag.size = ftell(fp);
    printf("filesize:    %d\n", cmiosTag.size);
    // read full Dol file into memory at cmiosTag.payloads
    for (int i=0; i<sizeof(cmiosTag.payloads)/sizeof(cmiosTag.payloads[0]); i++) {
        if ((u32)cmiosTag.payloads[i] < 0x80000000 || (u32)cmiosTag.payloads[i] >= 0x81800000) { continue; }
        printf("swiss.dol  : %p -- %p\n", cmiosTag.payloads[i], cmiosTag.payloads[i]+cmiosTag.size);
        fseek(fp, 0, SEEK_SET);
        fread(cmiosTag.payloads[i], cmiosTag.size, 1, fp);
        //*(volatile u32*)(0x803a8010) = 0;
        DCFlushRange(cmiosTag.payloads[i], cmiosTag.size);
        ICInvalidateRange(cmiosTag.payloads[i], cmiosTag.size);
    }
    // hash
    printf("hash");
    for (int i=sizeof(cmiosTag.payloads)/sizeof(cmiosTag.payloads[0])-1; i>=0; i--) {
        if ((u32)cmiosTag.payloads[i] < 0x80000000 || (u32)cmiosTag.payloads[i] >= 0x81800000) { continue; }
        SHA1(cmiosTag.payloads[i], cmiosTag.size, cmiosTag.hash);
        printf(" %02x%02x", cmiosTag.hash[0], cmiosTag.hash[1]);
    }
    printf("\n");
    // communicate with cMIOS
    memcpy(cmiosTagLoc, &cmiosTag, sizeof(cmiosTag));
    DCFlushRange(cmiosTagLoc, sizeof(cmiosTag));
    ICInvalidateRange(cmiosTagLoc, sizeof(cmiosTag));
    /*// communicate with cMIOS (legacy)
    strcpy((char*)0x807FFFE0, "gchomebrew dol");
    DCFlushRange((char*)0x807FFFE0, 32);
    ICInvalidateRange((char*)0x807FFFE0, 32);*/
    return 0;
}
