/*
* UWEngineMesh 3DS Max Exporter
*
* 작성자: bumpsgoodman
* 작성일: 2023.01.31
*/

#define _CRT_SECURE_NO_WARNINGS

#include <unordered_map>
#include <vector>

#include "MeshExporter.h"
#include "MeshDefines.h"

using namespace std;

// Class ID. These must be unique and randomly generated!!
// If you use this as a sample project, this is the first thing
// you should change!
#define UWMESH_EXP_CLASS_ID	Class_ID(0x4f3d6ef1, 0x39dc4eb0)

HINSTANCE g_hInstance;

static Point3 GetVertexNormal(Mesh* pMesh, const DWORD faceIndex, RVertex* pRVertex);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        MaxSDK::Util::UseLanguagePackLocale();
        g_hInstance = hinstDLL;
        DisableThreadLibraryCalls(g_hInstance);
    }

    return (TRUE);
}

__declspec(dllexport) const TCHAR* LibDescription()
{
    return GetString(IDS_LIBDESCRIPTION);
}

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec(dllexport) int LibNumberClasses()
{
    return 1;
}

__declspec(dllexport) ClassDesc* LibClassDesc(int i)
{
    switch (i)
    {
    case 0: return GetUWMeshExpDesc();
    default: return 0;
    }
}

__declspec(dllexport) ULONG LibVersion()
{
    return VERSION_3DSMAX;
}

// Let the plug-in register itself for deferred loading
__declspec(dllexport) ULONG CanAutoDefer()
{
    return 1;
}

class UWMeshExpClassDesc :public ClassDesc {
public:
    int	IsPublic() { return 1; }
    void* Create(BOOL loading = FALSE) { return new UWMeshExporter; }
    const TCHAR* ClassName() { return GetString(IDS_UWMESH_EXP); }
    const TCHAR* NonLocalizedClassName() { return _T("UWMeshExporter"); }
    SClass_ID SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
    Class_ID ClassID() { return UWMESH_EXP_CLASS_ID; }
    const TCHAR* Category() { return GetString(IDS_CATEGORY); }
};

static UWMeshExpClassDesc s_uwMeshExpDesc;

ClassDesc* GetUWMeshExpDesc()
{
    return &s_uwMeshExpDesc;
}

UWMeshExporter::~UWMeshExporter()
{
}

int UWMeshExporter::ExtCount()
{
    return 1;
}

const TCHAR* UWMeshExporter::Ext(int n)
{
    switch (n)
    {
    case 0:
        return _T("UWMesh");
    }

    return _T("");
}

const TCHAR* UWMeshExporter::LongDesc()
{
    return GetString(IDS_LONGDESC);
}

const TCHAR* UWMeshExporter::ShortDesc()
{
    return GetString(IDS_SHORTDESC);
}

const TCHAR* UWMeshExporter::AuthorName()
{
    return _T("bumpsgoodman");
}

const TCHAR* UWMeshExporter::CopyrightMessage()
{
    return GetString(IDS_COPYRIGHT);
}

const TCHAR* UWMeshExporter::OtherMessage1()
{
    return _T("");
}

const TCHAR* UWMeshExporter::OtherMessage2()
{
    return _T("");
}

unsigned int UWMeshExporter::Version()
{
    return 100;
}

static INT_PTR CALLBACK AboutBoxDlgProc(HWND hWnd, UINT msg,
                                        WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_INITDIALOG:
        CenterWindow(hWnd, GetParent(hWnd));
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            EndDialog(hWnd, 1);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void UWMeshExporter::ShowAbout(HWND hWnd)
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}

