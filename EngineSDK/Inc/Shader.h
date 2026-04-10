

#pragma once
#include "Resource.h"

NS_BEGIN(Engine)

class ENGINE_DLL Shader : public Resource
{
public:
    virtual ~Shader() = default;

public:
    virtual HRESULT SetShader() = 0;
    virtual HRESULT Recompile() = 0;

protected:
    ComPtr<ID3DBlob> CompileHLSL(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR target);
};

class ENGINE_DLL ShaderDX11 : public Shader
{
protected:
    ShaderDX11(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context);
public:
    virtual ~ShaderDX11() = default;


public:
    virtual HRESULT SetShader() override;
    virtual HRESULT Recompile() override;

    virtual HRESULT Initialize();
    
protected:
    HRESULT CreateVSAndInputLayout();
    HRESULT CreatePS();

protected:
    ComPtr<ID3D11Device> m_Device = nullptr;
    ComPtr<ID3D11DeviceContext> m_Context = nullptr;

    ComPtr<ID3D11InputLayout> m_InputLayout;
    ComPtr<ID3D11VertexShader> m_VS;
    ComPtr<ID3D11PixelShader> m_PS;

    ComPtr<ID3DBlob> m_VSBlob;
    ComPtr<ID3DBlob> m_PSBlob;

    ComPtr<ID3D11SamplerState> m_pSampler;
    BASESHADER_DESC m_BaseDesc;
};

NS_END