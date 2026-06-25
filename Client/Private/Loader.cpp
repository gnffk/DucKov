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
#include "Bullet.h"
#include "Player_Mouse.h"
#include "BossMonster.h"
#include "Boss_Weapon.h"
#include "Block.h"
#include "InteractBox.h"
#include "FindUI.h"
#include "Player_State_UI.h"
#include "LittleMonster.h"
#include "LittleMonster_Weapon.h"
#include "LittleMonster_StateUI.h"
#include "Particle_Blood.h"
#include "Player_Armor.h"
#include "Player_Helmat.h"
#include "Boss_State_UI.h"
#include "PerformaceCamera.h"
#include "BossMonster_Page2.h"
#include "LaserTrail.h"
#include "BossPage2UI.h"
#include "MainMenu_UI.h"
#include "Loading_BackGround_UI.h"
#include "Portal.h"
#include "PortalUI.h"
#include "Cloud.h"
CLoader::CLoader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext): m_pDevice{ pDevice }
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
	case LEVEL::HOME:
		hr = Loading_For_Home();
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

	//---- Shader Prototype ----//

	/* For.Prototype_Com_Shader_VtxPosTex */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_VtxPosTex"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_VtxPosTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	//---- VIBuffer Prototype ----//

	/* For.Prototype_Com_VIBuffer_Rect */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Rect"),
		VIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//---- Texture Prototype ----//

	/* For.Prototype_Com_Texture_MainMenu_BackGround */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_MainMenu_BackGround"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainMenu.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Texture_MainMenu_Logo */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_MainMenu_Logo"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseRect0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_BaseRectCustom1 */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectCustom1"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseRect1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_BaseRectCustom */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectCustom"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseRect0.png"), 1))))
		return E_FAIL;



	//---- GameObject Prototype ----//

	// For.Prototype_GameObject_MainMenu_UI
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_GameObject_MainMenu_UI"), MainMenu_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Home()
{
	CGameInstance::Get().Set_Level(ETOUI(LEVEL::HOME));
#pragma region Texture Component Prototype

	/* For.Prototype_GameObject_Portal*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Portal"),
		Portal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PortalUI*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PortalUI"),
		PortalUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Sky"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Sky_Home */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Sky_Home"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/SkyBox/Sky_Make.dds"), 1))))
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
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_BaseRect2.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Heart */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Heart"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_Main_heart.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_MouseMarker */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_MouseMarker"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/point_scan.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Texture_Inven_Gun */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Gun"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Gun.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Inven_Clothes */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Clothes"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Clothes.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Inven_Head */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Head"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Head.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Inven_Swords */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Swords"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Swords.png"), 1))))
		return E_FAIL;





	/* For.Prototype_Com_Texture_UI_BaseRectCustom */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectCustom"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseRect0.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Texture_UI_BaseRectCustom1 */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectCustom1"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseRect1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_BaseRectItemRect */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseItemRect.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Texture_UI_Item_Gun1*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Gun1"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Pfb_WPM_Glock_02.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Texture_UI_Item_Gun2*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Gun2"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/MachineGun01.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Gun3*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Gun3"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Pfb_WPN_SonicFist_P.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Stick*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Stick"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/WoodStick.png"), 1))))
		return E_FAIL;



	/* For.Prototype_Com_Texture_UI_Item_Armor0*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Armor0"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Clothes_LV1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Armor1*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Armor1"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Clothes_LV3.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Armor2*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Armor2"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Clothes_LV5_3.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Helmat0*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Helmat0"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/hat_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Helmat1*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Helmat1"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Hair_Hat_LV1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Helmat2*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Helmat2"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Hair_Hat_LV6.png"), 1))))
		return E_FAIL;



	/* For.Prototype_Com_Texture_UI_Item_Gun2*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_BulletTrail"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Bullet/BulletTrail.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Blood*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Blood"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Blood.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Noise*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Noise"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Noise.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Mask*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Mask"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Mask.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_BloodN*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_BloodN"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Blood_N.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_LaserTrail*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_LaserTrail"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Laser/LaserTrail.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_BossPage2_Name*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_BossPage2_Name"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/BossPage2_Name.png"), 1))))
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

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_Instance_Tex"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_Instance_Tex.hlsl"), VTXMESHINSTANCE::Elements, VTXMESHINSTANCE::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_VtxPosTex"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_VtxPosTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Obstacle"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Obstacle.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_MainUI"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_MainUI.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_BulletTrail"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_BulletTrail.hlsl"), VTXBULLETTRAIL::Elements, VTXBULLETTRAIL::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Blood_Particle_Rect"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Blood_Particle.hlsl"), VTXINSTANCE_PARTICLE_RECT::Elements, VTXINSTANCE_PARTICLE_RECT::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_BossMonster"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_BossMonster.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_BossPage2"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_BossPage2.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_LaserTrail"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Laser.hlsl"), VTXBULLETTRAIL::Elements, VTXBULLETTRAIL::iNumElements))))
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



	namespace fs = std::filesystem;

	const fs::path basePath = "../../Resources/Model/StaticMesh/Home/NonAnim/";

	for (const auto& dirEntry : fs::directory_iterator(basePath))
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

		// Home만 다른 Transform 사용
		_matrix transformMatrix =
			(folderName == L"SM_MeshV2")
			? HomeTransformMatrix
			: PreTransformMatrix;

		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),prototypeTag.c_str(),
			Model::Create(m_pDevice,m_pContext,CGameInstance::Get().Get_Level(),folderName.c_str(),ETOUI(MODELTYPE::NONANIM),binPath.string().c_str(),transformMatrix))))
		{
			return E_FAIL;
		}
	}

	_matrix GunMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun1",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Gun1", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun1/SM_Gun1.bin", GunMatrix))))
	{
		return E_FAIL;
	}

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun2",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Gun2", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun2/SM_Gun2.bin", GunMatrix))))
	{
		return E_FAIL;
	}

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun3",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Gun3", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun3/SM_Gun3.bin", GunMatrix))))
	{
		return E_FAIL;
	}

	_matrix LittleMonsterWeaponMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_LittleMonsterWeapon",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"LittleMonsterWeapon", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_LittleMonsterWeapon/SM_LittleMonsterWeapon.bin", LittleMonsterWeaponMatrix))))
	{
		return E_FAIL;
	}
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Stick",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Stick", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Stick/SM_Stick.bin", LittleMonsterWeaponMatrix))))
	{
		return E_FAIL;
	}
	_matrix BulletMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Bullet",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Bullet", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Bullet/SM_Bullet.bin", BulletMatrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerArmor0Matrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Armor0",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Armor0", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Armor0/SM_Armor0.bin", PlayerArmor0Matrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerArmor1Matrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Armor1",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Armor1", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Armor1/SM_Armor1.bin", PlayerArmor1Matrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerArmor2Matrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Armor2",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Armor2", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Armor2/SM_Armor2.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerHelmet =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Helmat0",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Helmat0", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Helmat0/SM_Helmat0.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Helmat1",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Helmat1", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Helmet1/SM_Helmet1.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Helmat2",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Helmat2", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Helmet2/SM_Helmet2.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}


#pragma endregion
#pragma region VIBuffer Component Prototype

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
		AABB_Collider::Create(m_pDevice, m_pContext)))) {
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

	/* Prototype_GameObject_PerspectiveCamera */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PerspectiveCamera"),
		PerspectiveCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Prototype_GameObject_PlayerCamera */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PlayerCamera"),
		PlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Sky"),
		Sky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Sky_Home"),
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

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_MainUI"),
		MainUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InvenUI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_InvenUI"),
		InvenUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Weapon */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Weapon"),
		Player_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Bullet */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Bullet"),
		Bullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Mouse */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Mouse"),
		Player_Mouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_State_UI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_State_UI"),
		Player_State_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Armor */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Armor"),
		Player_Armor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Helmat */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Helmat"),
		Player_Helmat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossPerformanceCamera */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossPerformanceCamera"),
		PerformaceCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Navigation

	/* For.Prototype_Component_Navigation_Home */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Navigation_Home"),
		Navigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/DataFiles/Navigation_Home.dat"), TEXT("../../Resources/DataFiles/Navigation_Neighbors_Home.dat")))))
		return E_FAIL;
