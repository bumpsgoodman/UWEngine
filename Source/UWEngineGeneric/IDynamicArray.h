/*
* DynamicArray 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.23
*/

#pragma once

#include "UWEngineCommon/Defines.h"

interface IDynamicArray
{
    virtual bool __stdcall Initialize(const size_t elementSize, const size_t numDefaultElements) = 0;
    virtual void __stdcall Release() = 0;
    virtual void __stdcall Clear() = 0;

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    virtual bool __stdcall PushBack(const void* pElementOrNull, const size_t elementSize) = 0;
    virtual bool __stdcall PopBack() = 0;

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    virtual bool __stdcall Insert(const void* pElementOrNull, const size_t elementSize, const size_t index) = 0;
    virtual bool __stdcall Remove(const size_t index) = 0;

    virtual void* __stdcall GetBackOrNull() const = 0;
    virtual void* __stdcall GetElementOrNull(const size_t index) const = 0;
    virtual void* __stdcall GetElementsOrNull() const = 0;
    virtual size_t __stdcall GetElementSize() const = 0;
    virtual size_t __stdcall GetNumMaxElements() const = 0;
    virtual size_t __stdcall GetNumElements() const = 0;
};

GLOBAL_FUNC bool __stdcall CreateDynamicArray(IDynamicArray** ppOutDynamicArray);
GLOBAL_FUNC void __stdcall DestroyDynamicArray(IDynamicArray* pDynamicArray);