static INT_PTR CALLBACK ExportDlgProc(HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    Interval animRange;
    ISpinnerControl* spin;

    UWMeshExporter* exp = (UWMeshExporter*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    switch (msg) {
    case WM_INITDIALOG:
        exp = (UWMeshExporter*)lParam;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, lParam);
        CenterWindow(hWnd, GetParent(hWnd));
        //CheckDlgButton(hWnd, IDC_MESHDATA, exp->GetIncludeMesh());
        //CheckDlgButton(hWnd, IDC_ANIMKEYS, exp->GetIncludeAnim());
        //CheckDlgButton(hWnd, IDC_MATERIAL, exp->GetIncludeMtl());
        //CheckDlgButton(hWnd, IDC_MESHANIM, exp->GetIncludeMeshAnim());
        //CheckDlgButton(hWnd, IDC_CAMLIGHTANIM, exp->GetIncludeCamLightAnim());
        //CheckDlgButton(hWnd, IDC_IKJOINTS, exp->GetIncludeIKJoints());
        //CheckDlgButton(hWnd, IDC_NORMALS, exp->GetIncludeNormals());
        CheckDlgButton(hWnd, IDC_TEXCOORDS, exp->GetIncludeTextureCoords());
        //CheckDlgButton(hWnd, IDC_VERTEXCOLORS, exp->GetIncludeVertexColors());
        //CheckDlgButton(hWnd, IDC_OBJ_GEOM, exp->GetIncludeObjGeom());
        //CheckDlgButton(hWnd, IDC_OBJ_SHAPE, exp->GetIncludeObjShape());
        //CheckDlgButton(hWnd, IDC_OBJ_CAMERA, exp->GetIncludeObjCamera());
        //CheckDlgButton(hWnd, IDC_OBJ_LIGHT, exp->GetIncludeObjLight());
        //CheckDlgButton(hWnd, IDC_OBJ_HELPER, exp->GetIncludeObjHelper());

        //CheckRadioButton(hWnd, IDC_RADIO_USEKEYS, IDC_RADIO_SAMPLE,
        //    exp->GetAlwaysSample() ? IDC_RADIO_SAMPLE : IDC_RADIO_USEKEYS);

        // Setup the spinner controls for the controller key sample rate 
        //spin = GetISpinner(GetDlgItem(hWnd, IDC_CONT_STEP_SPIN));
        //spin->LinkToEdit(GetDlgItem(hWnd, IDC_CONT_STEP), EDITTYPE_INT);
        //spin->SetLimits(1, 100, TRUE);
        //spin->SetScale(1.0f);
        //spin->SetValue(exp->GetKeyFrameStep(), FALSE);
        //ReleaseISpinner(spin);

        // Setup the spinner controls for the mesh definition sample rate 
        //spin = GetISpinner(GetDlgItem(hWnd, IDC_MESH_STEP_SPIN));
        //spin->LinkToEdit(GetDlgItem(hWnd, IDC_MESH_STEP), EDITTYPE_INT);
        //spin->SetLimits(1, 100, TRUE);
        //spin->SetScale(1.0f);
        //spin->SetValue(exp->GetMeshFrameStep(), FALSE);
        //ReleaseISpinner(spin);

        // Setup the spinner controls for the floating point precision 
        //spin = GetISpinner(GetDlgItem(hWnd, IDC_PREC_SPIN));
        //spin->LinkToEdit(GetDlgItem(hWnd, IDC_PREC), EDITTYPE_INT);
        //spin->SetLimits(1, 10, TRUE);
        //spin->SetScale(1.0f);
        //spin->SetValue(exp->GetPrecision(), FALSE);
        //ReleaseISpinner(spin);

        // Setup the spinner control for the static frame#
        // We take the frame 0 as the default value
        //animRange = exp->GetInterface()->GetAnimRange();
        //spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN));
        //spin->LinkToEdit(GetDlgItem(hWnd, IDC_STATIC_FRAME), EDITTYPE_INT);
        //spin->SetLimits(animRange.Start() / GetTicksPerFrame(), animRange.End() / GetTicksPerFrame(), TRUE);
        //spin->SetScale(1.0f);
        //spin->SetValue(0, FALSE);
        //ReleaseISpinner(spin);

        // Enable / disable mesh options
        //EnableWindow(GetDlgItem(hWnd, IDC_NORMALS), true);
        EnableWindow(GetDlgItem(hWnd, IDC_TEXCOORDS), true);
        EnableWindow(GetDlgItem(hWnd, IDC_VERTEXCOLORS), true);
        break;

    case CC_SPINNER_CHANGE:
        spin = (ISpinnerControl*)lParam;
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_MESHDATA:
            // Enable / disable mesh options
            //EnableWindow(GetDlgItem(hWnd, IDC_NORMALS), IsDlgButtonChecked(hWnd,
            //             IDC_MESHDATA));
            EnableWindow(GetDlgItem(hWnd, IDC_TEXCOORDS), IsDlgButtonChecked(hWnd,
                         IDC_MESHDATA));
            EnableWindow(GetDlgItem(hWnd, IDC_VERTEXCOLORS), IsDlgButtonChecked(hWnd,
                         IDC_MESHDATA));
            break;
        case IDOK:
            //exp->SetIncludeMesh(IsDlgButtonChecked(hWnd, IDC_MESHDATA));
            //exp->SetIncludeAnim(IsDlgButtonChecked(hWnd, IDC_ANIMKEYS));
            //exp->SetIncludeMtl(IsDlgButtonChecked(hWnd, IDC_MATERIAL));
            //exp->SetIncludeMeshAnim(IsDlgButtonChecked(hWnd, IDC_MESHANIM));
            //exp->SetIncludeCamLightAnim(IsDlgButtonChecked(hWnd, IDC_CAMLIGHTANIM));
            //exp->SetIncludeIKJoints(IsDlgButtonChecked(hWnd, IDC_IKJOINTS));
            //exp->SetIncludeNormals(IsDlgButtonChecked(hWnd, IDC_NORMALS));
            exp->SetIncludeTextureCoords(IsDlgButtonChecked(hWnd, IDC_TEXCOORDS));
            //exp->SetIncludeVertexColors(IsDlgButtonChecked(hWnd, IDC_VERTEXCOLORS));
            //exp->SetIncludeObjGeom(IsDlgButtonChecked(hWnd, IDC_OBJ_GEOM));
            //exp->SetIncludeObjShape(IsDlgButtonChecked(hWnd, IDC_OBJ_SHAPE));
            //exp->SetIncludeObjCamera(IsDlgButtonChecked(hWnd, IDC_OBJ_CAMERA));
            //exp->SetIncludeObjLight(IsDlgButtonChecked(hWnd, IDC_OBJ_LIGHT));
            //exp->SetIncludeObjHelper(IsDlgButtonChecked(hWnd, IDC_OBJ_HELPER));
            //exp->SetAlwaysSample(IsDlgButtonChecked(hWnd, IDC_RADIO_SAMPLE));

            //spin = GetISpinner(GetDlgItem(hWnd, IDC_CONT_STEP_SPIN));
            //exp->SetKeyFrameStep(spin->GetIVal());
            //ReleaseISpinner(spin);

            //spin = GetISpinner(GetDlgItem(hWnd, IDC_MESH_STEP_SPIN));
            //exp->SetMeshFrameStep(spin->GetIVal());
            //ReleaseISpinner(spin);

            //spin = GetISpinner(GetDlgItem(hWnd, IDC_PREC_SPIN));
            //exp->SetPrecision(spin->GetIVal());
            //ReleaseISpinner(spin);

            //spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN));
            //exp->SetStaticFrame(spin->GetIVal() * GetTicksPerFrame());
            //ReleaseISpinner(spin);

            EndDialog(hWnd, 1);
            break;
        case IDCANCEL:
            EndDialog(hWnd, 0);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

int	UWMeshExporter::DoExport(const TCHAR* pName, ExpInterface* pExpInterface, Interface* pInterface, BOOL bSuppressPrompts, DWORD options)
{
    m_bExportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;

    m_pInterface = pInterface;

    if (!DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_UWMESH_EXPORT_DLG),
        pInterface->GetMAXHWnd(), ExportDlgProc, (LPARAM)this)) {
        return IMPEXP_CANCEL;
    }

    m_pFile = _wfopen(pName, L"wb");
    if (m_pFile == nullptr)
    {
        return IMPEXP_FAIL;
    }

    // 진행 표시줄 시작
    m_pInterface->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, nullptr, nullptr);

    m_numCurNode = 0;
    m_numTotalNode = 0;

    // 총 노드 수 계산
    int numChildren = m_pInterface->GetRootNode()->NumberOfChildren();
    for (int idx = 0; idx < numChildren; idx++)
    {
        if (pInterface->GetCancel())
        {
            break;
        }

        preProcessRecursion(m_pInterface->GetRootNode()->GetChildNode(idx));
    }

    // 시그니처 저장
    const char signature[8] = "UWMesh";
    fwrite(signature, sizeof(signature), 1, m_pFile);

    // 텍스쳐 파일 위치 저장
    if (!m_texturePathList.empty())
    {
        // 머티리얼 개수 저장
        const DWORD listSize = m_texturePathList.size();
        fwrite(&listSize, sizeof(DWORD), 1, m_pFile);

        for (DWORD i = 0; i < listSize; ++i)
        {
            // 머티리얼에 있는 텍스처 개수 저장
            const DWORD pathSize = m_texturePathList[i].size();
            fwrite(&pathSize, sizeof(DWORD), 1, m_pFile);

            for (DWORD j = 0; j < pathSize; ++j)
            {
                // 텍스처 경로 저장
                wchar_t texturePath[UWMESH_MAX_PATH];
                wcscpy(texturePath, m_texturePathList[i][j]);
                fwrite(texturePath, sizeof(texturePath), 1, m_pFile);
            }
        }
    }

    // 오브젝트 개수 저장
    fwrite(&m_numTotalNode, sizeof(DWORD), 1, m_pFile);

    for (int idx = 0; idx < numChildren; idx++)
    {
        if (m_pInterface->GetCancel())
        {
            break;
        }

        exportNodeRecursion(m_pInterface->GetRootNode()->GetChildNode(idx));
    }

    // 진행 표시줄 마침
    m_pInterface->ProgressEnd();

    fclose(m_pFile);

    return IMPEXP_SUCCESS;
}

