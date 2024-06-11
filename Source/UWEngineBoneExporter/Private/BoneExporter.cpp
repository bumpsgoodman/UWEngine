/*
* UWEngineBone 3DS Max Exporter
*
* 작성자: bumpsgoodman
* 작성일: 2023.06.02
*/

#define _CRT_SECURE_NO_WARNINGS

#include <unordered_map>
#include <vector>
#include <list>

#include "BoneExporter.h"

using namespace std;

// Class ID. These must be unique and randomly generated!!
// If you use this as a sample project, this is the first thing
// you should change!
#define UWBONE_EXP_CLASS_ID	Class_ID(0x30202dab, 0x132b5576)

HINSTANCE g_hInstance;

uint UWBoneExporter::DEFAULT_KEYFRAME = 5;

// Not truly the correct way to compare floats of arbitary magnitude...
BOOL EqualPoint3(Point3 p1, Point3 p2);

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
    case 0: return GetUWBoneExpDesc();
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

class UWBoneExpClassDesc :public ClassDesc {
public:
    int	IsPublic() { return 1; }
    void* Create(BOOL loading = FALSE) { return new UWBoneExporter; }
    const TCHAR* ClassName() { return GetString(IDS_UWBONEEXP); }
    const TCHAR* NonLocalizedClassName() { return _T("UWBoneExporter"); }
    SClass_ID SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
    Class_ID ClassID() { return UWBONE_EXP_CLASS_ID; }
    const TCHAR* Category() { return GetString(IDS_CATEGORY); }
};

static UWBoneExpClassDesc s_UWBoneExpDesc;

ClassDesc* GetUWBoneExpDesc()
{
    return &s_UWBoneExpDesc;
}

UWBoneExporter::~UWBoneExporter()
{
}

int UWBoneExporter::ExtCount()
{
    return 1;
}

const TCHAR* UWBoneExporter::Ext(int n)
{
    switch (n)
    {
    case 0:
        return _T("UWBone");
    }

    return _T("");
}

const TCHAR* UWBoneExporter::LongDesc()
{
    return GetString(IDS_LONGDESC);
}

const TCHAR* UWBoneExporter::ShortDesc()
{
    return GetString(IDS_SHORTDESC);
}

const TCHAR* UWBoneExporter::AuthorName()
{
    return _T("bumpsgoodman");
}

const TCHAR* UWBoneExporter::CopyrightMessage()
{
    return GetString(IDS_COPYRIGHT);
}

const TCHAR* UWBoneExporter::OtherMessage1()
{
    return _T("");
}

const TCHAR* UWBoneExporter::OtherMessage2()
{
    return _T("");
}

unsigned int UWBoneExporter::Version()
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

void UWBoneExporter::ShowAbout(HWND hWnd)
{
    DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}

