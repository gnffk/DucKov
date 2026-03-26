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


public:
	static unique_ptr<Level_Logo> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END