/*
* FileSystem precompiled 헤더
*
* 작성자: bumpsgoodman
* 작성일: 2023.05.17
*/

#pragma once

#define _CRT_SECURE_NO_WARNINGS

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
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

// intrinsic
#include <intrin.h>

// UWEngine
#include "UWEngineCommon/Defines.h"

#include "UWEngineMath/UWMath.inl"

#include "UWEngineGeneric/IFixedArray.h"