/*
* RendererD3D11_2D precompiled 헤더
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.05
*/

#pragma once

// D3D11 library
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// UWEngine library
#if defined(_WIN64)
    #if defined(_DEBUG)
        #pragma comment(lib, "../../../Output/LIB/UWEngineGeneric_x64d.lib")
        #pragma comment(lib, "../../../Output/LIB/UWEngineMath_x64d.lib")
    #else
        #pragma comment(lib, "../../../Output/LIB/UWEngineGeneric_x64.lib")
        #pragma comment(lib, "../../../Output/LIB/UWEngineMath_x64.lib")
    #endif
#elif defined(_WIN32)
    #if defined(_DEBUG)
        #pragma comment(lib, "../../../Output/LIB/UWEngineGeneric_x86d.lib")
        #pragma comment(lib, "../../../Output/LIB/UWEngineMath_x86d.lib")
    #else
        #pragma comment(lib, "../../../Output/LIB/UWEngineGeneric_x86.lib")
        #pragma comment(lib, "../../../Output/LIB/UWEngineMath_x86.lib")
    #endif
#endif

// Win32
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

// D3D11
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// CRT

// UWEngine
#include "UWEngineCommon/Defines.h"
#include "UWEngineCommon/Util/HashFunction.h"
#include "UWEngineCommon/Util/Timer.h"

#include "UWEngineGeneric/IDynamicArray.h"
#include "UWEngineGeneric/IHashMap.h"
#include "UWEngineGeneric/IChunkedMemoryPool.h"

#include "UWEngineMath/UWMath.h"