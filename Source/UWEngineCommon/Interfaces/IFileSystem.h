/*
* UWEngine FileSystem 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.17
*/

#pragma once

#include "UWEngineCommon/Defines.h"
#include "UWEngineCommon/Structures.h"

interface IFileSystem : public IRefObject
{
    virtual bool    __stdcall   Initialize() = 0;
    
    // UWMesh
    virtual bool    __stdcall   LoadUWMesh(const wchar_t* pFilename, UWMesh* pOutUWMesh) = 0;
    virtual void    __stdcall   UnloadUWMesh(const wchar_t* pFilename) = 0;

    // UWBone
    virtual bool    __stdcall   LoadUWBone(const wchar_t* pFilename, UWBone* pOutUWBone) = 0;
    virtual void    __stdcall   UnloadUWBone(const wchar_t* pFilename) = 0;
};