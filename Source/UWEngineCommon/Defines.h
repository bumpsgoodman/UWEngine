/*
* UWEngine 공용 정의
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#pragma once

#include "Types.h"

// switch case 문 break 생략 시 삽입
#define FALLTHROUGH

// 문자열로 치환
#define TO_STR(s) #s

// 호출 규약
#define UWAPI                   __stdcall
#define UWAPI_VECTOR            __vectorcall
#define UWAPI_VAR               __cdecl

#define UWMETHOD(type)          type UWAPI
#define UWMETHOD_VECTOR(type)   type UWAPI_VECTOR
#define UWMETHOD_VAR(type)      type UWAPI_VAR

// 순수 가상 함수
#define PURE = 0

// ASSERT
#if defined(NDEBUG)
#   define ASSERT(cond, msg) ((void)0)
#else
#   define ASSERT(cond, msg) { if (!(cond)) { __debugbreak(); } }
#endif // ASSERT

// SAFE DELETE
#define SAFE_DELETE(p)              { delete (p); (p) = nullptr; }
#define SAFE_DELETE_ARRAY(p)        { delete[] (p); (p) = nullptr; }
#define SAFE_FREE(p)                { free((p)); (p) = nullptr; }
#define SAFE_RELEASE(p)             { if ((p)) { (p)->Release(); (p) = nullptr; } }
#define SAFE_VIRTUAL_FREE(p)        { VirtualFree((p), 0, MEM_RELEASE); (p) = nullptr; }
#define SAFE_FREE_LIBRARY(handle)   { if ((handle)) { FreeLibrary(handle); (handle) = nullptr; } }

// dll export
#ifdef UW_ENGINE_DLL
#   define GLOBAL_FUNC __declspec(dllexport)
#else
#   define GLOBAL_FUNC __declspec(dllimport)
#endif // UW_ENGINE_DLL

// Alignment
#define ALIGN8 _declspec(align(8))
#define ALIGN16 _declspec(align(16))
#define ALIGN32 _declspec(align(32))

// 인터페이스
#ifndef interface
#   define interface struct
#endif // interface

typedef void(UWAPI* CreateDllInstanceFunc)(void** ppOutInstance);

// UW3D
enum UW3D_INCLUDE_FLAG
{
    UW3D_INCLUDE_FLAG_NONE = 0,
    UW3D_INCLUDE_FLAG_TEXTURE = (1 << 0),
    UW3D_INCLUDE_FLAG_COLOR = (1 << 1)
};
#define UW3D_HAS_INCLUDE_FLAG(flag, other) (((flag) & (other)) == (other))