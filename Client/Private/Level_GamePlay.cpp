
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "UIObject.h"
#include "GameObject.h"
#include "Camera.h"
#include "Layer.h"
#include "BaseCollider.h"
#include "NavMeshEditor.h"
#include "Player.h"
#include "Tree.h"
#include "Cloud.h"
#include "CircleMask_UI.h"
#include "BossMonster_Page2.h"
Level_GamePlay::Level_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

Level_GamePlay::~Level_GamePlay()
{
}

HRESULT Level_GamePlay::Initialize()
{
    if (FAILED(Ready_GamePlay()))
        return E_FAIL;

    CGameInstance::Get().Load_Lights_FromJson(L"../../Resources/Data/Light/Lights.json");
    CGameInstance::Get().Load("Stage_Test", CGameInstance::Get().Get_Level());


    CGameInstance::Get().Change_Camera(ETOUI(CAMERA::MAIN));

    m_pNavMeshEditor = NavMeshEditor::Create(m_pDevice, m_pDeviceContext);


    CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player"), CGameInstance::Get().Get_Level(), L"PlayerTag");

    auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");



    static_pointer_cast<Player>(pPlayer)->Change_Navigation(L"Prototype_Component_Navigation", static_cast<uint32_t>(1));

    if (pPlayer != nullptr &&
        CGameInstance::Get().Has_PlayerSaveData())
    {
        static_pointer_cast<Player>(pPlayer)->Apply_SaveData(CGameInstance::Get().Get_PlayerSaveData());
    }


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


    //--------------------------------Little Monster------------------------------------------------
    for (int32_t i = 0; i < 5; ++i) {
        GameObject::GAMEOBJECT_DESC desc{};

        desc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_MONSTER);

        desc.m_strName = Make_UniqueObjectName(L"Monster", L"LittleMonster");

        desc.m_strPrototypeObjectName = L"Prototype_GameObject_LittleMonster";

        desc.m_strPrototypeBaseName = L"SK_LittleMonster";

        desc.pCameraType = ETOUI(CAMERA::NONE);

        desc.fSpeedPerSec = 3.f;

        desc.fRotationPerSec = 180.f;

        CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LittleMonster"), CGameInstance::Get().Get_Level(), L"Monster", &desc);

        auto pLittleMonster = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"Monster", desc.m_strName);

        _vector vPos = XMVectorSet(375.f,36.f,123.f,1.f);
        pLittleMonster->GetTransform()->Set_State(STATE::POSITION, vPos);
    }
  
    GameObject::GAMEOBJECT_DESC desc{};

    desc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_MONSTER);

    desc.m_strName = L"BossStage1";

    desc.m_strPrototypeObjectName = L"Prototype_GameObject_BossMonster";

    desc.m_strPrototypeBaseName = L"SK_Boss_Test";

    desc.pCameraType = ETOUI(CAMERA::NONE);

    desc.fSpeedPerSec = 3.f;

    desc.fRotationPerSec = 180.f;

    CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster"), CGameInstance::Get().Get_Level(), L"Boss", &desc);
    auto pBossMonster = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"Boss", desc.m_strName);

    _vector vPos = XMVectorSet(421.f, 33.f, 299.f, 1.f);
    pBossMonster->GetTransform()->Set_State(STATE::POSITION, vPos);

    //------------------------------------------------------------------Tree -----------------------------------------------------------------------
    GameObject::GAMEOBJECT_DESC descTree{};

    descTree.ObjectType = ETOUI(OBJECTTYPE::OBJECT_STATIC);
    descTree.m_strName = L"Tree_Instance_Manager";
    descTree.m_strPrototypeObjectName = L"Prototype_GameObject_Tree";
    descTree.m_strPrototypeBaseName = L"Tree";
    descTree.pCameraType = ETOUI(CAMERA::NONE);
    descTree.fSpeedPerSec = 0.f;
    descTree.fRotationPerSec = 0.f;

    if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Tree"), CGameInstance::Get().Get_Level(), L"Instance", &descTree)))
    {
        return E_FAIL;
    }

    auto pTree = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"Instance", descTree.m_strName);
    static_pointer_cast<Tree>(pTree)->Load_Trees_JSON("../../Resources/Data/Instance/TreeData.json");


    {
        const uint32_t iCloudCount = 20;

        const wstring CloudModelNames[] =
        {
            L"Prototype_Com_Model_SM_Cloud_Polygon_Blender_1",
            L"Prototype_Com_Model_SM_Cloud_Polygon_Blender_1",
            L"Prototype_Com_Model_SM_Cloud_Polygon_Blender_1"
        };

        for (uint32_t i = 0; i < iCloudCount; ++i)
        {
            Cloud::CLOUD_DESC CloudDesc{};

            CloudDesc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_STATIC);

            CloudDesc.m_strName =
                L"Cloud_" + to_wstring(i);

            CloudDesc.m_strPrototypeObjectName =
                L"Prototype_GameObject_Cloud";

            int iModelIndex =
                rand() % 3;

            CloudDesc.m_strPrototypeBaseName =
                CloudModelNames[iModelIndex];

            CloudDesc.pCameraType =
                ETOUI(CAMERA::NONE);

            CloudDesc.fSpeedPerSec = 0.f;
            CloudDesc.fRotationPerSec = 0.f;

            float fX =
                CGameInstance::Get().Random(250.f, 480.f);

            float fY =
                CGameInstance::Get().Random(40.f,40.f);

            float fZ =
                CGameInstance::Get().Random(50.f, 320.f);

            CloudDesc.vSpawnPos =
            {
                fX,
                fY,
                fZ
            };

            CloudDesc.fMoveSpeed =
                CGameInstance::Get().Random(0.5f, 0.6f);

            CloudDesc.fResetDistance = CGameInstance::Get().Random(100.f, 150.f);

            CGameInstance::Get().Add_GameObject_toLayer(
                CGameInstance::Get().Get_Level(),
                TEXT("Prototype_GameObject_Cloud"),
                CGameInstance::Get().Get_Level(),
                TEXT("Sky"),
                &CloudDesc
            );
        }
    }


    CGameInstance::Get().PlaySoundLoop(L"BGM_PLAY", CHANNELID::BGM_HOME, 0.3f, 1.f);

    Shadow::SHADOW_LIGHT_DESC			ShadowLightDesc{};

    ShadowLightDesc.vEye = _float4(461.f, 200.f, 89.f, 1.f);
    ShadowLightDesc.vAt = _float4(419.f, 5.f, 149.f, 1.f);
    ShadowLightDesc.fFovy = XMConvertToRadians(60.f);
    ShadowLightDesc.fNear = 0.1f;
    ShadowLightDesc.fFar = 1000.f;

    if (FAILED(CGameInstance::Get().Add_Shadow_Light(ShadowLightDesc)))
        return E_FAIL;


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

    auto pCircleMask = dynamic_pointer_cast<GameObject>(
        CGameInstance::Get().Clone_Prototype(
            ETOUI(LEVEL::NEVER),
            TEXT("Prototype_GameObject_CircleMask_UI"),
            &MaskDesc
        )
    );

    if (pCircleMask == nullptr)
    {
        MSG_BOX("CircleMask_UI Clone Failed");
        return E_FAIL;
    }

    m_pUI.emplace("CircleMask_UI", pCircleMask);

    m_eFadeState = GAMEPLAY_FADE_STATE::OPENING;
    m_fMaskRadius = -0.05f;


	return S_OK;
}

