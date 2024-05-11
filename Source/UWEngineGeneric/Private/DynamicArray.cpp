/*
* DynamicArray
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#include "Precompiled.h"
#include "IDynamicArray.h"

class DynamicArray final : public IDynamicArray
{
public:
    DynamicArray() = default;
    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;
    DynamicArray(DynamicArray&&) = default;
    DynamicArray& operator=(DynamicArray&&) = default;
    ~DynamicArray();

    virtual bool    __stdcall   Initialize(const vsize elementSize, const vsize numDefaultElements) override;
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
    virtual void*   __stdcall   GetElementsOrNull() const override;
    virtual vsize   __stdcall   GetElementSize() const override;
    virtual vsize   __stdcall   GetNumMaxElements() const override;
    virtual vsize   __stdcall   GetNumElements() const override;

private:
    bool            __stdcall   expand();

private:
    void* m_pElements = nullptr;

    vsize m_elementSize = 0;
    vsize m_numElements = 0;
    vsize m_numMaxElements = 0;
};

DynamicArray::~DynamicArray()
{
    Release();
}

bool __stdcall DynamicArray::Initialize(const vsize elementSize, const vsize numDefaultElements)
{
    ASSERT(elementSize > 0, "elementSize == 0");
    ASSERT(numDefaultElements > 0, "numDefaultElements == 0");

    Release();

    m_pElements = malloc(elementSize * numDefaultElements);
    ASSERT(m_pElements != nullptr, "Failed to malloc elements");

    m_elementSize = elementSize;
    m_numElements = 0;
    m_numMaxElements = numDefaultElements;

    return true;
}

void __stdcall DynamicArray::Release()
{
    SAFE_FREE(m_pElements);
}

void __stdcall DynamicArray::Clear()
{
    m_numElements = 0;
}

bool __stdcall DynamicArray::PushBack(const void* pElementOrNull, const vsize elementSize)
{
    ASSERT(pElementOrNull == nullptr || elementSize == m_elementSize, "mismatch element size");

    bool bResult = false;

    if (m_numElements >= m_numMaxElements)
    {
        if (!expand())
        {
            goto lb_return;
        }
    }

    if (pElementOrNull != nullptr)
    {
        memcpy((char*)m_pElements + m_elementSize * m_numElements, pElementOrNull, elementSize);
    }

    ++m_numElements;

    bResult = true;

lb_return:
    return bResult;
}

bool __stdcall DynamicArray::PopBack()
{
    --m_numElements;
    return true;
}

bool __stdcall DynamicArray::Insert(const void* pElementOrNull, const vsize elementSize, const vsize index)
{
    ASSERT(pElementOrNull == nullptr || elementSize == m_elementSize, "mismatch element size");
    ASSERT(index <= m_numElements, "Invalid index");

    bool bResult = false;

    if (m_numElements >= m_numMaxElements)
    {
        if (!expand())
        {
            goto lb_return;
        }
    }

    // element 한 칸씩 밀기
    const vsize len = m_numElements - index;
    char* pDst = (char*)m_pElements + m_elementSize * (index + 1);
    char* pSrc = pDst - m_elementSize;
    memmove(pDst, pSrc, m_elementSize * len);

    if (pElementOrNull != nullptr)
    {
        memcpy(pSrc, pElementOrNull, elementSize);
    }

    ++m_numElements;

    bResult = true;

lb_return:
    return bResult;
}

bool __stdcall DynamicArray::Remove(const vsize index)
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

bool __stdcall DynamicArray::expand()
{
    bool bResult = false;

    void* pElements = malloc(m_elementSize * m_numMaxElements * 2);
    ASSERT(pElements != nullptr, "Failed to malloc new elements");

    memcpy(pElements, m_pElements, m_elementSize * m_numMaxElements);

    free(m_pElements);

    m_pElements = pElements;
    m_numMaxElements *= 2;

    return true;
}

void* __stdcall DynamicArray::GetBackOrNull() const
{
    if (m_numElements == 0)
    {
        return nullptr;
    }

    return (char*)m_pElements + m_elementSize * (m_numElements - 1);
}

void* __stdcall DynamicArray::GetElementOrNull(vsize index) const
{
    if (index >= m_numElements)
    {
        return nullptr;
    }

    return (char*)m_pElements + m_elementSize * index;
}

void* __stdcall DynamicArray::GetElementsOrNull() const
{
    return m_pElements;
}

vsize __stdcall DynamicArray::GetElementSize() const
{
    return m_elementSize;
}

vsize __stdcall DynamicArray::GetNumMaxElements() const
{
    return m_numMaxElements;
}

vsize __stdcall DynamicArray::GetNumElements() const
{
    return m_numElements;
}

GLOBAL_FUNC bool __stdcall CreateDynamicArray(IDynamicArray** ppOutDynamicArray)
{
    ASSERT(ppOutDynamicArray != nullptr, "ppOutDynamicArray == nullptr");

    IDynamicArray* pDynamicArray = new DynamicArray;
    *ppOutDynamicArray = pDynamicArray;

    return true;
}

GLOBAL_FUNC void __stdcall DestroyDynamicArray(IDynamicArray* pDynamicArray)
{
    ASSERT(pDynamicArray != nullptr, "pDynamicArray == nullptr");
    delete (DynamicArray*)pDynamicArray;
}