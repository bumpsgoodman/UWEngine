#include "Precompiled.h"
#include "D3D11Helper.h"

UWMETHOD(HRESULT) CompileShaderFromFile(const wchar_t* pFileName, const char* pEntrypoint, const char* pShaderModel, ID3DBlob** ppShaderBlob)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(pFileName, nullptr, nullptr, pEntrypoint, pShaderModel,
                            dwShaderFlags, 0, ppShaderBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
        {
            OutputDebugStringA((const char*)pErrorBlob->GetBufferPointer());
            SAFE_RELEASE(pErrorBlob);
        }

        return hr;
    }

    if (pErrorBlob != nullptr)
    {
        SAFE_RELEASE(pErrorBlob);
    }

    return S_OK;
}
