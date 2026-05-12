#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLevel abstract 
{
protected:
	CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

	string  WStringToString(const std::wstring& wstr);
protected:
	ComPtr<ID3D11Device>			m_pDevice				{ nullptr };
	ComPtr<ID3D11DeviceContext>		m_pDeviceContext		{ nullptr };
};

NS_END
