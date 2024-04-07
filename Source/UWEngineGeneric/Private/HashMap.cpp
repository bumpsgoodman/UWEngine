/*
* Map
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.20
*/
#include "Precompiled.h"
#include "IHashMap.h"
#include "List.h"

class HashMap final : public IHashMap
{
public:
    HashMap() = default;
    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;
    HashMap(HashMap&&) = default;
    HashMap& operator=(HashMap&&) = default;
    ~HashMap();

    virtual UWMETHOD(bool) Initialize(const vsize keySize, const vsize valueSize, const vsize numDefaultKeyValues) override;
    virtual UWMETHOD(void) Release() override;
    virtual UWMETHOD(void) Clear() override;

    virtual UWMETHOD(bool) Insert(const void* pKey, const vsize keySize,
                                  const void* pValue, const vsize valueSize) override;
    virtual UWMETHOD(bool) InsertByHash(const void* pKey, const vsize keySize,
                                        const void* pValue, const vsize valueSize,
                                        const uint64 hash) override;

    virtual UWMETHOD(bool) Remove(const void* pKey, const vsize keySize) override;
    virtual UWMETHOD(bool) RemoveByHash(const void* pKey, const vsize keySize, const uint64 hash) override;

    virtual UWMETHOD(vsize) GetCount(const void* pKey, const vsize keySize) const override;
    virtual UWMETHOD(vsize) GetCountByHash(const void* pKey, const vsize keySize, const uint64 hash) const override;

    virtual UWMETHOD(void*) GetValueOrNull(const void* pKey, const vsize keySize) const override;
    virtual UWMETHOD(void*) GetValueByHashOrNull(const void* pKey, const vsize keySize, const uint64 hash) const override;

    virtual UWMETHOD(KeyValue*) GetKeyValuesOrNull() const override;
    virtual UWMETHOD(vsize) GetKeySize() const override;
    virtual UWMETHOD(vsize) GetValueSize() const override;
    virtual UWMETHOD(vsize) GetNumKeyValues() const override;
    virtual UWMETHOD(vsize) GetNumMaxKeyValues() const override;
    virtual UWMETHOD(float) GetFactor() const override;

    virtual UWMETHOD(void) SetFactor(float factor) override;

private:
    UWMETHOD(bool) expand();

private:
    vsize m_keySize = 0;
    vsize m_valueSize = 0;
    vsize m_numMaxKeyValues = 0;
    vsize m_numKeyValues = 0;
    float m_factor = 0.0f;

    List* m_pBucket = nullptr;
    vsize m_bucketSize = 0;

    KeyValue* m_pKeyValues = nullptr;

    void* m_pKeyPool = nullptr;
    void* m_pValuePool = nullptr;
    ListNode* m_pNodePool = nullptr;
};

static constexpr vsize S_BUCKET_SIZE_LIST[] =
{
    7u, 23u, 97u, 397u, 1597u, 6421u, 25717u, 102877u,
    411527u, 879743u, 1799639u, 6584983u, 26339969u, 52679969u
};
static constexpr vsize S_NUM_BUCKET_SIZE_LIST = sizeof(S_BUCKET_SIZE_LIST) / sizeof(vsize);

HashMap::~HashMap()
{
    Release();
}

UWMETHOD(bool) HashMap::Initialize(const vsize keySize, const vsize valueSize, const vsize numDefaultKeyValues)
{
    ASSERT(keySize > 0, "keySize == 0");
    ASSERT(valueSize > 0, "valueSize == 0");
    ASSERT(numDefaultKeyValues > 0, "numDefaultKeyValues == 0");

    bool bResult = false;

    Release();

    // 버켓 크기 구하기
    const vsize baseSize = numDefaultKeyValues * 2;
    for (vsize i = 0; i < S_NUM_BUCKET_SIZE_LIST; ++i)
    {
        const vsize size = S_BUCKET_SIZE_LIST[i];
        if (size > baseSize)
        {
            m_bucketSize = size;
            break;
        }
    }

    // 버켓 할당
    m_pBucket = (List*)malloc(sizeof(List) * m_bucketSize);
    ASSERT(m_pBucket != nullptr, "Failed to malloc bucket");
    memset(m_pBucket, 0, sizeof(List) * m_bucketSize);

    // 키-값 저장소 할당
    m_pKeyValues = (KeyValue*)malloc(sizeof(KeyValue) * numDefaultKeyValues);

    // 키 풀 할당
    m_pKeyPool = malloc(keySize * numDefaultKeyValues);
    ASSERT(m_pKeyPool != nullptr, "Failed to malloc m_pKeyPool");

    // 값 풀 할당
    m_pValuePool = malloc(valueSize * numDefaultKeyValues);
    ASSERT(m_pValuePool != nullptr, "Failed to malloc m_pValuePool");

    // 노드 풀 할당
    m_pNodePool = (ListNode*)malloc(sizeof(ListNode) * numDefaultKeyValues);
    ASSERT(m_pNodePool != nullptr, "Failed to malloc m_pNodePool");

    m_keySize = keySize;
    m_valueSize = valueSize;
    m_numMaxKeyValues = numDefaultKeyValues;
    m_numKeyValues = 0;
    m_factor = 0.75f;

    bResult = true;

    return bResult;
}

