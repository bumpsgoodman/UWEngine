/*
* UWEngine 자료형
*
* 작성자: bumpsgoodman
* 작성일: 2024.03.09
*/

#pragma once

// fixed int
typedef unsigned int                uint;
typedef signed char                 int8;
typedef short                       int16;
typedef int                         int32;
typedef long long                   int64;
typedef unsigned char               uint8;
typedef unsigned short              uint16;
typedef unsigned int                uint32;
typedef unsigned long long          uint64;

// variadic size
#if defined (_WIN64)
    typedef uint64 vsize;
#else
    typedef uint32 vsize;
#endif

// stdint.h
#define UW_INT8_MIN        (-127 - 1)
#define UW_INT16_MIN       (-32767 - 1)
#define UW_INT32_MIN       (-2147483647 - 1)
#define UW_INT64_MIN       (-9223372036854775807ll - 1)
#define UW_INT8_MAX        127
#define UW_INT16_MAX       32767
#define UW_INT32_MAX       2147483647
#define UW_INT64_MAX       9223372036854775807ll
#define UW_UINT8_MAX       0xffu
#define UW_UINT16_MAX      0xffffu
#define UW_UINT32_MAX      0xffffffffu
#define UW_UINT64_MAX      0xffffffffffffffffull

#if defined (_WIN64)
    #define UW_VSIZE_MAX   0xffffffffffffffffu
#else
    #define UW_VSIZE_MAX   0xffffffffu
#endif

#define UW_PTR_SIZE sizeof(void*)

// 타입 검증
static_assert(sizeof(int8) == 1, "Mismatch size");
static_assert(sizeof(int16) == 2, "Mismatch size");
static_assert(sizeof(int32) == 4, "Mismatch size");
static_assert(sizeof(int64) == 8, "Mismatch size");
static_assert(sizeof(uint8) == 1, "Mismatch size");
static_assert(sizeof(uint16) == 2, "Mismatch size");
static_assert(sizeof(uint32) == 4, "Mismatch size");
static_assert(sizeof(uint64) == 8, "Mismatch size");

#if defined (_WIN64)
    static_assert(sizeof(vsize) == 8, "Mismatch size");
#else
    static_assert(sizeof(vsize) == 4, "Mismatch size");
#endif