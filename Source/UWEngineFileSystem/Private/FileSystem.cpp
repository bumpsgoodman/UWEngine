/*
* UWEngine FileSystem
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.17
*/

#include "Precompiled.h"
#include "FileSystem.h"
#include "UW3D.h"
#include "UWEngineGeneric/IHashMap.h"

#define DEFAULT_UW3D_MAP_SIZE 16

vsize __stdcall FileSystem::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

vsize __stdcall FileSystem::Release()
{
    --m_refCount;
    if (m_refCount == 0)
    {
        // UW3D 해시맵 해제
        KeyValue* pKeyValues = m_pUW3DMap->GetKeyValuesOrNull();
        for (vsize i = 0; i < m_pUW3DMap->GetNumKeyValues(); ++i)
        {
            UW3DUnload((const wchar_t*)pKeyValues[i].pKey);

            void** ppValue = (void**)pKeyValues[i].pValue;
            SAFE_FREE(*ppValue);
        }
        DestroyHashMap(m_pUW3DMap);

        delete this;
        return 0;
    }

    return m_refCount;
}

vsize __stdcall FileSystem::GetRefCount() const
{
    return m_refCount;
}

bool __stdcall FileSystem::Initialize()
{
    bool bResult = false;

    CreateHashMap(&m_pUW3DMap);
    if (!m_pUW3DMap->Initialize(sizeof(wchar_t) * UW_MAX_FILE_PATH, UW_PTR_SIZE, DEFAULT_UW3D_MAP_SIZE))
    {
        CRASH();
        goto lb_return;
    }

    bResult = true;

lb_return:
    if (!bResult)
    {
        DestroyHashMap(m_pUW3DMap);
    }

    return bResult;
}

