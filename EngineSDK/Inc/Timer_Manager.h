#pragma once

#include "Engine_Defines.h"


NS_BEGIN(Engine)

class CTimer;
class CTimer_Manager final
{
private:
	CTimer_Manager();
public:
	virtual ~CTimer_Manager();

public:
	_float			Get_TimeDelta(const _wstring& strTimerTag);
	void			Set_TimeDelta(const _wstring& strTimerTag);

public:
	HRESULT			Ready_Timer(const _wstring& strTimerTag);

private:
	map<const _wstring, unique_ptr<CTimer>>		m_Timers;


private:
	CTimer* Find_Timer(const _wstring& strTimerTag);

public:
	static unique_ptr<CTimer_Manager> Create();

};

NS_END