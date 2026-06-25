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

public:
	HRESULT Ready_GamePlay();
	wstring Make_UniqueObjectName(const wstring& LayerName, const wstring& baseName);
private:
	map<string, shared_ptr<class GameObject>> m_pUI;
private:
	unique_ptr<class NavMeshEditor> m_pNavMeshEditor = nullptr;

public:
	static unique_ptr<Level_GamePlay> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END