UWMETHOD(void) HashMap::Release()
{
    SAFE_FREE(m_pNodePool);
    SAFE_FREE(m_pValuePool);
    SAFE_FREE(m_pKeyPool);
    SAFE_FREE(m_pKeyValues);
    SAFE_FREE(m_pBucket);
}

UWMETHOD(void) HashMap::Clear()
{
    m_numKeyValues = 0;
    memset(m_pBucket, 0, sizeof(List) * m_bucketSize);
}

UWMETHOD(bool) HashMap::Insert(const void* pKey, const vsize keySize,
                               const void* pValue, const vsize valueSize)
{
    ASSERT(pKey != nullptr, "pKey == nullptr");
    ASSERT(keySize == m_keySize, "Mismatch key size");

    const uint64 hash = FNV1a64((const char*)pKey, keySize);
    return InsertByHash(pKey, keySize, pValue, valueSize, hash);
}

UWMETHOD(bool) HashMap::InsertByHash(const void* pKey, const vsize keySize,
                                     const void* pValue, const vsize valueSize,
                                     const uint64 hash)
{
    ASSERT(pKey != nullptr, "pKey == nullptr");
    ASSERT(keySize == m_keySize, "Mismatch key size");
    ASSERT(pValue != nullptr, "pValue == nullptr");
    ASSERT(valueSize == m_valueSize, "Mismatch value size");

    bool bResult = false;

    if (m_numKeyValues >= m_numMaxKeyValues * m_factor)
    {
        if (!expand())
        {
            goto lb_return;
        }
    }

    const vsize index = hash % m_bucketSize;
    List* pList = m_pBucket + index;

    // 이미 같은 키가 있는지 검사
    // 같은 키가 있다면 값 바꾸기
    {
        ListNode* pNode = pList->pHead;
        while (pNode != nullptr && memcmp(((KeyValue*)pNode->pElement)->pKey, pKey, keySize) != 0)
        {
            pNode = pNode->pNext;
        }

        if (pNode != nullptr)
        {
            KeyValue* pKeyValue = (KeyValue*)pNode->pElement;
            memcpy(pKeyValue->pValue, pValue, valueSize);
        }
    }

    ListNode* pNode = m_pNodePool + m_numKeyValues;
    KeyValue* pKeyValue = m_pKeyValues + m_numKeyValues;

    // 키-값 복사
    pKeyValue->pKey = (char*)m_pKeyPool + keySize * m_numKeyValues;
    pKeyValue->pValue = (char*)m_pValuePool + valueSize * m_numKeyValues;
    memcpy(pKeyValue->pKey, pKey, keySize);
    memcpy(pKeyValue->pValue, pValue, valueSize);

    pKeyValue->Index = index; // 제거할 때 필요

    pNode->pElement = pKeyValue;
    ListAddTail(&pList->pHead, &pList->pTail, pNode);

    ++m_numKeyValues;

    bResult = true;

lb_return:
    return bResult;
}

UWMETHOD(bool) HashMap::Remove(const void* pKey, const vsize keySize)
{
    ASSERT(pKey != nullptr, "pKey == nullptr");
    ASSERT(keySize == m_keySize, "Mismatch key size");

    const uint64 hash = FNV1a64((const char*)pKey, keySize);
    return RemoveByHash(pKey, keySize, hash);
}

