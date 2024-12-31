#include <string.h>
#include <gccore.h>
#include "execgcdol.h"
#include "dolloader_dol.h" // generated in _/cache/data

#define PI_CMD_REG      0xCC003024      // memory address set for GameCube mode
#define ADDR_ENTRYPOINT 0xE0            // memory address of entrypoint in Dol header

int EXECGCDOL_LoadFile(FILE* fp) {
    u32 filesize = 0;   // size of Dol
    u32 entrypoint;     // address of main() in Dol
    u8* baseAddress;    // address to load Dol to in RAM
    // set filesize
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    if (filesize < 0x100) { return -1; }
    // set entrypoint
    fseek(fp, ADDR_ENTRYPOINT, SEEK_SET);
    fread(&entrypoint, 4, 1, fp);
    if (entrypoint < 0x80000000 || entrypoint >= 0x81800000) { return -2; }
    // set baseAddress
    if (entrypoint > 0x80700000) {
        baseAddress = (u8*)0x80100000;
    } else {
        baseAddress = (u8*)0x80A00000;
        memset((void*)0x80100000, 0, 32);
        DCFlushRange((char*)0x80100000, 32);
        ICInvalidateRange((char*)0x80100000, 32);
    }
    // read full Dol file into memory at baseAddress
    fseek(fp, 0, SEEK_SET);
    fread(baseAddress, filesize, 1, fp);
    DCFlushRange(baseAddress, filesize);
    ICInvalidateRange(baseAddress, filesize);
    return 0;
};

void EXECGCDOL_LoadBooter() {
    // copy the integrated DolLoader into RAM
    memcpy((void*)0x80800000, dolloader_dol, dolloader_dol_size);
    DCFlushRange((char*)0x80800000, dolloader_dol_size);
    ICInvalidateRange((char*)0x80800000, dolloader_dol_size);
    // signal to the cMIOS to execute the DolLoader
    strcpy((char*)0x807FFFE0, "gchomebrew dol");
    DCFlushRange((char*)0x807FFFE0, 32);
    ICInvalidateRange((char*)0x807FFFE0, 32);
    // trigger gc mode
    *(vu32*)PI_CMD_REG |= 7;
}
