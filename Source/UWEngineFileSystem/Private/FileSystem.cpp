/*
* UWEngine FileSystem
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.17
*/

#include "Precompiled.h"
#include "FileSystem.h"
#include "UWEngineGeneric/IHashMap.h"

#define DEFAULT_UWMESH_MAP_SIZE 16

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
        // UWMesh 해시맵 해제
        KeyValue* pKeyValues = m_pUWMeshMap->GetKeyValuesOrNull();
        for (vsize i = 0; i < m_pUWMeshMap->GetNumKeyValues(); ++i)
        {
            UnloadUWMesh((const wchar_t*)pKeyValues[i].pKey);
        }
        DestroyHashMap(m_pUWMeshMap);

        // UWBone 해시맵 해제
        pKeyValues = m_pUWBoneMap->GetKeyValuesOrNull();
        for (vsize i = 0; i < m_pUWBoneMap->GetNumKeyValues(); ++i)
        {
            UnloadUWBone((const wchar_t*)pKeyValues[i].pKey);
        }
        DestroyHashMap(m_pUWBoneMap);

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

    CreateHashMap(&m_pUWMeshMap);
    if (!m_pUWMeshMap->Initialize(sizeof(wchar_t) * UW_MAX_FILE_PATH, UW_PTR_SIZE, DEFAULT_UWMESH_MAP_SIZE))
    {
        CRASH();
        goto lb_return;
    }

    CreateHashMap(&m_pUWBoneMap);
    if (!m_pUWBoneMap->Initialize(sizeof(wchar_t) * UW_MAX_FILE_PATH, UW_PTR_SIZE, DEFAULT_UWMESH_MAP_SIZE))
    {
        CRASH();
        goto lb_return;
    }

    bResult = true;

lb_return:
    if (!bResult)
    {
        DestroyHashMap(m_pUWMeshMap);
    }

    if (!bResult)
    {
        DestroyHashMap(m_pUWBoneMap);
    }

    return bResult;
}

