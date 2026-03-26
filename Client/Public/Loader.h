#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

NS_BEGIN(Client)

class CLoader final
{
private:
	CLoader(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CLoader();

public:
	HRESULT Initialize(LEVEL eNextLevelIndex);
	HRESULT Loading();
	_bool isFinished() const {
		return m_isFinished;
	}


private:
	LEVEL							m_eNextLevelIndex = { LEVEL::END };
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

private: /* For.Thread */
	HANDLE							m_hThread = {};
	CRITICAL_SECTION				m_CriticalSection = {};

private: /* For.Loading */
	_bool							m_isFinished = { false };
	_tchar							m_szLoadingText[MAX_PATH] = {};

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();

public:
	static unique_ptr<CLoader> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex);
};

NS_END