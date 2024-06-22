/*
* FixedArray
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#include "Precompiled.h"
#include "UWEngineCommon/Util/FixedArray.h"

FixedArray::~FixedArray()
{
    Release();
}

bool __stdcall FixedArray::Initialize(const uint elementSize, const uint numMaxElements)
{
    m_pElements = (char*)malloc(elementSize * numMaxElements);

    m_elementSize = elementSize;
    m_numMaxElements = numMaxElements;
    m_numElements = 0;

    return true;
}

void __stdcall FixedArray::Release()
{
    SAFE_FREE(m_pElements);
}

void __stdcall FixedArray::Clear()
{
    m_numElements = 0;
}

void* __stdcall FixedArray::PushBack(const void* pElement, const uint elementSize)
{
    if (m_elementSize != elementSize)
    {
        CRASH();
    }

    if (m_numElements >= m_numMaxElements)
    {
        CRASH();
    }

    void* pDst = m_pElements + m_elementSize * m_numElements;
    memcpy(pDst, pElement, elementSize);

    ++m_numElements;
    return pDst;
}

void __stdcall FixedArray::PopBack()
{
    if (m_numElements == 0)
    {
        CRASH();
    }

    --m_numElements;
}

void* __stdcall FixedArray::Insert(const void* pElement, const uint elementSize, const uint index)
{
    if (m_elementSize != elementSize)
    {
        CRASH();
    }

    if (index > m_numElements || m_numElements >= m_numMaxElements)
    {
        CRASH();
    }

    // element 한 칸씩 밀기
    const uint len = m_numElements - index;
    char* pDst = m_pElements + m_elementSize * (index + 1);
    char* pSrc = pDst - m_elementSize;
    memmove(pDst, pSrc, m_elementSize * len);

    memcpy(pSrc, pElement, elementSize);

    ++m_numElements;
    return pDst;
}

void __stdcall FixedArray::Remove(const uint index)
{
    if (index > m_numElements)
    {
        CRASH();
    }

    // element 한 칸씩 당기기
    const uint len = m_numElements - index;
    char* pDst = m_pElements + m_elementSize * index;
    const char* pSrc = pDst + m_elementSize;
    memmove(pDst, pSrc, m_elementSize * len);

    --m_numElements;
}

void* __stdcall FixedArray::GetBack() const
{
    if (m_numElements == 0)
    {
        CRASH();
    }

    return m_pElements + m_elementSize * (m_numElements - 1);
}

void* __stdcall FixedArray::GetElement(const uint index) const
{
    if (index >= m_numElements)
    {
        CRASH();
    }

    return m_pElements + m_elementSize * index;
}

void* __stdcall FixedArray::GetElementsPtr() const
{
    return m_pElements;
}

uint __stdcall FixedArray::GetElementSize() const
{
    return m_elementSize;
}

uint __stdcall FixedArray::GetNumMaxElements() const
{
    return m_numMaxElements;
}

uint __stdcall FixedArray::GetNumElements() const
{
    return m_numElements;
}