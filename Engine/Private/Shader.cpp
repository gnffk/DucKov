#include "Shader.h"


ComPtr<ID3DBlob> Shader::CompileHLSL(LPCWSTR fileName,LPCSTR entryPoint,LPCSTR target)
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG;
    flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(
        fileName,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint,
        target,
        flags,
        0,
        shaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf()
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        return nullptr;
    }

    return shaderBlob;
}

ShaderDX11::ShaderDX11(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context) : m_Device(Device), m_Context(Context)
{
}

HRESULT ShaderDX11::SetShader()
{
    return S_OK;
}

HRESULT ShaderDX11::Recompile()
{
    return S_OK;
}

HRESULT ShaderDX11::Initialize()
{
    return S_OK;
}

HRESULT ShaderDX11::CreateVSAndInputLayout()
{
    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = m_Device->CreateSamplerState(&sampDesc, m_pSampler.GetAddressOf());
    if (FAILED(hr))
        return hr;


    hr = m_Device->CreateVertexShader(
        m_VSBlob->GetBufferPointer(),
        m_VSBlob->GetBufferSize(),
        nullptr,
        m_VS.GetAddressOf()
    );
    if (FAILED(hr))
        return hr;

    hr = m_Device->CreateInputLayout(
        m_BaseDesc.m_InputDescs->data(),
        m_BaseDesc.m_NumElements,
        m_VSBlob->GetBufferPointer(),
        m_VSBlob->GetBufferSize(),
        m_InputLayout.GetAddressOf()
    );
    if (FAILED(hr))
        return hr;

    return S_OK;
}

HRESULT ShaderDX11::CreatePS()
{
    return m_Device->CreatePixelShader(
        m_PSBlob->GetBufferPointer(),
        m_PSBlob->GetBufferSize(),
        nullptr,
        m_PS.GetAddressOf()
    );
}

