

#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class MainMenu final : public CLevel
{
private:
	MainMenu(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~MainMenu();

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_UI();

private:
	void Check_ButtonClick();

private:
	_bool IsMouseInRect(_float fCenterX, _float fCenterY, _float fSizeX, _float fSizeY);

private:
	_bool m_bSettingOpen = false;

private:
	// 버튼 위치 저장
	_float m_fStartX = 640.f;
	_float m_fStartY = 430.f;
	_float m_fSettingX = 640.f;
	_float m_fSettingY = 530.f;
	_float m_fExitX = 640.f;
	_float m_fExitY = 630.f;

	_float m_fButtonSizeX = 300.f;
	_float m_fButtonSizeY = 80.f;

public:
	static unique_ptr<MainMenu> Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);
};

NS_END