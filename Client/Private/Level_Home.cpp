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





	return S_OK;
}

void Level_Home::Update(_float fTimeDelta)
{

	for (auto& Pair : m_pUI)
	{
		const string& strUIName = Pair.first;
		auto pUIObject = Pair.second;

		if (nullptr == pUIObject)
			continue;


		CGameInstance::Get().Add_UIObject(L"MainMenu_UI", static_pointer_cast<UIObject>(pUIObject));
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
