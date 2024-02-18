/*
* 해시 함수
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.21
*/

#include "Precompiled.h"
#include "Util/HashFunction.h"

uint32_t __stdcall FNV1a32(const char* pBytes, const size_t length)
{
    ASSERT(pBytes != NULL, "pBytes == NULL");

    static constexpr uint32_t FNV_PRIME_32 = 16777619U;
    static constexpr uint32_t FNV_OFFSET_32 = 2166136261U;

    uint32_t hash = FNV_OFFSET_32;
    for (uint32_t i = 0; i < length; ++i)
    {
        hash = hash * FNV_PRIME_32;
        hash = hash + pBytes[i];
    }

    return hash;
}

uint64_t __stdcall FNV1a64(const char* pBytes, const size_t length)
{
    ASSERT(pBytes != NULL, "pBytes == NULL");

    static constexpr uint64_t FNV_PRIME = 1099511628211U;
    static constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037U;

    uint64_t hash = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < length; ++i)
    {
        hash = hash ^ pBytes[i];
        hash = hash * FNV_PRIME;
    }

    return hash;
}