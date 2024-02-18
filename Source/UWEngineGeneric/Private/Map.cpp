/*
* Map
*
* �ۼ���: bumpsgoodman
* �ۼ���: 2023.12.20
*/
#include "Precompiled.h"
#include "Map.h"
#include "Util/HashFunction.h"

static constexpr size_t S_BUCKET_SIZE_LIST[] =
{
    7u, 23u, 97u, 397u, 1597u, 6421u, 25717u, 102877u,
    411527u, 879743u, 1799639u, 6584983u, 26339969u, 52679969u
};
static constexpr size_t S_NUM_BUCKET_SIZE_LIST = sizeof(S_BUCKET_SIZE_LIST) / sizeof(size_t);

Map::~Map()
{
    Release();
}

bool __stdcall Map::Initialize(const size_t keySize, const size_t valueSize, const size_t numKeyValuesPerBlock, const size_t numMaxBlocks)
{
    ASSERT(keySize > 0, "keySize == 0");
    ASSERT(valueSize > 0, "valueSize == 0");
    ASSERT(numKeyValuesPerBlock > 0, "numKeyValuesPerBlock == 0");
    ASSERT(numMaxBlocks > 0, "numMaxBlocks == 0");

    Release();

    // ���� ũ�� ���ϱ�
    const size_t baseSize = numKeyValuesPerBlock * 2;
    bool bLock = false;
    for (size_t i = 0; i < S_NUM_BUCKET_SIZE_LIST; ++i)
    {
        const size_t size = S_BUCKET_SIZE_LIST[i];

        if (!bLock && size > baseSize)
        {
            mBucketSize = size;
            bLock = true;
        }

        if (size > baseSize * numMaxBlocks)
        {
            mMaxBucketSize = size;
            break;
        }
    }

    // ����
    {
        // ����
        mBucket = (List*)VirtualAlloc(nullptr,
                                      sizeof(List) * mMaxBucketSize,
                                      MEM_RESERVE, PAGE_READWRITE);
        ASSERT(mBucket != nullptr, "Failed to reserve bucket");

        // Ű-�� �����
        mKeyValues = (KeyValue*)VirtualAlloc(nullptr,
                                             sizeof(KeyValue) * numKeyValuesPerBlock * numMaxBlocks,
                                             MEM_RESERVE, PAGE_READWRITE);
        ASSERT(mKeyValues != nullptr, "Failed to reserve key-value pool");

        // Ű Ǯ
        mKeyPool = VirtualAlloc(nullptr,
                                keySize * numKeyValuesPerBlock * numMaxBlocks,
                                MEM_RESERVE, PAGE_READWRITE);
        ASSERT(mKeyPool != nullptr, "Failed to reserve key pool");

        // �� Ǯ
        mValuePool = VirtualAlloc(nullptr,
                                  valueSize * numKeyValuesPerBlock * numMaxBlocks,
                                  MEM_RESERVE, PAGE_READWRITE);
        ASSERT(mValuePool != nullptr, "Failed to reserve value pool");

        // ��� Ǯ
        mNodePool = (ListNode*)VirtualAlloc(nullptr,
                                            sizeof(ListNode) * numKeyValuesPerBlock * numMaxBlocks,
                                            MEM_RESERVE, PAGE_READWRITE);
        ASSERT(mNodePool != nullptr, "Failed to reserve node pool");
    }

    // Ŀ��
    {
        // ����
        mBucket = (List*)VirtualAlloc(nullptr, sizeof(List) * mBucketSize, MEM_COMMIT, PAGE_READWRITE);
        ASSERT(mBucket != nullptr, "Failed to commit bucket");

        // Ű-�� �����
        mKeyValues = (KeyValue*)VirtualAlloc(nullptr, sizeof(KeyValue) * numMaxBlocks, MEM_RESERVE, PAGE_READWRITE);
        ASSERT(mKeyValues != nullptr, "Failed to commit key-value pool");

        // Ű Ǯ
        mKeyPool = VirtualAlloc(nullptr, keySize * numKeyValuesPerBlock, MEM_COMMIT, PAGE_READWRITE);
        ASSERT(mKeyPool != nullptr, "Failed to commit key pool");

        // �� Ǯ
        mValuePool = VirtualAlloc(nullptr, valueSize * numKeyValuesPerBlock, MEM_COMMIT, PAGE_READWRITE);
        ASSERT(mValuePool != nullptr, "Failed to commit value pool");

        // ��� Ǯ
        mNodePool = (ListNode*)VirtualAlloc(nullptr, sizeof(ListNode) * numKeyValuesPerBlock, MEM_COMMIT, PAGE_READWRITE);
        ASSERT(mNodePool != nullptr, "Failed to commit node pool");
    }

    mKeySize = keySize;
    mValueSize = valueSize;
    mNumKeyValuesPerBlock = numKeyValuesPerBlock;
    mNumMaxBlocks = numMaxBlocks;
    mNumBlocks = 1;
    mNumKeyValues = 0;

    return true;
}

