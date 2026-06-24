#include "Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Home.h"
#include "MapEditor.h"
#include "UIObject.h"
#include "Loading_BackGround_UI.h"
Level_Loading::Level_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

Level_Loading::~Level_Loading()
{
}

HRESULT Level_Loading::Initialize(LEVEL eNextLevelIndex)
{
	m_eNextLevelIndex = eNextLevelIndex;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	m_pLoader = CLoader::Create(
		m_pDevice,
		m_pDeviceContext,
		m_eNextLevelIndex);

	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void Level_Loading::Update(_float fTimeDelta)
{
	for (auto& Pair : m_pUI)
	{
		auto pUIObject = Pair.second;

		if (nullptr == pUIObject)
			continue;

		auto pUI = static_pointer_cast<UIObject>(pUIObject);

		if (nullptr == pUI)
			continue;

		CGameInstance::Get().Add_UIObject(
			L"Loading_UI",
			pUI);
	}

	if (m_bLevelChanged)
		return;

	if (nullptr == m_pLoader)
		return;

	shared_ptr<Loading_BackGround_UI> pLoadingUI = nullptr;

	auto iter = m_pUI.find("Loading_BackGround_UI");

	if (iter != m_pUI.end())
	{
		pLoadingUI =
			dynamic_pointer_cast<Loading_BackGround_UI>(iter->second);
	}

	// 1. 실제 리소스 로딩이 끝나야 함
	if (false == m_pLoader->isFinished())
		return;

	// 2. GAMEPLAY 로딩 애니메이션도 끝나야 함
	if (pLoadingUI != nullptr)
	{
		if (false == pLoadingUI->Is_AnimationFinished())
			return;
	}

	// 3. 그 다음에 클릭/스페이스 입력 가능
	if (false == Any_Clicked())
		return;

	Change_ToNextLevel();
}
HRESULT Level_Loading::Render()
{
	return S_OK;
}

HRESULT Level_Loading::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//---- Texture Prototype ----//


	Loading_BackGround_UI::LOADING_UI_DESC Desc{};

	Desc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
	Desc.m_strName = L"Loading_BackGround_UI";
	Desc.m_strPrototypeObjectName = L"Prototype_GameObject_Loading_BackGround_UI";
	Desc.m_strPrototypeBaseName = L"Loading_BackGround_UI";
	Desc.pCameraType = ETOUI(CAMERA::NONE);

	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	Desc.fX = 640.f;
	Desc.fY = 360.f;
	Desc.fSizeX = 1280.f;
	Desc.fSizeY = 720.f;

	// 핵심
	// Loading_BackGround_UI가 GAMEPLAY 로딩인지 판단하려면 이걸 넘겨야 함
	Desc.eNextLevel = m_eNextLevelIndex;

	auto pLoadingUI =dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::NEVER),TEXT("Prototype_GameObject_Loading_BackGround_UI"),&Desc));

	if (nullptr == pLoadingUI)
		return E_FAIL;

	// key 이름도 실제 클래스 이름으로 맞춤
	m_pUI.emplace("Loading_BackGround_UI", pLoadingUI);

	return S_OK;

}

HRESULT Level_Loading::Change_ToNextLevel()
{
	if (m_bLevelChanged)
		return S_OK;

	m_bLevelChanged = true;

	unique_ptr<CLevel> pNewLevel = nullptr;

	switch (m_eNextLevelIndex)
	{
	case LEVEL::LOGO:
		pNewLevel = Level_Logo::Create(m_pDevice, m_pDeviceContext);
		break;
	case LEVEL::HOME:
		pNewLevel = Level_Home::Create(m_pDevice, m_pDeviceContext);
		break;
	case LEVEL::GAMEPLAY:
		pNewLevel = Level_GamePlay::Create(m_pDevice, m_pDeviceContext);
		break;

	case LEVEL::MAPEDITOR:
		pNewLevel = MapEditor::Create(m_pDevice, m_pDeviceContext);
		break;
	}

	if (pNewLevel == nullptr)
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Change_Level(ETOUI(m_eNextLevelIndex),std::move(pNewLevel))))
	{
		return E_FAIL;
	}

	return S_OK;
}

_bool Level_Loading::Any_Clicked()
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		return true;

	if (GetAsyncKeyState(VK_RBUTTON) & 0x0001)
		return true;

	if (GetAsyncKeyState(VK_MBUTTON) & 0x0001)
		return true;

	if (GetAsyncKeyState(VK_SPACE) & 0x0001)
		return true;

	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
		return true;

	return false;
}

unique_ptr<Level_Loading> Level_Loading::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex)
{
	auto	pInstance = unique_ptr<Level_Loading>(new Level_Loading(pDevice, pContext));

	if (FAILED(pInstance->Initialize(eNextLevelIndex)))
		MSG_BOX("Failed to Created : CLevel_Loading");

	return pInstance;
}