#pragma endregion


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	CGameInstance::Get().Set_Level(ETOUI(LEVEL::GAMEPLAY));

#pragma region Texture Component Prototype
	/* For.Prototype_Com_Texture_Sky_Home */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Sky_Home"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/SkyBox/Sky_Make.dds"), 1))))
		return E_FAIL;

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
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_BaseRect2.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Heart */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Heart"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_Main_heart.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_MouseMarker */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_MouseMarker"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/point_scan.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Texture_Inven_Gun */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Gun"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Gun.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Inven_Clothes */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Clothes"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Clothes.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Inven_Head */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Head"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Head.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Inven_Swords */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Swords"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Swords.png"), 1))))
		return E_FAIL;





	/* For.Prototype_Com_Texture_UI_BaseRectCustom */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectCustom"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseRect0.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Texture_UI_BaseRectCustom1 */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectCustom1"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseRect1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_BaseRectItemRect */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseItemRect.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Texture_UI_Item_Gun1*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Gun1"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Pfb_WPM_Glock_02.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Texture_UI_Item_Gun2*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Gun2"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/MachineGun01.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Gun3*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Gun3"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Pfb_WPN_SonicFist_P.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Stick*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Stick"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/WoodStick.png"), 1))))
		return E_FAIL;



	/* For.Prototype_Com_Texture_UI_Item_Armor0*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Armor0"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Clothes_LV1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Armor1*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Armor1"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Clothes_LV3.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Armor2*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Armor2"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Clothes_LV5_3.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Helmat0*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Helmat0"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/hat_0.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Helmat1*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Helmat1"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Hair_Hat_LV1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_UI_Item_Helmat2*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Helmat2"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Hair_Hat_LV6.png"), 1))))
		return E_FAIL;



	/* For.Prototype_Com_Texture_UI_Item_Gun2*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_BulletTrail"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Bullet/BulletTrail.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Blood*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Blood"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Blood.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Noise*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Noise"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Noise.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Mask*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Mask"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Mask.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_BloodN*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_BloodN"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Blood_N.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_LaserTrail*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_LaserTrail"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Laser/LaserTrail.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_BossPage2_Name*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_BossPage2_Name"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/BossPage2_Name.png"), 1))))
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

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_Instance_Tex"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_Instance_Tex.hlsl"), VTXMESHINSTANCE::Elements, VTXMESHINSTANCE::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_VtxPosTex"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_VtxPosTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Obstacle"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Obstacle.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_MainUI"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_MainUI.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_BulletTrail"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_BulletTrail.hlsl"), VTXBULLETTRAIL::Elements, VTXBULLETTRAIL::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Blood_Particle_Rect"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Blood_Particle.hlsl"), VTXINSTANCE_PARTICLE_RECT::Elements, VTXINSTANCE_PARTICLE_RECT::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_BossMonster"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_BossMonster.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_BossPage2"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_BossPage2.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_LaserTrail"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Laser.hlsl"), VTXBULLETTRAIL::Elements, VTXBULLETTRAIL::iNumElements))))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Cloud"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Cloud.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
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

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SK_Boss_Test"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Custom_Duck_Boss", ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_Boss_Test/SK_Boss_Test.bin", CustomMatrix))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SK_LittleMonster"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Custom_Duck_LittleMonster", ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_LittleMonster/SK_LittleMonster.bin", CustomMatrix))))
		return E_FAIL;

	// static Mesh
	_matrix HomeTransformMatrix =
		XMMatrixScaling(0.001f, 0.001f, 0.001f) *
		XMMatrixTranslation(0.f, 0.f, -6.5f);
	// 	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SM_MeshV2"),
		  //Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Home", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/Home/SM_MeshV2/SM_MeshV2.bin", HomeTransformMatrix))))
		  //return E_FAIL;



	namespace fs = std::filesystem;


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
		InstanceModel::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Tree", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/Stage1/NonAnim/SM_Tree/SM_Tree.bin", Treematrix))))
	{
		return E_FAIL;
	}

	_matrix GunMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun1",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Gun1", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun1/SM_Gun1.bin", GunMatrix))))
	{
		return E_FAIL;
	}

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun2",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Gun2", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun2/SM_Gun2.bin", GunMatrix))))
	{
		return E_FAIL;
	}

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun3",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Gun3", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun3/SM_Gun3.bin", GunMatrix))))
	{
		return E_FAIL;
	}

	_matrix LittleMonsterWeaponMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_LittleMonsterWeapon",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"LittleMonsterWeapon", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_LittleMonsterWeapon/SM_LittleMonsterWeapon.bin", LittleMonsterWeaponMatrix))))
	{
		return E_FAIL;
	}
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Stick",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Stick", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Stick/SM_Stick.bin", LittleMonsterWeaponMatrix))))
	{
		return E_FAIL;
	}
	_matrix BulletMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Bullet",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Bullet", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Bullet/SM_Bullet.bin", BulletMatrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerArmor0Matrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Armor0",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Armor0", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Armor0/SM_Armor0.bin", PlayerArmor0Matrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerArmor1Matrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Armor1",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Armor1", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Armor1/SM_Armor1.bin", PlayerArmor1Matrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerArmor2Matrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Armor2",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Armor2", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Armor2/SM_Armor2.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerHelmet =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Helmat0",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Helmat0", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Helmat0/SM_Helmat0.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Helmat1",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Helmat1", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Helmet1/SM_Helmet1.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Helmat2",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Helmat2", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Helmet2/SM_Helmet2.bin", PlayerArmor2Matrix))))
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


	/* For.Prototype_Com_VIBuffer_Particle_Blood */
	VIBuffer_Particle_Rect::PARTICLE_INSTANCE_DESC		BloodDesc{};
	BloodDesc.iNumInstances = 200;
	BloodDesc.vCenter = _float3(0.f, 0.f, 0.f);
	BloodDesc.vRange = _float3(0.f, 0.f, 0.f);
	BloodDesc.vSize = _float2(10.f, 12.f);
	BloodDesc.vSpeed = _float2(1.f, 4.f);
	BloodDesc.vLifeTime = _float2(2.f, 4.f);
	BloodDesc.isLoop = false;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Particle_Blood"),
		VIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, &BloodDesc))))
		return E_FAIL;

