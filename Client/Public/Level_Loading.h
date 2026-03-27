#pragma once

#include "Client_Defines.h"
#include "CLevel.h"


NS_BEGIN(Client)
class Level_Loading : public CLevel
{
private:
	Level_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~Level_Loading();
public:
	virtual HRESULT Initialize(LEVEL eNextLevelIndex);
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	LEVEL							m_eNextLevelIndex = { LEVEL::END };
	unique_ptr<class CLoader>		m_pLoader = { nullptr };

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);

public:
	static unique_ptr<Level_Loading> Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex);



};

NS_END