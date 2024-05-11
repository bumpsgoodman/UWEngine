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

// ASSERT
#if defined(NDEBUG)
#   define ASSERT(cond, msg) ((void)0)
#else
#   define ASSERT(cond, msg) { if (!(cond)) { __debugbreak(); } }
#endif // ASSERT

// Debug/Release 둘 모두 크래시
#define CRASH __debugbreak

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

interface IRefObject
{
    virtual vsize   __stdcall   AddRef() = 0;
    virtual vsize   __stdcall   Release() = 0;
    virtual vsize   __stdcall   GetRefCount() const = 0;
};

typedef void(__stdcall* CreateDllInstanceFunc)(void** ppOutInstance);

// UW3D
enum UW3D_INCLUDE_FLAG
{
    UW3D_INCLUDE_FLAG_NONE = 0,
    UW3D_INCLUDE_FLAG_TEXTURE = (1 << 0),
    UW3D_INCLUDE_FLAG_COLOR = (1 << 1)
};
#define UW3D_HAS_INCLUDE_FLAG(flag, other) (((flag) & (other)) == (other))

enum RENDER_MODE
{
    RENDER_MODE_SOLID,
    RENDER_MODE_WIREFRAME
};