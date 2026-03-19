#include "GameInstance.h"
#include "Timer_Manager.h"

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine()
{
    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    return S_OK;
}

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
    return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

void CGameInstance::Set_TimeDelta(const _wstring& strTimerTag)
{
    m_pTimer_Manager->Set_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
    return m_pTimer_Manager->Ready_Timer(strTimerTag);
}

