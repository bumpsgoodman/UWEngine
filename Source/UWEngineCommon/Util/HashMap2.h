#pragma once

#include "List.h"

struct BUCKET2_DESC
{
    ListNode* pHead;
    ListNode* pTail;
};

class HashMap2 final
{
public:
    HashMap2() = default;
    HashMap2(const HashMap2&) = delete;
    HashMap2& operator=(const HashMap2&) = delete;
    ~HashMap2();

    bool    __stdcall   Initialize(const uint keySize, const uint valueSize, const uint numMaxKeyValues);
    void    __stdcall   Release();
    void    __stdcall   Clear();

    void*   __stdcall   Insert(const void* pKey, const uint keySize, const void* pValue, const uint valueSize);

private:
    BUCKET2_DESC*   m_pBuckets = nullptr;
    ListNode*       m_pNodePool = nullptr;
    char*           m_pKeyValuePool = nullptr;

    uint            m_keySize = 0;
    uint            m_valueSize = 0;
    uint            m_numKeyValues = 0;
    uint            m_numMaxKeyValues = 0;

    BUCKET2_DESC*   m_pFirstBucket = nullptr;
    BUCKET2_DESC*   m_pLastBucket = nullptr;
};