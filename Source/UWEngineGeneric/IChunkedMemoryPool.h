/*
* ChunkedMemoryPool 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.27
*/

#pragma once

interface IChunkedMemoryPool
{
    virtual bool    __stdcall   Initialize(const uint elementSize, const uint numElementsPerBlock) = 0;
    virtual void    __stdcall   Release() = 0;

    virtual void*   __stdcall   Allocate() = 0;
    virtual void    __stdcall   Free(void* pMemory) = 0;
    virtual void    __stdcall   Reset() = 0;
    virtual bool    __stdcall   IsValidMemory(const void* pMemory) const = 0;

    virtual uint   __stdcall    GetElementSize() const = 0;
    virtual uint   __stdcall    GetNumAllocElements() const = 0;
    virtual uint   __stdcall    GetNumElementsPerChunk() const = 0;
};

GLOBAL_FUNC bool __stdcall CreateChunkedMemoryPool(IChunkedMemoryPool** ppOutChunkedMemoryPool);
GLOBAL_FUNC void __stdcall DestroyChunkedMemoryPool(IChunkedMemoryPool* pChunkedMemoryPoolOrNull);