bool __stdcall FileSystem::LoadUWMesh(const wchar_t* pFilename, UWMesh* pOutUWMesh)
{
    ASSERT(pFilename != nullptr, "pFilename == nullptr");
    ASSERT(pOutUWMesh != nullptr, "pOutUWMesh == nullptr");

    bool bResult =false;
    FILE* fp = nullptr;

    UWMesh** ppUWMesh = (UWMesh**)m_pUWMeshMap->GetValueOrNull(pFilename, sizeof(wchar_t) * UW_MAX_FILE_PATH);
    if (ppUWMesh != nullptr)
    {
        pOutUWMesh = *ppUWMesh;
        goto lb_return;
    }

    fp = _wfopen(pFilename, L"rb");
    if (fp == nullptr)
    {
        CRASH();
        goto lb_return;
    }

    char magicNumber[8] = {};
    fread(magicNumber, 8, 1, fp);
    if (strcmp(magicNumber, "UWMesh") != 0)
    {
        CRASH();
        goto lb_return;
    }

    // 머티리얼 개수 불러오기
    uint numMaterials;
    fread(&numMaterials, sizeof(uint), 1, fp);

    // 머티리얼 불러오기
    MATERIAL_DESC* pMaterials = new MATERIAL_DESC[numMaterials];
    for (uint i = 0; i < numMaterials; ++i)
    {
        uint numTextures;
        fread(&numTextures, sizeof(uint), 1, fp);

        // 텍스처 경로 불러오기
        wchar_t** ppTextureFilenames = new wchar_t*[numTextures];
        for (uint j = 0; j < numTextures; ++j)
        {
            wchar_t* pTextureFilename = new wchar_t[UW_MAX_FILE_PATH];
            fread(pTextureFilename, sizeof(wchar_t), UW_MAX_FILE_PATH, fp);

            ppTextureFilenames[j] = pTextureFilename;
        }

        pMaterials[i].NumTextures = numTextures;
        pMaterials[i].ppTextureFilenames = ppTextureFilenames;
    }

    // 메시 개수 불러오기
    uint numMeshes;
    fread(&numMeshes, sizeof(uint), 1, fp);

    // 메시 불러오기
    MESH_DESC* pMeshes = new MESH_DESC[numMeshes];
    for (uint i = 0; i < numMeshes; ++i)
    {
        // 인클루드 플래그 불러오기
        uint includeFlag;
        fread(&includeFlag, sizeof(uint), 1, fp);

        // 머티리얼 ID 불러오기
        uint materialID = UINT_MAX;
        if (GET_MASK(includeFlag, UWMESH_INCLUDE_FLAG_TEXTURE))
        {
            materialID;
            fread(&materialID, sizeof(uint), 1, fp);
        }

        // 버텍스 개수 불러오기
        uint numVertices;
        fread(&numVertices, sizeof(uint), 1, fp);

        // 버텍스 불러오기
        void* pVertices = new char[numVertices * UWMESH_VERTEX_SIZE];
        fread(pVertices, UWMESH_VERTEX_SIZE, numVertices, fp);

        // 텍스처 좌표 불러오기
        void* pTexCoords = nullptr;
        if (GET_MASK(includeFlag, UWMESH_INCLUDE_FLAG_TEXTURE))
        {
            pTexCoords = new char[numVertices * UWMESH_TEXCOORD_SIZE];
            fread(pTexCoords, UWMESH_TEXCOORD_SIZE, numVertices, fp);
        }

        // 본 가중치 불러오기
        BoneWeight* pBoneWeights = nullptr;
        if (GET_MASK(includeFlag, UWMESH_INCLUDE_FLAG_SKINNED))
        {
            pBoneWeights = new BoneWeight[numVertices];
            fread(pBoneWeights, sizeof(BoneWeight), numVertices, fp);
        }

        // 인덱스 버퍼 개수 불러오기
        uint numIndexBuffers;
        fread(&numIndexBuffers, sizeof(uint), 1, fp);

        uint16* pNumIndices = new uint16[numIndexBuffers];
        uint16** ppIndices = new uint16*[numIndexBuffers];

        // 인덱스 버퍼 불러오기
        for (uint j = 0; j < numIndexBuffers; ++j)
        {
            // 인덱스 개수 불러오기
            fread(&pNumIndices[j], sizeof(uint16), 1, fp);

            // 인덱스 불러오기
            ppIndices[j] = new uint16[pNumIndices[j]];
            fread(ppIndices[j], sizeof(uint16), pNumIndices[j], fp);
        }

        pMeshes[i].IncludeFlag = includeFlag;
        pMeshes[i].MaterialID = materialID;
        pMeshes[i].NumVertices = numVertices;
        pMeshes[i].pVertices = pVertices;
        pMeshes[i].pTexCoords = pTexCoords;
        pMeshes[i].pBoneWeights = pBoneWeights;
        pMeshes[i].NumIndexBuffers = numIndexBuffers;
        pMeshes[i].pNumIndices = pNumIndices;
        pMeshes[i].ppIndices = ppIndices;
    }

    pOutUWMesh->NumMaterials = numMaterials;
    pOutUWMesh->pMaterials = pMaterials;
    pOutUWMesh->NumMeshes = numMeshes;
    pOutUWMesh->pMeshes = pMeshes;

    m_pUWMeshMap->Insert(pFilename, sizeof(wchar_t) * UW_MAX_FILE_PATH, &pOutUWMesh, UW_PTR_SIZE);

    bResult = true;

lb_return:
    if (fp != nullptr)
    {
        fclose(fp);
    }

    return bResult;
}

void __stdcall FileSystem::UnloadUWMesh(const wchar_t* pFilename)
{
    UWMesh** ppUWMesh = (UWMesh**)m_pUWMeshMap->GetValueOrNull(pFilename, sizeof(wchar_t) * UW_MAX_FILE_PATH);
    if (ppUWMesh != nullptr)
    {
        UWMesh* pUWMesh = *ppUWMesh;

        // 머티리얼 해제
        for (uint i = 0; i < pUWMesh->NumMaterials; ++i)
        {
            MATERIAL_DESC* pMaterial = &pUWMesh->pMaterials[i];
            wchar_t** ppTextureFilenames = pMaterial->ppTextureFilenames;
            ASSERT(ppTextureFilenames != nullptr, "");

            for (uint j = 0; j < pMaterial->NumTextures; ++j)
            {
                SAFE_DELETE_ARRAY(ppTextureFilenames[j]);
            }
            SAFE_DELETE_ARRAY(pMaterial->ppTextureFilenames);
        }
        SAFE_DELETE_ARRAY(pUWMesh->pMaterials);

        // 메시 해제
        for (uint i = 0; i < pUWMesh->NumMeshes; ++i)
        {
            MESH_DESC* pMesh = &pUWMesh->pMeshes[i];

            SAFE_DELETE_ARRAY(pMesh->pVertices);
            SAFE_DELETE_ARRAY(pMesh->pTexCoords);
            SAFE_DELETE_ARRAY(pMesh->pBoneWeights);

            uint16** ppIndices = pMesh->ppIndices;
            ASSERT(ppIndices != nullptr, "");
            for (uint j = 0; j < pMesh->NumIndexBuffers; ++j)
            {
                SAFE_DELETE_ARRAY(ppIndices[j]);
            }
            SAFE_DELETE_ARRAY(pMesh->ppIndices);
            SAFE_DELETE_ARRAY(pMesh->pNumIndices);
        }

        SAFE_DELETE_ARRAY(pUWMesh->pMeshes);
    }
}

