/* This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this software, either in source code form or
as a compiled binary, for any purpose, commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this software dedicate any and all copyright
interest in the software to the public domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an overt act of relinquishment in perpetuity
of all present and future rights to this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "ntp.h"

bool setsysInternetTimeSyncIsOn() {
    Result rs = setsysInitialize();
    if (R_FAILED(rs)) {
        printf("       setsysInitialize failed, %x\n", rs);
        return false;
    }

    bool internetTimeSyncIsOn;
    rs = setsysIsUserSystemClockAutomaticCorrectionEnabled(&internetTimeSyncIsOn);
    setsysExit();
    if (R_FAILED(rs)) {
        printf("       Unable to detect if Internet time sync is enabled, %x\n", rs);
        return false;
    }

    return internetTimeSyncIsOn;
}

/*

Type   | SYNC | User | Local | Network
=======|======|======|=======|========
User   |      |      |       |
-------+------+------+-------+--------
Menu   |      |  *   |   X   |
-------+------+------+-------+--------
System |      |      |       |   X
-------+------+------+-------+--------
User   |  ON  |      |       |
-------+------+------+-------+--------
Menu   |  ON  |      |       |
-------+------+------+-------+--------
System |  ON  |  *   |   *   |   X

*/
TimeServiceType __nx_time_service_type = TimeServiceType_System;
bool setNetworkSystemClock(time_t time) {
    Result rs = timeSetCurrentTime(TimeType_NetworkSystemClock, (uint64_t)time);
    if (R_FAILED(rs)) {
        printf("       timeSetCurrentTime failed with %x\n", rs);
        return false;
    }
    printf("\r       Successfully set NetworkSystemClock.\n");
    printf("\r     ----------------------------------------\n\n");
    return true;
}






int main(int argc, char* argv[]) {
    consoleInit(NULL);
	// Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    printf("\n       SwitchTime v0.1.1 MOD\n");

    if (!setsysInternetTimeSyncIsOn()) {
        consoleExit(NULL);
        return 0;
    }
    printf(
        "\n\n\n"
        "                 MINUS/PLUS: Change Minute\n"
        "                    DOWN/UP: Change Hour\n"
        "                 LEFT/RIGHT: Change Day\n"
        "                        L/R: Change Week\n"
        "                      ZL/ZR: Change Month\n"
        "                        Y/X: Change Year\n"
        "\n"
        "                          A: Save Time\n"
        "                       HOME: Exit\n\n\n\n");

    int dayChange = 0, hourChange = 0, minuteChange = 0;

    // Main loop
    while (appletMainLoop()) {
		padUpdate(&pad);
		u64 kDown = padGetButtonsDown(&pad);
        
        //We're running this as an applet for overwriting the user page, so "quitting" isn't a thing i this version
        //if (kDown & HidNpadButton_PLUS) {
        //    consoleExit(NULL);
        //    return 0;  // return to hbmenu
        //}
        
        if (kDown & HidNpadButton_Minus) {
            minuteChange -= 1;
        }
        
        if (kDown & HidNpadButton_Plus) {
            minuteChange += 1;
        }
        
        if (kDown & HidNpadButton_L) {
            dayChange -= 7;
        }
        
        if (kDown & HidNpadButton_R) {
            dayChange += 7;
        }
        if (kDown & HidNpadButton_ZR) {
            dayChange += 28;
        }
        
        if (kDown & HidNpadButton_ZL) {
            dayChange -= 28;
        }
        
        if (kDown & HidNpadButton_X) {
            dayChange += 365;
        }
        
        if (kDown & HidNpadButton_Y) {
            dayChange -= 365;
        }
        
        
        time_t currentTime;
        Result rs = timeGetCurrentTime(TimeType_UserSystemClock, (u64*)&currentTime);
        if (R_FAILED(rs)) {
            consoleExit(NULL);
			return 0;
        }
        
        struct tm* p_tm_timeToSet = localtime(&currentTime);
        p_tm_timeToSet->tm_mday += dayChange;
        p_tm_timeToSet->tm_hour += hourChange;
        p_tm_timeToSet->tm_min += minuteChange;
        time_t timeToSet = mktime(p_tm_timeToSet);
        
        if (kDown & HidNpadButton_A) {
            printf("\n\n");
            if (setNetworkSystemClock(timeToSet)) {
				dayChange = 0;
				hourChange = 0;
				minuteChange = 0;
		    }
        }
        
        
        if (kDown & HidNpadButton_Left) {
            dayChange--;
        } else if (kDown & HidNpadButton_Right) {
            dayChange++;
        } else if (kDown & HidNpadButton_Down) {
            hourChange--;
        } else if (kDown & HidNpadButton_Up) {
            hourChange++;
        }
        
        char timeToSetStr[25];
        strftime(timeToSetStr, sizeof timeToSetStr, "%a %b %d %I:%M %P %G", p_tm_timeToSet);
        printf("\r       Time to set: %s", timeToSetStr);
        consoleUpdate(NULL);
    }
}
