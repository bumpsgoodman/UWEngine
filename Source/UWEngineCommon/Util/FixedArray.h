/*
* FixedArray
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#pragma once

class FixedArray final
{
public:
    FixedArray() = default;
    FixedArray(const FixedArray&) = delete;
    FixedArray& operator=(const FixedArray&) = delete;
    ~FixedArray();

    bool    __stdcall   Initialize(const uint elementSize, const uint numMaxElements);
    void    __stdcall   Release();
    void    __stdcall   Clear();

    void    __stdcall   PushBack(const void* pElement, const uint elementSize);
    void    __stdcall   PopBack();

    void    __stdcall   Insert(const void* pElement, const uint elementSize, const uint index);
    void    __stdcall   Remove(const uint index);

    void*   __stdcall   GetBack() const;
    void*   __stdcall   GetElement(const uint index) const;
    void*   __stdcall   GetElementsPtr() const;
    uint    __stdcall   GetElementSize() const;
    uint    __stdcall   GetNumMaxElements() const;
    uint    __stdcall   GetNumElements() const;

private:
    char*   m_pElements = nullptr;

    uint    m_elementSize = 0;
    uint    m_numMaxElements = 0;
    uint    m_numElements = 0;
};