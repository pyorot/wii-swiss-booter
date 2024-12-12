#include <stdio.h>

int EXECGCDOL_LoadFile(FILE* fp);   // loads a GameCube Dol from file to memory; returns:
                                    //    0: success
                                    //   -1: Dol file corrupted: header too small
                                    //   -2: Dol file corrupted: entrypoint is invalid pointer
int EXECGCDOL_BootLoaded(void);     // boots a GameCube Dol from memory; returns:
                                    //    0: success (doesn't really return at all)
                                    //    *: ES errors (https://wiibrew.org/wiki//dev/es)