UWMETHOD(bool) HashMap::RemoveByHash(const void* pKey, const vsize keySize, const uint64 hash)
{
    ASSERT(pKey != nullptr, "pKey == nullptr");
    ASSERT(keySize == m_keySize, "Mismatch key size");

    bool bResult = false;

    const vsize index = hash % m_bucketSize;

    List* pList = m_pBucket + index;

    // 키 찾기
    ListNode* pNode = pList->pHead;
    while (pNode != nullptr && memcmp(((KeyValue*)pNode->pElement)->pKey, pKey, keySize) != 0)
    {
        pNode = pNode->pNext;
    }

    if (pNode == nullptr)
    {
        goto lb_return;
    }

    KeyValue* pKeyValue = (KeyValue*)pNode->pElement;
    KeyValue* pLastKeyValue = m_pKeyValues + (m_numKeyValues - 1);
    void* pDeletedKey = pKeyValue->pKey;
    void* pDeletedValue = pKeyValue->pValue;
    *pKeyValue = *pLastKeyValue;

    List* pLastList = m_pBucket + pLastKeyValue->Index;
    ListNode* pLastNode = pLastList->pHead;
    while (pLastNode != nullptr
           && memcmp(pLastKeyValue->pKey, ((KeyValue*)pLastNode->pElement)->pKey, keySize) != 0)
    {
        pLastNode = pLastNode->pNext;
    }

    // 이 시점에서 마지막 노드는 NULL이 될 수 없음
    ASSERT(pLastNode != nullptr, "pLastNode == nullptr");
    pLastNode->pElement = pKeyValue;

    ListDeleteNode(&pList->pHead, &pList->pTail, pNode);
    --m_numKeyValues;

    bResult = true;

lb_return:
    return bResult;
}

UWMETHOD(vsize) HashMap::GetCount(const void* pKey, const vsize keySize) const
{
    ASSERT(pKey != nullptr, "pKey == nullptr");
    ASSERT(keySize == m_keySize, "Mismatch key size");

    const uint64 hash = FNV1a64((const char*)pKey, keySize);
    return GetCountByHash(pKey, keySize, hash);
}

UWMETHOD(vsize) HashMap::GetCountByHash(const void* pKey, const vsize keySize, const uint64 hash) const
{
    ASSERT(pKey != nullptr, "pKey == nullptr");
    ASSERT(keySize == m_keySize, "Mismatch key size");

    const vsize index = hash % m_bucketSize;
    vsize count = 0;

    List* pList = m_pBucket + index;
    ListNode* pNode = pList->pHead;
    while (pNode != nullptr)
    {
        if (memcmp(((KeyValue*)pNode->pElement)->pKey, pKey, keySize) == 0)
        {
            ++count;
        }

        pNode = pNode->pNext;
    }

    return count;
}

UWMETHOD(void*) HashMap::GetValueOrNull(const void* pKey, const vsize keySize) const
{
    ASSERT(pKey != nullptr, "pKey == nullptr");
    ASSERT(keySize == m_keySize, "Mismatch key size");

    const uint64 hash = FNV1a64((const char*)pKey, keySize);
    return GetValueByHashOrNull(pKey, keySize, hash);
}

UWMETHOD(void*) HashMap::GetValueByHashOrNull(const void* pKey, const vsize keySize, const uint64 hash) const
{
    ASSERT(pKey != nullptr, "pKey == nullptr");
    ASSERT(keySize == m_keySize, "Mismatch key size");

    const vsize index = hash % m_bucketSize;

    List* pList = m_pBucket + index;
    ListNode* pNode = pList->pHead;
    while (pNode != nullptr && memcmp(((KeyValue*)pNode->pElement)->pKey, pKey, keySize) != 0)
    {
        pNode = pNode->pNext;
    }

    if (pNode == nullptr)
    {
        return nullptr;
    }

    KeyValue* pKeyValue = (KeyValue*)pNode->pElement;
    return pKeyValue->pValue;
}

UWMETHOD(KeyValue*) HashMap::GetKeyValuesOrNull() const
{
    return m_pKeyValues;
}

UWMETHOD(vsize) HashMap::GetKeySize() const
{
    return m_keySize;
}

UWMETHOD(vsize) HashMap::GetValueSize() const
{
    return m_valueSize;
}

UWMETHOD(vsize) HashMap::GetNumKeyValues() const
{
    return m_numKeyValues;
}

UWMETHOD(vsize) HashMap::GetNumMaxKeyValues() const
{
    return (vsize)(m_numMaxKeyValues * m_factor);
}

UWMETHOD(float) HashMap::GetFactor() const
{
    return m_factor;
}

