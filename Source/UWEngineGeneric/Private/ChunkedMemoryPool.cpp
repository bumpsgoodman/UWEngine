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
    ~ChunkedMemoryPool();

    virtual bool __stdcall Initialize(const vsize elementSize, const vsize numElementsPerChunk) override;
    virtual void __stdcall Release() override;

    virtual void* __stdcall AllocateOrNull() override;
    virtual void __stdcall Free(void* pMemory) override;
    virtual void __stdcall Reset() override;
    virtual bool __stdcall IsValidMemory(const void* pMemory) const override;

    virtual vsize __stdcall GetElementSize() const override;
    virtual vsize __stdcall GetNumAllocElements() const override;
    virtual vsize __stdcall GetNumElementsPerChunk() const override;

private:
    bool __stdcall addChunk();

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

bool __stdcall ChunkedMemoryPool::Initialize(const vsize elementSize, const vsize numElementsPerChunk)
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

void __stdcall ChunkedMemoryPool::Release()
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

void* __stdcall ChunkedMemoryPool::AllocateOrNull()
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

void __stdcall ChunkedMemoryPool::Free(void* pMemory)
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

void __stdcall ChunkedMemoryPool::Reset()
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

bool __stdcall ChunkedMemoryPool::IsValidMemory(const void* pMemory) const
{
    const Header* pHeader = (Header*)pMemory;
    const Chunk* pChunk = pHeader->pChunk;

    if (*pChunk->pIndexTablePtr != pHeader->Index)
    {
        return false;
    }

    return pHeader->Alloc == 1;
}

vsize __stdcall ChunkedMemoryPool::GetElementSize() const
{
    return m_elementSize;
}

vsize __stdcall ChunkedMemoryPool::GetNumAllocElements() const
{
    return m_numAllocElements;
}

vsize __stdcall ChunkedMemoryPool::GetNumElementsPerChunk() const
{
    return m_numElementsPerChunk;
}

bool __stdcall ChunkedMemoryPool::addChunk()
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

GLOBAL_FUNC bool __stdcall CreateChunkedMemoryPool(IChunkedMemoryPool** ppOutChunkedMemoryPool)
{
    ASSERT(ppOutChunkedMemoryPool != nullptr, "ppOutChunkedMemoryPool == nullptr");

    IChunkedMemoryPool* pChunkedMemoryPool = new ChunkedMemoryPool;
    *ppOutChunkedMemoryPool = pChunkedMemoryPool;

    return true;
}

GLOBAL_FUNC void __stdcall DestroyChunkedMemoryPool(IChunkedMemoryPool* pChunkedMemoryPool)
{
    ASSERT(pChunkedMemoryPool != nullptr, "pChunkedMemoryPool == nullptr");
    delete (ChunkedMemoryPool*)pChunkedMemoryPool;
}