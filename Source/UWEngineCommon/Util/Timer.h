/*
* 타이머
*
* 작성자: bumpsgoodman
* 작성일: 2024.03.10
*/

#pragma once

struct Timer
{
    uint64 Frequency;
    uint64 PrevCounter;
};

inline bool __stdcall TimerInit(Timer* pTimer)
{
    ASSERT(pTimer != nullptr, "pTimer == nullptr");

    QueryPerformanceFrequency((LARGE_INTEGER*)&pTimer->Frequency);
    QueryPerformanceCounter((LARGE_INTEGER*)&pTimer->PrevCounter);

    return true;
}

inline float __stdcall TimerGetTime(const Timer* pTimer)
{
    ASSERT(pTimer != nullptr, "pTimer == nullptr");

    uint64 curCounter;
    QueryPerformanceCounter((LARGE_INTEGER*)&curCounter);

    const double clock = (double)(curCounter - pTimer->PrevCounter);
    const double milliSecond = clock / ((double)pTimer->Frequency / 1000.0);
    return (float)milliSecond;
}