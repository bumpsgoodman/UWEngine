/*
* HashMap
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.20
* 
* ~ByHash 함수로 호출자가 직접 hash를 넣을 수 있음
*/

#pragma once

#include "List.h"

struct Bucket
{
    ListNode* pHead;
    ListNode* pTail;
};

struct KeyValue
{
    void* pKey;
    void* pValue;

    // 제거 및 확장에 필요
    // 수정 금지
    vsize Index;
};

class HashMap
{
public:
    HashMap() = default;
    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;
    ~HashMap();

    bool        __stdcall   Initialize(const vsize keySize, const vsize valueSize, const vsize numMaxKeyValues);
    void        __stdcall   Release();
    void        __stdcall   Clear();

    void*       __stdcall   Insert(const void* pKey, const vsize keySize, const void* pValue, const vsize valueSize);
    void*       __stdcall   InsertByHash(const void* pKey, const vsize keySize, const void* pValue, const vsize valueSize, const uint32 hash);

    void        __stdcall   Remove(const void* pKey, const vsize keySize);
    void        __stdcall   RemoveByHash(const void* pKey, const vsize keySize, const uint32 hash);

    vsize       __stdcall   GetCount(const void* pKey, const vsize keySize) const;
    vsize       __stdcall   GetCountByHash(const void* pKey, const vsize keySize, const uint32 hash) const;

    void*       __stdcall   GetValue(const void* pKey, const vsize keySize) const;
    void*       __stdcall   GetValueByHash(const void* pKey, const vsize keySize, const uint32 hash) const;

    KeyValue*   __stdcall   GetKeyValues() const;
    vsize       __stdcall   GetKeySize() const;
    vsize       __stdcall   GetValueSize() const;
    vsize       __stdcall   GetNumKeyValues() const;
    vsize       __stdcall   GetNumMaxKeyValues() const;

private:
    vsize       m_keySize = 0;
    vsize       m_valueSize = 0;
    vsize       m_numMaxKeyValues = 0;
    vsize       m_numKeyValues = 0;

    Bucket*     m_pBuckets = nullptr;

    KeyValue*   m_pKeyValues = nullptr;

    void*       m_pKeyPool = nullptr;
    void*       m_pValuePool = nullptr;
    ListNode*   m_pNodePool = nullptr;
};