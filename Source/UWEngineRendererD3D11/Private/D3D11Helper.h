/*
* D3D11 헬퍼 함수
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.01
*/

#pragma once

HRESULT __stdcall CompileShaderFromFile(const wchar_t* pFileName, const char* pEntrypoint, const char* pShaderModel, ID3DBlob** ppShaderBlob);