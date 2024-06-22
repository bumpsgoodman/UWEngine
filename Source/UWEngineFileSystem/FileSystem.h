/*
* UWEngine FileSystem
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.17
*/

#pragma once

#include "UWEngineCommon/Interfaces/IFileSystem.h"

class FileSystem final : public IFileSystem
{
public:
    FileSystem() = default;
    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;
    ~FileSystem() = default;

    virtual uint    __stdcall   AddRef() override;
    virtual uint    __stdcall   Release() override;
    virtual uint    __stdcall   GetRefCount() const override;

    virtual bool    __stdcall   Initialize() override;

    // UWMesh
    virtual bool    __stdcall   LoadUWMesh(const wchar_t* pFilename, UWMesh* pOutUWMesh) override;
    virtual void    __stdcall   UnloadUWMesh(const wchar_t* pFilename) override;

    // UWBone
    virtual bool    __stdcall   LoadUWBone(const wchar_t* pFilename, UWBone* pOutUWBone) override;
    virtual void    __stdcall   UnloadUWBone(const wchar_t* pFilename) override;
private:
    uint            m_refCount = 0;

    HashMap         m_UWMeshMap;     // { path : MESH_DESC }
    HashMap         m_UWBoneMap;     // { path : BONE_DESC }
};