/*
* HashMap
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.20
*/

#include "Precompiled.h"
#include "HashMap.h"

#pragma warning(push)
#pragma warning(disable : 6387)
#pragma warning(disable : 6011)

HashMap::~HashMap()
{
    Release();
}

bool __stdcall HashMap::Initialize(const uint keySize, const uint valueSize, const uint numMaxKeyValues)
{
    m_pBuckets = (Bucket*)malloc(sizeof(Bucket) * numMaxKeyValues);
    memset(m_pBuckets, 0, sizeof(Bucket) * numMaxKeyValues);

    m_pKeyValues = (KeyValue*)malloc(sizeof(KeyValue) * numMaxKeyValues);

    m_pKeyPool = malloc(keySize * numMaxKeyValues);
    m_pValuePool = malloc(valueSize * numMaxKeyValues);
    m_pNodePool = (ListNode*)malloc(sizeof(ListNode) * numMaxKeyValues);

    m_keySize = keySize;
    m_valueSize = valueSize;
    m_numMaxKeyValues = numMaxKeyValues;
    m_numKeyValues = 0;

    return true;
}

void __stdcall HashMap::Release()
{
    SAFE_FREE(m_pNodePool);
    SAFE_FREE(m_pValuePool);
    SAFE_FREE(m_pKeyPool);
    SAFE_FREE(m_pKeyValues);
    SAFE_FREE(m_pBuckets);
}

void __stdcall HashMap::Clear()
{
    m_numKeyValues = 0;
    memset(m_pBuckets, 0, sizeof(Bucket) * m_numMaxKeyValues);
}

void* __stdcall HashMap::Insert(const void* pKey, const uint keySize, const void* pValue, const uint valueSize)
{
    const uint32 hash = UWHash32((const char*)pKey, keySize);
    return InsertByHash(pKey, keySize, pValue, valueSize, hash);
}

void* __stdcall HashMap::InsertByHash(const void* pKey, const uint keySize, const void* pValue, const uint valueSize, const uint32 hash)
{
    if (keySize != m_keySize || valueSize != m_valueSize)
    {
        CRASH();
    }

    void* pValueHandle = nullptr;

    const uint index = hash % m_numMaxKeyValues;
    Bucket* pBucket = m_pBuckets + index;

    // 이미 추가된 키-값이면 바꾸기
    {
        ListNode* pNode = pBucket->pHead;
        while (pNode != nullptr && memcmp(((KeyValue*)pNode->pElement)->pKey, pKey, keySize) != 0)
        {
            pNode = pNode->pNext;
        }

        if (pNode != nullptr)
        {
            KeyValue* pKeyValue = (KeyValue*)pNode->pElement;
            memcpy(pKeyValue->pValue, pValue, valueSize);

            pValueHandle = pKeyValue->pValue;
            goto lb_return;
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
    ListAddTail(&pBucket->pHead, &pBucket->pTail, pNode);

    ++m_numKeyValues;
    pValueHandle = pKeyValue->pValue;

lb_return:
    return pValueHandle;
}

void __stdcall HashMap::Remove(const void* pKey, const uint keySize)
{
    const uint32 hash = UWHash32((const char*)pKey, keySize);
    RemoveByHash(pKey, keySize, hash);
}

void __stdcall HashMap::RemoveByHash(const void* pKey, const uint keySize, const uint32 hash)
{
    if (keySize != m_keySize)
    {
        CRASH();
    }

    const uint index = hash % m_numMaxKeyValues;
    Bucket* pBucket = m_pBuckets + index;

    // 노드 찾기
    ListNode* pNode = pBucket->pHead;
    while (pNode != nullptr && memcmp(((KeyValue*)pNode->pElement)->pKey, pKey, keySize) != 0)
    {
        pNode = pNode->pNext;
    }

    if (pNode == nullptr)
    {
        CRASH();
    }

    // 교환
    KeyValue* pKeyValue = (KeyValue*)pNode->pElement;
    KeyValue* pLastKeyValue = m_pKeyValues + (m_numKeyValues - 1);
    void* pDeletedKey = pKeyValue->pKey;
    void* pDeletedValue = pKeyValue->pValue;
    *pKeyValue = *pLastKeyValue;

    Bucket* pLastBucket = m_pBuckets + pLastKeyValue->Index;
    ListNode* pLastNode = pLastBucket->pHead;
    while (pLastNode != nullptr
           && memcmp(pLastKeyValue->pKey, ((KeyValue*)pLastNode->pElement)->pKey, keySize) != 0)
    {
        pLastNode = pLastNode->pNext;
    }

    pLastNode->pElement = pKeyValue;

    ListDeleteNode(&pBucket->pHead, &pBucket->pTail, pNode);
    --m_numKeyValues;
}

uint __stdcall HashMap::GetCount(const void* pKey, const uint keySize) const
{
    const uint32 hash = UWHash32((const char*)pKey, keySize);
    return GetCountByHash(pKey, keySize, hash);
}

uint __stdcall HashMap::GetCountByHash(const void* pKey, const uint keySize, const uint32 hash) const
{
    if (keySize != m_keySize)
    {
        CRASH();
    }

    const uint index = hash % m_numMaxKeyValues;
    uint count = 0;

    Bucket* pBucket = m_pBuckets + index;
    ListNode* pNode = pBucket->pHead;
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

void* __stdcall HashMap::GetValue(const void* pKey, const uint keySize) const
{
    const uint32 hash = UWHash32((const char*)pKey, keySize);
    return GetValueByHash(pKey, keySize, hash);
}

void* __stdcall HashMap::GetValueByHash(const void* pKey, const uint keySize, const uint32 hash) const
{
    if (keySize != m_keySize)
    {
        CRASH();
    }

    const uint index = hash % m_numMaxKeyValues;

    Bucket* pBucket = m_pBuckets + index;
    ListNode* pNode = pBucket->pHead;
    while (pNode != nullptr && memcmp(((KeyValue*)pNode->pElement)->pKey, pKey, keySize) != 0)
    {
        pNode = pNode->pNext;
    }

    if (pNode == nullptr)
    {
        CRASH();
    }

    KeyValue* pKeyValue = (KeyValue*)pNode->pElement;
    return pKeyValue->pValue;
}

KeyValue* __stdcall HashMap::GetKeyValues() const
{
    return m_pKeyValues;
}

uint __stdcall HashMap::GetKeySize() const
{
    return m_keySize;
}

uint __stdcall HashMap::GetValueSize() const
{
    return m_valueSize;
}

uint __stdcall HashMap::GetNumKeyValues() const
{
    return m_numKeyValues;
}

uint __stdcall HashMap::GetNumMaxKeyValues() const
{
    return m_numMaxKeyValues;
}

#pragma warning(pop)