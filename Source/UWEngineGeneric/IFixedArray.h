/*
* FixedArray 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#pragma once

#include "UWEngineCommon/Defines.h"

interface IFixedArray
{
    virtual bool __stdcall Initialize(const vsize elementSize, const vsize numMaxElements) = 0;
    virtual void __stdcall Release() = 0;
    virtual void __stdcall Clear() = 0;

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    virtual bool __stdcall PushBack(const void* pElementOrNull, const vsize elementSize) = 0;
    virtual bool __stdcall PopBack() = 0;

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    virtual bool __stdcall Insert(const void* pElementOrNull, const vsize elementSize, const vsize index) = 0;
    virtual bool __stdcall Remove(const vsize index) = 0;

    virtual void* __stdcall GetBackOrNull() const = 0;
    virtual void* __stdcall GetElementOrNull(const vsize index) const = 0;
    virtual void* __stdcall GetElementsPtrOrNull() const = 0;
    virtual vsize __stdcall GetElementSize() const = 0;
    virtual vsize __stdcall GetNumMaxElements() const = 0;
    virtual vsize __stdcall GetNumElements() const = 0;
};

GLOBAL_FUNC bool __stdcall CreateFixedArray(IFixedArray** ppOutFixedArray);
GLOBAL_FUNC void __stdcall DestroyFixedArray(IFixedArray* pFixedArray);