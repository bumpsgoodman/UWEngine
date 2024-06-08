/*
* UWEngine 공용 정의
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#pragma once

#include "Types.h"

#define UW_MAX_FILE_PATH 260

// switch case 문 break 생략 시 삽입
#define FALLTHROUGH

#define TO_STR(s) #s

#define GET_MASK(flag, mask) ((flag) & (mask))

#define UW_DEFAULT_PORT ("9585")

// ASSERT
#if defined(NDEBUG)
#   define ASSERT(cond, msg) ((void)0)
#else
#   define ASSERT(cond, msg) { if (!(cond)) { __debugbreak(); } }
#endif // ASSERT

// Debug/Release 둘 모두 크래시
#define CRASH __debugbreak

// SAFE DELETE
#define SAFE_DELETE(p)              { delete (p); (p) = nullptr; }
#define SAFE_DELETE_ARRAY(p)        { delete[] (p); (p) = nullptr; }
#define SAFE_FREE(p)                { free((p)); (p) = nullptr; }
#define SAFE_RELEASE(p)             { if ((p)) { (p)->Release(); (p) = nullptr; } }
#define SAFE_VIRTUAL_FREE(p)        { VirtualFree((p), 0, MEM_RELEASE); (p) = nullptr; }
#define SAFE_FREE_LIBRARY(handle)   { if ((handle)) { FreeLibrary(handle); (handle) = nullptr; } }

// dll export
#ifdef UW_ENGINE_DLL
#   define GLOBAL_FUNC __declspec(dllexport)
#else
#   define GLOBAL_FUNC __declspec(dllimport)
#endif // UW_ENGINE_DLL

// Alignment
#define DEFAULT_ALIGN 16
#define ALIGN8 _declspec(align(8))
#define ALIGN16 _declspec(align(16))
#define ALIGN32 _declspec(align(32))

// 인터페이스
#ifndef interface
#   define interface struct
#endif // interface

interface IRefObject
{
    virtual vsize   __stdcall   AddRef() = 0;
    virtual vsize   __stdcall   Release() = 0;
    virtual vsize   __stdcall   GetRefCount() const = 0;
};

typedef void(__stdcall* CreateDllInstanceFunc)(void** ppOutInstance);

struct AnimationControlBlock
{
    struct
    {
        float X;
        float Y;
        float Z;
    } Position;

    struct
    {
        float X;
        float Y;
        float Z;
        float W;
    } Rotation;

    struct
    {
        float X;
        float Y;
        float Z;
    } Scale;
};

typedef enum MODEL_TYPE
{
    MODEL_TYPE_MESH,
    MODEL_TYPE_BONE
} MODEL_TYPE;

// 핸들 정의
typedef void* UW3D_HANDLE;

#define UWMESH_VERTEX_SIZE    24
#define UWMESH_TEXCOORD_SIZE  8

enum UWMESH_INCLUDE_FLAG
{
    UWMESH_INCLUDE_FLAG_COLOR =     (1 << 0),
    UWMESH_INCLUDE_FLAG_TEXTURE =   (1 << 1),
    UWMESH_INCLUDE_FLAG_SKINNED =   (1 << 2)
};

struct UWMaterial
{
    uint        NumTextures;
    wchar_t**   ppTextureFilenames;
};

struct UWBoneWeightBlock
{
    uint    NumWeights;
    uint16  Indices[4];
    float   Weights[4];
};

struct UWMeshBlock
{
    UWMESH_INCLUDE_FLAG IncludeFlag;

    uint                MaterialID;

    uint                NumVertices;
    void*               pVertices;          // [Position(xyz), Normal(xyz)]
    void*               pTexCoords;         // [TexCoord(xy)]
    UWBoneWeightBlock*  pBoneWeights;

    uint                NumIndexBuffers;
    uint16*             pNumIndices;        // 각 인덱스 버퍼에 인덱스가 몇 개인지
    uint16**            ppIndices;          // 각 인덱스 버퍼의 저장된 인덱스
};

struct UWMesh
{
    char            MagicNumber[8];

    uint            NumMaterials;
    UWMaterial*     pMaterials;

    uint            NumMeshBlocks;
    UWMeshBlock*    pMeshBlocks;
};

enum RENDER_MODE
{
    RENDER_MODE_SOLID,
    RENDER_MODE_WIREFRAME
};

enum VERTEX_BUFFER_FLAG
{
    VERTEX_BUFFER_FLAG_POS =           (1 << 0),
    VERTEX_BUFFER_FLAG_NORMAL =        (1 << 1),
    VERTEX_BUFFER_FLAG_TEXCOORD =      (1 << 2),
    VERTEX_BUFFER_FLAG_COLOR =         (1 << 3),

    VERTEX_BUFFER_FLAG_DEFAULT=        (VERTEX_BUFFER_FLAG_POS | VERTEX_BUFFER_FLAG_NORMAL),
};