UW3D_HANDLE __stdcall FileSystem::UW3DLoadOrNull(const wchar_t* pFilePath)
{
    ASSERT(pFilePath != nullptr, "pPath == nullptr");

    UW3D_HANDLE handle = nullptr;
    FILE* fp = nullptr;

    void** ppUW3D = (void**)m_pUW3DMap->GetValueOrNull(pFilePath, sizeof(wchar_t) * UW_MAX_FILE_PATH);
    if (ppUW3D != nullptr)
    {
        handle = *ppUW3D;
        goto lb_return;
    }

    fp = _wfopen(pFilePath, L"rb");
    if (fp == nullptr)
    {
        CRASH();
        goto lb_return;
    }

    // 시그니처 읽기
    char magic[8] = {};
    fread(magic, sizeof(magic), 1, fp);
    if (strcmp(magic, "uw3d") != 0)
    {
        CRASH();
        goto lb_return;
    }

    UW3D_DESC* pDesc = new UW3D_DESC;
    memset(pDesc, 0, sizeof(UW3D_DESC));

    // 머티리얼 개수 읽기
    fread(&pDesc->NumMaterials, sizeof(uint), 1, fp);

    if (pDesc->NumMaterials > 0)
    {
        pDesc->pppTexturesPathOrNull = (wchar_t***)malloc(UW_PTR_SIZE * pDesc->NumMaterials);
        ASSERT(pDesc->pppTexturesPathOrNull != nullptr, "Failed to malloc pppTexturesPathOrNull");

        pDesc->pNumTexturesOrNull = (uint*)malloc(UW_PTR_SIZE * pDesc->NumMaterials);
        ASSERT(pDesc->pNumTexturesOrNull != nullptr, "Failed to malloc pNumTexturesOrNull");
    }

    // 텍스처 경로 읽기
    for (vsize i = 0; i < pDesc->NumMaterials; ++i)
    {
        fread(&pDesc->pNumTexturesOrNull[i], sizeof(uint), 1, fp);

        if (pDesc->pNumTexturesOrNull[i] > 0)
        {
            pDesc->pppTexturesPathOrNull[i] = (wchar_t**)malloc(UW_PTR_SIZE * pDesc->pNumTexturesOrNull[i]);
            ASSERT(pDesc->pppTexturesPathOrNull[i] != nullptr, "Failed to malloc pppTexturesPathOrNull[i]");
        }

        for (vsize j = 0; j < pDesc->pNumTexturesOrNull[i]; ++j)
        {
            pDesc->pppTexturesPathOrNull[i][j] = (wchar_t*)malloc(sizeof(wchar_t) * UW_MAX_FILE_PATH);
            ASSERT(pDesc->pppTexturesPathOrNull[i][j] != nullptr, "Failed to malloc pppTexturesPathOrNull[i][j]");

            fread(pDesc->pppTexturesPathOrNull[i][j], sizeof(wchar_t), UW_MAX_FILE_PATH, fp);
        }
    }

    // 오브젝트 개수 읽기
    fread(&pDesc->NumObjects, sizeof(uint), 1, fp);

    pDesc->pIncludeFlags = (uint*)malloc(sizeof(uint) * pDesc->NumObjects);
    pDesc->pNumVertices = (uint*)malloc(sizeof(uint) * pDesc->NumObjects);
    pDesc->pNumIndexBuffers = (uint*)malloc(sizeof(uint) * pDesc->NumObjects);
    ASSERT(pDesc->pIncludeFlags != nullptr, "Failed to malloc pIncludeFlags");
    ASSERT(pDesc->pNumVertices != nullptr, "Failed to malloc pNumVertices");
    ASSERT(pDesc->pNumIndexBuffers != nullptr, "Failed to malloc pNumIndexBuffers");

    pDesc->ppVertices = (void**)malloc(UW_PTR_SIZE * pDesc->NumObjects);
    ASSERT(pDesc->ppVertices != nullptr, "Failed to malloc ppVertices");

    pDesc->pppIndexBuffers = (uint16***)malloc(UW_PTR_SIZE * pDesc->NumObjects);
    pDesc->ppNumIndices = (uint16**)malloc(UW_PTR_SIZE * pDesc->NumObjects);
    ASSERT(pDesc->pppIndexBuffers != nullptr, "Failed to malloc pppIndexBuffers");
    ASSERT(pDesc->ppNumIndices != nullptr, "Failed to malloc ppNumIndices");

    pDesc->pMaterialIDsOrNull = (uint*)malloc(sizeof(uint) * pDesc->NumObjects);
    ASSERT(pDesc->pMaterialIDsOrNull != nullptr, "Failed to malloc pMaterialIDsOrNull");

    for (vsize i = 0; i < pDesc->NumObjects; ++i)
    {
        // 오브젝트 정보 읽기
        fread(&pDesc->pIncludeFlags[i], sizeof(uint), 1, fp);
        fread(&pDesc->pNumVertices[i], sizeof(uint), 1, fp);
        
        fread(&pDesc->pMaterialIDsOrNull[i], sizeof(uint), 1, fp);
        fread(&pDesc->pNumIndexBuffers[i], sizeof(uint), 1, fp);

        pDesc->ppVertices[i] = malloc(UW3D_VERTEX_SIZE * pDesc->pNumVertices[i]);
        ASSERT(pDesc->ppVertices[i] != nullptr, "Failed to malloc ppVertices[i]");

        if (GET_MASK(pDesc->pIncludeFlags[i], UW3D_INCLUDE_FLAG_TEXTURE))
        {
            if (pDesc->ppTexCoordsOrNull == nullptr)
            {
                pDesc->ppTexCoordsOrNull = (void**)malloc(UW_PTR_SIZE * pDesc->NumObjects);
                ASSERT(pDesc->ppTexCoordsOrNull != nullptr, "Failed to malloc ppTexCoordsOrNull");
            }

            pDesc->ppTexCoordsOrNull[i] = malloc(UW3D_TEXCOORD_SIZE * pDesc->pNumVertices[i]);
            ASSERT(pDesc->ppTexCoordsOrNull[i] != nullptr, "Failed to malloc ppTexCoordsOrNull[i]");
        }

        // 버텍스 읽기
        fread(pDesc->ppVertices[i], UW3D_VERTEX_SIZE, pDesc->pNumVertices[i], fp);

        // 텍스처 좌표 읽기
        if (GET_MASK(pDesc->pIncludeFlags[i], UW3D_INCLUDE_FLAG_TEXTURE))
        {
            fread(pDesc->ppTexCoordsOrNull[i], UW3D_TEXCOORD_SIZE, pDesc->pNumVertices[i], fp);
        }

        pDesc->pppIndexBuffers[i] = (uint16**)malloc(UW_PTR_SIZE * pDesc->pNumIndexBuffers[i]);
        ASSERT(pDesc->pppIndexBuffers[i] != nullptr, "Failed to malloc pppIndexBuffers[i]");

        pDesc->ppNumIndices[i] = (uint16*)malloc(sizeof(uint16) * pDesc->pNumIndexBuffers[i]);
        ASSERT(pDesc->ppNumIndices[i] != nullptr, "Failed to malloc ppNumIndices[i]");

        for (vsize j = 0; j < pDesc->pNumIndexBuffers[i]; ++j)
        {
            // 인덱스 개수 읽기
            fread(&pDesc->ppNumIndices[i][j], sizeof(uint16), 1, fp);

            // 인덱스 읽기
            pDesc->pppIndexBuffers[i][j] = (uint16*)malloc(sizeof(uint16) * pDesc->ppNumIndices[i][j]);
            ASSERT(pDesc->pppIndexBuffers[i][j] != nullptr, "Failed to malloc pppIndexBuffers");

            fread(pDesc->pppIndexBuffers[i][j], sizeof(uint16), pDesc->ppNumIndices[i][j], fp);
        }
    }

    m_pUW3DMap->Insert(pFilePath, sizeof(wchar_t)* UW_MAX_FILE_PATH, &pDesc, UW_PTR_SIZE);

    handle = pDesc;

lb_return:
    if (fp != nullptr)
    {
        fclose(fp);
    }

    return handle;
}

