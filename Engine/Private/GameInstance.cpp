#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "ProtoType_Manager.h"
#include "Object_Manager.h"
#include "ImGUI_Manager.h"
#include "Renderer.h"
#include "Camera_Manager.h"
#include "Key_Manager.h"
#include "Collider_Manager.h"
#include "Map_Manager.h"
#include "Font_Manager.h"
#include "UI_Manager.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Picking.h"
#include "Layer.h"
#include "Particle_Manager.h"
#include "Particle_System.h"

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& Engine_Desc, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutDeviceContext)
{
    m_vViewportSize = _float2((float)Engine_Desc.iWinSizeX, (float)Engine_Desc.iWinSizeY);


    m_pGraphic_Device = Graphic_Device::Create(Engine_Desc.hWnd, Engine_Desc.eWinMode, Engine_Desc.iWinSizeX, Engine_Desc.iWinSizeY, pOutDevice, pOutDeviceContext);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pKey_Manager = Key_Manager::Create();
    if (nullptr == m_pKey_Manager)
        return E_FAIL;

    m_pCollider_Manager = Collider_Manager::Create(pOutDevice, pOutDeviceContext);
    if (nullptr == m_pCollider_Manager)
        return E_FAIL;

    m_pMap_Manager = Map_Manager::Create();
    if (nullptr == m_pMap_Manager)
        return E_FAIL;

    m_pCamera_Manager = Camera_Manager::Create();
    if (nullptr == m_pCamera_Manager)
        return E_FAIL;

    m_pImGUI_Manager = ImGUI_Manager::Create(Engine_Desc.hWnd,  pOutDevice, pOutDeviceContext, m_pGraphic_Device->Get_BackBufferRTV(), m_pGraphic_Device->Get_DepthStencilView());
    if (nullptr == m_pImGUI_Manager)
        return E_FAIL;

    m_pPrototype_Manager = Prototype_Manager::Create(Engine_Desc.iNumLevels);
    if (nullptr == m_pPrototype_Manager)
        return E_FAIL;

    m_pObject_Manager = Object_Manager::Create(Engine_Desc.iNumLevels);
    if (nullptr == m_pObject_Manager)
        return E_FAIL;

    m_pTarget_Manager = Target_Manager::Create(pOutDevice, pOutDeviceContext);
    if (nullptr == m_pTarget_Manager)
        return E_FAIL;


    m_pRenderer = Renderer::Create(pOutDevice, pOutDeviceContext);
    if (nullptr == m_pRenderer)
        return E_FAIL;

    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    m_pLevel_Manager = Level_Manager::Create();
    if (nullptr == m_pLevel_Manager)
        return E_FAIL;

    m_pFont_Manager = Font_Manager::Create(pOutDevice, pOutDeviceContext);
    if (nullptr == m_pFont_Manager)
        return E_FAIL;

    m_pUI_Manager = UI_Manager::Create();
    if (nullptr == m_pFont_Manager)
        return E_FAIL;

    m_pLight_Manager = Light_Manager::Create(pOutDevice, pOutDeviceContext);
    if (nullptr == m_pLight_Manager)
        return E_FAIL;

    m_pPicking = Picking::Create(pOutDevice, pOutDeviceContext, Engine_Desc.hWnd);
    if (nullptr == m_pPicking)
        return E_FAIL;

    m_pParticle_Manager = Particle_Manager::Create(pOutDevice, pOutDeviceContext);
    if (nullptr == m_pParticle_Manager)
        return E_FAIL;


    return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
    m_pPicking->Update();

    m_pKey_Manager->Update_InputDev();

    m_pImGUI_Manager->Update_Imgui(fTimeDelta);

    m_pObject_Manager->Priority_Update(fTimeDelta);

    m_pCollider_Manager->Primitive_Update(fTimeDelta);

    m_pObject_Manager->Update(fTimeDelta);

    m_pCollider_Manager->Update(fTimeDelta);

    m_pObject_Manager->Late_Update(fTimeDelta);

    m_pUI_Manager->Priority_Update(fTimeDelta);

    m_pUI_Manager->Update(fTimeDelta);

    m_pUI_Manager->Late_Update(fTimeDelta);

    m_pLevel_Manager->Update(fTimeDelta);


}

