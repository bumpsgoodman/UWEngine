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

bool __stdcall TimerInit(Timer* pTimer);
float __stdcall TimerGetTime(const Timer* pTimer);