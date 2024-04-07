/*
* ChunkedMemoryPool 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.27
*/

#pragma once

#include "UWEngineCommon/Defines.h"

interface IChunkedMemoryPool
{
    virtual UWMETHOD(bool) Initialize(const vsize elementSize, const vsize numElementsPerBlock) PURE;
    virtual UWMETHOD(void) Release() PURE;

    virtual UWMETHOD(void*) AllocateOrNull() PURE;
    virtual UWMETHOD(void) Free(void* pMemory) PURE;
    virtual UWMETHOD(void) Reset() PURE;
    virtual UWMETHOD(bool) IsValidMemory(const void* pMemory) const PURE;

    virtual UWMETHOD(vsize) GetElementSize() const PURE;
    virtual UWMETHOD(vsize) GetNumAllocElements() const PURE;
    virtual UWMETHOD(vsize) GetNumElementsPerChunk() const PURE;
};

GLOBAL_FUNC UWMETHOD(bool) CreateChunkedMemoryPool(IChunkedMemoryPool** ppOutChunkedMemoryPool);
GLOBAL_FUNC UWMETHOD(void) DestroyChunkedMemoryPool(IChunkedMemoryPool* pChunkedMemoryPool);