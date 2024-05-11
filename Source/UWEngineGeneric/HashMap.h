/*
* HashMap
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.20
* 
* ~ByHash 함수로 호출자가 직접 hash를 넣을 수 있음
* 매우 조심히 사용할 것
*/

#pragma once

struct KeyValue
{
    void* pKey;
    void* pValue;

    // 제거 및 확장에 필요
    // 수정 금지
    vsize Index;
};

class HashMap final
{
public:
    HashMap() = default;
    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;
    HashMap(HashMap&&) = default;
    HashMap& operator=(HashMap&&) = default;
    ~HashMap();

    bool        __stdcall   Initialize(const vsize keySize, const vsize valueSize, const vsize numDefaultKeyValues);
    void        __stdcall   Release();
    void        __stdcall   Clear();

    bool        __stdcall   Insert(const void* pKey, const vsize keySize, const void* pValue, const vsize valueSize);
    bool        __stdcall   InsertByHash(const void* pKey, const vsize keySize, const void* pValue, const vsize valueSize, const uint64 hash);

    bool        __stdcall   Remove(const void* pKey, const vsize keySize);
    bool        __stdcall   RemoveByHash(const void* pKey, const vsize keySize, const uint64 hash);

    vsize       __stdcall   GetCount(const void* pKey, const vsize keySize) const;
    vsize       __stdcall   GetCountByHash(const void* pKey, const vsize keySize, const uint64 hash) const;

    void*       __stdcall   GetValueOrNull(const void* pKey, const vsize keySize) const;
    void*       __stdcall   GetValueByHashOrNull(const void* pKey, const vsize keySize, const uint64 hash) const;

    KeyValue*   __stdcall   GetKeyValuesOrNull() const;
    vsize       __stdcall   GetKeySize() const;
    vsize       __stdcall   GetValueSize() const;
    vsize       __stdcall   GetNumKeyValues() const;
    vsize       __stdcall   GetNumMaxKeyValues() const;

    float       __stdcall   GetFactor() const;

    // [0.1, 1.0]
    void        __stdcall   SetFactor(float factor);

private:
    bool        __stdcall   expand();

private:
    vsize       m_keySize = 0;
    vsize       m_valueSize = 0;
    vsize       m_numMaxKeyValues = 0;
    vsize       m_numKeyValues = 0;
    float       m_factor = 0.0f;

    List*       m_pBucket = nullptr;
    vsize       m_bucketSize = 0;

    KeyValue*   m_pKeyValues = nullptr;

    void*       m_pKeyPool = nullptr;
    void*       m_pValuePool = nullptr;
    ListNode*   m_pNodePool = nullptr;
};