UWMETHOD(void) HashMap::SetFactor(float factor)
{
    if (factor > 1.0f)
    {
        factor = 1.0f;
    }
    else if (factor < 0.1f)
    {
        factor = 0.1f;
    }
    else
    {
        factor = factor;
    }
}

UWMETHOD(bool) HashMap::expand()
{
    bool bResult = false;

    const vsize newNumMaxKeyValues = m_numMaxKeyValues * 2;

    // 버켓 크기 구하기
    const vsize baseSize = m_numKeyValues * 2;
    vsize newBucketSize = 0;
    for (vsize i = 0; i < S_NUM_BUCKET_SIZE_LIST; ++i)
    {
        const vsize size = S_BUCKET_SIZE_LIST[i];
        if (size > baseSize)
        {
            newBucketSize = size;
            break;
        }
    }

    // 버켓 할당
    List* pNewBucket = (List*)malloc(sizeof(List) * newBucketSize);
    ASSERT(pNewBucket != nullptr, "Failed to malloc new bucket");

    // 키-값 저장소 할당
    KeyValue* pNewKeyValues = (KeyValue*)malloc(sizeof(KeyValue) * newNumMaxKeyValues);
    ASSERT(pNewKeyValues != nullptr, "Failed to malloc new key-values");

    // 키 풀 할당
    void* pNewKeyPool = malloc(m_keySize * newNumMaxKeyValues);
    ASSERT(pNewKeyPool != nullptr, "Failed to malloc new key pool");

    // 값 풀 할당
    void* pNewValuePool = malloc(m_valueSize * newNumMaxKeyValues);
    ASSERT(pNewValuePool != nullptr, "Failed to malloc new value pool");

    // 노드 풀 할당
    ListNode* pNewNodePool = (ListNode*)malloc(sizeof(ListNode) * newNumMaxKeyValues);
    ASSERT(pNewNodePool != nullptr, "Failed to malloc new node pool");

    // 풀 초기화
    memcpy(pNewKeyValues, m_pKeyValues, sizeof(KeyValue) * m_numKeyValues);
    memcpy(pNewKeyPool, m_pKeyPool, m_keySize * m_numKeyValues);
    memcpy(pNewValuePool, m_pValuePool, m_valueSize * m_numKeyValues);
    memcpy(pNewNodePool, m_pNodePool, sizeof(ListNode) * m_numKeyValues);

    // 버켓 초기화
    memset(pNewBucket, 0, sizeof(List) * newBucketSize);
    for (vsize i = 0; i < m_numKeyValues; ++i)
    {
        const KeyValue* pKeyValue = &m_pKeyValues[i];
        KeyValue* pNewKeyValue = &pNewKeyValues[i];

        const uint64 hash = FNV1a64((const char*)pKeyValue->pKey, m_keySize);
        const vsize index = hash % newBucketSize;

        // 키, 값 복사
        pNewKeyValue->pKey = memcpy((char*)pNewKeyPool + m_keySize * i, pKeyValue->pKey, m_keySize);
        pNewKeyValue->pValue = memcpy((char*)pNewValuePool + m_valueSize * i, pKeyValue->pValue, m_valueSize);
        pNewKeyValue->Index = index;

        // 노드 추가
        ListNode* pNewNode = &pNewNodePool[i];
        pNewNode->pElement = pNewKeyValue;
        ListAddTail(&pNewBucket[index].pHead, &pNewBucket[index].pTail, pNewNode);
    }

    free(m_pBucket);
    free(m_pKeyValues);
    free(m_pKeyPool);
    free(m_pValuePool);
    free(m_pNodePool);

    m_pBucket = pNewBucket;
    m_pKeyValues = pNewKeyValues;
    m_pKeyPool = pNewKeyPool;
    m_pValuePool = pNewValuePool;
    m_pNodePool = pNewNodePool;

    bResult = true;

    return bResult;
}

GLOBAL_FUNC UWMETHOD(bool) CreateHashMap(IHashMap** ppOutHashMap)
{
    ASSERT(ppOutHashMap != nullptr, "ppOutHashMap == nullptr");

    IHashMap* pHashMap = new HashMap;
    *ppOutHashMap = pHashMap;

    return true;
}

GLOBAL_FUNC UWMETHOD(void) DestroyHashMap(IHashMap* pHashMap)
{
    ASSERT(pHashMap != nullptr, "pHashMap == nullptr");
    delete (HashMap*)pHashMap;
}