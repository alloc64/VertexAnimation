/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include <windows.h>
#include "timer.h"

double xmTimer::xmGetSystemTime() {
    long long freq, time;
    SetThreadAffinityMask(GetCurrentThread(), 1);
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&freq));
    double factor = 1000 / double((freq) ? freq : 1);
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&time));
    return time * factor;
}

void xmTimer::xmCalculateFrameRate(void) {

    static int iFramesPerSecond = 0;
    static float fLastTime = 0.0f;
    static float fFrameTime = 0.0f;
    unsigned long iCurrentTime = (unsigned long) xmGetSystemTime();
    float fSecond = iCurrentTime * 0.001f;

    fFrameInterval = fSecond - fFrameTime;
    fFrameTime = fSecond;

    ++iFramesPerSecond;

    if (fSecond - fLastTime > 1.0f) {
        fLastTime = fSecond;
        iFps = iFramesPerSecond;
        iFramesPerSecond = 0;
        if (iFps > iMaxFps) iMaxFps = iFps;
    }
}
