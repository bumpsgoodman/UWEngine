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
    vsize Index;
};

interface IHashMap
{
    virtual UWMETHOD(bool) Initialize(const vsize keySize, const vsize valueSize, const vsize numDefaultKeyValues) PURE;
    virtual UWMETHOD(void) Release() PURE;
    virtual UWMETHOD(void) Clear() PURE;

    virtual UWMETHOD(bool) Insert(const void* pKey, const vsize keySize, const void* pValue, const vsize valueSize) PURE;
    virtual UWMETHOD(bool) InsertByHash(const void* pKey, const vsize keySize, const void* pValue, const vsize valueSize, const uint64 hash) PURE;

    virtual UWMETHOD(bool) Remove(const void* pKey, const vsize keySize) PURE;
    virtual UWMETHOD(bool) RemoveByHash(const void* pKey, const vsize keySize, const uint64 hash) PURE;

    virtual UWMETHOD(vsize) GetCount(const void* pKey, const vsize keySize) const PURE;
    virtual UWMETHOD(vsize) GetCountByHash(const void* pKey, const vsize keySize, const uint64 hash) const PURE;

    virtual UWMETHOD(void*) GetValueOrNull(const void* pKey, const vsize keySize) const PURE;
    virtual UWMETHOD(void*) GetValueByHashOrNull(const void* pKey, const vsize keySize, const uint64 hash) const PURE;

    virtual UWMETHOD(KeyValue*) GetKeyValuesOrNull() const PURE;
    virtual UWMETHOD(vsize) GetKeySize() const PURE;
    virtual UWMETHOD(vsize) GetValueSize() const PURE;
    virtual UWMETHOD(vsize) GetNumKeyValues() const PURE;
    virtual UWMETHOD(vsize) GetNumMaxKeyValues() const PURE;

    virtual UWMETHOD(float) GetFactor() const PURE;

    // [0.1, 1.0]
    virtual UWMETHOD(void) SetFactor(float factor) PURE;
};

GLOBAL_FUNC UWMETHOD(bool) CreateHashMap(IHashMap** ppOutHashMap);
GLOBAL_FUNC UWMETHOD(void) DestroyHashMap(IHashMap* pHashMap);