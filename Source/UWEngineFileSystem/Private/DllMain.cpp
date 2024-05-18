/*
* UWEngineFileSystem DllMain
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.17
*/

#include "Precompiled.h"
#include "FileSystem.h"

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    default:
        break;
    }

    return TRUE;
}

bool __stdcall CreateDllInstance(void** ppOutInstance)
{
    ASSERT(ppOutInstance != nullptr, "ppOutInstance == nullptr");

    IFileSystem* pFileSystem = new FileSystem;
    pFileSystem->AddRef();

    *ppOutInstance = pFileSystem;

    return true;
}