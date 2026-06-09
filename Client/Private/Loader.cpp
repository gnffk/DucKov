#include "Loader.h"
#include "GameInstance.h"
#include "Monster.h"
#include "PerspectiveCamera.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"
#include "Sphere_Collider.h"
#include "Terrain.h"
#include "Sky.h"
#include "Obstacle.h"
#include "Body_Player.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Tree.h"
#include "MainUI.h"
#include "InvenUI.h"
#include "Player_Weapon.h"
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

		/* For.Prototype_Com_Texture_Terrain_Grass */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Terrain_Grass"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrian/T_Tile_Grass.dds"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Terrain_Mud */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Terrain_Mud"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrian/T_Tile_DryMud.dds"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Terrain_Road */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Terrain_Road"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrian/T_Tile_AsphaltRoad_01_C.dds"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Terrain_Splat */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Terrain_Splat"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrian/Terrain_Splat.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Icon */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Icon"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/UI_BaseRect_Toggle.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Number */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Number"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_Main_%d.png"), 9))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_V */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_V"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_Main_v.png"), 9))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Graph */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Graph"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_Main_Slider01.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Heart */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Heart"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_Main_heart.png"), 1))))
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
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_Terrian.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_VtxCube"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_Vtx_Instance_Tex"),
		Shader::Create(m_pDevice,m_pContext,TEXT("../../Resources/Shaders/Shader_Vtx_Instance_Tex.hlsl"),VTXMESHINSTANCE::Elements,VTXMESHINSTANCE::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_VtxPosTex"),
		Shader::Create(m_pDevice,m_pContext,TEXT("../../Resources/Shaders/Shader_VtxPosTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;
#pragma endregion

#pragma region Model Component Prototype

	_matrix		PreTransformMatrix = XMMatrixIdentity();



	_matrix CustomMatrix =
	XMMatrixScaling(0.01f, 0.01f, 0.01f) *
	XMMatrixRotationY(XMConvertToRadians(180.f));

 	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SK_CustomBody"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Custom_Duck", ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_CustomBody/SK_CustomBody.bin", CustomMatrix))))
		return E_FAIL;

	// static Mesh
	_matrix HomeTransformMatrix =
		XMMatrixScaling(0.001f, 0.001f, 0.001f) *
		XMMatrixTranslation(0.f, 0.f, -6.5f);
  // 	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SM_MeshV2"),
		//Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Home", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/Home/SM_MeshV2/SM_MeshV2.bin", HomeTransformMatrix))))
		//return E_FAIL;



	namespace fs = std::filesystem;

	//const fs::path basePath = "../../Resources/Model/StaticMesh/Home/";

	//for (const auto& dirEntry : fs::directory_iterator(basePath))
	//{
	//	if (!dirEntry.is_directory())
	//		continue;

	//	const fs::path folderPath = dirEntry.path();

	//	// 폴더 이름
	//	std::wstring folderName = folderPath.filename().wstring();

	//	// bin 파일 경로 자동 생성
	//	fs::path binPath = folderPath / (folderName + L".bin");

	//	if (!fs::exists(binPath))
	//		continue;

	//	// Prototype 이름 자동 생성
	//	std::wstring prototypeTag =
	//		L"Prototype_Com_Model_" + folderName;

	//	// Home만 다른 Transform 사용
	//	_matrix transformMatrix =
	//		(folderName == L"SM_MeshV2")
	//		? HomeTransformMatrix
	//		: PreTransformMatrix;

	//	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),prototypeTag.c_str(),
	//		Model::Create(m_pDevice,m_pContext,CGameInstance::Get().Get_Level(),folderName.c_str(),ETOUI(MODELTYPE::NONANIM),binPath.string().c_str(),transformMatrix))))
	//	{
	//		return E_FAIL;
	//	}
	//}



	const fs::path basePath1 = "../../Resources/Model/StaticMesh/Stage1/Anim/";

	for (const auto& dirEntry : fs::directory_iterator(basePath1))
	{
		if (!dirEntry.is_directory())
			continue;

		const fs::path folderPath = dirEntry.path();

		// 폴더 이름
		std::wstring folderName = folderPath.filename().wstring();

		// bin 파일 경로 자동 생성
		fs::path binPath = folderPath / (folderName + L".bin");

		if (!fs::exists(binPath))
			continue;

		// Prototype 이름 자동 생성
		std::wstring prototypeTag =
			L"Prototype_Com_Model_" + folderName;

  		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), prototypeTag.c_str(), 
			Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), folderName.c_str(), ETOUI(MODELTYPE::ANIM), binPath.string().c_str(), PreTransformMatrix))))
		{
			return E_FAIL;
		}
	}


	const fs::path basePath2 = "../../Resources/Model/StaticMesh/Stage1/NonAnim/";

	for (const auto& dirEntry : fs::directory_iterator(basePath2))
	{
		if (!dirEntry.is_directory())
			continue;

		const fs::path folderPath = dirEntry.path();

		// 폴더 이름
		std::wstring folderName = folderPath.filename().wstring();

		// bin 파일 경로 자동 생성
		fs::path binPath = folderPath / (folderName + L".bin");

		if (!fs::exists(binPath))
			continue;

		// Prototype 이름 자동 생성
		std::wstring prototypeTag =
			L"Prototype_Com_Model_" + folderName;

		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), prototypeTag.c_str(),
			Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), folderName.c_str(), ETOUI(MODELTYPE::NONANIM), binPath.string().c_str(), PreTransformMatrix))))
		{
			return E_FAIL;
		}
	}

	_matrix Treematrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Tree",
		InstanceModel::Create(m_pDevice,m_pContext,CGameInstance::Get().Get_Level(), L"Tree",ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/Stage1/NonAnim/SM_Tree/SM_Tree.bin", Treematrix))))
	{
		return E_FAIL;
	}

	_matrix GunMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun2",
		Model::Create(m_pDevice,m_pContext,CGameInstance::Get().Get_Level(), L"Gun2",ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun2/SM_Gun2.bin", GunMatrix))))
	{
		return E_FAIL;
	}


#pragma endregion



#pragma region VIBuffer Component Prototype
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_VIBuffer_Terrain"),
		VIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/terrain.raw")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Cube"),
		VIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Com_VIBuffer_Rect */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Rect"),
		VIBuffer_Rect::Create(m_pDevice, m_pContext))))
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
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Sphere_Collider"),
		Sphere_Collider::Create(m_pDevice, m_pContext)))) {
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

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Tree"),
		Tree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_MainUI"),
		MainUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_EvenUI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_EvenUI"),
		InvenUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Weapon */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Weapon"),
		Player_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;



#pragma endregion

#pragma region Navigation
	/* For.Prototype_Component_Navigation */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Navigation"),
		Navigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/DataFiles/Navigation.dat"), TEXT("../../Resources/DataFiles/Navigation_Neighbors.dat")))))
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
