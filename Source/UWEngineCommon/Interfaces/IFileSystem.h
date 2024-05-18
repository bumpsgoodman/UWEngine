/*
* UWEngine FileSystem 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.17
*/

#pragma once

#include "UWEngineCommon/Defines.h"

interface IFileSystem : public IRefObject
{
    virtual bool            __stdcall   Initialize() = 0;
    
    // UW3D
    virtual UW3D_HANDLE     __stdcall   UW3DLoadOrNull(const wchar_t* pFilePath) = 0;
    virtual void            __stdcall   UW3DUnload(const wchar_t* pFilePath) = 0;

    // UW3D Material
    virtual uint            __stdcall   UW3DGetNumMaterials(const UW3D_HANDLE handle) = 0;
    virtual uint            __stdcall   UW3DGetNumTextures(const UW3D_HANDLE handle, const uint materialID) = 0;
    virtual const wchar_t** __stdcall   UW3DGetTexturesPath(const UW3D_HANDLE handle, const uint materialID) = 0;

    // UW3D Object
    virtual uint            __stdcall   UW3DGetNumObjects(const UW3D_HANDLE handle) = 0;
    virtual uint            __stdcall   UW3DGetIncludeFlag(const UW3D_HANDLE handle, const uint objectID) = 0;
    virtual uint            __stdcall   UW3DGetNumVertices(const UW3D_HANDLE handle, const uint objectID) = 0;
    virtual uint            __stdcall   UW3DGetMaterialID(const UW3D_HANDLE handle, const uint objectID) = 0;
    virtual uint            __stdcall   UW3DGetNumIndexBuffers(const UW3D_HANDLE handle, const uint objectID) = 0;

    virtual const void*     __stdcall   UW3DGetVertices(const UW3D_HANDLE handle, const uint objectID) = 0;
    virtual const void*     __stdcall   UW3DGetTexCoords(const UW3D_HANDLE handle, const uint objectID) = 0;
    virtual const uint16**  __stdcall   UW3DGetIndexBuffers(const UW3D_HANDLE handle, const uint objectID) = 0;
    virtual const uint16*   __stdcall   UW3DGetNumIndices(const UW3D_HANDLE handle, const uint objectID) = 0;

    virtual uint            __stdcall   UW3DGetVertexSize() = 0;
    virtual uint            __stdcall   UW3DGetTexCoordSize() = 0;
};