void __stdcall Map::Release()
{
    SAFE_VIRTUAL_FREE(mNodePool);
    SAFE_VIRTUAL_FREE(mValuePool);
    SAFE_VIRTUAL_FREE(mKeyPool);
    SAFE_VIRTUAL_FREE(mKeyValues);
    SAFE_VIRTUAL_FREE(mBucket);
}

void __stdcall Map::Clear()
{
    mNumKeyValues = 0;
    memset(mBucket, 0, sizeof(List) * mBucketSize);
}

void __stdcall Map::Insert(const void* const key, const size_t keySize, const void* const value, const size_t valueSize)
{
    ASSERT(key != nullptr, "key == nullptr");
    ASSERT(keySize == mKeySize, "Mismatch key size");
    ASSERT(value != nullptr, "value == nullptr");
    ASSERT(valueSize == mValueSize, "Mismatch value size");

    const uint64_t hash = FNV1a64((const char*)key, keySize);
    InsertByHash(key, keySize, value, valueSize, hash);
}

void __stdcall Map::InsertByHash(const void* const key, const size_t keySize, const void* const value, const size_t valueSize, const uint64_t hash)
{
    ASSERT(key != nullptr, "key == nullptr");
    ASSERT(keySize == mKeySize, "Mismatch key size");
    ASSERT(value != nullptr, "value == nullptr");
    ASSERT(valueSize == mValueSize, "Mismatch value size");

    if (mNumKeyValues >= mNumKeyValuesPerBlock * mNumBlocks)
    {
        expand();
    }

    size_t index = hash % mBucketSize;
    List* list = mBucket + index;

    // �̹� ���� Ű�� �ִ��� �˻�
    // ���� Ű�� �ִٸ� �� �ٲٱ�
    {
        ListNode* node = list->Head;
        while (node != nullptr && memcmp(((KeyValue*)node->Element)->Key, key, keySize) != 0)
        {
            node = node->Next;
        }

        if (node != nullptr)
        {
            KeyValue* keyValue = (KeyValue*)node->Element;
            memcpy(keyValue->Value, value, valueSize);
        }
    }

    ListNode* node = mNodePool + mNumKeyValues;
    KeyValue* keyValue = mKeyValues + mNumKeyValues;

    // Ű-�� ����
    keyValue->Key = (char*)mKeyPool + keySize * mNumKeyValues;
    keyValue->Value = (char*)mValuePool + valueSize * mNumKeyValues;
    memcpy(keyValue->Key, key, keySize);
    memcpy(keyValue->Value, value, valueSize);

    keyValue->Index = index; // ������ �� �ʿ�

    node->Element = keyValue;
    ListAddTail(&list->Head, &list->Tail, node);

    ++mNumKeyValues;
}

void __stdcall Map::Remove(const void* const key, const size_t keySize)
{
    ASSERT(key != nullptr, "key == nullptr");
    ASSERT(keySize == mKeySize, "Mismatch key size");

    const uint64_t hash = FNV1a64((const char*)key, keySize);
    RemoveByHash(key, keySize, hash);
}

void __stdcall Map::RemoveByHash(const void* const key, const size_t keySize, const uint64_t hash)
{
    ASSERT(key != nullptr, "key == nullptr");
    ASSERT(keySize == mKeySize, "Mismatch key size");

    const size_t index = hash % mBucketSize;

    List* list = mBucket + index;

    // Ű ã��
    ListNode* node = list->Head;
    while (node != nullptr && memcmp(((KeyValue*)node->Element)->Key, key, keySize) != 0)
    {
        node = node->Next;
    }

    if (node == nullptr)
    {
        return;
    }

    KeyValue* key_value = (KeyValue*)node->Element;
    KeyValue* last_key_value = mKeyValues + (mNumKeyValues - 1);
    void* deleted_key = key_value->Key;
    void* deleted_value = key_value->Value;
    *key_value = *last_key_value;

    List* last_list = mBucket + last_key_value->Index;
    ListNode* last_node = last_list->Head;
    while (last_node != nullptr
           && memcmp(last_key_value->Key, ((KeyValue*)last_node->Element)->Key, keySize) != 0)
    {
        last_node = last_node->Next;
    }

    // �� �������� ������ ���� NULL�� �� �� ����
    ASSERT(last_node != nullptr, "last_node == nullptr");
    last_node->Element = key_value;

    ListRemoveNode(&list->Head, &list->Tail, node);
    --mNumKeyValues;
}

