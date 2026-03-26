#include "Level_GamePlay.h"

Level_GamePlay::Level_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

Level_GamePlay::~Level_GamePlay()
{
}

HRESULT Level_GamePlay::Initialize()
{
	return S_OK;
}

void Level_GamePlay::Update(_float fTimeDelta)
{
	uint32_t iData = 10;
}

HRESULT Level_GamePlay::Render()
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
