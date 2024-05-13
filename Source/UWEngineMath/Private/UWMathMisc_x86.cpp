/*
* Math misc x86 어셈
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.22
*/

#include "Precompiled.h"
#include "UWMathMisc.inl"

GLOBAL_FUNC __declspec(naked) int __stdcall Log2Int64(const uint64 num)
{
    // [esp + 4] = num의 하위 32비트
    // [esp + 8] = num의 상위 32비트

    __asm
    {
        ; 상위 32비트 불러오기
        mov eax, dword ptr[esp + 8]

        ; 상위 32비트가 0인지 판단
        test eax, eax
        je zero

        ; 하위 32비트에 대해서 bsr 연산 후, 32 더해줌
        bsr eax, eax
        add eax, 32
        ret 8

    zero:
        bsr eax, dword ptr[esp + 4]
        ret 8
    }
}