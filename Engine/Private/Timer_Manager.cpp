#include "Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

CTimer_Manager::~CTimer_Manager()
{
	m_Timers.clear();
}

_float CTimer_Manager::Get_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::Set_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
}

HRESULT CTimer_Manager::Ready_Timer(const _wstring& strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	auto	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.emplace(strTimerTag, std::move(pTimer));

	return S_OK;
}

CTimer* CTimer_Manager::Find_Timer(const _wstring& strTimerTag)
{
	auto		iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second.get();
}

unique_ptr<CTimer_Manager> CTimer_Manager::Create()
{
	return unique_ptr<CTimer_Manager>(new CTimer_Manager());
}


