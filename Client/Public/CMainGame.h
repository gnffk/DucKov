#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

NS_BEGIN(Client)

class CMainGame
{
private:
	CMainGame();
public:
	~CMainGame();

public:
	HRESULT Initialize();
	void Update(float fTimeDelta);
	HRESULT Render();


public:
	static unique_ptr<CMainGame> Create();
};

NS_END

