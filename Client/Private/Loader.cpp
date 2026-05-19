#include "Loader.h"
#include "GameInstance.h"
#include "Monster.h"
#include "PerspectiveCamera.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"
#include "Terrain.h"
#include "Sky.h"
#include "Obstacle.h"
#include "Body_Player.h"
#include "Player.h"
#include "PlayerCamera.h"
CLoader::CLoader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
}

CLoader::~CLoader()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);
}

uint32_t APIENTRY ThreadMain(void* pArg)
{
	auto		pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelIndex)
{
	InitializeCriticalSection(&m_CriticalSection);

	m_eNextLevelIndex = eNextLevelIndex;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	HRESULT		hr = {};

	switch (m_eNextLevelIndex)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;

	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;

	case LEVEL::MAPEDITOR:
		hr = Loading_For_MapEditor();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	CoUninitialize();

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}


HRESULT CLoader::Loading_For_Logo()
{
	CGameInstance::Get().Set_Level(ETOUI(LEVEL::LOGO));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	CGameInstance::Get().Set_Level(ETOUI(LEVEL::GAMEPLAY));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MapEditor()
{
	CGameInstance::Get().Set_Level(ETOUI(LEVEL::MAPEDITOR));
#pragma region Texture Component Prototype

		/* For.Prototype_Component_Texture_Sky */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Sky"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
			return E_FAIL;

		

#pragma endregion

#pragma region Shader Component Prototype

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_FBX_Tex"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_Fbx.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_AnimFbx"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_AnimFbx.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_Terrian"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_Terrian.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_VtxCube"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region Model Component Prototype

	_matrix		PreTransformMatrix = XMMatrixIdentity();


	//if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Model_Duck"),
	//	Model::Create(m_pDevice, m_pContext, ETOUI(LEVEL::MAPEDITOR), L"DUCK_NPC",ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_Monster_Palicus/SK_Monster_Palicus.bin", PreTransformMatrix))))
	//	return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SK_Monster_Palicus"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"MONSTER_DUCK",ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_Monster_Palicus/SK_Monster_Palicus.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SK_CharacterModel_Duck_Jeff"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"MONSTER_DUCK1", ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_CharacterModel_Duck_Jeff/SK_CharacterModel_Duck_Jeff.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SK_Fiona"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"MONSTER_DUCK2", ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_Fiona/SK_Fiona.bin", PreTransformMatrix))))
		return E_FAIL;


	// static Mesh
	_matrix HomeTransformMatrix =
		XMMatrixScaling(0.001f, 0.001f, 0.001f) *
		XMMatrixTranslation(0.f, 0.f, -6.5f);
   	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SM_MeshV2"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Home", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/SM_MeshV2/SM_MeshV2.bin", HomeTransformMatrix))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SM_Indoors_Study_Shelf_04"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Home", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/SM_Indoors_Study_Shelf_04/SM_Indoors_Study_Shelf_04.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SM_ForkLift"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Home", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/SM_ForkLift/SM_ForkLift.bin", PreTransformMatrix))))
		return E_FAIL;




	// player Mesh
	_matrix PlayerTransformMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SK_Player"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Player", ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_Player/SK_Player.bin", PlayerTransformMatrix))))
		return E_FAIL;
#pragma endregion



#pragma region VIBuffer Component Prototype
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_VIBuffer_Terrain"),
		VIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Cube"),
		VIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Collider Component Prototype
	///* Prototype_GameObject_TestModel */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_AABB_Collider"),
		AABB_Collider::Create(m_pDevice, m_pContext)))){
		return E_FAIL;
	}

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_OBB_Collider"),
		OBB_Collider::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


#pragma endregion


#pragma region GameObject Prototype
	/* Prototype_GameObject_TestModel */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Monster"),
		Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_PerspectiveCamera */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PerspectiveCamera"),
		PerspectiveCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Prototype_GameObject_PlayerCamera */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PlayerCamera"),
		PlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Terrain"),
		Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Sky"),
		Sky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Obstacle"),
		Obstacle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Body_Player"),
		Body_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player"),
		Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion




	m_isFinished = true;

	return S_OK;
}


unique_ptr<CLoader> CLoader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex)
{
	auto	pInstance = unique_ptr<CLoader>(new CLoader(pDevice, pContext));

	if (FAILED(pInstance->Initialize(eNextLevelIndex)))
		MSG_BOX("Failed to Created : CLoader");

	return pInstance;
}
