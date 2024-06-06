/*
* UWEngine FileSystem 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.17
*/

#pragma once

#include "UWEngineCommon/Defines.h"
#include "UWEngineGeneric/IFixedArray.h"

interface IModelObject
{
     virtual MODEL_OBJECT_TYPE  __stdcall   GetObjectType() const = 0;
};

interface ITexture : public IRefObject
{
    virtual uint            __stdcall   GetWidth() const = 0;
    virtual void            __stdcall   SetWidth(const uint width) = 0;

    virtual uint            __stdcall   GetHeight() const = 0;
    virtual void            __stdcall   SetHeight(const uint height) = 0;

    virtual const void*     __stdcall   GetTexture() const = 0;
    virtual void            __stdcall   SetTexture(void* pTexture);
};

interface IMaterial : public IRefObject
{
    virtual uint                __stdcall   GetNumTextures() const = 0;

    virtual const ITexture*     __stdcall   GetTexture(const uint textureID) const = 0;
    virtual bool                __stdcall   AddTexture(ITexture* pTexture) = 0;
};

interface IUWMesh : public IRefObject
{
    virtual uint            __stdcall   GetNumMaterials() const = 0;
    virtual IMaterial*      __stdcall   GetMaterial(const uint materialID) const = 0;
    virtual bool            __stdcall   AddMaterial(IMaterial* pMaterial) = 0;

    virtual uint            __stdcall   GetNumModels() const = 0;
    virtual IModelObject*   __stdcall   GetRootObject(const uint objectID) = 0;
};

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