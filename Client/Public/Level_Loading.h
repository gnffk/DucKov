#pragma once

#include "Client_Defines.h"
#include "CLevel.h"


NS_BEGIN(Client)
class Level_Loading final : public CLevel
{
private:
	Level_Loading(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~Level_Loading();

public:
	HRESULT Initialize(LEVEL eNextLevelIndex);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);

private:
	HRESULT Change_ToNextLevel();
	_bool Any_Clicked();

private:
	LEVEL m_eNextLevelIndex = LEVEL::END;

private:
	unique_ptr<class CLoader> m_pLoader = nullptr;

private:
	map<string, shared_ptr<GameObject>> m_pUI;

private:
	_bool m_bLevelChanged = false;

public:
	static unique_ptr<Level_Loading> Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext,LEVEL eNextLevelIndex);
};

NS_END