#include "Texture.h"

Texture::Texture(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context)
	:m_Device(Device), m_Context(Context)
{
}



Texture::~Texture()
{
}

HRESULT Texture::Initialize() {
    
    return S_OK;
}
HRESULT Texture::LoadFromFile(const wstring& path)
{
	return S_OK;
}

void Texture::Bind(UINT slot)
{
}

ComPtr<ID3D11ShaderResourceView> Texture::GetSRV() const
{
	return nullptr;
}


shared_ptr<Texture> Texture::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto		pInstance = shared_ptr<Texture>(new Texture(pDevice, pContext));

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : Texture");
        return nullptr;
    }

    return pInstance;
}



