/*
* StaticMemoryPool
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#include "Precompiled.h"
#include "IStaticMemoryPool.h"

#pragma warning(push)
#pragma warning(disable : 6387)
#pragma warning(disable : 6011)

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
    ~StaticMemoryPool();

    virtual bool    __stdcall   Initialize(const uint elementSize, const uint numElementsPerBlock, const uint numMaxElements) override;
    virtual void    __stdcall   Release() override;
    virtual void    __stdcall   Reset() override;

    virtual void*   __stdcall   Allocate() override;
    virtual void    __stdcall   Free(void* pMemory) override;
    virtual bool    __stdcall   IsValidMemory(const void* pMemory) const override;

    virtual uint   __stdcall    GetElementSize() const override;
    virtual uint   __stdcall    GetNumAllocElements() const override;
    virtual uint   __stdcall    GetNumMaxElements() const override;

private:
    void**  m_ppBlocks = nullptr;
    uint*   m_pIndexTable = nullptr;
    uint*   m_pIndexTablePtr = nullptr;

    uint    m_elementSize = 0;
    uint    m_elementSizeWithHeader = 0;
    uint    m_numMaxElements = 0;
    uint    m_numElementsPerBlock = 0;
    uint    m_numMaxBlocks = 0;
};

StaticMemoryPool::~StaticMemoryPool()
{
    Release();
}

bool __stdcall StaticMemoryPool::Initialize(const uint elementSize, const uint numElementsPerBlock, const uint numMaxElements)
{
    m_elementSize = elementSize;
    m_elementSizeWithHeader = m_elementSize + HEADER_SIZE;
    m_numElementsPerBlock = numElementsPerBlock;
    m_numMaxBlocks = numMaxElements / numElementsPerBlock + 1;

    m_ppBlocks = (void**)malloc(UW_PTR_SIZE * m_numMaxBlocks);
    memset(m_ppBlocks, 0, UW_PTR_SIZE * m_numMaxBlocks);

    m_pIndexTable = (uint*)malloc(sizeof(uint) * numMaxElements);

    // 인덱스 테이블 초기화
    for (uint i = 0; i < numMaxElements; ++i)
    {
        m_pIndexTable[i] = (uint)i;
    }

    // 블럭 할당
    {
        void* pElements = malloc(m_elementSizeWithHeader * m_numElementsPerBlock);
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

void* __stdcall StaticMemoryPool::Allocate()
{
    // 더 이상 할당 불가
    const uint* pBeyondLastIndexTable = m_pIndexTable + m_numMaxElements;
    if (m_pIndexTablePtr == pBeyondLastIndexTable)
    {
        CRASH();
    }

    void* pMemory = nullptr;


    // 할당 가능한 블럭과 그 블럭에서의 인덱스 구하기
    uint blockIndex;
    uint index;
    const uint numAllocElements = GetNumAllocElements();
    blockIndex = _div64(numAllocElements, m_numElementsPerBlock, (int*)&index);

    // 블럭 추가로 할당
    if (m_ppBlocks[blockIndex] == nullptr)
    {
        void* pElements = malloc(m_elementSizeWithHeader * m_numElementsPerBlock);
        memset(pElements, 0, m_elementSizeWithHeader * m_numElementsPerBlock);

        m_ppBlocks[blockIndex] = pElements;
    }

    Header* pHeader = (Header*)((char*)m_ppBlocks[blockIndex] + m_elementSizeWithHeader * index);
    ASSERT(pHeader->Alloc == 0, "Already alloc memory");

    pHeader->Alloc = 1;
    pHeader->Index = index;

    return pHeader + 1;
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

        for (uint i = 0; i < m_numElementsPerBlock; ++i)
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
    const uint index = pHeader->Index;
    
    if (m_pIndexTable[index] != index)
    {
        return false;
    }

    return pHeader->Alloc == 1;
}

uint __stdcall StaticMemoryPool::GetElementSize() const
{
    return m_elementSize;
}

uint __stdcall StaticMemoryPool::GetNumAllocElements() const
{
    return (uint)(m_pIndexTablePtr - m_pIndexTable);
}

uint __stdcall StaticMemoryPool::GetNumMaxElements() const
{
    return m_numElementsPerBlock;
}

GLOBAL_FUNC bool __stdcall CreateStaticMemoryPool(IStaticMemoryPool** ppOutStaticMemoryPool)
{
    IStaticMemoryPool* pStaticMemoryPool = new StaticMemoryPool;
    *ppOutStaticMemoryPool = pStaticMemoryPool;

    return true;
}

GLOBAL_FUNC void __stdcall DestroyStaticMemoryPool(IStaticMemoryPool* pStaticMemoryPoolOrNull)
{
    if (pStaticMemoryPoolOrNull != nullptr)
    {
        delete (StaticMemoryPool*)pStaticMemoryPoolOrNull;
    }
}

#pragma warning(pop)