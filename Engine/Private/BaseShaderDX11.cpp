#include "BaseShaderDX11.h"

BaseShaderDX11::BaseShaderDX11(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context) 
    : ShaderDX11{ Device, Context }
{
}

HRESULT BaseShaderDX11::SetShader()
{

    // 1. Input Layout
    m_Context->IASetInputLayout(m_InputLayout.Get());

    

    // 5. Shader

    m_Context->VSSetShader(m_VS.Get(), nullptr, 0);
    m_Context->PSSetShader(m_PS.Get(), nullptr, 0);
    m_Context->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());

    return S_OK;
}

HRESULT BaseShaderDX11::Recompile()
{
	return S_OK;
}

HRESULT BaseShaderDX11::Initialize()
{
    __super::Initialize();

    m_BaseDesc.m_FileName = L"../../Resources/Shaders/Shader_Base.hlsl";
    m_BaseDesc.m_VSEntry = "VS_MAIN";
    m_BaseDesc.m_PSEntry = "PS_MAIN";

    m_BaseDesc.m_StencilRef = 1;
    m_BaseDesc.m_SamplerSlot = 0;

    m_BaseDesc.m_InputDescs = make_shared<vector<D3D11_INPUT_ELEMENT_DESC>>();
    m_BaseDesc.m_InputDescs->reserve(3);
    (*m_BaseDesc.m_InputDescs) =
    {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
          D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    m_BaseDesc.m_NumElements = static_cast<UINT>(m_BaseDesc.m_InputDescs->size());

    m_VSBlob = CompileHLSL(m_BaseDesc.m_FileName.c_str(), m_BaseDesc.m_VSEntry.c_str(), "vs_5_0");
    m_PSBlob = CompileHLSL(m_BaseDesc.m_FileName.c_str(), m_BaseDesc.m_PSEntry.c_str(), "ps_5_0");

    if (!m_VSBlob || !m_PSBlob)
        return E_FAIL;

    if (FAILED(CreateVSAndInputLayout()))
        return E_FAIL;

    if (FAILED(CreatePS()))
        return E_FAIL;

    return S_OK;

}

shared_ptr<BaseShaderDX11> BaseShaderDX11::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto		pInstance = shared_ptr<BaseShaderDX11>(new BaseShaderDX11(pDevice, pContext));

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : BaseShaderDX11");
        return nullptr;
    }

    return pInstance;
}
