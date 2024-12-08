#include <string.h>
#include "booter.h"
#include "console.h"
#include "systitver.h"

int testMIOS() {
    // load BC to SysTitVer
    ret = STV_LoadTitle(256, false);
    if      (ret == -5 ) { errStr = "Access denied; is the original meta.xml in the app's folder?"; return ERR_DENIED; }
    else if (ret == -33) { errStr = "No BC found; please install one."; return ERR_MIOS_NOTFOUND; }
    else if (ret == -37) { errStr = "BC corrupt; please reinstall it."; return ERR_MIOS_CORRUPT; }
    else if (ret < 0   ) { errStr = "(STV_LoadTitle(256))"; return ret; }
    // integrity-test BC
    ret = STV_VerifyCurrentTitle(false);
    if (ret == -34 || (-500 < ret && ret <= -400)) { errStr = "BC failed integrity test; please reinstall it."; return ERR_MIOS_CORRUPT; }
    else if (ret < 0) { errStr = "BC failed integrity test (reason unknown)"; return ret; }
    // identify BC
    SysTitTag stt = STV_IdentifyCurrentTitle(false);
    printf("- BC detected: v%d | %s.\n", stt.rev, stt.name);
    if (strcmp(stt.name, "official") == 0) {
        printf(CON_GREEN("- Official BC works with GameCube homebrew.\n"));
    } else {
        printf(CON_YELLOW("- This is a modded BC; install an original for better compatibility.\n"));
    }
    
    // load MIOS to SysTitVer
    ret = STV_LoadTitle(257, false);    // title = MIOS
    if      (ret == -33) { errStr = "No MIOS found; please install one."; return ERR_MIOS_NOTFOUND; }
    else if (ret == -37) { errStr = "MIOS corrupt; please reinstall it."; return ERR_MIOS_CORRUPT; }
    else if (ret < 0   ) { errStr = "(STV_LoadTitle(257))"; return ret; }
    // integrity-test MIOS
    ret = STV_VerifyCurrentTitle(false);
    if (ret == -34 || (-500 < ret && ret <= -400)) { errStr = "MIOS failed integrity test; please reinstall it."; return ERR_MIOS_CORRUPT; }
    else if (ret < 0) { errStr = "MIOS failed integrity test (reason unknown)"; return ret; }
    // identify MIOS
    stt = STV_IdentifyCurrentTitle(false);
    printf("- MIOS detected: v%d | %s.\n", stt.rev, stt.name);
    if (stt.rev == 10 && strcmp(stt.name, "WiiGator (WiiPower)") == 0) {
        printf(CON_GREEN("- This is the standard cMIOS and can load GameCube homebrew.\n"));
        ret = 0;
    } else {
        if (strcmp(stt.name, "official") == 0) {
            printf(CON_YELLOW("- This is an original unmodded MIOS that cannot load homebrew.\n"));
            errStr = "Incompatible MIOS"; ret = ERR_MIOS_LOOKUP;
        } else if (
            strcmp(stt.name, "WiiGator (GCBL 0.2)") == 0
         || strcmp(stt.name, "Waninkoko (rev5)"   ) == 0
         || strcmp(stt.name, "DIOS MIOS Lite v1.3") == 0
        ) {
            printf(CON_YELLOW("- This MIOS is hacked to load its own homebrew; it cannot load other homebrew.\n"));
            errStr = "Incompatible MIOS"; ret = ERR_MIOS_LOOKUP;
        } else {
            printf(CON_YELLOW("- This MIOS is DIOS MIOS or something rare; idk if it can load homebrew.\n"));
            ret = 0;
        }
        printf("- Visit this app's GitHub readme to install the standard cMIOS for homebrew.\n");
    }
    return ret;
}