bool __stdcall FileSystem::LoadUWBone(const wchar_t* pFilename, UWBone* pOutUWBone)
{
    ASSERT(pFilename != nullptr, "pFilename == nullptr");
    ASSERT(pOutUWBone != nullptr, "pOutUWBone == nullptr");

    bool bResult = false;
    FILE* fp = nullptr;

    UWBone** ppUWBone = (UWBone**)m_pUWMeshMap->GetValueOrNull(pFilename, sizeof(wchar_t) * UW_MAX_FILE_PATH);
    if (ppUWBone != nullptr)
    {
        pOutUWBone = *ppUWBone;
        goto lb_return;
    }

    fp = _wfopen(pFilename, L"rb");
    if (fp == nullptr)
    {
        CRASH();
        goto lb_return;
    }

    char magicNumber[8] = {};
    fread(magicNumber, 8, 1, fp);
    if (strcmp(magicNumber, "UWBone") != 0)
    {
        CRASH();
        goto lb_return;
    }

    bResult = true;

    // 본 개수 불러오기
    uint numBones;
    fread(&numBones, sizeof(uint), 1, fp);

    // 본 불러오기
    BONE_DESC* pBones = new BONE_DESC[numBones];
    for (uint i = 0; i < numBones; ++i)
    {
        // TM 불러오기
        fread(&pBones[i].TransformMatrix, sizeof(Matrix44), 1, fp);

        ANIMATION_DESC* pACB = &pBones[i].Animation;
        memset(pACB, 0, sizeof(ANIMATION_DESC));
        
        // 키 프레임 불러오기
        fread(&pACB->KeyFrame, sizeof(uint), 1, fp);

        // 키 여부 불러오기
        fread(&pACB->bHasPosition, sizeof(bool), 1, fp);
        fread(&pACB->bHasRotation, sizeof(bool), 1, fp);
        fread(&pACB->bHasScale, sizeof(bool), 1, fp);
        
        pACB->pPositions = nullptr;
        if (pACB->bHasPosition)
        {
            // 키 개수 불러오기
            fread(&pACB->NumPositionKeys, sizeof(uint), 1, fp);

            pACB->pPositions = new ANIMATION_DESC::Position[pACB->NumPositionKeys];
            fread(pACB->pPositions, sizeof(ANIMATION_DESC::Position), pACB->NumPositionKeys, fp);
        }

        pACB->pRotations = nullptr;
        if (pACB->bHasRotation)
        {
            // 키 개수 불러오기
            fread(&pACB->NumRotationKeys, sizeof(uint), 1, fp);

            pACB->pRotations = new ANIMATION_DESC::Rotation[pACB->NumRotationKeys];
            fread(pACB->pRotations, sizeof(ANIMATION_DESC::Rotation), pACB->NumRotationKeys, fp);
        }

        pACB->pScales = nullptr;
        if (pACB->bHasScale)
        {
            // 키 개수 불러오기
            fread(&pACB->NumScaleKeys, sizeof(uint), 1, fp);

            pACB->pScales = new ANIMATION_DESC::Scale[pACB->NumScaleKeys];
            fread(pACB->pScales, sizeof(ANIMATION_DESC::Scale), pACB->NumScaleKeys, fp);
        }
    }

    pOutUWBone->NumBones = numBones;
    pOutUWBone->pBones = pBones;

    m_pUWBoneMap->Insert(pFilename, sizeof(wchar_t) * UW_MAX_FILE_PATH, &pOutUWBone, UW_PTR_SIZE);

lb_return:
    if (fp != nullptr)
    {
        fclose(fp);
    }

    return bResult;
}

void __stdcall FileSystem::UnloadUWBone(const wchar_t* pFilename)
{
    UWBone** ppUWBone = (UWBone**)m_pUWBoneMap->GetValueOrNull(pFilename, sizeof(wchar_t) * UW_MAX_FILE_PATH);
    if (ppUWBone != nullptr)
    {
        UWBone* pUWBone = *ppUWBone;

        for (uint i = 0; i < pUWBone->NumBones; ++i)
        {
            BONE_DESC* pBoneBlock = &pUWBone->pBones[i];

            SAFE_DELETE_ARRAY(pBoneBlock->Animation.pPositions);
            SAFE_DELETE_ARRAY(pBoneBlock->Animation.pRotations);
            SAFE_DELETE_ARRAY(pBoneBlock->Animation.pScales);
        }
        SAFE_DELETE_ARRAY(pUWBone->pBones);
    }
}