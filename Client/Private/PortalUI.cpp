#include "PortalUI.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Portal.h"
#include "Level_Loading.h"

PortalUI::PortalUI(
    ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext)
    : UIObject{ pDevice, pContext }
{
}

PortalUI::PortalUI(const PortalUI& Prototype)
    : UIObject{ Prototype }
{
}

PortalUI::~PortalUI()
{
}

HRESULT PortalUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT PortalUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void PortalUI::Priority_Update(_float fTimeDelta)
{
}

void PortalUI::Update(_float fTimeDelta)
{
    Update_PortalPrompt(fTimeDelta);

    if (m_bVisible)
    {
        Try_MoveLevel();
    }
}

void PortalUI::Late_Update(_float fTimeDelta)
{
}

HRESULT PortalUI::Render()
{
    if (false == m_bVisible)
        return S_OK;

    // 일단 텍스트만 출력.
    // 상자 FindUI처럼 배경 Rect까지 쓰고 싶으면 Add_UIRect 방식으로 확장하면 됨.
    CGameInstance::Get().Draw_Text(TEXT("Font_Default"),TEXT("[F] 이동"), m_vTextPos,0.65f,XMVectorSet(1.f, 1.f, 1.f, 1.f),0.f,_float2(0.f, 0.f));

    return S_OK;
}

void PortalUI::Update_PortalPrompt(_float fTimeDelta)
{
    m_bVisible = false;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    if (false == pOwner->GetCollision())
        return;

    auto pTransform = pOwner->GetTransform();

    if (pTransform == nullptr)
        return;

    _vector vOwnerPos = pTransform->Get_State(STATE::POSITION);

    vOwnerPos += XMVectorSet(
        m_vOwnerUIOffset.x,
        m_vOwnerUIOffset.y,
        m_vOwnerUIOffset.z,
        0.f);

    _float2 vScreenPos{};

    if (false == WorldToScreen(vOwnerPos, vScreenPos))
        return;

    m_vBackPos = vScreenPos;

    // 텍스트 위치 보정
    m_vTextPos =
    {
        vScreenPos.x - 45.f,
        vScreenPos.y + 20.f
    };

    m_bVisible = true;
}

void PortalUI::Try_MoveLevel()
{
    if (m_bChangingLevel)
        return;

    if (CGameInstance::Get().Key_Down(DIK_F)) {
        auto pOwner = m_pOwner.lock();

        if (pOwner == nullptr)
            return;

        auto pPortal = static_pointer_cast<Portal>(pOwner);

        if (pPortal == nullptr)
            return;

        m_bChangingLevel = true;

        if (FAILED(CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING), Level_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY)))) {

            return;
        }
    }

}

_bool PortalUI::WorldToScreen(_fvector vWorldPos, _float2& vOutScreenPos)
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    if (vViewportSize.x <= 0.f || vViewportSize.y <= 0.f)
        return false;

    _float4x4 ViewMatrix{};
    _float4x4 ProjMatrix{};

    // 일단 기존 함수 유지
    CGameInstance::Get().Get_MainCamerwaViewMatrix(ViewMatrix);
    CGameInstance::Get().Get_MainCamerwaProjectionMatrix(ProjMatrix);

    _matrix matView = XMLoadFloat4x4(&ViewMatrix);
    _matrix matProj = XMLoadFloat4x4(&ProjMatrix);

    _vector vView = XMVector3TransformCoord(vWorldPos, matView);
    _vector vClip = XMVector3TransformCoord(vView, matProj);

    float x = XMVectorGetX(vClip);
    float y = XMVectorGetY(vClip);
    float z = XMVectorGetZ(vClip);

    // 카메라 뒤쪽이면 숨김
    if (z < 0.f || z > 1.f)
        return false;

    vOutScreenPos.x = (x * 0.5f + 0.5f) * vViewportSize.x;
    vOutScreenPos.y = (-y * 0.5f + 0.5f) * vViewportSize.y;

    // 화면 밖으로 튀어나가는 값 보정
    vOutScreenPos.x = std::clamp(vOutScreenPos.x, 20.f, vViewportSize.x - 120.f);
    vOutScreenPos.y = std::clamp(vOutScreenPos.y, 20.f, vViewportSize.y - 40.f);

    return true;
}
unique_ptr<PortalUI> PortalUI::Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext)
{
    unique_ptr<PortalUI> pInstance = unique_ptr<PortalUI>(new PortalUI(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : PortalUI");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> PortalUI::Clone(void* pArg)
{
    shared_ptr<PortalUI> pInstance =shared_ptr<PortalUI>(new PortalUI(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : PortalUI");
        return nullptr;
    }

    return pInstance;
}