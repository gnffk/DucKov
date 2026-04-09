#include "Level_Logo.h"
#include "GameInstance.h"


#include "Level_Loading.h"

Level_Logo::Level_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

Level_Logo::~Level_Logo()
{
}

HRESULT Level_Logo::Initialize()
{
	return S_OK;
}

void Level_Logo::Update(_float fTimeDelta)
{
#if _DEBUG
	ImGui::Begin("NEXT_SCENE", nullptr, ImGuiWindowFlags_NoTitleBar);


	ImGui::Text(u8"다음 씬으로 넘어가기");
	if (ImGui::Button(u8"게임 플레이")) {
		if (FAILED(CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING),
			Level_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL::GAMEPLAY)))) {

			return;
		}
	}
	if (ImGui::Button(u8"맵 에디터")) {
		if (FAILED(CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING),
			Level_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL::MAPEDITOR)))) {

			return;
		}
	}
	
	ImGui::End();
#endif
}

HRESULT Level_Logo::Render()
{

	return S_OK;
}

unique_ptr<Level_Logo> Level_Logo::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Level_Logo>(new Level_Logo(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
		MessageBox(g_hWnd, TEXT("Failed to Created : CLevel_Logo"), nullptr, MB_OK);

	return pInstance;
}
