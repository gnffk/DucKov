#include "AABB_Collider.h"
#include "OBB_Collider.h"
#include "CMainGame.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "Loading_BackGround_UI.h"

Client::CMainGame::CMainGame()
{
}

Client::CMainGame::~CMainGame()
{
	CGameInstance::Get().Release_Engine();
}

HRESULT Client::CMainGame::Initialize()
{
	/* ¿£ÁøÀ» »ç¿ëÇÒ ÁØºñ¸¦ ÇÑ´Ù.*/
	ENGINE_DESC EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ETOUI(LEVEL::END);
	if (FAILED(CGameInstance::Get().Initialize_Engine(EngineDesc, m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	//---- Texture Prototype ----//

	/* For.Prototype_Com_Texture_MainMenu_BackGround_GAMEPLAY */
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::NEVER), TEXT("Prototype_Com_Texture_MainMenu_BackGround_GAMEPLAY"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Loading_Frame/frame_%d.png"), 90))))
		return E_FAIL;

	/* For.Prototype_Com_Texture_Loading_Mask */
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::NEVER), TEXT("Prototype_Com_Texture_Loading_Mask"),
		Texture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Loading_Frame/frame_90.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Com_Shader_VtxPosTex_Loading */
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::NEVER), TEXT("Prototype_Com_Shader_VtxPosTex_Loading"),
		Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_VtxPosTex_Loading.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Com_VIBuffer_Rect */
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::NEVER), TEXT("Prototype_Com_VIBuffer_Rect"),
		VIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	//---- GameObject Prototype ----//

	// For.Prototype_GameObject_Loading_BackGround_UI
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::NEVER), TEXT("Prototype_GameObject_Loading_BackGround_UI"), Loading_BackGround_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(CGameInstance::Get().Add_Sound(L"BGM_HOME", L"../../Resources/Sounds/DuckFunk.mp3")))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Sound(L"EFFECT_Player_Roll", L"../../Resources/Sounds/Roll.wav")))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Sound(L"EFFECT_Player_Walk", L"../../Resources/Sounds/walk.wav")))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Sound(L"EFFECT_Player_RUN", L"../../Resources/Sounds/FastWalk.wav")))
		return E_FAIL;



	if (FAILED(Start_Level(LEVEL::MAPEDITOR)))
		return E_FAIL;






	return S_OK;
}

HRESULT Client::CMainGame::Start_Level(LEVEL eStartLevelIndex)
{
	if (FAILED(CGameInstance::Get().Change_Level(static_cast<uint32_t>(LEVEL::LOADING),
		Level_Loading::Create(m_pDevice, m_pContext, eStartLevelIndex))))
		return E_FAIL;



	return S_OK;
}

void Client::CMainGame::Update(float fTimeDelta)
{
	CGameInstance::Get().Update_Engine(fTimeDelta);
}

HRESULT Client::CMainGame::Render()
{
	_float4			vClearColor = { 0.f, 0.f, 1.f, 1.f };

	if (FAILED(CGameInstance::Get().Clear_BackBuffer_View(&vClearColor)))
		return E_FAIL;
	if (FAILED(CGameInstance::Get().Clear_DepthStencil_View()))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Draw()))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Present()))
		return E_FAIL;

	return S_OK;
}


HRESULT CMainGame::Ready_Fonts()
{
	/*
	MakeSpriteFont "³Ø½¼lv1°íµñ Bold" /FontSize:20 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 160ex.spritefont
	*/


	if (FAILED(CGameInstance::Get().Add_Font(TEXT("Font_Default"), TEXT("../../Resources/Fonts/160.spriteFont"))))
		return E_FAIL;

	return S_OK;
}


unique_ptr<CMainGame> Client::CMainGame::Create()
{
	auto	pInstance = unique_ptr<CMainGame>(new CMainGame());

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : CMainGame"), nullptr, MB_OK);

	return pInstance;
}