BOOL UWMeshExporter::SupportsOptions(int ext, DWORD options)
{
    return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

TCHAR* GetString(int id)
{
    static TCHAR buf[256];

    if (g_hInstance)
    {
        return LoadString(g_hInstance, id, buf, _countof(buf)) ? buf : NULL;
    }

    return NULL;
}

void UWMeshExporter::preProcessRecursion(INode* pNode)
{
    if (m_bExportSelected && pNode->Selected() == FALSE)
    {
        return;
    }

    if (pNode->IsGroupHead())
    {
        return;
    }

    // 사용되는 텍스쳐 경로 추가
    Mtl* pMtl = pNode->GetMtl();
    if (GetIncludeTextureCoords() && pMtl != nullptr)
    {
        // 이미 추가된 머티리얼인지 검사
        bool bFoundMtl = false;
        for (DWORD i = 0; i < m_materials.size(); ++i)
        {
            if (m_materials[i] == pMtl)
            {
                bFoundMtl = true;
                break;
            }
        }

        // 새로운 머티리얼인 경우에만 추가
        if (!bFoundMtl)
        {
            m_materials.push_back(pMtl);

            vector<const wchar_t*> texturePath;

            Texmap* pMap = pMtl->GetSubTexmap(ID_DI);
            const wchar_t* pTexturePath = nullptr;
            if (pMap != nullptr)
            {
                pTexturePath = ((BitmapTex*)pMap)->GetMapName();
                texturePath.push_back(pTexturePath);
            }

            const int numSubMtls = pMtl->NumSubMtls();
            for (DWORD i = 0; i < numSubMtls; ++i)
            {
                Mtl* pSubMtl = pMtl->GetSubMtl(i);
                Texmap* pMap = pSubMtl->GetSubTexmap(ID_DI);
                const wchar_t* pTexturePath = nullptr;
                if (pMap != nullptr)
                {
                    pTexturePath = ((BitmapTex*)pMap)->GetMapName();
                    texturePath.push_back(pTexturePath);
                }
            }

            m_texturePathList.push_back(texturePath);
        }
    }

    ++m_numTotalNode;

    for (int i = 0; i < pNode->NumberOfChildren(); ++i)
    {
        preProcessRecursion(pNode->GetChildNode(i));
    }
}

void UWMeshExporter::exportNodeRecursion(INode* pNode)
{
    ++m_numCurNode;
    m_pInterface->ProgressUpdate((int)((float)m_numCurNode / m_numTotalNode * 100.0f));

    if (m_pInterface->GetCancel())
    {
        return;
    }

    if (!m_bExportSelected || pNode->Selected())
    {
        ObjectState os = pNode->EvalWorldState(0);
        if (os.obj != nullptr)
        {
            switch (os.obj->SuperClassID())
            {
            case GEOMOBJECT_CLASS_ID:
                exportGeomObject(pNode);
                break;
            default:
                break;
            }
        }
    }

    for (int i = 0; i < pNode->NumberOfChildren(); ++i)
    {
        exportNodeRecursion(pNode->GetChildNode(i));
    }
}

// 주의사항
// 1. 정점에 UV 매핑이 되어 있지 않으면 작동하지 않음
void UWMeshExporter::exportGeomObject(INode* pNode)
{
    ObjectState os = pNode->EvalWorldState(0);
    Object* pObject = os.obj;
    TriObject* pTri = nullptr;
    bool bDeleteTri = false;

    if (pObject->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
    {
        pTri = (TriObject*)pObject->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));

        // Note that the TriObject should only be deleted
        // if the pointer to it is not equal to the object
        // pointer that called ConvertToType()
        if (pObject != pTri)
        {
            bDeleteTri = true;
        }
    }

    Mesh* pMesh = &pTri->GetMesh();

    // 법선 불러오려면 꼭 호출 해야 됨
    pMesh->buildNormals();

    DWORD includeFlag = 0;
    DWORD numMaxIndexBuffers = 1;  // 기본적으로 1개
    DWORD materialID = 0;

    vector<Point3> vertices;
    vector<Point3> normals;
    vector<Point3> uvs;
    vector<vector<WORD>> indexBuffers;

    uvs.resize(pMesh->numTVerts);

    Mtl* pMtl = pNode->GetMtl();
    if (pMtl != nullptr && GetIncludeTextureCoords())
    {
        includeFlag |= UWMESH_INCLUDE_FLAG_TEXTURE;

        // materialID 찾기
        for (DWORD i = 0; i < m_materials.size(); ++i)
        {
            if (pMtl == m_materials[i])
            {
                materialID = i;
                break;
            }
        }

        // 인덱스 버퍼 최대 개수 구하기
        // 인덱스 버퍼 최대 개수는 머티리얼이 가지고 있는 텍스쳐 개수임
        numMaxIndexBuffers = m_texturePathList[materialID].size();

        // 인덱스 버퍼 개수만큼 늘려주기
        for (DWORD i = 0; i < numMaxIndexBuffers; ++i)
        {
            indexBuffers.push_back(vector<WORD>());
        }

        // 추가되어야 할 부분은 아직 추가되지 않았으므로 false로 초기화
        vector<bool> bUseVertex;
        bUseVertex.resize(pMesh->numTVerts, false);

        // 기존 버텍스 복사
        for (DWORD i = 0; i < pMesh->numVerts; ++i)
        {
            vertices.push_back(pMesh->verts[i]);
        }

        normals.resize(pMesh->numTVerts);

        // 버텍스를 UV 좌표 개수만큼 늘리기
        unordered_map<DWORD, DWORD> indexMap;
        for (DWORD i = 0; i < pMesh->numFaces; ++i)
        {
            const DWORD textureID = pMesh->faces[i].getMatID() % numMaxIndexBuffers;
            vector<WORD>& indexBuffer = indexBuffers[textureID];

            for (DWORD j = 0; j < 3; ++j)
            {
                const DWORD vertexIndex = pMesh->faces[i].v[j];
                const DWORD uvIndex = pMesh->tvFace[i].t[j];

                // 추가되지 않은 버텍스만 추가
                if (indexMap.count(uvIndex) == 0)
                {
                    if (bUseVertex[vertexIndex])
                    {
                        indexMap[uvIndex] = vertices.size();
                        bUseVertex[indexMap[uvIndex]] = true;

                        vertices.push_back(pMesh->verts[vertexIndex]);
                    }
                    else
                    {
                        indexMap[uvIndex] = vertexIndex;
                        bUseVertex[vertexIndex] = true;
                    }

                    const DWORD index = indexMap[uvIndex];

                    // 법선 추가
                    RVertex* pRVertex = pMesh->getRVertPtr(vertexIndex);
                    const Point3 vertexNormal = GetVertexNormal(pMesh, i, pRVertex);
                    normals[index] = vertexNormal;

                    // UV 추가
                    uvs[index] = pMesh->tVerts[uvIndex];
                }

                indexBuffer.push_back(indexMap[uvIndex]);
            }
        }
    }
    else
    {
        // 인덱스 버퍼는 기본적으로 1개
        indexBuffers.push_back(vector<WORD>());

        // 기존 버텍스 복사
        for (DWORD i = 0; i < pMesh->numVerts; ++i)
        {
            vertices.push_back(pMesh->verts[i]);
            normals.push_back(pMesh->getNormal(i));
        }

        // 기존 인덱스 버퍼 복사
        for (DWORD i = 0; i < pMesh->numFaces; ++i)
        {
            for (DWORD j = 0; j < 3; ++j)
            {
                const DWORD uvIndex = pMesh->tvFace[i].t[j];
                const DWORD vertexIndex = pMesh->faces[i].v[j];

                indexBuffers[0].push_back(uvIndex);
            }
        }
    }

    // 인클루드 플래그 저장
    fwrite(&includeFlag, sizeof(DWORD), 1, m_pFile);

    // 버텍스 개수 저장
    const DWORD numVertices = (DWORD)vertices.size();
    fwrite(&numVertices, sizeof(DWORD), 1, m_pFile);

    // 머티리얼 ID 저장
    if (pMtl != nullptr && GetIncludeTextureCoords())
    {
        fwrite(&materialID, sizeof(DWORD), 1, m_pFile);
    }

    // 인덱스 버퍼 개수 저장
    DWORD numIndexBuffers = 0;
    for (DWORD i = 0; i < indexBuffers.size(); ++i)
    {
        if (!indexBuffers[i].empty())
        {
            ++numIndexBuffers;
        }
    }
    fwrite(&numIndexBuffers, sizeof(DWORD), 1, m_pFile);

    const Matrix3 tm = pNode->GetObjTMAfterWSM(0);
    for (DWORD i = 0; i < numVertices; ++i)
    {
        // 위치 저장
        const Point3 v = tm * vertices[i];
        const float pos[3] = { v.x, v.z, v.y };
        fwrite(pos, sizeof(pos), 1, m_pFile);

        // 법선 저장
        const Point3& vn = normals[i];
        const float normal[3] = { vn.x, vn.z, vn.y };
        fwrite(normal, sizeof(normal), 1, m_pFile);
    }

    // UV 저장
    if (pMtl != nullptr && GetIncludeTextureCoords())
    {
        for (DWORD i = 0; i < numVertices; ++i)
        {
            const float uv[2] = { uvs[i].x, 1.0f - uvs[i].y };
            fwrite(uv, sizeof(uv), 1, m_pFile);
        }
    }

    WORD indices[3];
    for (DWORD i = 0; i < numMaxIndexBuffers; ++i)
    {
        vector<WORD>& indexBuffer = indexBuffers[i];
        if (indexBuffer.empty())
        {
            continue;
        }

        // 인덱스 개수 저장
        const WORD numIndices = indexBuffer.size();
        fwrite(&numIndices, sizeof(WORD), 1, m_pFile);

        // 인덱스 저장
        for (DWORD j = 0; j < numIndices; j += 3)
        {
            indices[0] = indexBuffer[j];
            indices[1] = indexBuffer[j + 2];
            indices[2] = indexBuffer[j + 1];

            fwrite(indices, sizeof(indices), 1, m_pFile);
        }
    }
    
    if (bDeleteTri)
    {
        delete pTri;
    }
}

static Point3 GetVertexNormal(Mesh* pMesh, const DWORD faceIndex, RVertex* pRVertex)
{
    Face* pFace = &pMesh->getFace(faceIndex);
    const DWORD smGroup = pFace->smGroup;
    const DWORD numNormals = pRVertex->rFlags & NORCT_MASK;
    Point3 vertexNormal;

    if (numNormals != 0 && smGroup)
    {
        // 법선이 한 개면 RVertex의 rn 멤버에 저장되어있음
        if (numNormals == 1)
        {
            vertexNormal = pRVertex->rn.getNormal();
        }
        // 법선이 여러 개라면 RVertex의 ern 멤버에 저장되어 있음
        else
        {
            // 현재 면과 같은 스무딩 그룹인 법선을 찾아서 추가
            for (DWORD k = 0; k < numNormals; ++k)
            {
                if (pRVertex->ern[k].getSmGroup() & smGroup)
                {
                    vertexNormal = pRVertex->ern[k].getNormal();
                }
            }
        }
    }
    // 면이 스무딩 그룹에 포함되어 있지 않다면 getFaceNormal()로 얻어올 수 있음
    else
    {
        vertexNormal = pMesh->getFaceNormal(faceIndex);
    }

    return vertexNormal;
}