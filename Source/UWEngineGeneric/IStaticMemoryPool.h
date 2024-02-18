/*
* StaticMemoryPool 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#pragma once

#include "UWEngineCommon/Defines.h"

interface IStaticMemoryPool
{
    virtual bool __stdcall Initialize(const size_t elementSize, const size_t numElementsPerBlock, const size_t numMaxElements) = 0;
    virtual void __stdcall Release() = 0;

    virtual void* __stdcall AllocateOrNull() = 0;
    virtual void __stdcall Free(void* pMemory) = 0;
    virtual void __stdcall Reset() = 0;
    virtual bool __stdcall IsValidMemory(const void* pMemory) const = 0;

    virtual size_t __stdcall GetElementSize() const = 0;
    virtual size_t __stdcall GetNumAllocElements() const = 0;
    virtual size_t __stdcall GetNumMaxElements() const = 0;
};

GLOBAL_FUNC bool __stdcall CreateStaticMemoryPool(IStaticMemoryPool** ppOutStaticMemoryPool);
GLOBAL_FUNC void __stdcall DestroyStaticMemoryPool(IStaticMemoryPool* pStaticMemoryPool);