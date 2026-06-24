
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