size_t __stdcall Map::GetCount(const void* const key, const size_t keySize) const
{
    ASSERT(key != nullptr, "key == nullptr");
    ASSERT(keySize == mKeySize, "Mismatch key size");

    const uint64_t hash = FNV1a64((const char*)key, keySize);
    return GetCountByHash(key, keySize, hash);
}

size_t __stdcall Map::GetCountByHash(const void* const key, const size_t keySize, const uint64_t hash) const
{
    ASSERT(key != nullptr, "key == nullptr");
    ASSERT(keySize == mKeySize, "Mismatch key size");

    const size_t index = hash % mBucketSize;
    size_t count = 0;

    List* list = mBucket + index;
    ListNode* node = list->Head;
    while (node != NULL)
    {
        if (memcmp(((KeyValue*)node->Element)->Key, key, keySize) == 0)
        {
            ++count;
        }

        node = node->Next;
    }

    return count;
}

void* Map::GetValueOrNull(const void* const key, const size_t keySize) const
{
    ASSERT(key != nullptr, "key == nullptr");
    ASSERT(keySize == mKeySize, "Mismatch key size");

    const uint64_t hash = FNV1a64((const char*)key, keySize);
    return GetValueByHashOrNull(key, keySize, hash);
}

void* Map::GetValueByHashOrNull(const void* const key, const size_t keySize, const uint64_t hash) const
{
    ASSERT(key != nullptr, "key == nullptr");
    ASSERT(keySize == mKeySize, "Mismatch key size");

    const size_t index = hash % mBucketSize;

    List* list = mBucket + index;
    ListNode* node = list->Head;
    while (node != nullptr && memcmp(((KeyValue*)node->Element)->Key, key, keySize) != 0)
    {
        node = node->Next;
    }

    if (node == nullptr)
    {
        return nullptr;
    }

    KeyValue* keyValue = (KeyValue*)node->Element;
    return keyValue->Value;
}

void __stdcall Map::expand()
{
    ASSERT(mNumBlocks < mNumMaxBlocks, "Full");

    // ���� ũ�� ���ϱ�
    const size_t baseSize = mNumKeyValues * 2;
    size_t bucketSize = 0;
    for (size_t i = 0; i < S_NUM_BUCKET_SIZE_LIST; ++i)
    {
        const size_t size = S_BUCKET_SIZE_LIST[i];
        if (size > baseSize)
        {
            bucketSize = bucketSize;
            break;
        }
    }

    // ���� ũ�� �ø���
    const size_t expandedBucketSize = bucketSize - mBucketSize;
    if (expandedBucketSize > 0)
    {
        List* bucket = mBucket + mBucketSize;
        mBucket = (List*)VirtualAlloc(bucket, expandedBucketSize, MEM_COMMIT, PAGE_READWRITE);
        ASSERT(mBucket != nullptr, "Failed to commit bucket");

        mBucketSize = bucketSize;
    }

    void* keyPool = (char*)mKeyPool + mKeySize * mNumKeyValues;
    void* valuePool = (char*)mKeyPool + mValueSize * mNumKeyValues;
    ListNode* nodePool = (ListNode*)mKeyPool + mNumKeyValues;

    // Ű Ǯ
    mKeyPool = VirtualAlloc(keyPool, mKeySize * mNumKeyValuesPerBlock, MEM_COMMIT, PAGE_READWRITE);
    ASSERT(mKeyPool != nullptr, "Failed to commit key pool");

    // �� Ǯ
    mValuePool = VirtualAlloc(valuePool, mValueSize * mNumKeyValuesPerBlock, MEM_COMMIT, PAGE_READWRITE);
    ASSERT(mValuePool != nullptr, "Failed to commit value pool");

    // ��� Ǯ
    mNodePool = (ListNode*)VirtualAlloc(nodePool, sizeof(ListNode) * mNumKeyValuesPerBlock, MEM_COMMIT, PAGE_READWRITE);
    ASSERT(mNodePool != nullptr, "Failed to commit node pool");

    ++mNumBlocks;
}