/*
* D3D11 버텍스 버퍼
*
* 작성자: bumpsgoodman
* 작성일: 2023.05.13
*/

#pragma once

interface IRenderer;

class VertexBuffer final
{
public:
    VertexBuffer() = default;
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    ~VertexBuffer();

    bool            __stdcall   Initialize(IRenderer* pRenderer, const uint flag, const vsize vertexSize, const vsize numVertices);
    void            __stdcall   Release();

    vsize           __stdcall   GetStartIndex() const;
    void            __stdcall   SetStartIndex(const vsize startIndex);

    bool            __stdcall   SetVertex(const void* pVertices, const vsize num, const vsize vertexSize, const vsize offset, const uint flag);
    bool            __stdcall   SetVertex1(const void* pVertex, const vsize num, const vsize vertexSize, const vsize offset, const uint flag);

    ID3D11Buffer*   __stdcall   GetBuffer() const;

private:
    IRenderer*          m_pRenderer = nullptr;

    uint                m_flag = 0;
    vsize               m_numVertices = 0;
    vsize               m_vertexSize = 0;
    ID3D11Buffer*       m_pVertexBuffer = nullptr;

    vsize               m_startIndex = 0;
};