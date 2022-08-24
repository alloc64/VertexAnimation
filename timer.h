/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

class xmTimer {
public:
    xmTimer() {};

    ~xmTimer() {}


    unsigned int iFps;
    unsigned int iMaxFps;
    unsigned int iCountTick;
    unsigned int iMinCountTick;
    unsigned int iStartTick;
    float fFrameInterval;
    float fMilliseconds;

    double xmGetSystemTime();

    float xmGetMilliseconds() {
        return fMilliseconds;
    }

    void xmCalculateFrameRate(void);

    void xmPrintStats();

private:

};

#endif	

