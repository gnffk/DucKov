#include "MainMenu.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "UIObject.h"

MainMenu::MainMenu(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

MainMenu::~MainMenu()
{
}

HRESULT MainMenu::Initialize()
{
	if (FAILED(Ready_UI()))
		return E_FAIL;

	UIObject::UIOBJECT_DESC Desc{};

	Desc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
	Desc.m_strName = L"MainMenu_UI";
	Desc.m_strPrototypeObjectName = L"Prototype_GameObject_MainMenu_UI";
	Desc.m_strPrototypeBaseName = L"MainMenu_UI";
	Desc.pCameraType = ETOUI(CAMERA::NONE);

	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;
	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 1.f;
	Desc.fSizeY = 1.f;

	if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(),TEXT("Prototype_GameObject_MainMenu_UI"),CGameInstance::Get().Get_Level(),TEXT("UI"),&Desc)))
		return E_FAIL;

	return S_OK;


}

void MainMenu::Update(_float fTimeDelta)
{
	Check_ButtonClick();
}

HRESULT MainMenu::Render()
{
	return S_OK;
}

HRESULT MainMenu::Ready_UI()
{
	CGameInstance& GameInstance = CGameInstance::Get();



	return S_OK;
}

void MainMenu::Check_ButtonClick()
{
	CGameInstance& GameInstance = CGameInstance::Get();

	if (!GameInstance.Mouse_Down(MOUSEKEYSTATE::DIM_LB))
		return;

	// Start
	if (IsMouseInRect(m_fStartX, m_fStartY, m_fButtonSizeX, m_fButtonSizeY))
	{
		
	}

	// Setting
	if (IsMouseInRect(m_fSettingX, m_fSettingY, m_fButtonSizeX, m_fButtonSizeY))
	{
		m_bSettingOpen = !m_bSettingOpen;

		return;
	}

	// Exit
	if (IsMouseInRect(m_fExitX, m_fExitY, m_fButtonSizeX, m_fButtonSizeY))
	{
		PostQuitMessage(0);
		return;
	}
}

_bool MainMenu::IsMouseInRect(_float fCenterX, _float fCenterY, _float fSizeX, _float fSizeY)
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_float fMouseX = static_cast<_float>(ptMouse.x);
	_float fMouseY = static_cast<_float>(ptMouse.y);

	_float fLeft = fCenterX - fSizeX * 0.5f;
	_float fRight = fCenterX + fSizeX * 0.5f;
	_float fTop = fCenterY - fSizeY * 0.5f;
	_float fBottom = fCenterY + fSizeY * 0.5f;

	return fMouseX >= fLeft &&
		fMouseX <= fRight &&
		fMouseY >= fTop &&
		fMouseY <= fBottom;
}

unique_ptr<MainMenu> MainMenu::Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<MainMenu>(new MainMenu(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(g_hWnd, TEXT("Failed to Created : MainMenu"), nullptr, MB_OK);
		return nullptr;
	}

	return pInstance;
}