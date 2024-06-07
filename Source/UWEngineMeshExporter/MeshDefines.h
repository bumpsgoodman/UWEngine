/*
* UWMesh 정의
*
* 작성자: bumpsgoodman
* 작성일: 2023.01.31
* 
* 메모리 레이아웃
* -----------------------
* UWMesh 시그니처 =>                  8바이트 => "UWMesh"
* 
* 머티리얼 개수 =>                    4바이트
*   머티리얼
*   텍스처 개수 =>                    4바이트
*     텍스처 경로 =>                  경로당 520바이트 => wchar_t[260]
* 
* 오브젝트
* 오브젝트 개수 =>                    4바이트
*   인클루드 플래그                   4바이트
*   정점 개수 =>                      4바이트
*   머티리얼 ID(선택) =>              4바이트
*   인덱스 버퍼 개수 =>               4바이트
*   
*   정점
*     위치 =>                         위치당 12바이트 => float[3]
*     법선 =>                         법선당 12바이트 => float[3]
* 
*   UV 좌표(선택) =>                  좌표당 8바이트 => float[2]
*   
*   인덱스 버퍼
*     인덱스 개수                     2바이트
*     인덱스                          인덱스당 2바이트
*/

#pragma once

#define UWMESH_MAX_PATH (260)

enum UWMESH_INCLUDE_FLAG
{
    UWMESH_INCLUDE_FLAG_NONE      = 0,
    UWMESH_INCLUDE_FLAG_TEXTURE   = (1 << 0),
    UWMESH_INCLUDE_FLAG_COLOR     = (1 << 1),
    UWMESH_INCLUDE_FLAG_SKIN      = (1 << 2)
};