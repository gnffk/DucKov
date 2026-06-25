
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


    Shadow::SHADOW_LIGHT_DESC			ShadowLightDesc{};

    ShadowLightDesc.vEye = _float4(461.f, 200.f, 89.f, 1.f);
    ShadowLightDesc.vAt = _float4(419.f, 5.f, 149.f, 1.f);
    ShadowLightDesc.fFovy = XMConvertToRadians(60.f);
    ShadowLightDesc.fNear = 0.1f;
    ShadowLightDesc.fFar = 1000.f;

    if (FAILED(CGameInstance::Get().Add_Shadow_Light(ShadowLightDesc)))
        return E_FAIL;

	return S_OK;
}

void Level_GamePlay::Update(_float fTimeDelta)
{

}

HRESULT Level_GamePlay::Render()
{

	return S_OK;
}

HRESULT Level_GamePlay::Ready_GamePlay()
{
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