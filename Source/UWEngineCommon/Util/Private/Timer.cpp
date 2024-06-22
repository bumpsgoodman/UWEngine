/*
* 타이머
*
* 작성자: bumpsgoodman
* 작성일: 2024.03.10
*/

#include "Precompiled.h"
#include "UWEngineCommon/Util/Timer.h"

bool __stdcall TimerInit(Timer* pTimer)
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&pTimer->Frequency);
    QueryPerformanceCounter((LARGE_INTEGER*)&pTimer->PrevCounter);

    return true;
}

float __stdcall TimerGetTime(const Timer* pTimer)
{
    uint64 curCounter;
    QueryPerformanceCounter((LARGE_INTEGER*)&curCounter);

    const double clock = (double)(curCounter - pTimer->PrevCounter);
    const double milliSecond = clock / ((double)pTimer->Frequency / 1000.0);
    return (float)milliSecond;
}