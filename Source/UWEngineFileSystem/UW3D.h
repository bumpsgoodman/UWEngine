/*
* UW3D 헤더
*
* 작성자: bumpsgoodman
* 작성일: 2023.05.18
*/

#pragma once

#include "UWEngineCommon/Defines.h"

#define UW3D_VERTEX_SIZE    24
#define UW3D_TEXCOORD_SIZE  8

struct UW3D_DESC
{
    uint NumMaterials;
    uint* pNumTexturesOrNull;
    wchar_t*** pppTexturesPathOrNull;

    uint NumObjects;

    uint* pIncludeFlags;
    uint* pNumVertices;
    uint* pMaterialIDsOrNull;
    uint* pNumIndexBuffers;

    void** ppVertices;
    void** ppTexCoordsOrNull;

    uint16** ppNumIndices;
    uint16*** pppIndexBuffers;
};