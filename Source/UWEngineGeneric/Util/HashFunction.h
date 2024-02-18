/*
* 해시 함수
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.21
*/

#pragma once

#include "UWEngineCommon/Defines.h"

GLOBAL_FUNC uint32_t __stdcall FNV1a32(const char* pBytes, const size_t length);
GLOBAL_FUNC uint64_t __stdcall FNV1a64(const char* pBytes, const size_t length);