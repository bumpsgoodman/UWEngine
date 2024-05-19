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

interface IHashMap
{
    virtual bool        __stdcall   Initialize(const vsize keySize, const vsize valueSize, const vsize numDefaultKeyValues) = 0;
    virtual void        __stdcall   Release() = 0;
    virtual void        __stdcall   Clear() = 0;

    virtual bool        __stdcall   Insert(const void* pKey, const vsize keySize, const void* pValue, const vsize valueSize) = 0;
    virtual bool        __stdcall   InsertByHash(const void* pKey, const vsize keySize, const void* pValue, const vsize valueSize, const uint64 hash) = 0;

    virtual bool        __stdcall   Remove(const void* pKey, const vsize keySize) = 0;
    virtual bool        __stdcall   RemoveByHash(const void* pKey, const vsize keySize, const uint64 hash) = 0;

    virtual vsize       __stdcall   GetCount(const void* pKey, const vsize keySize) const = 0;
    virtual vsize       __stdcall   GetCountByHash(const void* pKey, const vsize keySize, const uint64 hash) const = 0;

    virtual void*       __stdcall   GetValueOrNull(const void* pKey, const vsize keySize) const = 0;
    virtual void*       __stdcall   GetValueByHashOrNull(const void* pKey, const vsize keySize, const uint64 hash) const = 0;

    virtual KeyValue*   __stdcall   GetKeyValuesOrNull() const = 0;
    virtual vsize       __stdcall   GetKeySize() const = 0;
    virtual vsize       __stdcall   GetValueSize() const = 0;
    virtual vsize       __stdcall   GetNumKeyValues() const = 0;
    virtual vsize       __stdcall   GetNumMaxKeyValues() const = 0;

    virtual float       __stdcall   GetFactor() const = 0;

    // [0.1, 1.0]
    virtual void        __stdcall   SetFactor(float factor) = 0;
};

GLOBAL_FUNC bool __stdcall CreateHashMap(IHashMap** ppOutHashMap);
GLOBAL_FUNC void __stdcall DestroyHashMap(IHashMap* pHashMapOrNull);