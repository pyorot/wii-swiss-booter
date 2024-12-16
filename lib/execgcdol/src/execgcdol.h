#pragma once
#pragma GCC visibility push(default)
#include <stdio.h>

int EXECGCDOL_LoadFile(FILE* fp);   // loads a GameCube Dol from file to memory; returns 0
void EXECGCDOL_Signal(u32 signal);  // signals to CMIOS what to load (writes word to 0x80001800)

#pragma GCC visibility pop
