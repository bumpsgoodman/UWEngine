/*
* 해시 함수
*
* 작성자: bumpsgoodman
* 작성일: 2024.03.10
*/

#pragma once

inline uint32 __stdcall UWHash32(const char* pBytes, vsize length)
{
    uint32 hash = 0;

    if (length & 0x1)
    {
        hash += (uint32)(*(uint8*)pBytes);
        ++pBytes;
        --length;
    }

    if (length & 0x2)
    {
        hash += (uint32)(*(uint16*)pBytes);
        pBytes += 2;
        length -= 2;
    }

    for (vsize i = 0; i < length; ++i)
    {
        hash += (uint32)(*(uint32*)pBytes);
        pBytes += 4;
    }

    return hash;
}

inline uint32 __stdcall FNV1a32(const char* pBytes, const vsize length)
{
    static constexpr uint32 FNV_PRIME_32 = 16777619U;
    static constexpr uint32 FNV_OFFSET_32 = 2166136261U;

    uint32 hash = FNV_OFFSET_32;
    for (vsize i = 0; i < length; ++i)
    {
        hash = hash * FNV_PRIME_32;
        hash = hash + pBytes[i];
    }

    return hash;
}

inline uint64 __stdcall FNV1a64(const char* pBytes, const vsize length)
{
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