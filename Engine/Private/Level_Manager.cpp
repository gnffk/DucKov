#include "Level_Manager.h"
#include "GameInstance.h"

#include "CLevel.h"

Level_Manager::Level_Manager()
{
}



HRESULT Level_Manager::Change_Level(uint32_t iNewLevelIndex, unique_ptr<class CLevel> pNewLevel)
{
	if (nullptr == pNewLevel)
		return E_FAIL;

	if (nullptr != m_pCurrentLevel)
	{
		CGameInstance::Get().Clear_Resource(m_iCurrentLevelIndex);
		m_pCurrentLevel.reset();
	}

	m_pCurrentLevel = std::move(pNewLevel);

	m_iCurrentLevelIndex = iNewLevelIndex;

	return S_OK;
}

void Level_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT Level_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;;

	return m_pCurrentLevel->Render();
}

unique_ptr<Level_Manager> Level_Manager::Create()
{
	return unique_ptr<Level_Manager>(new Level_Manager());
}
