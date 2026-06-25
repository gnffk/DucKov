
#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class Level_Home final : public CLevel
{
private:
	Level_Home(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~Level_Home();

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	enum class HOME_FADE_STATE
	{
		CLOSED,
		OPENING,
		OPENED,
		CLOSING
	};

private:
	HOME_FADE_STATE m_eFadeState = HOME_FADE_STATE::CLOSED;

	float m_fMaskRadius = -0.05f;
	float m_fMaskMaxRadius = 1.2f;
	float m_fMaskSpeed = 1.8f;

public:
	HRESULT Ready_Home();
private:
	map<string, shared_ptr<class GameObject>> m_pUI;
private:
	unique_ptr<class NavMeshEditor> m_pNavMeshEditor = nullptr;
public:
	static unique_ptr<Level_Home> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END