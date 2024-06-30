#include "Precompiled.h"
#include "UWEngineCommon/Util/HashMap2.h"

HashMap2::~HashMap2()
{
    Release();
}

bool __stdcall HashMap2::Initialize(const uint keySize, const uint valueSize, const uint numMaxKeyValues)
{
    CRASH(keySize > 0, "keySize == 0");
    CRASH(valueSize > 0, "valueSize == 0");
    CRASH(numMaxKeyValues > 0, "numMaxKeyValues == 0");

    m_pBuckets = (BUCKET2_DESC*)malloc(sizeof(BUCKET2_DESC) * numMaxKeyValues);
    m_pNodePool = (ListNode*)malloc(sizeof(ListNode) * numMaxKeyValues);
    m_pKeyValuePool = (char*)malloc((keySize + valueSize) * numMaxKeyValues);

    m_keySize = keySize;
    m_valueSize = valueSize;
    m_numMaxKeyValues = numMaxKeyValues;
    m_numKeyValues = 0;

    m_pFirstBucket = nullptr;
    m_pLastBucket = nullptr;

    return true;
}

void __stdcall HashMap2::Release()
{
    SAFE_FREE(m_pKeyValuePool);
    SAFE_FREE(m_pNodePool);
    SAFE_FREE(m_pBuckets);
}

void __stdcall HashMap2::Clear()
{
    memset(m_pBuckets, 0, sizeof(BUCKET2_DESC) * m_numMaxKeyValues);
    m_numKeyValues = 0;
}

void* __stdcall HashMap2::Insert(const void* pKey, const uint keySize, const void* pValue, const uint valueSize)
{
    CRASH(pKey != nullptr, "pKey == nullptr");
    CRASH(keySize != m_keySize, "Mismatch key size");
    CRASH(pValue != nullptr, "pValue == nullptr");
    CRASH(valueSize != m_valueSize, "Mismatch key size");

    CRASH(m_numKeyValues < m_numMaxKeyValues, "Overflow");

    const uint32 hash = UWHash32((const char*)pKey, keySize);
    const uint index = hash % m_numMaxKeyValues;

    char* pKeyPool = m_pKeyValuePool + (keySize + valueSize) * m_numKeyValues;
    char* pValuePool = pKeyPool + keySize;

    memcpy(pKeyPool, pKey, keySize);
    memcpy(pValuePool, pValue, valueSize);

    ListNode* pNode = m_pNodePool + m_numKeyValues;
    pNode->pElement = pKeyPool;
    pNode->pNext = nullptr;
    pNode->pPrev = nullptr;

    BUCKET2_DESC* pBucket = m_pBuckets + index;
    ListAddTail(&pBucket->pHead, &pBucket->pTail, pNode);

    if (m_pFirstBucket != nullptr && pBucket < m_pFirstBucket)
    {
        m_pFirstBucket = pBucket;
    }

    if (m_pLastBucket != nullptr && pBucket > m_pLastBucket)
    {

    }

    return pValuePool;
}