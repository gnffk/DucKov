#pragma once
#include "Resource.h"

NS_BEGIN(Engine)

class ENGINE_DLL Texture final : public Resource
{
private:
    Texture(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context);
public:
	virtual ~Texture();

public:
    HRESULT Initialize();
    HRESULT LoadFromFile( const wstring& path);
    void Bind( UINT slot);
    ComPtr<ID3D11ShaderResourceView> GetSRV() const;
 

private:
    ComPtr<ID3D11Device> m_Device = nullptr;
    ComPtr<ID3D11DeviceContext> m_Context = nullptr;

    wstring m_FilePath;
    wstring m_Name;

    UINT m_Width = 0;
    UINT m_Height = 0;
    UINT m_MipLevels = 0;
    DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;

    ComPtr<ID3D11Texture2D> m_Texture;
    ComPtr<ID3D11ShaderResourceView> m_SRV;

public:
    static shared_ptr<Texture> Create(ComPtr<ID3D11Device> Device, ComPtr<ID3D11DeviceContext> Context);
};



NS_END
