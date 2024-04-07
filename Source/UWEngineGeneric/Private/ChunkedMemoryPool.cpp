/*
* ChunkedMemoryPool
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.27
*/

#include "Precompiled.h"
#include "IChunkedMemoryPool.h"
#include "List.h"

struct Chunk
{
    void* pElements;
    uint* pIndexTable;
    uint* pIndexTablePtr;
};

struct Header
{
    Chunk* pChunk;

    struct
    {
        uint Index : sizeof(uint) * 8 - 1;
        uint Alloc : 1;
    };
};
#define HEADER_SIZE sizeof(Header)

class ChunkedMemoryPool final : public IChunkedMemoryPool
{
public:
    ChunkedMemoryPool() = default;
    ChunkedMemoryPool(const ChunkedMemoryPool&) = delete;
    ChunkedMemoryPool& operator=(const ChunkedMemoryPool&) = delete;
    ChunkedMemoryPool(ChunkedMemoryPool&&) = default;
    ChunkedMemoryPool& operator=(ChunkedMemoryPool&&) = default;
    ~ChunkedMemoryPool();

    virtual UWMETHOD(bool) Initialize(const vsize elementSize, const vsize numElementsPerChunk) override;
    virtual UWMETHOD(void) Release() override;

    virtual UWMETHOD(void*) AllocateOrNull() override;
    virtual UWMETHOD(void) Free(void* pMemory) override;
    virtual UWMETHOD(void) Reset() override;
    virtual UWMETHOD(bool) IsValidMemory(const void* pMemory) const override;

    virtual UWMETHOD(vsize) GetElementSize() const override;
    virtual UWMETHOD(vsize) GetNumAllocElements() const override;
    virtual UWMETHOD(vsize) GetNumElementsPerChunk() const override;

private:
    UWMETHOD(bool) addChunk();

private:
    ListNode* m_pChunkHead = nullptr;
    ListNode* m_pChunkTail = nullptr;

    vsize m_elementSize = 0;
    vsize m_elementSizeWithHeader = 0;
    vsize m_numElementsPerChunk = 0;
    vsize m_numAllocElements = 0;
};

ChunkedMemoryPool::~ChunkedMemoryPool()
{
    Release();
}

UWMETHOD(bool) ChunkedMemoryPool::Initialize(const vsize elementSize, const vsize numElementsPerChunk)
{
    ASSERT(elementSize > 0, "elementSize == 0");
    ASSERT(numElementsPerChunk > 0 && numElementsPerChunk <= NUM_MAX_GENERIC_ELEMENTS, "Invalid range");

    bool bResult = false;

    Release();

    m_pChunkHead = nullptr;
    m_pChunkTail = nullptr;

    m_elementSize = elementSize;
    m_elementSizeWithHeader = m_elementSize + HEADER_SIZE;
    m_numElementsPerChunk = numElementsPerChunk;
    m_numAllocElements = 0;

    if (!addChunk())
    {
        goto lb_return;
    }

    Reset();

    bResult = true;

lb_return:
    return bResult;
}

UWMETHOD(void) ChunkedMemoryPool::Release()
{
    ListNode* pNode = m_pChunkHead;
    while (pNode != nullptr)
    {
        ListNode* pNext = pNode->pNext;

        Chunk* pChunk = (Chunk*)pNode->pElement;

        SAFE_FREE(pChunk->pElements);
        SAFE_FREE(pChunk->pIndexTable);

        SAFE_DELETE(pChunk);
        SAFE_DELETE(pNode);

        pNode = pNext;
    }
}

UWMETHOD(void*) ChunkedMemoryPool::AllocateOrNull()
{
    char* pMemory = nullptr;

    // 할당 가능한 청크 찾기
    ListNode* pNode = m_pChunkHead;
    while (pNode != nullptr)
    {
        Chunk* pChunk = (Chunk*)pNode->pElement;
        if ((vsize)(pChunk->pIndexTablePtr - pChunk->pIndexTable) < m_numElementsPerChunk)
        {
            break;
        }

        pNode = pNode->pNext;
    }

    // 할당 가능한 청크 없으면 청크 추가
    if (pNode == nullptr)
    {
        if (addChunk())
        {
            goto lb_return;
        }

        pNode = m_pChunkTail;
    }

    Chunk* pChunk = (Chunk*)pNode->pElement;
    const uint index = *pChunk->pIndexTablePtr;

    Header* pHeader = (Header*)((char*)pChunk->pElements + m_elementSizeWithHeader * index);
    ASSERT(IsValidMemory(pHeader), "Invalid memory");

    pHeader->Alloc = 1;
    pHeader->Index = index;

    ++pChunk->pIndexTablePtr;

    ++m_numAllocElements;

lb_return:
    return pMemory;
}

