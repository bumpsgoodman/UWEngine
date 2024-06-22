/*
* UWEngine 공용 구조체 정의
*
* 작성자: bumpsgoodman
* 작성일: 2024.06.10
*/

#pragma once

#include "Defines.h"
#include "UWEngineMath/UWMath.inl"

struct MATERIAL_DESC
{
    uint        NumTextures;
    wchar_t**   ppTextureFilenames;
};

struct BoneWeight
{
    uint    NumWeights;
    uint    Indices[NUM_MAX_BONES_PER_VERTEX];
    float   Weights[NUM_MAX_BONES_PER_VERTEX];
};

struct MESH_DESC
{
    uint                IncludeFlag;

    uint                MaterialID;         //                                  * UWMESH_INCLUDE_FLAG_TEXTURE

    uint                NumVertices;
    void*               pVertices;          // [Position(xyz), Normal(xyz)]
    void*               pTexCoords;         // [TexCoord(xy)],                  * UWMESH_INCLUDE_FLAG_TEXTURE
    BoneWeight*         pBoneWeights;       //                                  * UWMESH_INCLUDE_FLAG_SKINNED

    uint                NumIndexBuffers;
    uint16*             pNumIndices;        // 각 인덱스 버퍼에 인덱스가 몇 개인지
    uint16**            ppIndices;          // 각 인덱스 버퍼의 저장된 인덱스
};

struct UWMesh
{
    uint            NumMaterials;
    MATERIAL_DESC*  pMaterials;

    uint            NumMeshes;
    MESH_DESC*      pMeshes;
};

struct ANIMATION_DESC
{
    uint KeyFrame;

    bool bHasPosition;
    bool bHasRotation;
    bool bHasScale;

    uint NumPositionKeys;
    uint NumRotationKeys;
    uint NumScaleKeys;

    struct Position
    {
        float X;
        float Y;
        float Z;
    } *pPositions;

    struct Rotation
    {
        float X;
        float Y;
        float Z;
        float W;
    } *pRotations;

    struct Scale
    {
        float X;
        float Y;
        float Z;
    } *pScales;
};

struct BONE_DESC
{
    Matrix44        TransformMatrix;
    ANIMATION_DESC  Animation;
};

struct UWBone
{
    uint        NumBones;
    BONE_DESC*  pBones;
};