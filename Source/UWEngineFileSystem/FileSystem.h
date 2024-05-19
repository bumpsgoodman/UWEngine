/*
* UWEngine FileSystem
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.17
*/

#pragma once

#include "UWEngineCommon/Interfaces/IFileSystem.h"

interface IHashMap;

class FileSystem final : public IFileSystem
{
public:
    FileSystem() = default;
    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;
    ~FileSystem() = default;

    virtual vsize   __stdcall   AddRef() override;
    virtual vsize   __stdcall   Release() override;
    virtual vsize   __stdcall   GetRefCount() const override;

    virtual bool    __stdcall   Initialize() override;

    // UW3D
    virtual UW3D_HANDLE     __stdcall   UW3DLoadOrNull(const wchar_t* pFilePath) override;
    virtual void            __stdcall   UW3DUnload(const wchar_t* pFilePath) override;

    // UW3D Material
    virtual uint            __stdcall   UW3DGetNumMaterials(const UW3D_HANDLE handle) override;
    virtual uint            __stdcall   UW3DGetNumTextures(const UW3D_HANDLE handle, const uint materialID) override;
    virtual const wchar_t** __stdcall   UW3DGetTexturesPath(const UW3D_HANDLE handle, const uint materialID) override;

    // UW3D Object
    virtual uint            __stdcall   UW3DGetNumObjects(const UW3D_HANDLE handle) override;
    virtual uint            __stdcall   UW3DGetIncludeFlag(const UW3D_HANDLE handle, const uint objectID) override;
    virtual uint            __stdcall   UW3DGetNumVertices(const UW3D_HANDLE handle, const uint objectID) override;
    virtual uint            __stdcall   UW3DGetMaterialID(const UW3D_HANDLE handle, const uint objectID) override;
    virtual uint            __stdcall   UW3DGetNumIndexBuffers(const UW3D_HANDLE handle, const uint objectID) override;

    virtual const void*     __stdcall   UW3DGetVertices(const UW3D_HANDLE handle, const uint objectID) override;
    virtual const void*     __stdcall   UW3DGetTexCoords(const UW3D_HANDLE handle, const uint objectID) override;
    virtual const uint16**  __stdcall   UW3DGetIndexBuffers(const UW3D_HANDLE handle, const uint objectID) override;
    virtual const uint16*   __stdcall   UW3DGetNumIndices(const UW3D_HANDLE handle, const uint objectID) override;

    virtual uint            __stdcall   UW3DGetVertexSize() override;
    virtual uint            __stdcall   UW3DGetTexCoordSize() override;

private:
    vsize           m_refCount = 0;

    IHashMap*       m_pUW3DMap = nullptr;     // { path : UW3D_HANDLE }
};