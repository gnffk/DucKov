#include "CMainGame.h"
#include "GameInstance.h"

Client::CMainGame::CMainGame()
{
}

Client::CMainGame::~CMainGame()
{

}

HRESULT Client::CMainGame::Initialize()
{
	/* 엔진을 사용할 준비를 한다. */
	if (FAILED(CGameInstance::Get().Initialize_Engine()))
		return E_FAIL;



	CGameInstance::Get();


	return S_OK;
}

void Client::CMainGame::Update(float fTimeDelta)
{
}

HRESULT Client::CMainGame::Render()
{
	return S_OK;
}

unique_ptr<CMainGame> Client::CMainGame::Create()
{
	auto	pInstance = unique_ptr<CMainGame>(new CMainGame());

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : CMainGame"), nullptr, MB_OK);

	return pInstance;
}
