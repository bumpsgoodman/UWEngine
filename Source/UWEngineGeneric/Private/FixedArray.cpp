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
    FixedArray(FixedArray&&) = default;
    FixedArray& operator=(FixedArray&&) = default;
    ~FixedArray();

    virtual UWMETHOD(bool) Initialize(const vsize elementSize, const vsize numMaxElements) override;
    virtual UWMETHOD(void) Release() override;
    virtual UWMETHOD(void) Clear() override;

    virtual UWMETHOD(bool) PushBack(const void* pElementOrNull, const vsize elementSize) override;
    virtual UWMETHOD(bool) PopBack() override;

    virtual UWMETHOD(bool) Insert(const void* pElementOrNull, const vsize elementSize, const vsize index) override;
    virtual UWMETHOD(bool) Remove(const vsize index) override;

    virtual UWMETHOD(void*) GetBackOrNull() const override;
    virtual UWMETHOD(void*) GetElementOrNull(const vsize index) const override;
    virtual UWMETHOD(void*) GetElementsPtrOrNull() const override;
    virtual UWMETHOD(vsize) GetElementSize() const override;
    virtual UWMETHOD(vsize) GetNumMaxElements() const override;
    virtual UWMETHOD(vsize) GetNumElements() const override;

private:
    void* m_pElements = nullptr;

    vsize m_elementSize = 0;
    vsize m_numMaxElements = 0;
    vsize m_numElements = 0;
};

FixedArray::~FixedArray()
{
    Release();
}

UWMETHOD(bool) FixedArray::Initialize(const vsize elementSize, const vsize numMaxElements)
{
    ASSERT(elementSize > 0, "elementSize == 0");
    ASSERT(numMaxElements > 0, "numMaxElements == 0");

    Release();

    m_pElements = malloc(elementSize * numMaxElements);
    ASSERT(m_pElements != nullptr, "Failed to malloc elements");

    m_elementSize = elementSize;
    m_numMaxElements = numMaxElements;
    m_numElements = 0;

    return true;
}

UWMETHOD(void) FixedArray::Release()
{
    SAFE_FREE(m_pElements);
}

UWMETHOD(void) FixedArray::Clear()
{
    m_numElements = 0;
}

UWMETHOD(bool) FixedArray::PushBack(const void* pElementOrNull, const vsize elementSize)
{
    ASSERT(pElementOrNull == nullptr || elementSize == m_elementSize, "mismatch element size");

    bool bResult = false;

    if (m_numElements >= m_numMaxElements)
    {
        goto lb_return;
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

UWMETHOD(bool) FixedArray::PopBack()
{
    --m_numElements;
    return true;
}

UWMETHOD(bool) FixedArray::Insert(const void* pElementOrNull, const vsize elementSize, const vsize index)
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
        memcpy(pSrc, pElementOrNull, elementSize);
    }

    ++m_numElements;

    bResult = true;

lb_return:
    return bResult;
}

UWMETHOD(bool) FixedArray::Remove(const vsize index)
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

UWMETHOD(void*) FixedArray::GetBackOrNull() const
{
    if (m_numElements == 0)
    {
        return nullptr;
    }

    return (char*)m_pElements + m_elementSize * (m_numElements - 1);
}

UWMETHOD(void*) FixedArray::GetElementOrNull(vsize index) const
{
    if (index >= m_numElements)
    {
        return nullptr;
    }

    return (char*)m_pElements + m_elementSize * index;
}

UWMETHOD(void*) FixedArray::GetElementsPtrOrNull() const
{
    return m_pElements;
}

UWMETHOD(vsize) FixedArray::GetElementSize() const
{
    return m_elementSize;
}

UWMETHOD(vsize) FixedArray::GetNumMaxElements() const
{
    return m_numMaxElements;
}

UWMETHOD(vsize) FixedArray::GetNumElements() const
{
    return m_numElements;
}

GLOBAL_FUNC UWMETHOD(bool) CreateFixedArray(IFixedArray** ppOutFixedArray)
{
    ASSERT(ppOutFixedArray != nullptr, "ppOutFixedArray == nullptr");

    IFixedArray* pFixedArray = new FixedArray;
    *ppOutFixedArray = pFixedArray;

    return true;
}

GLOBAL_FUNC UWMETHOD(void) DestroyFixedArray(IFixedArray* pFixedArray)
{
    ASSERT(pFixedArray != nullptr, "pFixedArray == nullptr");
    delete (FixedArray*)pFixedArray;
}