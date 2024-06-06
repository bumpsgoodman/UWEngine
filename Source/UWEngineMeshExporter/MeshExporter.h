/*
* UWEngineMesh 3DS Max Exporter
*
* 작성자: bumpsgoodman
* 작성일: 2023.01.31
*/

#pragma once

#include <vector>

#include "max.h"
#include "resource.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"
#include "iskin.h"
#include "modstack.h"

#include "istdplug.h"

#define CFGFILENAME		_T("UWMESHEXPORTER.CFG")	// Configuration file

using namespace std;

extern ClassDesc* GetUWMeshExpDesc();
extern TCHAR* GetString(int id);
extern HINSTANCE g_hInstance;

class UWMeshExporter final : public SceneExport
{
public:
    UWMeshExporter() = default;
    UWMeshExporter(const UWMeshExporter&) = delete;
    UWMeshExporter& operator=(const UWMeshExporter&) = delete;
    UWMeshExporter(UWMeshExporter&&) = default;
    UWMeshExporter& operator=(UWMeshExporter&&) = default;
    ~UWMeshExporter();

    // SceneExport 메서드
    int ExtCount();                     // 지원되는 확장자 수
    const TCHAR* Ext(int n);            // 확장자
    const TCHAR* LongDesc();            // 긴 설명
    const TCHAR* ShortDesc();           // 짧은 설명
    const TCHAR* AuthorName();          // 작성자
    const TCHAR* CopyrightMessage();    // 저작권 메시지
    const TCHAR* OtherMessage1();       // Other message #1
    const TCHAR* OtherMessage2();       // Other message #2
    unsigned int Version();             // 버전 번호
    void ShowAbout(HWND hWnd);          // DLL About 박스
    int	DoExport(const TCHAR* pName, ExpInterface* pExpInterface, Interface* pInterface, BOOL bSuppressPrompts = FALSE, DWORD options = 0); // Export 실행 함수
    BOOL SupportsOptions(int ext, DWORD options);

    inline bool GetIncludeTextureCoords() const;

    inline void SetIncludeTextureCoords(const bool bActive);

private:
    void preProcessRecursion(INode* pNode);
    void exportNodeRecursion(INode* pNode);
    void exportGeomObject(INode* pNode);

private:
    DWORD m_numTotalNode = 0;
    DWORD m_numCurNode = 0;
    Interface* m_pInterface = nullptr;

    bool m_bExportSelected = false;

    bool m_bIncludeTextureCoord = false;

    FILE* m_pFile = nullptr;

    vector<Mtl*> m_materials;
    vector<vector<const wchar_t*>> m_texturePathList;    // <vector<texturePath>>
};

inline bool UWMeshExporter::GetIncludeTextureCoords() const
{
    return m_bIncludeTextureCoord;
}

inline void UWMeshExporter::SetIncludeTextureCoords(const bool bActive)
{
    m_bIncludeTextureCoord = bActive;
}