#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC Engine_Desc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext)
{
    m_pGraphic_Device = Graphic_Device::Create(Engine_Desc.hWnd, Engine_Desc.eWinMode, Engine_Desc.iWinSizeX, Engine_Desc.iWinSizeY, pOutDevice, pOutDeviceContext);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

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

void CGameInstance::Release_Engine()
{
    m_pTimer_Manager.reset();

   // m_pGraphic_Device->Shutdown();

    m_pGraphic_Device.reset();
}

