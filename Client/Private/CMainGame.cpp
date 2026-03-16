#include "CMainGame.h"

CMainGame::CMainGame()
{
}

CMainGame::~CMainGame()
{
}

HRESULT CMainGame::Initalize()
{
	return S_OK;
}

HRESULT CMainGame::Update(float deltaTime)
{
	return S_OK;
}

void CMainGame::Render()
{
}

unique_ptr<CMainGame> CMainGame::Create()
{
	unique_ptr<CMainGame> p_MainGame = unique_ptr<CMainGame>(new CMainGame());

	if (FAILED(p_MainGame->Initalize()))
	{
		MSG_BOX("p_MainGame Create Failed");
	}

	return p_MainGame;
}