void Level_GamePlay::Update(_float fTimeDelta)
{
    auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");
    if (static_pointer_cast<Player>(pPlayer)->m_bNext == true) {
        m_eFadeState = GAMEPLAY_FADE_STATE::CLOSING;
    }

      
    if (m_eFadeState == GAMEPLAY_FADE_STATE::OPENING)
    {
        m_fMaskRadius += m_fMaskSpeed * fTimeDelta;

        if (m_fMaskRadius >= m_fMaskMaxRadius)
        {
            m_fMaskRadius = m_fMaskMaxRadius;
            m_eFadeState = GAMEPLAY_FADE_STATE::OPENED;
        }
    }

    if (m_eFadeState == GAMEPLAY_FADE_STATE::CLOSING)
    {
        m_fMaskRadius -= m_fMaskSpeed * fTimeDelta;

        if (m_fMaskRadius <= -0.05f)
        {
            m_fMaskRadius = -0.05f;
            m_eFadeState = GAMEPLAY_FADE_STATE::CLOSED;
            auto pPlayerObj = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(),L"PlayerTag",L"Player" );

            if (pPlayerObj != nullptr)
            {
                auto pPlayer = dynamic_pointer_cast<Player>(pPlayerObj);

                if (pPlayer != nullptr)
                {
                    PLAYER_SAVE_DATA SaveData = pPlayer->Make_SaveData();

                    CGameInstance::Get().Save_PlayerData(SaveData);
                }
            }

            Ready_GamePlay2();
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

            // 완전히 열리면 안 보이게 하려면 이걸로 변경
            // pMask->Set_Visible(m_eFadeState != GAMEPLAY_FADE_STATE::OPENED);
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

        CGameInstance::Get().Add_UIObject(
            L"MainMenu_UI",
            static_pointer_cast<UIObject>(pUIObject)
        );
    }


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

HRESULT Level_GamePlay::Render()
{

	return S_OK;
}

HRESULT Level_GamePlay::Ready_GamePlay()
{
    return S_OK;
}

HRESULT Level_GamePlay::Ready_GamePlay2()
{
    CGameInstance::Get().Load_Lights_FromJson(L"../../Resources/Data/Light/Lights_Boss.json");
    CGameInstance::Get().Load("Stage_Boss", CGameInstance::Get().Get_Level());


    CGameInstance::Get().Change_Camera(ETOUI(CAMERA::MAIN));

    CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player"), CGameInstance::Get().Get_Level(), L"PlayerTag");

    auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");

    static _float3 vBossWarpPos = { 438.f, 37.f, 220 };
    static int iBossStartCell = 1;

    static_pointer_cast<Player>(pPlayer)->Change_Navigation_AndWarp(L"Prototype_Component_Navigation_Boss", vBossWarpPos, static_cast<uint32_t>(iBossStartCell));


    if (pPlayer != nullptr &&
        CGameInstance::Get().Has_PlayerSaveData())
    {
        static_pointer_cast<Player>(pPlayer)->Apply_SaveData(CGameInstance::Get().Get_PlayerSaveData());
    }

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

    GameObject::GAMEOBJECT_DESC desc{};

    desc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_MONSTER);

    desc.m_strName = L"BossStage2";

    desc.m_strPrototypeObjectName = L"Prototype_GameObject_BossMonster_Page2";

    desc.m_strPrototypeBaseName = L"SK_Boss_Test";

    desc.pCameraType = ETOUI(CAMERA::NONE);

    desc.fSpeedPerSec = 3.f;

    desc.fRotationPerSec = 180.f;

    CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster_Page2"), CGameInstance::Get().Get_Level(), L"Boss", &desc);

 

    m_eFadeState = GAMEPLAY_FADE_STATE::OPENING;
    m_fMaskRadius = -0.05f;
    return S_OK;
}

unique_ptr<Level_GamePlay> Level_GamePlay::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Level_GamePlay>(new Level_GamePlay(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : Level_GamePlay"), nullptr, MB_OK);

	return pInstance;
}
wstring Level_GamePlay::Make_UniqueObjectName(const wstring& LayerName, const wstring& baseName)
{
    wstring finalName = baseName;

    if (finalName.empty())
        finalName = L"Object";

    if (!CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), LayerName, finalName))
        return finalName;

    for (uint32_t i = 1; i < 10000; ++i)
    {
        wchar_t szNumber[32] = {};
        swprintf_s(szNumber, L"_%03d", i);

        wstring newName = finalName + szNumber;

        if (!CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), LayerName, newName))
            return newName;
    }

    return finalName;
}