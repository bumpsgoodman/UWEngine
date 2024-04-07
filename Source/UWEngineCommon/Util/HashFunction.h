/*
* 해시 함수
*
* 작성자: bumpsgoodman
* 작성일: 2024.03.10
*/

#pragma once

#include "../Defines.h"

inline UWMETHOD(uint32) FNV1a32(const char* pBytes, const vsize length)
{
    ASSERT(pBytes != nullptr, "pBytes == nullptr");

    static constexpr uint32 FNV_PRIME_32 = 16777619U;
    static constexpr uint32 FNV_OFFSET_32 = 2166136261U;

    uint32 hash = FNV_OFFSET_32;
    for (uint32 i = 0; i < length; ++i)
    {
        hash = hash * FNV_PRIME_32;
        hash = hash + pBytes[i];
    }

    return hash;
}

inline UWMETHOD(uint64) FNV1a64(const char* pBytes, const vsize length)
{
    ASSERT(pBytes != nullptr, "pBytes == nullptr");

    static constexpr uint64 FNV_PRIME = 1099511628211U;
    static constexpr uint64 FNV_OFFSET_BASIS = 14695981039346656037U;

    uint64 hash = FNV_OFFSET_BASIS;
    for (vsize i = 0; i < length; ++i)
    {
        hash = hash ^ pBytes[i];
        hash = hash * FNV_PRIME;
    }

    return hash;
}