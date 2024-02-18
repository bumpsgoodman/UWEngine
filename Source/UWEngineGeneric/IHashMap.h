/*
* HashMap 인터페이스
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
    size_t Index;
};

interface IHashMap
{
    virtual bool __stdcall Initialize(const size_t keySize, const size_t valueSize, const size_t numDefaultKeyValues) = 0;
    virtual void __stdcall Release() = 0;
    virtual void __stdcall Clear() = 0;

    virtual bool __stdcall Insert(const void* pKey, const size_t keySize, const void* pValue, const size_t valueSize) = 0;
    virtual bool __stdcall InsertByHash(const void* pKey, const size_t keySize, const void* pValue, const size_t valueSize, const uint64_t hash) = 0;

    virtual bool __stdcall Remove(const void* pKey, const size_t keySize) = 0;
    virtual bool __stdcall RemoveByHash(const void* pKey, const size_t keySize, const uint64_t hash) = 0;

    virtual size_t __stdcall GetCount(const void* pKey, const size_t keySize) const = 0;
    virtual size_t __stdcall GetCountByHash(const void* pKey, const size_t keySize, const uint64_t hash) const = 0;

    virtual void* GetValueOrNull(const void* pKey, const size_t keySize) const = 0;
    virtual void* GetValueByHashOrNull(const void* pKey, const size_t keySize, const uint64_t hash) const = 0;

    virtual KeyValue* GetKeyValuesOrNull() const = 0;
    virtual size_t __stdcall GetKeySize() const = 0;
    virtual size_t __stdcall GetValueSize() const = 0;
    virtual size_t __stdcall GetNumKeyValues() const = 0;
    virtual size_t __stdcall GetNumMaxKeyValues() const = 0;

    virtual float __stdcall GetFactor() const = 0;

    // [0.1, 1.0]
    virtual void __stdcall SetFactor(float factor) = 0;
};

GLOBAL_FUNC bool __stdcall CreateHashMap(IHashMap** ppOutHashMap);
GLOBAL_FUNC void __stdcall DestroyHashMap(IHashMap* pHashMap);