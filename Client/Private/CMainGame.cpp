#include "CMainGame.h"
#include "GameInstance.h"
#include "Level_Loading.h"

Client::CMainGame::CMainGame()
{
}

Client::CMainGame::~CMainGame()
{

	CGameInstance::Get().Release_Engine();
}

HRESULT Client::CMainGame::Initialize()
{
	/* 엔진을 사용할 준비를 한다. */
	ENGINE_DESC EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(CGameInstance::Get().Initialize_Engine(EngineDesc, m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	CGameInstance::Get();


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

unique_ptr<CMainGame> Client::CMainGame::Create()
{
	auto	pInstance = unique_ptr<CMainGame>(new CMainGame());

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : CMainGame"), nullptr, MB_OK);

	return pInstance;
}
