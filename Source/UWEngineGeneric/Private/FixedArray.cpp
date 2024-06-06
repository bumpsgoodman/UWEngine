/*
* FixedArray
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#include "Precompiled.h"
#include "IFixedArray.h"

class FixedArray final : public IFixedArray
{
public:
    FixedArray() = default;
    FixedArray(const FixedArray&) = delete;
    FixedArray& operator=(const FixedArray&) = delete;
    ~FixedArray();

    virtual bool    __stdcall   Initialize(const vsize elementSize, const vsize numMaxElements, const bool bAddressMode) override;
    virtual void    __stdcall   Release() override;
    virtual void    __stdcall   Clear() override;

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    virtual bool    __stdcall   PushBack(const void* pElementOrNull, const vsize elementSize) override;
    virtual bool    __stdcall   PopBack() override;

    // element가 nullptr이면 elementSize에 상관없이 공간만 예약
    virtual bool    __stdcall   Insert(const void* pElementOrNull, const vsize elementSize, const vsize index) override;
    virtual bool    __stdcall   Remove(const vsize index) override;

    virtual void*   __stdcall   GetBackOrNull() const override;
    virtual void*   __stdcall   GetElementOrNull(const vsize index) const override;
    virtual void*   __stdcall   GetElementsPtrOrNull() const override;
    virtual vsize   __stdcall   GetElementSize() const override;
    virtual vsize   __stdcall   GetNumMaxElements() const override;
    virtual vsize   __stdcall   GetNumElements() const override;

private:
    void* m_pElements = nullptr;

    vsize m_elementSize = 0;
    vsize m_numMaxElements = 0;
    vsize m_numElements = 0;

    bool m_bAddressMode = false;
};

FixedArray::~FixedArray()
{
    Release();
}

bool __stdcall FixedArray::Initialize(const vsize elementSize, const vsize numMaxElements, const bool bAddressMode)
{
    ASSERT(elementSize > 0, "elementSize == 0");
    ASSERT(numMaxElements > 0, "numMaxElements == 0");

    m_pElements = malloc(elementSize * numMaxElements);
    ASSERT(m_pElements != nullptr, "Failed to malloc elements");

    m_elementSize = elementSize;
    m_numMaxElements = numMaxElements;
    m_numElements = 0;

    m_bAddressMode = bAddressMode;

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

bool __stdcall FixedArray::PushBack(const void* pElementOrNull, const vsize elementSize)
{
    ASSERT(pElementOrNull == nullptr || elementSize == m_elementSize, "mismatch element size");

    bool bResult = false;

    if (m_numElements >= m_numMaxElements)
    {
        goto lb_return;
    }

    if (pElementOrNull != nullptr)
    {
        if (m_bAddressMode)
        {
            memcpy((char*)m_pElements + m_elementSize * m_numElements, &pElementOrNull, elementSize);
        }
        else
        {
            memcpy((char*)m_pElements + m_elementSize * m_numElements, pElementOrNull, elementSize);
        }
    }

    ++m_numElements;

    bResult = true;

lb_return:
    return bResult;
}

bool __stdcall FixedArray::PopBack()
{
    --m_numElements;
    return true;
}

bool __stdcall FixedArray::Insert(const void* pElementOrNull, const vsize elementSize, const vsize index)
{
    ASSERT(pElementOrNull == nullptr || elementSize == m_elementSize, "mismatch element size");
    ASSERT(index <= m_numElements, "Invalid index");

    bool bResult = false;

    if (m_numElements >= m_numMaxElements)
    {
        goto lb_return;
    }

    // element 한 칸씩 밀기
    const vsize len = m_numElements - index;
    char* pDst = (char*)m_pElements + m_elementSize * (index + 1);
    char* pSrc = pDst - m_elementSize;
    memmove(pDst, pSrc, m_elementSize * len);

    if (pElementOrNull != nullptr)
    {
        if (m_bAddressMode)
        {
            memcpy(pSrc, &pElementOrNull, elementSize);
        }
        else
        {
            memcpy(pSrc, pElementOrNull, elementSize);
        }
    }

    ++m_numElements;

    bResult = true;

lb_return:
    return bResult;
}

bool __stdcall FixedArray::Remove(const vsize index)
{
    ASSERT(index <= m_numElements, "Invalid index");

    // element 한 칸씩 당기기
    const vsize len = m_numElements - index;
    char* pDst = (char*)m_pElements + m_elementSize * index;
    const char* pSrc = pDst + m_elementSize;
    memmove(pDst, pSrc, m_elementSize * len);

    --m_numElements;

    return true;
}

void* __stdcall FixedArray::GetBackOrNull() const
{
    if (m_numElements == 0)
    {
        return nullptr;
    }

    if (m_bAddressMode)
    {
        return *(char**)m_pElements + m_elementSize * (m_numElements - 1);
    }

    return (char*)m_pElements + m_elementSize * (m_numElements - 1);
}

void* __stdcall FixedArray::GetElementOrNull(vsize index) const
{
    if (index >= m_numElements)
    {
        return nullptr;
    }

    if (m_bAddressMode)
    {
        return *(char**)m_pElements + m_elementSize * index;
    }

    return (char*)m_pElements + m_elementSize * index;
}

void* __stdcall FixedArray::GetElementsPtrOrNull() const
{
    return m_pElements;
}

vsize __stdcall FixedArray::GetElementSize() const
{
    return m_elementSize;
}

vsize __stdcall FixedArray::GetNumMaxElements() const
{
    return m_numMaxElements;
}

vsize __stdcall FixedArray::GetNumElements() const
{
    return m_numElements;
}

GLOBAL_FUNC bool __stdcall CreateFixedArray(IFixedArray** ppOutFixedArray)
{
    ASSERT(ppOutFixedArray != nullptr, "ppOutFixedArray == nullptr");

    IFixedArray* pFixedArray = new FixedArray;
    *ppOutFixedArray = pFixedArray;

    return true;
}

GLOBAL_FUNC void __stdcall DestroyFixedArray(IFixedArray* pFixedArrayOrNull)
{
    if (pFixedArrayOrNull != nullptr)
    {
        delete (FixedArray*)pFixedArrayOrNull;
    }
}