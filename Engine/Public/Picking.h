#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Picking final
{
private:
	Picking(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~Picking();

public:
	HRESULT Initialize(HWND hWnd);
	void Update();
	_bool Picking_to_Shader(_float4* pOut);

private:
	HWND							m_hWnd = {};
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

private:
	ComPtr<ID3D11Texture2D>			m_pTexture2D = { nullptr };

private:
	unique_ptr<_float4[]>			m_pPixels = {};

public:
	static unique_ptr<Picking> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, HWND hWnd);

};

NS_END