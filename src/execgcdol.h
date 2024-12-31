#include <stdio.h>

int EXECGCDOL_LoadFile(FILE* fp);   // loads a GameCube Dol from open file to memory; returns:
                                    //    0: success
                                    //   -1: Dol file corrupted: header too small
                                    //   -2: Dol file corrupted: entrypoint is invalid pointer
void EXECGCDOL_LoadBooter(void);    // loads the DolLoader and signals to the cMIOS
