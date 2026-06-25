#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class Level_Logo final : public CLevel
{
private:
	Level_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~Level_Logo();

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	

private:
	enum class LOGO_FADE_STATE
	{
		CLOSED,
		OPENING,
		OPENED,
		CLOSING
	};

private:
	LOGO_FADE_STATE m_eFadeState = LOGO_FADE_STATE::CLOSED;

	float m_fMaskRadius = -0.05f;
	float m_fMaskMaxRadius = 1.2f;
	float m_fMaskSpeed = 1.8f;

	bool m_bRequestStart = false;
public:
	void Request_StartGame()
	{
		if (m_eFadeState == LOGO_FADE_STATE::OPENED)
			m_eFadeState = LOGO_FADE_STATE::CLOSING;
	}
private:
	map<string, shared_ptr<class GameObject>> m_pUI;

public:
	static unique_ptr<Level_Logo> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END