HRESULT CGameInstance::Draw()
{


    if (FAILED(m_pRenderer->Draw()))
        return E_FAIL;

    if (FAILED(m_pLevel_Manager->Render()))
        return E_FAIL;

    m_pUI_Manager->Clear();
    m_pCollider_Manager->Render();
    m_pImGUI_Manager->Render_Imgui();

    End_Frame();
    return S_OK;
}

void CGameInstance::Clear_Resource(uint32_t iClearLevelIndex)
{
    m_pCollider_Manager->Clear();
    m_pObject_Manager->Clear(iClearLevelIndex);
    m_pPrototype_Manager->Clear(iClearLevelIndex);

   
}

void CGameInstance::Clear_Resource_SameLevel(uint32_t iClearLevelIndex)
{
    m_pCollider_Manager->Clear();
    m_pObject_Manager->Clear(iClearLevelIndex);
    m_pImGUI_Manager->Clear();
   
}

void CGameInstance::End_Frame()
{
    auto& LayerMap = Find_Layer_Lists(Get_Level());

    for (auto& Pair : LayerMap)
    {
        unique_ptr<Layer>& pLayer = Pair.second;

        if (pLayer == nullptr)
            continue;

        pLayer->Clear_DeadObjects();
    }
   
}

string CGameInstance::WStringToString(const wstring& wstr)
{
    if (wstr.empty())
        return "";

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8,
        0,
        &wstr[0],
        (int)wstr.size(),
        nullptr,
        0,
        nullptr,
        nullptr);

    string str(sizeNeeded, 0);

    WideCharToMultiByte(
        CP_UTF8,
        0,
        &wstr[0],
        (int)wstr.size(),
        &str[0],
        sizeNeeded,
        nullptr,
        nullptr);

    return str;
}

