#include <gccore.h>
#include "rtip.h"
#include "booter.h"
#include "console.h"

typedef enum {
    MIOS_UNKNOWN, MIOS_ORIGINAL, CMIOS_STANDARD, CMIOS_OTHER, CMIOS_PAYLOAD,
} MIOSOutcome;

typedef struct {
    char* text; MIOSOutcome outcome; int error;
} MIOSResult;

// identifies installed MIOS
static fstats filestat  ATTRIBUTE_ALIGN(32);     // requires alignment, so static
static u32 values[4]    ATTRIBUTE_ALIGN(32);
MIOSResult _checkMIOS() {
    // find mios file via contentID (filename), which increased by 2 each revision
    // (a mios extracts + decrypts to 2 sequential content files; we want the second/higher one)
    const char miosContentIDs[6] = {'c', 'a', '8', '6', '4', '2'};
    char filename[48] = MIOS_DIR "/0000000x.app";
    s32 fd = 0;
    for (int i = 0; i < sizeof(miosContentIDs) / sizeof(char); i++) {
        filename[40] = miosContentIDs[i]; // replaces "x" above
        fd = ISFS_Open(filename, ISFS_OPEN_READ);
        if (fd != -106) { break; }
    }
    if (fd == -106) { errStr = "No MIOS found; please install one."; return (MIOSResult){"", 0, ERR_MIOS_NOTFOUND }; }
    else if (fd < 0) { errStr = "(ISFS_Open)"; return (MIOSResult){"", 0, fd}; }
    // read this (2nd) content file: want word at file+0x8
    ret = ISFS_GetFileStats(fd, &filestat);
    if (ret != 0) { errStr = "(ISFS_GetFileStats)"; return (MIOSResult){"", 0, ret}; }
    if (filestat.file_length <= 16) { errStr = "Failed to read MIOS file: too small";  return (MIOSResult){"", 0, ERR_MIOS_SIZE}; }
    ret = ISFS_Read(fd, values, sizeof(values));
    if (ret < 0) { errStr = "(ISFS_Read)";  return (MIOSResult){"", 0, ret}; }
    // identify mios by file+0x8 (some sort of size, close to (2nd) content filesize)
    char* text = "[unknown MIOS]";
    MIOSOutcome outcome = MIOS_UNKNOWN;
    switch (values[2]) {    // values[2] = *(file+0x8); we will hope these are uniquely keyed
        case 0x000407B4:    text = "cMIOS v10 WiiGator (WiiPower)"; outcome = CMIOS_STANDARD;   break;
        case 0x0005F3B4:    text = "cMIOS v4 WiiGator (GCBL 0.2)";  outcome = CMIOS_OTHER;      break;
        case 0x000545F4:    text = "cMIOS v4 Waninkoko (rev5)";     outcome = CMIOS_PAYLOAD;    break;
        case 0x0018895C:    text = "DIOS MIOS Lite v1.3";           outcome = CMIOS_PAYLOAD;    break;
        case 0x0002C0D4:    text = "MIOS v10";                      outcome = MIOS_ORIGINAL;    break;
        case 0x0002BF50:    text = "MIOS v9";                       outcome = MIOS_ORIGINAL;    break;
        case 0x0002BDC8:    text = "MIOS v8";                       outcome = MIOS_ORIGINAL;    break;
        case 0x0002B954:    text = "MIOS v5";                       outcome = MIOS_ORIGINAL;    break;
        case 0x0002CB94:    text = "MIOS v4";                       outcome = MIOS_ORIGINAL;    break;
    }
    return (MIOSResult){text, outcome, 0};
}

int testMIOS() {
    // live-patch ios for full nand access
    ret = IosPatch_RUNTIME(PATCH_WII, PATCH_ISFS_PERMISSIONS, false); // returns -5 if no AHB access, -7 if patch failed
    if (ret == -5) { errStr = "Access denied; is the original meta.xml in the app's folder?"; return ERR_DENIED; }
    else if (ret < 0) { errStr = "(IosPatch_RUNTIME)"; return ret; }
    // init nand
    ret = ISFS_Initialize();
    if (ret != 0) { errStr = "(ISFS_Initialize)"; return ret; }
    // run mios test + deinit nand
    MIOSResult mios = _checkMIOS();
    ret = mios.error;
    ISFS_Deinitialize();
    if (mios.error != 0) { return mios.error; }
    // interpret test results
    printf("- MIOS detected: %s.\n", mios.text);
    if (mios.outcome == CMIOS_STANDARD) {
        printf(CON_GREEN("- This is the standard cMIOS and can load GameCube homebrew.\n"));
    } else {
        if (mios.outcome == CMIOS_OTHER) {
            printf(CON_YELLOW("- This is a non-standard cMIOS that can probably load homebrew.\n"));
        } else if (mios.outcome == MIOS_UNKNOWN) {
            printf(CON_YELLOW("- This MIOS is DIOS MIOS or something rare; idk if it can load homebrew.\n"));
        } else if (mios.outcome == CMIOS_PAYLOAD) {
            printf(CON_YELLOW("- This MIOS is hacked to load its own homebrew; it cannot load other homebrew.\n"));
        } else if (mios.outcome == MIOS_ORIGINAL) {
            printf(CON_YELLOW("- This is an original unmodded MIOS that cannot load homebrew.\n"));
        }
        printf("- Visit this app's GitHub readme to install the standard cMIOS for homebrew.\n");
    }
    if (mios.outcome == MIOS_ORIGINAL || mios.outcome == CMIOS_PAYLOAD) { errStr = "Incompatible MIOS"; return ERR_MIOS_LOOKUP; }
    return 0;
}
