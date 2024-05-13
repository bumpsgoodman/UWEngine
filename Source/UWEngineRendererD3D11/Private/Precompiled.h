/*
* RendererD3D11 precompiled 헤더
*
* 작성자: bumpsgoodman
* 작성일: 2023.01.28
*/

#pragma once

// D3D11 library
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

// UWEngine library
#if defined(_WIN64)
#   if defined(_DEBUG)
#       pragma comment(lib, "../../../Output/LIB/UWEngineGeneric_x64d.lib")
#       pragma comment(lib, "../../../Output/LIB/UWEngineMath_x64d.lib")
#   else
#       pragma comment(lib, "../../../Output/LIB/UWEngineGeneric_x64.lib")
#       pragma comment(lib, "../../../Output/LIB/UWEngineMath_x64.lib")
#   endif
#elif defined(_WIN32)
#   if defined(_DEBUG)
#       pragma comment(lib, "../../../Output/LIB/UWEngineGeneric_x86d.lib")
#       pragma comment(lib, "../../../Output/LIB/UWEngineMath_x86d.lib")
#   else
#       pragma comment(lib, "../../../Output/LIB/UWEngineGeneric_x86.lib")
#       pragma comment(lib, "../../../Output/LIB/UWEngineMath_x86.lib")
#   endif
#endif

// Win32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

// CRT
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

// intrinsic
#include <intrin.h>

// D3D
#include <d3d11.h>
#include <d3dcompiler.h>

// UWEngine
#include "UWEngineCommon/Defines.h"
#include "UWEngineCommon/Util/HashFunction.h"
#include "UWEngineCommon/Util/Timer.h"

#include "UWEngineMath/UWMath.inl"

#include "UWEngineGeneric/IFixedArray.h"

#include "D3D11Helper.h"