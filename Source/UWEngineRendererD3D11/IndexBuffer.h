/*
* D3D11 인덱스 버퍼
*
* 작성자: bumpsgoodman
* 작성일: 2023.05.14
*/

#pragma once

interface IRenderer;

class IndexBuffer final
{
public:
    IndexBuffer() = default;
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;
    ~IndexBuffer();

    bool            __stdcall   Initialize(IRenderer* pRenderer, const uint numIndices);
    void            __stdcall   Release();

    uint            __stdcall   GetStartIndex() const;
    void            __stdcall   SetStartIndex(const uint startIndex);

    bool            __stdcall   SetIndex(const void* pIndices, const uint numIndices);

    uint            __stdcall   GetNumIndices() const;
    ID3D11Buffer*   __stdcall   GetBuffer() const;

private:
    IRenderer*          m_pRenderer = nullptr;

    uint                m_numIndices = 0;
    ID3D11Buffer*       m_pIndexBuffer = nullptr;

    uint                m_startIndex = 0;
};