void __stdcall FileSystem::UW3DUnload(const wchar_t* pFilePath)
{
    UW3D_DESC** ppUW3D = (UW3D_DESC**)m_pUW3DMap->GetValueOrNull(pFilePath, sizeof(wchar_t) * UW_MAX_FILE_PATH);
    if (ppUW3D != nullptr)
    {
        UW3D_DESC* pDesc = *ppUW3D;
        
        for (vsize i = 0; i < pDesc->NumMaterials; ++i)
        {
            for (vsize j = 0; j < pDesc->pNumTexturesOrNull[i]; ++j)
            {
                SAFE_FREE(pDesc->pppTexturesPathOrNull[i][j]);
            }
            SAFE_FREE(pDesc->pppTexturesPathOrNull[i]);
        }
        SAFE_FREE(pDesc->pppTexturesPathOrNull);

        SAFE_FREE(pDesc->pNumTexturesOrNull);

        for (vsize i = 0; i < pDesc->NumObjects; ++i)
        {
            SAFE_FREE(pDesc->ppVertices[i]);
            SAFE_FREE(pDesc->ppTexCoordsOrNull[i]);

            for (vsize j = 0; j < pDesc->pNumIndexBuffers[i]; ++j)
            {
                SAFE_FREE(pDesc->pppIndexBuffers[i][j]);
            }
            SAFE_FREE(pDesc->pppIndexBuffers[i]);
            SAFE_FREE(pDesc->ppNumIndices[i]);
        }
        SAFE_FREE(pDesc->pppIndexBuffers);
        SAFE_FREE(pDesc->ppTexCoordsOrNull);
        SAFE_FREE(pDesc->ppNumIndices);
        SAFE_FREE(pDesc->ppVertices);
        SAFE_FREE(pDesc->pNumIndexBuffers);

        SAFE_FREE(pDesc->pIncludeFlags);
        SAFE_FREE(pDesc->pNumVertices);
        SAFE_FREE(pDesc->pMaterialIDsOrNull);
    }
}

uint __stdcall FileSystem::UW3DGetNumMaterials(const UW3D_HANDLE handle)
{
    ASSERT(handle != nullptr, "handle == nullptr");
    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    return pDesc->NumMaterials;
}