#pragma endregion

#pragma region Collider Component Prototype
	///* Prototype_GameObject_TestModel */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_AABB_Collider"),
		AABB_Collider::Create(m_pDevice, m_pContext)))) {
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
	//if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Monster"),
	//	Monster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

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

	/* For.Prototype_GameObject_InvenUI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_InvenUI"),
		InvenUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Weapon */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Weapon"),
		Player_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossPlayer_Weapon */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossPlayer_Weapon"),
		Boss_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bullet */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Bullet"),
		Bullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player_Mouse */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Mouse"),
		Player_Mouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossMonster */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster"),
		BossMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Block */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Block"),
		Block::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InteractBox */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_InteractBox"),
		InteractBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FindUI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_FindUI"),
		FindUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player_State_UI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_State_UI"),
		Player_State_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LittleMonster_Weapon */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LittleMonster_Weapon"),
		LittleMonster_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LittleMonster */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LittleMonster"),
		LittleMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LittleMonster_StateUI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LittleMonster_StateUI"),
		LittleMonster_StateUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_Blood */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Particle_Blood"),
		Particle_Blood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Armor */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Armor"),
		Player_Armor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Helmat */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Helmat"),
		Player_Helmat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossMonster_State_UI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster_State_UI"),
		Boss_State_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossPerformanceCamera */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossPerformanceCamera"),
		PerformaceCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossMonster_Page2 */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster_Page2"),
		BossMonster_Page2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LaserTrail*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LaserTrail"),
		LaserTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossMonster_Page2_State_UI*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster_Page2_State_UI"),
		BossPage2UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Portal*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Portal"),
		Portal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PortalUI*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PortalUI"),
		PortalUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Cloud*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Cloud"),
		Cloud::Create(m_pDevice, m_pContext))))
		return E_FAIL;




