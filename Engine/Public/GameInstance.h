#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
public:
	virtual ~CGameInstance();

public:
	HRESULT Initialize_Engine();

public:
	_float			Get_TimeDelta(const _wstring& strTimerTag);
	void			Set_TimeDelta(const _wstring& strTimerTag);
	HRESULT			Add_Timer(const _wstring& strTimerTag);

private:
	unique_ptr<class CTimer_Manager>				m_pTimer_Manager = {};



};

NS_END