static INT_PTR CALLBACK ExportDlgProc(HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    Interval animRange;
    ISpinnerControl* spin;

    UWBoneExporter* exp = (UWBoneExporter*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    switch (msg) {
    case WM_INITDIALOG:
        exp = (UWBoneExporter*)lParam;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, lParam);
        CenterWindow(hWnd, GetParent(hWnd));
        //CheckDlgButton(hWnd, IDC_MESHDATA, exp->GetIncludeMesh());
        //CheckDlgButton(hWnd, IDC_ANIMKEYS, exp->GetIncludeAnim());
        //CheckDlgButton(hWnd, IDC_MATERIAL, exp->GetIncludeMtl());
        //CheckDlgButton(hWnd, IDC_MESHANIM, exp->GetIncludeMeshAnim());
        //CheckDlgButton(hWnd, IDC_CAMLIGHTANIM, exp->GetIncludeCamLightAnim());
        //CheckDlgButton(hWnd, IDC_IKJOINTS, exp->GetIncludeIKJoints());
        //CheckDlgButton(hWnd, IDC_NORMALS, exp->GetIncludeNormals());
        //CheckDlgButton(hWnd, IDC_TEXCOORDS, exp->GetIncludeTextureCoords());
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
            //exp->SetIncludeTextureCoords(IsDlgButtonChecked(hWnd, IDC_TEXCOORDS));
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

int	UWBoneExporter::DoExport(const TCHAR* pName, ExpInterface* pExpInterface, Interface* pInterface, BOOL bSuppressPrompts, DWORD options)
{
    m_bExportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;

    m_pInterface = pInterface;

    if (!DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_UWBONE_EXPORT_DLG),
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
    const char signature[8] = "UWBone";
    fwrite(signature, sizeof(signature), 1, m_pFile);

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

BOOL UWBoneExporter::SupportsOptions(int ext, DWORD options)
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

void UWBoneExporter::preProcessRecursion(INode* pNode)
{
    if (m_bExportSelected && pNode->Selected() == FALSE)
    {
        return;
    }

    if (pNode->IsGroupHead())
    {
        return;
    }

    ++m_numTotalNode;

    for (int i = 0; i < pNode->NumberOfChildren(); ++i)
    {
        preProcessRecursion(pNode->GetChildNode(i));
    }
}

void UWBoneExporter::exportNodeRecursion(INode* pNode)
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
// 1. 정점에 UV 매핑이 되어 있지 않으면 작동하지 않음 -> 고쳐야 됨
void UWBoneExporter::exportGeomObject(INode* pNode)
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

    const wchar_t* pName = pNode->GetName();
    INode* pParentNode = pNode->GetParentNode();
    const wchar_t* pParentName = pParentNode->GetName();

    Mesh* pMesh = &pTri->GetMesh();

    const Matrix3 tm1 = pNode->GetObjTMAfterWSM(0);
    Matrix44 m;
    for (uint i = 0; i < 4; ++i)
    {
        const Point3 p = tm1.GetRow(i);
        m.M[i][0] = p.x;
        m.M[i][1] = p.z;
        m.M[i][2] = p.y;
    }
    m.M[0][3] = 0.0f;
    m.M[1][3] = 0.0f;
    m.M[2][3] = 0.0f;
    m.M[3][3] = 1.0f;

    TimeValue start = m_pInterface->GetAnimRange().Start();
    TimeValue end = m_pInterface->GetAnimRange().End();
    TimeValue t;
    Matrix3 tm;
    AffineParts ap;
    int delta = GetTicksPerFrame() * DEFAULT_KEYFRAME;
    Point3	prevPos;
    Quat	prevRot;
    Point3	prevScale;

    vector<Point3> positions;
    vector<Quat> rotations;
    vector<Point3> scales;

    for (t = start; t <= end; t += delta)
    {
        tm = pNode->GetNodeTM(t) * Inverse(pNode->GetParentTM(t));
        //tm = pNode->GetNodeTM(t);
        decomp_affine(tm, &ap);

        Point3 pos = ap.t;
        Quat rot = ap.q / prevRot;
        Point3 scale = ap.k;

        if (t == start || !EqualPoint3(pos, prevPos))
        {
            positions.push_back(pos);
            prevPos = pos;
        }

        if (!rot.IsIdentity())
        {
            Quat accumRotation = rot;
            if (t != start)
            {
                accumRotation *= rotations.back();
            }

            rotations.push_back(accumRotation);
            prevRot = ap.q;
        }

        if(t == start || !EqualPoint3(scale, prevScale))
        {
            scales.push_back(scale);
            prevScale = scale;
        }
    }

    // TM 저장
    fwrite(&m, sizeof(m), 1, m_pFile);

    // 키 프레임 저장
    const uint keyFrame = DEFAULT_KEYFRAME;
    fwrite(&keyFrame, sizeof(uint), 1, m_pFile);

    const bool bHasPosition = (positions.size() > 1) ? true : false;
    const bool bHasRotation = (rotations.size() > 1) ? true : false;
    const bool bHasScale = (scales.size() > 1) ? true : false;

    fwrite(&bHasPosition, sizeof(bool), 1, m_pFile);
    fwrite(&bHasRotation, sizeof(bool), 1, m_pFile);
    fwrite(&bHasScale, sizeof(bool), 1, m_pFile);

    if (bHasPosition)
    {
        // 위치 개수 저장
        const uint numKeys = positions.size();
        fwrite(&numKeys, sizeof(uint), 1, m_pFile);
        
        // 위치 저장
        for (uint i = 0; i < numKeys; ++i)
        {
            const Point3 pos = positions[i];
            const Point3 result(pos.x, pos.z, pos.y);
            fwrite(&result, sizeof(result), 1, m_pFile);
        }
    }

    if (bHasRotation)
    {
        // 회전 개수 저장
        const uint numKeys = rotations.size();
        fwrite(&numKeys, sizeof(uint), 1, m_pFile);

        // 회전 저장
        for (uint i = 0; i < numKeys; ++i)
        {
            const Quat rot = rotations[i];
            const Quat result(rot.x, rot.z, rot.y, rot.w);
            fwrite(&result, sizeof(result), 1, m_pFile);
        }
    }

    if (bHasScale)
    {
        // 스케일 개수 저장
        const uint numKeys = scales.size();
        fwrite(&numKeys, sizeof(uint), 1, m_pFile);

        // 스케일 저장
        for (uint i = 0; i < numKeys; ++i)
        {
            const Point3 scale = scales[i];
            const Point3 result(scale.x, scale.z, scale.y);
            fwrite(&result, sizeof(result), 1, m_pFile);
        }
    }

    if (bDeleteTri)
    {
        delete pTri;
    }
}

// Not truly the correct way to compare floats of arbitary magnitude...
BOOL EqualPoint3(Point3 p1, Point3 p2)
{
    if (fabs(p1.x - p2.x) > ALMOST_ZERO)
        return FALSE;
    if (fabs(p1.y - p2.y) > ALMOST_ZERO)
        return FALSE;
    if (fabs(p1.z - p2.z) > ALMOST_ZERO)
        return FALSE;

    return TRUE;
}