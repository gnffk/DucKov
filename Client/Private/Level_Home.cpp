#include "Level_Home.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "UIObject.h"
#include "GameObject.h"
#include "Camera.h"
#include "Layer.h"
#include "BaseCollider.h"
#include "NavMeshEditor.h"
#include "Player.h"
#include "CircleMask_UI.h"

Level_Home::Level_Home(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

Level_Home::~Level_Home()
{
}

HRESULT Level_Home::Initialize()
{
    if (FAILED(Ready_Home()))
        return E_FAIL;
   
    CGameInstance::Get().Load_Lights_FromJson(L"../../Resources/Data/Light/Lights_Home.json");
	CGameInstance::Get().Load("Stage_Home", CGameInstance::Get().Get_Level());


	CGameInstance::Get().Change_Camera(ETOUI(CAMERA::MAIN));

    m_pNavMeshEditor = NavMeshEditor::Create(m_pDevice, m_pDeviceContext);


    CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player"), CGameInstance::Get().Get_Level(), L"PlayerTag");

    auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");
    


    static_pointer_cast<Player>(pPlayer)->Change_Navigation_AndHome(L"Prototype_Component_Navigation_Home", {-4.8f,1.8f,-15.f}, static_cast<uint32_t>(1));


    GameObject::GAMEOBJECT_DESC descPlayer_Camera{};

    descPlayer_Camera.ObjectType = ETOUI(OBJECTTYPE::OBJECT_CAMERA);
    descPlayer_Camera.m_strName = L"Player_Camera";
    descPlayer_Camera.m_strPrototypeObjectName = L"Prototype_GameObject_PlayerCamera";
    descPlayer_Camera.m_strPrototypeBaseName = L"Prototype_GameObject_PlayerCamera";
    descPlayer_Camera.pCameraType = ETOUI(CAMERA::PLAYER);
    descPlayer_Camera.fSpeedPerSec = 10.f;
    descPlayer_Camera.fRotationPerSec = 0.1f;

    CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PlayerCamera"), CGameInstance::Get().Get_Level(), L"PlayerTag", &descPlayer_Camera);

    CGameInstance::Get().Add_Camera(ETOUI(CAMERA::PLAYER), dynamic_pointer_cast<Camera>(CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player_Camera")));

    CGameInstance::Get().Change_Camera(3);


    Shadow::SHADOW_LIGHT_DESC			ShadowLightDesc{};

    ShadowLightDesc.vEye = _float4(-20.f, 100.f, -5.5f, 1.f);
    ShadowLightDesc.vAt = _float4(0.f, 5.f, 5.f, 1.f);
    ShadowLightDesc.fFovy = XMConvertToRadians(60.f);
    ShadowLightDesc.fNear = 0.1f;
    ShadowLightDesc.fFar = 1000.f;

    if (FAILED(CGameInstance::Get().Add_Shadow_Light(ShadowLightDesc)))
        return E_FAIL;




    CGameInstance::Get().PlaySoundLoop(L"BGM_HOME", CHANNELID::BGM_HOME, 0.3f);



    CircleMask_UI::CIRCLEMASK_DESC MaskDesc{};

    MaskDesc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
    MaskDesc.m_strName = L"CircleMask_UI";
    MaskDesc.m_strPrototypeObjectName = L"Prototype_GameObject_CircleMask_UI";
    MaskDesc.m_strPrototypeBaseName = L"CircleMask_UI";
    MaskDesc.pCameraType = ETOUI(CAMERA::NONE);

    MaskDesc.fSpeedPerSec = 0.f;
    MaskDesc.fRotationPerSec = 0.f;

    MaskDesc.fX = 0.f;
    MaskDesc.fY = 0.f;
    MaskDesc.fSizeX = CGameInstance::Get().Get_ViewportSize().x;
    MaskDesc.fSizeY = CGameInstance::Get().Get_ViewportSize().y;

    MaskDesc.fStartRadius = -0.05f;
    MaskDesc.fMaxRadius = 1.2f;

    auto pCircleMask = dynamic_pointer_cast<GameObject>( CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::NEVER),TEXT("Prototype_GameObject_CircleMask_UI"), &MaskDesc ) );

    if (pCircleMask == nullptr)
    {
        MSG_BOX("CircleMask_UI Clone Failed");
        return E_FAIL;
    }

    m_pUI.emplace("CircleMask_UI", pCircleMask);

    m_eFadeState = HOME_FADE_STATE::OPENING;
    m_fMaskRadius = -0.05f;




	return S_OK;
}

void Level_Home::Update(_float fTimeDelta)
{
    auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");
    if (static_pointer_cast<Player>(pPlayer)->m_bNext) {
        m_eFadeState = HOME_FADE_STATE::CLOSING;
    }
    // =====================================================
    // Circle Mask Fade In
    // =====================================================
    if (m_eFadeState == HOME_FADE_STATE::OPENING)
    {
        m_fMaskRadius += m_fMaskSpeed * fTimeDelta;

        if (m_fMaskRadius >= m_fMaskMaxRadius)
        {
            m_fMaskRadius = m_fMaskMaxRadius;
            m_eFadeState = HOME_FADE_STATE::OPENED;
        }
    }

    if (m_eFadeState == HOME_FADE_STATE::CLOSING)
    {
        m_fMaskRadius -= m_fMaskSpeed * fTimeDelta;

        if (m_fMaskRadius <= -0.05f)
        {

            auto pPlayerObj = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(),L"PlayerTag", L"Player" );

            if (pPlayerObj != nullptr)
            {
                auto pPlayer = dynamic_pointer_cast<Player>(pPlayerObj);

                if (pPlayer != nullptr)
                {
                    PLAYER_SAVE_DATA SaveData = pPlayer->Make_SaveData();

                    CGameInstance::Get().Save_PlayerData(SaveData);
                }
            }


            m_fMaskRadius = -0.05f;
            m_eFadeState = HOME_FADE_STATE::CLOSED;

            CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING), Level_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL::GAMEPLAY));

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

            // 테스트 중에는 항상 보이게
            pMask->Set_Visible(true);

            // 완전히 열린 뒤에는 안 그리게 하고 싶으면 이걸로 변경
            // pMask->Set_Visible(m_eFadeState != HOME_FADE_STATE::OPENED);
        }
    }

    // =====================================================
    // 일반 UI 먼저
    // =====================================================
    for (auto& Pair : m_pUI)
    {
        const string& strUIName = Pair.first;
        auto pUIObject = Pair.second;

        if (nullptr == pUIObject)
            continue;

        if (strUIName == "CircleMask_UI")
            continue;

        CGameInstance::Get().Add_UIObject(
            L"MainMenu_UI",
            static_pointer_cast<UIObject>(pUIObject)
        );
    }

    // =====================================================
    // CircleMask는 무조건 마지막
    // =====================================================
    if (iterMask != m_pUI.end() && iterMask->second != nullptr)
    {
        CGameInstance::Get().Add_UIObject(
            L"MainMenu_UI",
            static_pointer_cast<UIObject>(iterMask->second)
        );
    }

    if (m_pNavMeshEditor)
        m_pNavMeshEditor->Update();
}

