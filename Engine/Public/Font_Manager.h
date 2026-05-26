#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Font_Manager final
{
private:
	Font_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~Font_Manager();

public:
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	void Draw(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, float fScale = 1.f, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, const _float2& vOrigin = { 0.f, 0.f });


private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

private:
	map<const _wstring, unique_ptr<class CustomFont>>		m_Font;

private:
	class CustomFont* Find_Font(const _wstring& strFontTag);

public:
	static unique_ptr<Font_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END