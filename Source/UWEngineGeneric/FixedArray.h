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

    bool    __stdcall   Initialize(const vsize elementSize, const vsize numMaxElements);
    void    __stdcall   Release();
    void    __stdcall   Clear();

    void    __stdcall   PushBack(const void* pElement, const vsize elementSize);
    void    __stdcall   PopBack();

    void    __stdcall   Insert(const void* pElement, const vsize elementSize, const vsize index);
    void    __stdcall   Remove(const vsize index);

    void*   __stdcall   GetBack() const;
    void*   __stdcall   GetElement(const vsize index) const;
    void*   __stdcall   GetElements() const;
    vsize   __stdcall   GetElementSize() const;
    vsize   __stdcall   GetNumMaxElements() const;
    vsize   __stdcall   GetNumElements() const;

private:
    char* m_pElements = nullptr;

    vsize m_elementSize = 0;
    vsize m_numMaxElements = 0;
    vsize m_numElements = 0;
};