/*
* StaticMemoryPool
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#include "Precompiled.h"
#include "IStaticMemoryPool.h"

struct Header
{
    uint Index : sizeof(uint) * 8 - 1;
    uint Alloc : 1;
};
#define HEADER_SIZE sizeof(Header)

class StaticMemoryPool final : public IStaticMemoryPool
{
public:
    StaticMemoryPool() = default;
    StaticMemoryPool(const StaticMemoryPool&) = delete;
    StaticMemoryPool& operator=(const StaticMemoryPool&) = delete;
    virtual ~StaticMemoryPool();

    virtual bool    __stdcall Initialize(const vsize elementSize, const vsize numElementsPerBlock, const vsize numMaxElements) override;
    virtual void    __stdcall Release() override;
    virtual void    __stdcall Reset() override;

    virtual void*   __stdcall AllocateOrNull() override;
    virtual void    __stdcall Free(void* pMemory) override;
    virtual bool    __stdcall IsValidMemory(const void* pMemory) const override;

    virtual vsize   __stdcall GetElementSize() const override;
    virtual vsize   __stdcall GetNumAllocElements() const override;
    virtual vsize   __stdcall GetNumMaxElements() const override;

private:
    void** m_ppBlocks = nullptr;
    uint* m_pIndexTable = nullptr;
    uint* m_pIndexTablePtr = nullptr;

    vsize m_elementSize = 0;
    vsize m_elementSizeWithHeader = 0;
    vsize m_numMaxElements = 0;
    vsize m_numElementsPerBlock = 0;
    vsize m_numMaxBlocks = 0;
};

StaticMemoryPool::~StaticMemoryPool()
{
    Release();
}

bool __stdcall StaticMemoryPool::Initialize(const vsize elementSize, const vsize numElementsPerBlock, const vsize numMaxElements)
{
    ASSERT(elementSize > 0, "elementSize == 0");
    ASSERT(numElementsPerBlock > 0, "numElementsPerBlock == 0");
    ASSERT(numElementsPerBlock <= numMaxElements, "numElementsPerBlock > numMaxElements");

    Release();

    m_elementSize = elementSize;
    m_elementSizeWithHeader = m_elementSize + HEADER_SIZE;
    m_numElementsPerBlock = numElementsPerBlock;
    m_numMaxBlocks = numMaxElements / numElementsPerBlock + 1;

    m_ppBlocks = (void**)malloc(UW_PTR_SIZE * m_numMaxBlocks);
    ASSERT(m_ppBlocks != nullptr, "Failed to malloc m_ppBlocks");
    memset(m_ppBlocks, 0, UW_PTR_SIZE * m_numMaxBlocks);

    m_pIndexTable = (uint*)malloc(sizeof(uint) * numMaxElements);
    ASSERT(m_pIndexTable != nullptr, "Failed to malloc index table");

    // 인덱스 테이블 초기화
    for (vsize i = 0; i < numMaxElements; ++i)
    {
        m_pIndexTable[i] = (uint)i;
    }

    // 블럭 할당
    {
        void* pElements = malloc(m_elementSizeWithHeader * m_numElementsPerBlock);
        ASSERT(pElements != nullptr, "Failed to malloc new elements");
        memset(pElements, 0, m_elementSizeWithHeader * m_numElementsPerBlock);

        m_ppBlocks[0] = pElements;
    }

    Reset();

    return true;
}

void __stdcall StaticMemoryPool::Release()
{
    SAFE_FREE(m_pIndexTable);
    SAFE_FREE(m_ppBlocks);
}

void* __stdcall StaticMemoryPool::AllocateOrNull()
{
    void* pMemory = nullptr;

    // 더 이상 할당 불가
    const uint* pBeyondLastIndexTable = m_pIndexTable + m_numMaxElements;
    if (m_pIndexTablePtr == pBeyondLastIndexTable)
    {
        goto lb_return;
    }

    // 할당 가능한 블럭과 그 블럭에서의 인덱스 구하기
    const vsize numAllocElements = GetNumAllocElements();
    const vsize blockIndex = numAllocElements / m_numElementsPerBlock;
    const vsize index = numAllocElements % m_numElementsPerBlock;

    // 블럭 추가로 할당
    if (m_ppBlocks[blockIndex] == nullptr)
    {
        void* pElements = malloc(m_elementSizeWithHeader * m_numElementsPerBlock);
        ASSERT(pElements != nullptr, "Failed to malloc new elements");
        memset(pElements, 0, m_elementSizeWithHeader * m_numElementsPerBlock);

        m_ppBlocks[blockIndex] = pElements;
    }

    Header* pHeader = (Header*)((char*)m_ppBlocks[blockIndex] + m_elementSizeWithHeader * index);
    ASSERT(pHeader->Alloc == 0, "Already alloc memory");

    pHeader->Alloc = 1;
    pHeader->Index = index;

    pMemory = pHeader + 1;

lb_return:
    return pMemory;
}

void __stdcall StaticMemoryPool::Free(void* pMemory)
{
    if (pMemory == nullptr)
    {
        return;
    }

    Header* pHeader = (Header*)pMemory - 1;
    const uint index = pHeader->Index;

    ASSERT(m_pIndexTable[index] == index, "Invalid memory");
    ASSERT(pHeader->Alloc == 1, "Invalid memory");

    pHeader->Alloc = 0;

    *(--m_pIndexTablePtr) = index;
}

void __stdcall StaticMemoryPool::Reset()
{
    uint index = 0;

    char** ppBlocks = (char**)m_ppBlocks;
    while (*ppBlocks != nullptr)
    {
        char* pElements = *ppBlocks;

        for (vsize i = 0; i < m_numElementsPerBlock; ++i)
        {
            Header* pHeader = (Header*)(pElements + m_elementSizeWithHeader * i);
            pHeader->Alloc = 0;

            m_pIndexTable[index] = index;
            ++index;
            if (index >= m_numMaxElements)
            {
                goto lb_return;
            }
        }

        ++ppBlocks;
    }

lb_return:
    m_pIndexTablePtr = m_pIndexTable;
}

bool __stdcall StaticMemoryPool::IsValidMemory(const void* pMemory) const
{
    const Header* pHeader = (Header*)pMemory - 1;
    const vsize index = pHeader->Index;
    
    if (m_pIndexTable[index] != index)
    {
        return false;
    }

    return pHeader->Alloc == 1;
}

vsize __stdcall StaticMemoryPool::GetElementSize() const
{
    return m_elementSize;
}

vsize __stdcall StaticMemoryPool::GetNumAllocElements() const
{
    return m_pIndexTablePtr - m_pIndexTable;
}

vsize __stdcall StaticMemoryPool::GetNumMaxElements() const
{
    return m_numElementsPerBlock;
}

GLOBAL_FUNC bool __stdcall CreateStaticMemoryPool(IStaticMemoryPool** ppOutStaticMemoryPool)
{
    ASSERT(ppOutStaticMemoryPool != nullptr, "ppOutStaticMemoryPool == nullptr");

    IStaticMemoryPool* pStaticMemoryPool = new StaticMemoryPool;
    *ppOutStaticMemoryPool = pStaticMemoryPool;

    return true;
}

GLOBAL_FUNC void __stdcall DestroyStaticMemoryPool(IStaticMemoryPool* pStaticMemoryPoolOrNull)
{
    if (pStaticMemoryPoolOrNull != nullptr)
    {
        delete pStaticMemoryPoolOrNull;
    }
}