#pragma endregion

#pragma region Navigation
	/* For.Prototype_Component_Navigation */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Navigation"),
		Navigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/DataFiles/Navigation.dat"), TEXT("../../Resources/DataFiles/Navigation_Neighbors.dat")))))
		return E_FAIL;

	/* For.Prototype_Component_Navigation_Boss */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Navigation_Boss"),
		Navigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/DataFiles/Navigation_Boss.dat"), TEXT("../../Resources/DataFiles/Navigation_Neighbors_Boss.dat")))))
		return E_FAIL;
#pragma endregion


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MapEditor()
{
	CGameInstance::Get().Set_Level(ETOUI(LEVEL::MAPEDITOR));
#pragma region Texture Component Prototype
		/* For.Prototype_Com_Texture_Sky_Home */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Sky_Home"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/SkyBox/Sky_Make.dds"), 1))))
			return E_FAIL;

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
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_BaseRect2.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Heart */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Heart"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/MainUI/UI_Main_heart.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_MouseMarker */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_MouseMarker"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/point_scan.png"), 1))))
			return E_FAIL;


		/* For.Prototype_Com_Texture_Inven_Gun */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Gun"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Gun.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Inven_Clothes */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Clothes"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Clothes.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Inven_Head */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Head"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Head.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Inven_Swords */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Inven_Swords"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_Inven_Swords.png"), 1))))
			return E_FAIL;





		/* For.Prototype_Com_Texture_UI_BaseRectCustom */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectCustom"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseRect0.png"), 1))))
			return E_FAIL;


		/* For.Prototype_Com_Texture_UI_BaseRectCustom1 */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectCustom1"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseRect1.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_UI_BaseRectItemRect */
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inven/UI_BaseItemRect.png"), 1))))
			return E_FAIL;


		/* For.Prototype_Com_Texture_UI_Item_Gun1*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Gun1"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Pfb_WPM_Glock_02.png"), 1))))
			return E_FAIL;


		/* For.Prototype_Com_Texture_UI_Item_Gun2*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Gun2"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/MachineGun01.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_UI_Item_Gun3*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Gun3"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Pfb_WPN_SonicFist_P.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_UI_Item_Stick*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Stick"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/WoodStick.png"), 1))))
			return E_FAIL;



		/* For.Prototype_Com_Texture_UI_Item_Armor0*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Armor0"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Clothes_LV1.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_UI_Item_Armor1*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Armor1"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Clothes_LV3.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_UI_Item_Armor2*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Armor2"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Clothes_LV5_3.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_UI_Item_Helmat0*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Helmat0"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/hat_0.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_UI_Item_Helmat1*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Helmat1"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Hair_Hat_LV1.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_UI_Item_Helmat2*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_UI_Item_Helmat2"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Item/Hair_Hat_LV6.png"), 1))))
			return E_FAIL;



		/* For.Prototype_Com_Texture_UI_Item_Gun2*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_BulletTrail"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Bullet/BulletTrail.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Blood*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Blood"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Blood.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Noise*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Noise"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Noise.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_Mask*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_Mask"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Mask.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_BloodN*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_BloodN"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Blood/Blood_N.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_LaserTrail*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_LaserTrail"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effect/Laser/LaserTrail.png"), 1))))
			return E_FAIL;

		/* For.Prototype_Com_Texture_BossPage2_Name*/
		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_BossPage2_Name"),
			Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/BossPage2_Name.png"), 1))))
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

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_Obstacle"),
		Shader::Create(m_pDevice,m_pContext,TEXT("../../Resources/Shaders/Shader_Obstacle.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_MainUI"),
		Shader::Create(m_pDevice,m_pContext,TEXT("../../Resources/Shaders/Shader_MainUI.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_BulletTrail"),
		Shader::Create(m_pDevice,m_pContext,TEXT("../../Resources/Shaders/Shader_BulletTrail.hlsl"), VTXBULLETTRAIL::Elements, VTXBULLETTRAIL::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_Blood_Particle_Rect"),
		Shader::Create(m_pDevice,m_pContext,TEXT("../../Resources/Shaders/Shader_Blood_Particle.hlsl"), VTXINSTANCE_PARTICLE_RECT::Elements, VTXINSTANCE_PARTICLE_RECT::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_BossMonster"),
		Shader::Create(m_pDevice,m_pContext,TEXT("../../Resources/Shaders/Shader_Vtx_BossMonster.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_BossPage2"),
		Shader::Create(m_pDevice,m_pContext,TEXT("../../Resources/Shaders/Shader_Vtx_BossPage2.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_LaserTrail"),
		Shader::Create(m_pDevice,m_pContext,TEXT("../../Resources/Shaders/Shader_Laser.hlsl"), VTXBULLETTRAIL::Elements, VTXBULLETTRAIL::iNumElements))))
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

  	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SK_Boss_Test"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Custom_Duck_Boss", ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_Boss_Test/SK_Boss_Test.bin", CustomMatrix))))
		return E_FAIL;

  	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SK_LittleMonster"),
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Custom_Duck_LittleMonster", ETOUI(MODELTYPE::ANIM), "../../Resources/Model/Skeleton/SK_LittleMonster/SK_LittleMonster.bin", CustomMatrix))))
		return E_FAIL;

	// static Mesh
	_matrix HomeTransformMatrix =
		XMMatrixScaling(0.001f, 0.001f, 0.001f) *
		XMMatrixTranslation(0.f, 0.f, -6.5f);
  // 	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_SM_MeshV2"),
		//Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Home", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/Home/SM_MeshV2/SM_MeshV2.bin", HomeTransformMatrix))))
		//return E_FAIL;



	namespace fs = std::filesystem;

	const fs::path basePath = "../../Resources/Model/StaticMesh/Home/NonAnim/";

	for (const auto& dirEntry : fs::directory_iterator(basePath))
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

		// Home만 다른 Transform 사용
		_matrix transformMatrix =
			(folderName == L"SM_MeshV2")
			? HomeTransformMatrix
			: PreTransformMatrix;

		if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(),prototypeTag.c_str(),
			Model::Create(m_pDevice,m_pContext,CGameInstance::Get().Get_Level(),folderName.c_str(),ETOUI(MODELTYPE::NONANIM),binPath.string().c_str(),transformMatrix))))
		{
			return E_FAIL;
		}
	}



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
		std::wstring prototypeTag = L"Prototype_Com_Model_" + folderName;

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
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun1",
		Model::Create(m_pDevice,m_pContext,CGameInstance::Get().Get_Level(), L"Gun1",ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun1/SM_Gun1.bin", GunMatrix))))
	{
		return E_FAIL;
	}

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun2",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Gun2", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun2/SM_Gun2.bin", GunMatrix))))
	{
		return E_FAIL;
	}

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Gun3",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Gun3", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Gun3/SM_Gun3.bin", GunMatrix))))
	{
		return E_FAIL;
	}

	_matrix LittleMonsterWeaponMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_LittleMonsterWeapon",
		Model::Create(m_pDevice,m_pContext,CGameInstance::Get().Get_Level(), L"LittleMonsterWeapon",ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_LittleMonsterWeapon/SM_LittleMonsterWeapon.bin", LittleMonsterWeaponMatrix))))
	{
		return E_FAIL;
	}
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Stick",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Stick", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Stick/SM_Stick.bin", LittleMonsterWeaponMatrix))))
	{
		return E_FAIL;
	}
	_matrix BulletMatrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Bullet",
		Model::Create(m_pDevice,m_pContext,CGameInstance::Get().Get_Level(), L"Bullet",ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Bullet/SM_Bullet.bin", BulletMatrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerArmor0Matrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Armor0",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Armor0", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Armor0/SM_Armor0.bin", PlayerArmor0Matrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerArmor1Matrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Armor1",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Armor1", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Armor1/SM_Armor1.bin", PlayerArmor1Matrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerArmor2Matrix =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Armor2",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Armor2", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Armor2/SM_Armor2.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}

	_matrix PlayerHelmet =
		XMMatrixScaling(0.01f, 0.01f, 0.01f) *
		XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Helmat0",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Helmat0", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Helmat0/SM_Helmat0.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Helmat1",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Helmat1", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Helmet1/SM_Helmet1.bin", PlayerArmor2Matrix))))
	{
		return E_FAIL;
	}
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), L"Prototype_Com_Model_Helmat2",
		Model::Create(m_pDevice, m_pContext, CGameInstance::Get().Get_Level(), L"Helmat2", ETOUI(MODELTYPE::NONANIM), "../../Resources/Model/StaticMesh/NonAnim/SM_Helmet2/SM_Helmet2.bin", PlayerArmor2Matrix))))
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


	/* For.Prototype_Com_VIBuffer_Particle_Blood */
	VIBuffer_Particle_Rect::PARTICLE_INSTANCE_DESC		BloodDesc{};
	BloodDesc.iNumInstances = 200;
	BloodDesc.vCenter = _float3(0.f, 0.f, 0.f);
	BloodDesc.vRange = _float3(0.f, 0.f, 0.f);
	BloodDesc.vSize = _float2(10.f, 12.f);
	BloodDesc.vSpeed = _float2(1.f, 4.f);
	BloodDesc.vLifeTime = _float2(2.f, 4.f);
	BloodDesc.isLoop = false;

	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Particle_Blood"),
		VIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, &BloodDesc))))
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
	//if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Monster"),
	//	Monster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

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

	/* For.Prototype_GameObject_InvenUI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_InvenUI"),
		InvenUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Weapon */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Weapon"),
		Player_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossPlayer_Weapon */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossPlayer_Weapon"),
		Boss_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bullet */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Bullet"),
		Bullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player_Mouse */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Mouse"),
		Player_Mouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossMonster */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster"),
		BossMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Block */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Block"),
		Block::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InteractBox */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_InteractBox"),
		InteractBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FindUI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_FindUI"),
		FindUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player_State_UI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_State_UI"),
		Player_State_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LittleMonster_Weapon */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LittleMonster_Weapon"),
		LittleMonster_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LittleMonster */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LittleMonster"),
		LittleMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LittleMonster_StateUI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LittleMonster_StateUI"),
		LittleMonster_StateUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Particle_Blood */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Particle_Blood"),
		Particle_Blood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Armor */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Armor"),
		Player_Armor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Helmat */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Helmat"),
		Player_Helmat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossMonster_State_UI */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster_State_UI"),
		Boss_State_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossPerformanceCamera */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossPerformanceCamera"),
		PerformaceCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossMonster_Page2 */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster_Page2"),
		BossMonster_Page2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LaserTrail*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LaserTrail"),
		LaserTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BossMonster_Page2_State_UI*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossMonster_Page2_State_UI"),
		BossPage2UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Portal*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Portal"),
		Portal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PortalUI*/
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_PortalUI"),
		PortalUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;




#pragma endregion

#pragma region Navigation
	/* For.Prototype_Component_Navigation */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Navigation"),
		Navigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/DataFiles/Navigation.dat"), TEXT("../../Resources/DataFiles/Navigation_Neighbors.dat")))))
		return E_FAIL;

	/* For.Prototype_Component_Navigation_Boss */
	if (FAILED(CGameInstance::Get().Add_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Navigation_Boss"),
		Navigation::Create(m_pDevice, m_pContext, TEXT("../../Resources/DataFiles/Navigation_Boss.dat"), TEXT("../../Resources/DataFiles/Navigation_Neighbors_Boss.dat")))))
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