wstring CGameInstance::StringToWString(const string& str)
{
    if (str.empty())
        return L"";

    int sizeNeeded = MultiByteToWideChar(
        CP_UTF8,
        0,
        &str[0],
        (int)str.size(),
        nullptr,
        0);

    wstring wstr(sizeNeeded, 0);

    MultiByteToWideChar(
        CP_UTF8,
        0,
        &str[0],
        (int)str.size(),
        &wstr[0],
        sizeNeeded);

    return wstr;
}
#pragma region TIMER_MANAGER
_float CGameInstance::Random(_float fMin, _float fMax)
{
    return fMin + static_cast<_float>(rand()) / RAND_MAX * (fMax - fMin);
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

vector<wstring> CGameInstance::Get_PrototypeNames(uint32_t levelIndex) {
    return m_pPrototype_Manager->Get_PrototypeNames(levelIndex);
}
#pragma endregion

#pragma region GAMEOBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject_toLayer(uint32_t iPrototypeLevelIndex, const _wstring& strPrototypeTag, uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
    return m_pObject_Manager->Add_GameObject_toLayer(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

shared_ptr<GameObject> CGameInstance::Find_Object(uint32_t iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strObjectTag) {
    return m_pObject_Manager->Find_Object(iLayerLevelIndex, strLayerTag, strObjectTag);
}

map<const _wstring, unique_ptr<Layer>>& CGameInstance::Find_Layer_Lists(uint32_t iLayerLevelIndex) {
    return m_pObject_Manager->Find_Layer_Lists(iLayerLevelIndex);
}
#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<GameObject> pRenderObject)
{
    return m_pRenderer->Add_RenderObject(eRenderGroup, pRenderObject);
}
#pragma endregion



#pragma region IMGUI_MANAGER
bool CGameInstance::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return m_pImGUI_Manager->ImGui_WndProcHandler(hWnd,msg,wParam,lParam);
}

void CGameInstance::Render_IMGUI()
{
    return m_pImGUI_Manager->Render_Imgui();

}

HRESULT CGameInstance::SetSeletObject(GameObject* select) {
    return m_pImGUI_Manager->SetSeletObject(select);
}

GameObject* CGameInstance::GetSelectObject() {
    return m_pImGUI_Manager->GetSelectObject();
}
#pragma endregion

#pragma region Camera_Manager
HRESULT CGameInstance::Change_Camera(uint32_t iCameraType) {

    return m_pCamera_Manager->Change_Camera(iCameraType);
}

HRESULT  CGameInstance::Add_Camera(uint32_t iCameraType, shared_ptr<class Camera> pCamera) {
    return m_pCamera_Manager->Add_Camera(iCameraType, pCamera);
}
HRESULT CGameInstance::Get_MainCameraMatrix( _float4x4& ViewMatrix, _float4x4& ProjectionMatrix) {
    return m_pCamera_Manager->Get_MainCameraMatrix( ViewMatrix, ProjectionMatrix);

}
HRESULT CGameInstance::Get_MainCameraWorldMatrix(_float4x4& WorldMatrix) {
    return m_pCamera_Manager->Get_MainCameraWorldMatrix(WorldMatrix);

}

HRESULT  CGameInstance::Get_MainCameraPosition(_float4& Position) {
    return m_pCamera_Manager->Get_MainCameraPosition(Position);
}

HRESULT CGameInstance::Get_MainCamerwaViewMatrix(_float4x4& ViewMatrix) {
    return m_pCamera_Manager->Get_MainCamerwaViewMatrix(ViewMatrix);
}
HRESULT CGameInstance::Get_MainCamerwaProjectionMatrix(_float4x4& ProjectionMatrix) {
    return m_pCamera_Manager->Get_MainCamerwaProjectionMatrix(ProjectionMatrix);
}
weak_ptr<Camera> CGameInstance::Find_Camera(uint32_t iCameraType)
{
    return m_pCamera_Manager->Find_Camera(iCameraType);
 
}
void  CGameInstance::GetWorldMatrix(_float4x4& WorldMatrix) {
    m_pCamera_Manager->Get_MainCameraWorldMatrix(WorldMatrix);
}
HRESULT  CGameInstance::Set_MainCameraPosition(_float4& CameraPosition) {
    return m_pCamera_Manager->Set_MainCameraPosition(CameraPosition);
}
HRESULT  CGameInstance::Set_MainCameraLookAt(_float4& CameraLookAT) {
    return m_pCamera_Manager->Set_MainCameraLookAt(CameraLookAT);
}

#pragma endregion
#pragma region Collider_Manager
HRESULT CGameInstance::Add_Collider(wstring GroupTag, class BaseCollider* pCollider) {

    return  m_pCollider_Manager->Add_Collider(GroupTag, pCollider);
}
std::vector<BaseCollider*>* CGameInstance::GetColliderGroups(wstring GroupTag) {
    return m_pCollider_Manager->GetColliderGroups(GroupTag);
}

std::unordered_map<wstring, std::vector<BaseCollider*>>& CGameInstance::GetAllCollders() {
    return m_pCollider_Manager->GetAllCollders();
}

void	CGameInstance::MousePicking(XMVECTOR rayOrigin, XMVECTOR rayDir, uint32_t LevelIndex) {
    return m_pCollider_Manager->MousePicking(rayOrigin,rayDir,LevelIndex);
}

_bool CGameInstance::Intersect(class BaseCollider* pCollider, BaseCollider* sCollider) {
    return m_pCollider_Manager->Intersect(pCollider, sCollider);
}
#pragma endregion

#pragma region Key_Manager
HRESULT CGameInstance::Ready_Key(HINSTANCE hInst, HWND hWnd) {
   return m_pKey_Manager->Ready_Key(hInst, hWnd);
}

_uchar	CGameInstance::Get_DIKeyState(_uchar byKeyID) {
    return m_pKey_Manager->Get_DIKeyState(byKeyID);
}

_uchar	CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse) {
    return m_pKey_Manager->Get_DIMouseState(eMouse);
}

_long	CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState){
    return m_pKey_Manager->Get_DIMouseMove(eMouseState);
}

bool CGameInstance::Key_Pressing(_uchar byKeyID) {
    return m_pKey_Manager->Key_Pressing(byKeyID);
}
bool CGameInstance::Key_Up(_uchar byKeyID) {
    return m_pKey_Manager->Key_Up(byKeyID);
}
bool CGameInstance::Key_Down(_uchar byKeyID) {
    return m_pKey_Manager->Key_Down(byKeyID);
}

bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouseState) {
    return m_pKey_Manager->Mouse_Pressing(eMouseState);
}
bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouseState) {
    return m_pKey_Manager->Mouse_Up(eMouseState);
}
bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouseState) {
    return m_pKey_Manager->Mouse_Down(eMouseState);
}

#pragma endregion

#pragma region FONT_MANAGER
HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}
void CGameInstance::Draw_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
    m_pFont_Manager->Draw(strFontTag, pText, vPosition, fScale, vColor, fRotation, vOrigin);
}

