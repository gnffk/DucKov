#include "Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "MapEditor.h"

Level_Loading::Level_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

Level_Loading::~Level_Loading()
{
}

HRESULT Level_Loading::Initialize(LEVEL eNextLevelIndex)
{
	m_eNextLevelIndex = eNextLevelIndex;

	/* 로딩에 필요한 객체를 생성한다.  */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	/* 다음레벨에 필요한 자원을 로드해주기위한 준비작업(로딩작업을 수행해줄 스레드를 생성한다)을 수행한다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, m_eNextLevelIndex);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void Level_Loading::Update(_float fTimeDelta)
{
	
	if (true == m_pLoader->isFinished())
	{
		ImGui::Begin("NEXT_SCENE", nullptr, ImGuiWindowFlags_NoTitleBar);


		ImGui::Text(u8"로딩 완료");
		if (ImGui::Button(u8"다음 씬")) {
			unique_ptr<CLevel>		pNewLevel = { nullptr };

			switch (m_eNextLevelIndex)
			{
			case LEVEL::LOGO:
				pNewLevel = Level_Logo::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL::GAMEPLAY:
				pNewLevel = Level_GamePlay::Create(m_pDevice, m_pDeviceContext);
				break;
			case LEVEL::MAPEDITOR:
				pNewLevel = MapEditor::Create(m_pDevice, m_pDeviceContext);
				break;
			}

			if (FAILED(CGameInstance::Get().Change_Level(ETOUI(m_eNextLevelIndex), std::move(pNewLevel)))) {

				return;
			}
		}

		ImGui::End();
		
	}
}

HRESULT Level_Loading::Render()
{


	return S_OK;
}

HRESULT Level_Loading::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	return S_OK;
}

unique_ptr<Level_Loading> Level_Loading::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex)
{
	auto	pInstance = unique_ptr<Level_Loading>(new Level_Loading(pDevice, pContext));

	if (FAILED(pInstance->Initialize(eNextLevelIndex)))
		MSG_BOX("Failed to Created : CLevel_Loading");

	return pInstance;
}
