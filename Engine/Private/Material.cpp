#include "Material.h"



Material::Material(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context) 
	: m_Device(Device), m_Context(Context)
{
}

HRESULT Material::Initialize()
{
	return S_OK;
}

HRESULT Material::Bind()
{
	return S_OK;
}

void Material::Update()
{
}

HRESULT Material::SetShader(shared_ptr<class Shader> shader)
{
	return S_OK;
}

HRESULT Material::SetTexture(shared_ptr<class Texture> tex)
{
	return S_OK;
}

HRESULT Material::SetColor(const _float4& color)
{
	return S_OK;
}
