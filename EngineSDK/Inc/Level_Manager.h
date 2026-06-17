#pragma once
#include "CLevel.h"


NS_BEGIN(Engine)

class Level_Manager
{
private:
	Level_Manager();

public:
	~Level_Manager() = default;

public:
	HRESULT Change_Level(uint32_t iNewLevelIndex, unique_ptr<class CLevel> pNewLevel);
	void Update(_float fTimeDelta);
	HRESULT Render();


private:
	uint32_t					m_iCurrentLevelIndex	{};
	unique_ptr<class CLevel>	m_pCurrentLevel			{ nullptr };

public:
	static unique_ptr<Level_Manager> Create();
};

NS_END