uint __stdcall FileSystem::UW3DGetNumTextures(const UW3D_HANDLE handle, const uint materialID)
{
    ASSERT(handle != nullptr, "handle == nullptr");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    ASSERT(materialID < pDesc->NumMaterials, "Invalid materialID");

    return pDesc->pNumTexturesOrNull[materialID];
}

const wchar_t** __stdcall FileSystem::UW3DGetTexturesPath(const UW3D_HANDLE handle, const uint materialID)
{
    ASSERT(handle != nullptr, "handle == nullptr");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    ASSERT(materialID < pDesc->NumMaterials, "Invalid materialID");

    return (const wchar_t**)pDesc->pppTexturesPathOrNull[materialID];
}

uint __stdcall FileSystem::UW3DGetNumObjects(const UW3D_HANDLE handle)
{
    ASSERT(handle != nullptr, "handle == nullptr");
    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    return pDesc->NumObjects;
}

uint __stdcall FileSystem::UW3DGetIncludeFlag(const UW3D_HANDLE handle, const uint objectID)
{
    ASSERT(handle != nullptr, "handle == nullptr");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    ASSERT(objectID < pDesc->NumObjects, "Invalid objectID");

    return pDesc->pIncludeFlags[objectID];
}

uint __stdcall FileSystem::UW3DGetNumVertices(const UW3D_HANDLE handle, const uint objectID)
{
    ASSERT(handle != nullptr, "handle == nullptr");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    ASSERT(objectID < pDesc->NumObjects, "Invalid objectID");

    return pDesc->pNumVertices[objectID];
}

uint __stdcall FileSystem::UW3DGetMaterialID(const UW3D_HANDLE handle, const uint objectID)
{
    ASSERT(handle != nullptr, "handle == nullptr");
    ASSERT(GET_MASK(UW3DGetIncludeFlag(handle, objectID), UW3D_INCLUDE_FLAG_TEXTURE), "Invalid includeFlag");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    return pDesc->pMaterialIDsOrNull[objectID];
}

uint __stdcall FileSystem::UW3DGetNumIndexBuffers(const UW3D_HANDLE handle, const uint objectID)
{
    ASSERT(handle != nullptr, "handle == nullptr");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    ASSERT(objectID < pDesc->NumObjects, "Invalid objectID");

    return pDesc->pNumIndexBuffers[objectID];
}

const void* __stdcall FileSystem::UW3DGetVertices(const UW3D_HANDLE handle, const uint objectID)
{
    ASSERT(handle != nullptr, "handle == nullptr");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    ASSERT(objectID < pDesc->NumObjects, "Invalid objectID");

    return (const void*)pDesc->ppVertices[objectID];
}

const void* __stdcall FileSystem::UW3DGetTexCoords(const UW3D_HANDLE handle, const uint objectID)
{
    ASSERT(handle != nullptr, "handle == nullptr");
    ASSERT(GET_MASK(UW3DGetIncludeFlag(handle, objectID), UW3D_INCLUDE_FLAG_TEXTURE), "Invalid includeFlag");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    return (const void*)pDesc->ppTexCoordsOrNull[objectID];
}

const uint16** __stdcall FileSystem::UW3DGetIndexBuffers(const UW3D_HANDLE handle, const uint objectID)
{
    ASSERT(handle != nullptr, "handle == nullptr");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    ASSERT(objectID < pDesc->NumObjects, "Invalid objectID");

    return (const uint16**)pDesc->pppIndexBuffers[objectID];
}

const uint16* __stdcall FileSystem::UW3DGetNumIndices(const UW3D_HANDLE handle, const uint objectID)
{
    ASSERT(handle != nullptr, "handle == nullptr");

    const UW3D_DESC* pDesc = (UW3D_DESC*)handle;
    ASSERT(objectID < pDesc->NumObjects, "Invalid objectID");

    return (const uint16*)pDesc->ppNumIndices[objectID];
}

uint __stdcall FileSystem::UW3DGetVertexSize()
{
    return UW3D_VERTEX_SIZE;
}

uint __stdcall FileSystem::UW3DGetTexCoordSize()
{
    return UW3D_TEXCOORD_SIZE;
}
