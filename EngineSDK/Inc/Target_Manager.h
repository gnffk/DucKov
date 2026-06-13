#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Target_Manager final
{
private:
	Target_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~Target_Manager();

public:
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResource(const _wstring& strTargetTag, shared_ptr<class Shader> pShader, const _char* pConstantName);

public:
	HRESULT Copy_RenderTarget(const _wstring& strTargetTag, ComPtr<ID3D11Texture2D> pOut);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Debug_Render(const _wstring& strMRTTag, shared_ptr<class Shader> pShader, const _char* pConstantName, shared_ptr<class VIBuffer_Rect> pVIBuffer);
#endif


private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };
	ComPtr<ID3D11RenderTargetView>	m_pBackBufferRTV = { nullptr };
	ComPtr<ID3D11DepthStencilView>	m_pOriginalDSV = { nullptr };

private:
	map<const _wstring, shared_ptr<class RenderTarget>>		m_RenderTargets;
	map<const _wstring, list<shared_ptr<class RenderTarget>>>	m_MRTs;

private:
	shared_ptr<class RenderTarget> Find_RenderTarget(const _wstring& strTargetTag);
	list<shared_ptr<class RenderTarget>>* Find_MRT(const _wstring& strMRTTag);

public:
	static unique_ptr<Target_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);


};

NS_END