#pragma endregion

#pragma region Map_Manager
HRESULT CGameInstance::Save(string _mapDataName, bool _overwrite) {
    return m_pMap_Manager->Save(_mapDataName, _overwrite);
}
HRESULT CGameInstance::Load(string _mapDataName, uint32_t Levelindex) {
    return m_pMap_Manager->Load(_mapDataName, Levelindex);
}

vector<string>& CGameInstance::GetMapNames() {
    return m_pMap_Manager->GetMapNames();
}

vector<string>& CGameInstance::FindCategories(string _category) {
    return m_pMap_Manager->FindCategories(_category);
}
#pragma endregion

#pragma region UI_Manager
HRESULT CGameInstance::Add_UIObject(const wstring& strUIGroup, shared_ptr<UIObject> pUIObject) {
    return m_pUI_Manager->Add_UIObject(strUIGroup,pUIObject);
}

vector<shared_ptr<class UIObject>>* CGameInstance::Find_UIGroup(const wstring& strUIGroup) {
    return m_pUI_Manager->Find_UIGroup(strUIGroup);
}

shared_ptr<class UIObject> CGameInstance::Find_UIObject(const wstring& strUIGroup, const wstring& strUIObjectTag) {
    return m_pUI_Manager->Find_UIObject(strUIGroup, strUIObjectTag);
}

void CGameInstance::Set_MainUIGroup(const wstring& strUIGroup) {
    return m_pUI_Manager->Set_MainUIGroup(strUIGroup);
}
const wstring& CGameInstance::Get_MainUIGroup() const {
    return m_pUI_Manager->Get_MainUIGroup();
}
#pragma endregion

#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{

    return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
    return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, _bool bUseDepth)
{
    return m_pTarget_Manager->Begin_MRT(strMRTTag, bUseDepth);
}


HRESULT CGameInstance::End_MRT()
{
    return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(const _wstring& strTargetTag, shared_ptr<class Shader> pShader, const _char* pConstantName)
{
    return m_pTarget_Manager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Copy_RenderTarget(const _wstring& strTargetTag, ComPtr<ID3D11Texture2D> pOut)
{
    return m_pTarget_Manager->Copy_RenderTarget(strTargetTag, pOut);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Debug_RT_Render(const _wstring& strMRTTag, shared_ptr<class Shader> pShader, const _char* pConstantName, shared_ptr<class VIBuffer_Rect> pVIBuffer)
{
    return m_pTarget_Manager->Debug_Render(strMRTTag, pShader, pConstantName, pVIBuffer);
}
#endif
#pragma endregion

#pragma region LIGHT_MANAGER

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
    return m_pLight_Manager->Add_Light(LightDesc);
}
HRESULT CGameInstance::Render_Lights(shared_ptr<class Shader> pShader, shared_ptr<class VIBuffer_Rect> pVIBuffer)
{
    return m_pLight_Manager->Render(pShader, pVIBuffer);
}
#pragma endregion

#pragma region PICKING
_bool CGameInstance::Picking_to_Shader(_float4* pOut)
{

    return m_pPicking->Picking_to_Shader(pOut);
}


#pragma endregion

#pragma region Particle_Manager
HRESULT CGameInstance::Register_ParticleSystem(PARTICLE_TYPE eType, shared_ptr<Particle_System> pParticleSystem) {

	return m_pParticle_Manager->Register_ParticleSystem(eType, pParticleSystem);
}

HRESULT CGameInstance::Add_Particle(PARTICLE_TYPE eType, void* pArg ) {
	return m_pParticle_Manager->Add_Particle(eType, pArg);
}
#pragma endregion

void CGameInstance::Release_Engine()
{
    m_pParticle_Manager.reset();

    m_pPicking.reset();

    m_pUI_Manager.reset();

    m_pFont_Manager.reset();

    m_pLight_Manager.reset();

    m_pTarget_Manager.reset();

    m_pCamera_Manager.reset();

    m_pCollider_Manager.reset();

    m_pKey_Manager.reset();

    m_pMap_Manager.reset();

    m_pRenderer.reset();

    m_pLevel_Manager.reset();

    m_pTimer_Manager.reset();

    m_pObject_Manager.reset();

    m_pPrototype_Manager.reset();

    m_pImGUI_Manager.reset();

    m_pGraphic_Device->Shutdown();

    m_pGraphic_Device.reset();
}