HRESULT Level_Home::Render()
{
    if (m_pNavMeshEditor)
        m_pNavMeshEditor->Render();
	return S_OK;
}

HRESULT Level_Home::Ready_Home()
{
    GameObject::GAMEOBJECT_DESC descSKY{};
    descSKY.ObjectType = ETOUI(OBJECTTYPE::OBJECT_SKYBOX);
    descSKY.m_strName = L"SkyBox";
    descSKY.m_strPrototypeObjectName = L"Prototype_GameObject_Sky_Home";
    descSKY.m_strPrototypeBaseName = L"SkyBox";
    descSKY.pCameraType = ETOUI(CAMERA::NONE); 
    descSKY.fSpeedPerSec = 5.f;
    descSKY.fRotationPerSec = 1.f;

    if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Sky_Home"),
        CGameInstance::Get().Get_Level(), L"Base", &descSKY)))
        return E_FAIL;


    GameObject::GAMEOBJECT_DESC descMain_Camera{};

    descMain_Camera.ObjectType = ETOUI(OBJECTTYPE::OBJECT_CAMERA);
    descMain_Camera.m_strName = L"Main_Camera";
    descMain_Camera.m_strPrototypeObjectName = L"Prototype_GameObject_PerspectiveCamera";
    descMain_Camera.m_strPrototypeBaseName = L"Prototype_GameObject_PerspectiveCamera";
    descMain_Camera.pCameraType = ETOUI(CAMERA::MAIN);
    descMain_Camera.fSpeedPerSec = 10.f;
    descMain_Camera.fRotationPerSec = 0.1f;


    if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PerspectiveCamera"), CGameInstance::Get().Get_Level(), L"Base", &descMain_Camera)))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_Camera(ETOUI(CAMERA::MAIN),
        dynamic_pointer_cast<Camera>(CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"Base", L"Main_Camera")))))
        return E_FAIL;

    LIGHT_DESC			LightDesc{};

    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
    LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(CGameInstance::Get().Add_Light(LightDesc)))
        return E_FAIL;


    return S_OK;
}

unique_ptr<Level_Home> Level_Home::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Level_Home>(new Level_Home(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : Level_Home"), nullptr, MB_OK);

	return pInstance;
}
