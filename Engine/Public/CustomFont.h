#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CustomFont final
{
private:
	CustomFont(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CustomFont();

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	void Draw(const _tchar* pText, const _float2& vPosition, float fScale = 1.f, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, const _float2& vOrigin = { 0.f, 0.f });


private:
	ComPtr<ID3D11Device>					m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>				m_pContext = { nullptr };

	unique_ptr<SpriteFont>					m_pFont = { nullptr };
	unique_ptr<SpriteBatch>					m_pBatch = { nullptr };

public:
	static unique_ptr<CustomFont> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pFontFilePath);
};

NS_END