#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "ProtoType_Manager.h"
#include "Object_Manager.h"
#include "ImGUI_Manager.h"
#include "Renderer.h"


CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& Engine_Desc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext)
{
    m_pGraphic_Device = Graphic_Device::Create(Engine_Desc.hWnd, Engine_Desc.eWinMode, Engine_Desc.iWinSizeX, Engine_Desc.iWinSizeY, pOutDevice, pOutDeviceContext);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pImGUI_Manager = ImGUI_Manager::Create(Engine_Desc.hWnd,  pOutDevice, pOutDeviceContext);
    if (nullptr == m_pImGUI_Manager)
        return E_FAIL;

    m_pRenderer = Renderer::Create(pOutDevice, pOutDeviceContext);
    if (nullptr == m_pRenderer)
        return E_FAIL;
    
    m_pResource_Manager = Resource_Manager::Create(pOutDevice, pOutDeviceContext);
    if (nullptr == m_pResource_Manager)
        return E_FAIL;

    m_pPrototype_Manager = Prototype_Manager::Create(Engine_Desc.iNumLevels);
    if (nullptr == m_pPrototype_Manager)
        return E_FAIL;

    m_pObject_Manager = Object_Manager::Create(Engine_Desc.iNumLevels);
    if (nullptr == m_pObject_Manager)
        return E_FAIL;


    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    m_pLevel_Manager = Level_Manager::Create();
    if (nullptr == m_pLevel_Manager)
        return E_FAIL;


    return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
    m_pImGUI_Manager->Update_Imgui(fTimeDelta);

    m_pObject_Manager->Priority_Update(fTimeDelta);

    m_pObject_Manager->Update(fTimeDelta);

    m_pObject_Manager->Late_Update(fTimeDelta);

    m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Draw()
{
    if (FAILED(m_pRenderer->Draw()))
        return E_FAIL;

    if (FAILED(m_pLevel_Manager->Render()))
        return E_FAIL;

    m_pImGUI_Manager->Render_Imgui();

    return S_OK;
}

void CGameInstance::Clear_Resource(uint32_t iClearLevelIndex)
{
    m_pObject_Manager->Clear(iClearLevelIndex);
    m_pPrototype_Manager->Clear(iClearLevelIndex);
    m_pResource_Manager->Clear();
}


#pragma region TIMER_MANAGER
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
#pragma endregion

#pragma region Graphic_Device
HRESULT CGameInstance::Clear_BackBuffer_View(const _float4* pClearColor)
{
    return m_pGraphic_Device->Clear_BackBuffer_View(pClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
    return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
    return m_pGraphic_Device->Present();
}
#pragma endregion


#pragma region Level_Manager
HRESULT CGameInstance::Change_Level(uint32_t iNewLevelIndex, unique_ptr<class CLevel> pNewLevel)
{
    return m_pLevel_Manager->Change_Level(iNewLevelIndex, std::move(pNewLevel));
}

#pragma endregion

#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, unique_ptr<Prototype> pPrototype)
{
    return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, std::move(pPrototype));
}
shared_ptr<Prototype> CGameInstance::Clone_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
    return m_pPrototype_Manager->Clone_Prototype(iLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion

#pragma region GAMEOBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject_toLayer(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag, uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
    return m_pObject_Manager->Add_GameObject_toLayer(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject)
{
    return m_pRenderer->Add_RenderObject(eRenderGroup, pRenderObject);
}
#pragma endregion

#pragma region Resource_Manager


shared_ptr<Resource> CGameInstance::Find_Resources(uint32_t num, wstring key)
{
    return m_pResource_Manager->Find_Resource(num,key);
}

HRESULT CGameInstance::Add_Resource(ERESOURCE num, wstring key, shared_ptr<Resource> pResource) {
    return m_pResource_Manager->Add_Resource(num, key, std::move(pResource));
}
#pragma endregion

#pragma region IMGUI_MANAGER
HRESULT CGameInstance::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return m_pImGUI_Manager->ImGui_WndProcHandler(hWnd,msg,wParam,lParam);
}
#pragma endregion


void CGameInstance::Release_Engine()
{
    m_pRenderer.reset();

    m_pLevel_Manager.reset();

    m_pTimer_Manager.reset();

    m_pObject_Manager.reset();

    m_pPrototype_Manager.reset();

    m_pResource_Manager.reset();

    m_pImGUI_Manager.reset();

    m_pGraphic_Device->Shutdown();

    m_pGraphic_Device.reset();
}