UWMETHOD(void) ChunkedMemoryPool::Free(void* pMemory)
{
    if (pMemory == nullptr)
    {
        return;
    }

    Header* pHeader = (Header*)pMemory;
    Chunk* pChunk = pHeader->pChunk;
    const vsize index = pHeader->Index;

    ASSERT(pHeader->Alloc == 1, "Invalid memory");
    ASSERT(pChunk->pIndexTable[index] == index, "Invalid memory");

    pHeader->Alloc = 0;

    *(--pChunk->pIndexTablePtr) = pHeader->Index;

    --m_numAllocElements;
}

UWMETHOD(void) ChunkedMemoryPool::Reset()
{
    ListNode* pNode = m_pChunkHead;
    while (pNode != nullptr)
    {
        Chunk* pChunk = (Chunk*)pNode->pElement;
        for (vsize i = 0; i < m_numElementsPerChunk; ++i)
        {
            Header* pHeader = (Header*)((char*)pChunk->pElements + m_elementSizeWithHeader * i);
            pHeader->pChunk = pChunk;
            pHeader->Alloc = 0;

            pChunk->pIndexTable[i] = (uint)i;
        }

        pChunk->pIndexTablePtr = pChunk->pIndexTable;

        pNode = pNode->pNext;
    }
}

UWMETHOD(bool) ChunkedMemoryPool::IsValidMemory(const void* pMemory) const
{
    const Header* pHeader = (Header*)pMemory;
    const Chunk* pChunk = pHeader->pChunk;

    if (*pChunk->pIndexTablePtr != pHeader->Index)
    {
        return false;
    }

    return pHeader->Alloc == 1;
}

UWMETHOD(vsize) ChunkedMemoryPool::GetElementSize() const
{
    return m_elementSize;
}

UWMETHOD(vsize) ChunkedMemoryPool::GetNumAllocElements() const
{
    return m_numAllocElements;
}

UWMETHOD(vsize) ChunkedMemoryPool::GetNumElementsPerChunk() const
{
    return m_numElementsPerChunk;
}

UWMETHOD(bool) ChunkedMemoryPool::addChunk()
{
    Chunk* pChunk = new Chunk;

    // 요소 저장할 공간 생성
    void* pElements = malloc(m_elementSizeWithHeader * m_numElementsPerChunk);
    ASSERT(pElements != nullptr, "Failed to malloc new elements");
    memset(pElements, 0, m_elementSizeWithHeader * m_numElementsPerChunk);

    // 인덱스 테이블 생성
    uint* pIndexTable = (uint*)malloc(sizeof(uint) * m_numElementsPerChunk);
    ASSERT(pIndexTable != nullptr, "Failed to malloc new index table");

    // 인덱스 테이블 초기화
    for (vsize i = 0; i < m_numElementsPerChunk; ++i)
    {
        pIndexTable[i] = (uint)i;
    }

    pChunk->pElements = pElements;
    pChunk->pIndexTable = pIndexTable;
    pChunk->pIndexTablePtr = pIndexTable;

    ListNode* pNode = new ListNode;
    pNode->pElement = pChunk;
    ListAddTail(&m_pChunkHead, &m_pChunkTail, pNode);

    return true;
}

GLOBAL_FUNC UWMETHOD(bool) CreateChunkedMemoryPool(IChunkedMemoryPool** ppOutChunkedMemoryPool)
{
    ASSERT(ppOutChunkedMemoryPool != nullptr, "ppOutChunkedMemoryPool == nullptr");

    IChunkedMemoryPool* pChunkedMemoryPool = new ChunkedMemoryPool;
    *ppOutChunkedMemoryPool = pChunkedMemoryPool;

    return true;
}

GLOBAL_FUNC UWMETHOD(void) DestroyChunkedMemoryPool(IChunkedMemoryPool* pChunkedMemoryPool)
{
    ASSERT(pChunkedMemoryPool != nullptr, "pChunkedMemoryPool == nullptr");
    delete (ChunkedMemoryPool*)pChunkedMemoryPool;
}