#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

NS_BEGIN(Client)

class CMainGame
{
private:
	CMainGame();
public:
	~CMainGame();

public:
	HRESULT Initialize();
	void Update(float fTimeDelta);
	HRESULT Render();

private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };
public:
	static unique_ptr<CMainGame> Create();
};

NS_END

