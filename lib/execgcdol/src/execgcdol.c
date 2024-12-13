#include <string.h>
#include <gccore.h>
#include "execgcdol.h"
#include "dolloader_dol.h" // generated in _/cache/data

#define BC      0x0000000100000100ULL   // title ID of GameCube bootloader
#define PI_CMD_REG      0xCC003024      // memory address set for GameCube mode
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
    // set baseAddress
    if (entrypoint > 0x80700000) {
        baseAddress = (u8*)0x80100000;
    } else {
        baseAddress = (u8*)0x80800000;
        memset((void*)0x80100000, 0, 32);
        DCFlushRange((char*)0x80100000, 32);
        ICInvalidateRange((char*)0x80100000, 32);
    }
    printf("swiss.dol  : %p -- %p\n", baseAddress, baseAddress+filesize);
    // read full Dol file into memory at baseAddress
    fseek(fp, 0, SEEK_SET);
    fread(baseAddress, filesize, 1, fp);
    DCFlushRange(baseAddress, filesize);
    ICInvalidateRange(baseAddress, filesize);
    return 0;
};

// boots a Dol from memory
int EXECGCDOL_BootLoaded() {
    /*// copy the integrated DolLoader into RAM
    printf("dL (ours) size: %d\n", dolloader_dol_size);
    memcpy((void*)0x80800000, dolloader_dol, dolloader_dol_size);
    DCFlushRange((char*)0x80800000, dolloader_dol_size);
    ICInvalidateRange((char*)0x80800000, dolloader_dol_size);
    printf("dL.dol     : 0x%8x -- 0x%8x\n", 0x80800000, 0x80800000+dolloader_dol_size);
    */// tell the cMIOS to load the DolLoader
    memset((void*)0x807FFFE0, 0, 32);
    strcpy((char*)0x807FFFE0, "gchomebrew dol");
    DCFlushRange((char*)0x807FFFE0, 32);
    ICInvalidateRange((char*)0x807FFFE0, 32);
    // trigger gc mode
    *(volatile unsigned int *)PI_CMD_REG |= 7;
    // launch
    return 0;
}
