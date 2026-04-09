#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class MapEditor final : public CLevel
{
private:
	MapEditor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~MapEditor();

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	HRESULT Ready_Layer_MapEditor(const _wstring& strLayerTag);
	virtual HRESULT Render() override;
	void	IMGUI_Update();
	


public:
	static unique_ptr<MapEditor> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END