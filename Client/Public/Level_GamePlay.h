#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class Level_GamePlay final : public CLevel
{
private:
	Level_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~Level_GamePlay();

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	enum class GAMEPLAY_FADE_STATE
	{
		CLOSED,
		OPENING,
		OPENED,
		CLOSING
	};

private:
	GAMEPLAY_FADE_STATE m_eFadeState = GAMEPLAY_FADE_STATE::CLOSED;

	float m_fMaskRadius = -0.05f;
	float m_fMaskMaxRadius = 1.2f;
	float m_fMaskSpeed = 1.8f;


public:
	HRESULT Ready_GamePlay();
	HRESULT Ready_GamePlay2();
	wstring Make_UniqueObjectName(const wstring& LayerName, const wstring& baseName);
private:
	map<string, shared_ptr<class GameObject>> m_pUI;
private:
	unique_ptr<class NavMeshEditor> m_pNavMeshEditor = nullptr;

public:
	static unique_ptr<Level_GamePlay> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END