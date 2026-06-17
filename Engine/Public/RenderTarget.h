
#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class RenderTarget final
{
private:
	RenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~RenderTarget();

public:
	ComPtr<ID3D11RenderTargetView> Get_RTV() const {
		return m_pRTV;
	}

public:
	HRESULT Initialize(uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	void Clear();
	HRESULT Bind_ShaderResource(shared_ptr<class Shader> pShader, const _char* pConstantName);
public:
	HRESULT Copy_RenderTarget(ComPtr<ID3D11Texture2D> pOut);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Debug_Render(shared_ptr<class Shader> pShader, const _char* pConstantName, shared_ptr<class VIBuffer_Rect> pVIBuffer);
#endif

private:
	uint32_t m_iWidth = 0;
	uint32_t m_iHeight = 0;

public:
	uint32_t Get_Width() const { return m_iWidth; }
	uint32_t Get_Height() const { return m_iHeight; }

private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	ComPtr<ID3D11Texture2D>				m_pTexture2D = { nullptr };
	ComPtr<ID3D11RenderTargetView>		m_pRTV = { nullptr };
	ComPtr<ID3D11ShaderResourceView>	m_pSRV = { nullptr };

	_float4								m_vClearColor = {};

#ifdef _DEBUG
	_float4x4							m_WorldMatrix = {};
#endif

public:
	static shared_ptr<class RenderTarget> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);

};

NS_END