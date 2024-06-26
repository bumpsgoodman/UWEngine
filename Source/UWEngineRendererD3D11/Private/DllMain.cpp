﻿/*
* RendererD3D11 DllMain
*
* 작성자: bumpsgoodman
* 작성일: 2024.01.28
*/

#include "Precompiled.h"
#include "RendererD3D11.h"

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

    RendererD3D11* pRenderer = new RendererD3D11;
    pRenderer->AddRef();

    *ppOutInstance = pRenderer;

    return true;
}