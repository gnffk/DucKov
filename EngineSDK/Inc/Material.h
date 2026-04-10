#pragma once
#include "Resource.h"


NS_BEGIN(Engine)

class ENGINE_DLL Material abstract : public Resource
{
private:
    Material(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context);
public:
	virtual ~Material() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Bind();
    virtual void Update();
    
    virtual HRESULT SetShader(shared_ptr<class Shader> shader);
    virtual HRESULT SetTexture(shared_ptr<class Texture> tex);
    virtual HRESULT SetColor(const _float4& color);

private:
    ComPtr<ID3D11Device> m_Device = nullptr;
    ComPtr<ID3D11DeviceContext> m_Context = nullptr;


    shared_ptr<class Shader> m_Shader;
    shared_ptr<class Texture> m_Diffuse;

    ComPtr<ID3D11Buffer> m_CBMaterial;


};

NS_END
