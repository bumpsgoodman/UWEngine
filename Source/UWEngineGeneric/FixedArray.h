﻿/*
* FixedArray
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#pragma once

class FixedArray
{
public:
    FixedArray() = default;
    FixedArray(const FixedArray&) = delete;
    FixedArray& operator=(const FixedArray&) = delete;
    FixedArray(FixedArray&&) = default;
    FixedArray& operator=(FixedArray&&) = default;
    ~FixedArray();

    bool    __stdcall   Initialize(const vsize elementSize, const vsize numMaxElements);
    void    __stdcall   Release();
    void    __stdcall   Clear();

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    bool    __stdcall   PushBack(const void* pElementOrNull, const vsize elementSize);
    bool    __stdcall   PopBack();

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    bool    __stdcall   Insert(const void* pElementOrNull, const vsize elementSize, const vsize index);
    bool    __stdcall   Remove(const vsize index);

    void*   __stdcall   GetBackOrNull() const;
    void*   __stdcall   GetElementOrNull(const vsize index) const;
    void*   __stdcall   GetElementsPtrOrNull() const;
    vsize   __stdcall   GetElementSize() const;
    vsize   __stdcall   GetNumMaxElements() const;
    vsize   __stdcall   GetNumElements() const;

private:
    void* m_pElements = nullptr;

    vsize m_elementSize = 0;
    vsize m_numMaxElements = 0;
    vsize m_numElements = 0;
};