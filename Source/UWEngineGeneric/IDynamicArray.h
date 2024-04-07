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
    virtual UWMETHOD(bool) Initialize(const vsize elementSize, const vsize numDefaultElements) PURE;
    virtual UWMETHOD(void) Release() PURE;
    virtual UWMETHOD(void) Clear() PURE;

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    virtual UWMETHOD(bool) PushBack(const void* pElementOrNull, const vsize elementSize) PURE;
    virtual UWMETHOD(bool) PopBack() PURE;

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    virtual UWMETHOD(bool) Insert(const void* pElementOrNull, const vsize elementSize, const vsize index) PURE;
    virtual UWMETHOD(bool) Remove(const vsize index) PURE;

    virtual UWMETHOD(void*) GetBackOrNull() const PURE;
    virtual UWMETHOD(void*) GetElementOrNull(const vsize index) const PURE;
    virtual UWMETHOD(void*) GetElementsOrNull() const PURE;
    virtual UWMETHOD(vsize) GetElementSize() const PURE;
    virtual UWMETHOD(vsize) GetNumMaxElements() const PURE;
    virtual UWMETHOD(vsize) GetNumElements() const PURE;
};

GLOBAL_FUNC UWMETHOD(bool) CreateDynamicArray(IDynamicArray** ppOutDynamicArray);
GLOBAL_FUNC UWMETHOD(void) DestroyDynamicArray(IDynamicArray* pDynamicArray);