#include "Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "UIObject.h"

Level_Logo::Level_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

Level_Logo::~Level_Logo()
{
}

HRESULT Level_Logo::Initialize()
{
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


	m_pUI.emplace("MainMenu_UI", dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_MainMenu_UI"), &Desc)));


	return S_OK;
}

void Level_Logo::Update(_float fTimeDelta)
{

	for (auto& Pair : m_pUI)
	{
		const string& strUIName = Pair.first;
		auto pUIObject = Pair.second;

		if (nullptr == pUIObject)
			continue;


		CGameInstance::Get().Add_UIObject(L"MainMenu_UI", static_pointer_cast<UIObject>(pUIObject));
	}
}

HRESULT Level_Logo::Render()
{

	return S_OK;
}

unique_ptr<Level_Logo> Level_Logo::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Level_Logo>(new Level_Logo(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : CLevel_Logo"), nullptr, MB_OK);

	return pInstance;
}
