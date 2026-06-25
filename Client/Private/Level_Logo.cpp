#include "Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "UIObject.h"
#include "CircleMask_UI.h"
#include "MainMenu_UI.h"
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

    CircleMask_UI::CIRCLEMASK_DESC MaskDesc{};

    MaskDesc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
    MaskDesc.m_strName = L"CircleMask_UI";
    MaskDesc.m_strPrototypeObjectName = L"Prototype_GameObject_CircleMask_UI";
    MaskDesc.m_strPrototypeBaseName = L"CircleMask_UI";
    MaskDesc.pCameraType = ETOUI(CAMERA::NONE);

    MaskDesc.fSpeedPerSec = 0.f;
    MaskDesc.fRotationPerSec = 0.f;

    // 풀스크린 UI
    MaskDesc.fX = 0.f;
    MaskDesc.fY = 0.f;
    MaskDesc.fSizeX = CGameInstance::Get().Get_ViewportSize().x;
    MaskDesc.fSizeY = CGameInstance::Get().Get_ViewportSize().y;

    MaskDesc.fStartRadius = -0.05f;
    MaskDesc.fMaxRadius = 1.2f;

    m_pUI.emplace("CircleMask_UI",dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::NEVER),TEXT("Prototype_GameObject_CircleMask_UI"),&MaskDesc )));

    m_eFadeState = LOGO_FADE_STATE::CLOSED;
    m_fMaskRadius = -0.05f;

	CGameInstance::Get().PlaySoundLoop(L"BGM_MENU", CHANNELID::BGM_HOME, 0.2f);

	return S_OK;
}

void Level_Logo::Update(_float fTimeDelta)
{
    auto pMainMenu = dynamic_pointer_cast<MainMenu_UI>(m_pUI["MainMenu_UI"]);

    if (pMainMenu && pMainMenu->otherOpen == true)
    {
        Request_StartGame();
    }

    // 처음 검은 화면 상태에서 클릭하면 중앙 원이 열림
    if (m_eFadeState == LOGO_FADE_STATE::CLOSED)
    {
        if (CGameInstance::Get().Mouse_Down(MOUSEKEYSTATE::DIM_LB))
        {
            m_eFadeState = LOGO_FADE_STATE::OPENING;
        }
    }

    if (m_eFadeState == LOGO_FADE_STATE::OPENING)
    {
        m_fMaskRadius += m_fMaskSpeed * fTimeDelta;

        if (m_fMaskRadius >= m_fMaskMaxRadius)
        {
            m_fMaskRadius = m_fMaskMaxRadius;
            m_eFadeState = LOGO_FADE_STATE::OPENED;
        }
    }

    if (m_eFadeState == LOGO_FADE_STATE::CLOSING)
    {
        m_fMaskRadius -= m_fMaskSpeed * fTimeDelta;

        if (m_fMaskRadius <= -0.05f)
        {
            m_fMaskRadius = -0.05f;
            m_eFadeState = LOGO_FADE_STATE::CLOSED;

            CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING),Level_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL::HOME));

            return;
        }
    }

    auto iterMask = m_pUI.find("CircleMask_UI");

    if (iterMask != m_pUI.end())
    {
        auto pMask = dynamic_pointer_cast<CircleMask_UI>(iterMask->second);

        if (pMask != nullptr)
        {
            pMask->Set_Radius(m_fMaskRadius);


            pMask->Set_Visible(true);

        }
    }


    for (auto& Pair : m_pUI)
    {
        const string& strUIName = Pair.first;
        auto pUIObject = Pair.second;

        if (pUIObject == nullptr)
            continue;

        if (strUIName == "CircleMask_UI")
            continue;

        CGameInstance::Get().Add_UIObject(L"MainMenu_UI",static_pointer_cast<UIObject>(pUIObject));
    }

    // 2. CircleMask는 무조건 마지막에 넣기
    if (iterMask != m_pUI.end() && iterMask->second != nullptr)
    {
        CGameInstance::Get().Add_UIObject(L"MainMenu_UI",static_pointer_cast<UIObject>(iterMask->second));
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
