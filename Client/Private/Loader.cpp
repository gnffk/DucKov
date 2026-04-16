#include "Loader.h"
#include "GameInstance.h"
#include "TestModel.h"
#include "PerspectiveCamera.h"

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
	
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MapEditor()
{

#pragma region Model Component Prototype


	//if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Texture_Logo"),
	//	Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fbx/Garden-Palette.png"), 1))))
	//	return E_FAIL;
	 
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Texture_Logo"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fbx/Player_Duck_Tex_AlbedoTint.png"), 1))))
		return E_FAIL;


#pragma endregion

#pragma region Model Component Prototype


	//if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Model_Duck"),
	//	Model::Create(m_pDevice, m_pContext, ETOUI(LEVEL::MAPEDITOR), L"../../Resources/Fbx/Banana_02_LOD0.fbx"))))
	//	return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Model_Duck"),
		Model::Create(m_pDevice, m_pContext, ETOUI(LEVEL::MAPEDITOR), L"../../Resources/Fbx/0_CharacterModel_Duck_Jeff.fbx"))))
		return E_FAIL;

#pragma endregion

#pragma region Shader Component Prototype

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Shader_Vtx_FBX_Tex"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Vtx_Fbx.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region VIBuffer Component Prototype
	///* Prototype_GameObject_TestModel */
	//if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_VIBuffer_Fbx_TestModel"),
	//	VIBuffer_Fbx::Create(m_pDevice, m_pContext, "../../Resources/Fbx/0_CharacterModel_Duck_Jeff.fbx")))){
	//	return E_FAIL;
	//}

	//if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_VIBuffer_Rect"),
	//	VIBuffer_Rect::Create(m_pDevice, m_pContext)))) {
	//	return E_FAIL;
	//}
#pragma endregion


#pragma region GameObject Prototype
	/* Prototype_GameObject_TestModel */
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_TestModel"),
		TestModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_PerspectiveCamera */
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_GameObject_PerspectiveCamera"),
		PerspectiveCamera::Create(m_pDevice, m_pContext))))
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
