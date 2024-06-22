/*
* 해시 함수
*
* 작성자: bumpsgoodman
* 작성일: 2024.03.10
*/

#pragma once

uint32 __stdcall UWHash32(const char* pBytes, uint length);
uint32 __stdcall FNV1a32(const char* pBytes, const uint length);