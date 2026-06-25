#include "Loading_BackGround_UI.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"

Loading_BackGround_UI::Loading_BackGround_UI(
    ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext)
    : UIObject{ pDevice, pContext }
{
}

Loading_BackGround_UI::Loading_BackGround_UI(const Loading_BackGround_UI& Prototype)
    : UIObject{ Prototype }
{
}

Loading_BackGround_UI::~Loading_BackGround_UI()
{
}

HRESULT Loading_BackGround_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Loading_BackGround_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    LOADING_UI_DESC* pDesc = static_cast<LOADING_UI_DESC*>(pArg);

    if (pDesc != nullptr)
    {
        m_eNextLevel = pDesc->eNextLevel;
    }

    m_bUseSpecialLoading = (m_eNextLevel == LEVEL::GAMEPLAY);

    m_fFrameAcc = 0.f;
    m_iFrameIndex = 0;

  
    m_bAnimationFinished = !m_bUseSpecialLoading;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void Loading_BackGround_UI::Priority_Update(_float fTimeDelta)
{
}

void Loading_BackGround_UI::Update(_float fTimeDelta)
{
    if (false == m_bUseSpecialLoading)
        return;

    if (m_bAnimationFinished)
        return;

    m_fFrameAcc += fTimeDelta;

    while (m_fFrameAcc >= m_fFrameInterval)
    {
        m_fFrameAcc -= m_fFrameInterval;

        ++m_iFrameIndex;

        if (m_iFrameIndex >= m_iFrameCount - 1)
        {
            m_iFrameIndex = m_iFrameCount - 1;
            m_bAnimationFinished = true;
            break;
        }
    }
}
void Loading_BackGround_UI::Late_Update(_float fTimeDelta)
{
    CGameInstance::Get().Add_RenderObject(RENDERGROUP::UI,SHARED_THIS(Loading_BackGround_UI));
}

HRESULT Loading_BackGround_UI::Render()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pTextureCom)
        return E_FAIL;

    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT Loading_BackGround_UI::Ready_Components()
{
    if (FAILED(__super::Add_Component(
        ETOUI(LEVEL::NEVER),
        TEXT("Prototype_Com_Shader_VtxPosTex_Loading"),
        TEXT("Com_Shader"),
        m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(
        ETOUI(LEVEL::NEVER),
        TEXT("Prototype_Com_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"),
        m_pVIBufferCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(
        ETOUI(LEVEL::NEVER),
        TEXT("Prototype_Com_Texture_MainMenu_BackGround_GAMEPLAY"),
        TEXT("Com_Texture_BackGround_GAMEPLAY"),
        m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT Loading_BackGround_UI::Bind_ShaderResources()
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    // 화면 전체 크기
    m_pTransformCom->Set_Scale(
        vViewportSize.x,
        vViewportSize.y,
        1.f);

    // UI 좌표계 중앙
    m_pTransformCom->Set_State(STATE::POSITION,XMVectorSet(0.f, 0.f, 0.f, 1.f));

    if (FAILED(m_pTransformCom->Bind_ShaderResource(
        m_pShaderCom,
        "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix(
        "g_ViewMatrix",
        &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix(
        "g_ProjMatrix",
        &m_ProjMatrix)))
        return E_FAIL;

    // =====================================================
    // Texture Index 결정
    // =====================================================
    uint32_t iTextureIndex = 0;

    if (m_bUseSpecialLoading)
    {
        iTextureIndex = m_iFrameIndex;

        if (iTextureIndex >= m_iFrameCount)
            iTextureIndex = m_iFrameCount - 1;
    }
    else
    {
        // GAMEPLAY 로딩이 아니면 0번 텍스처를 검정색으로 tint해서 사용
        iTextureIndex = 0;
    }

    if (FAILED(m_pTextureCom->Bind_ShaderResource(
        m_pShaderCom,
        "g_Texture",
        iTextureIndex)))
        return E_FAIL;

    // =====================================================
    // Shader Value
    // =====================================================
    _float fAlpha = 1.f;
    _float fIntensity = 1.f;
    _float4 vColor = { 1.f, 1.f, 1.f, 1.f };

    // GAMEPLAY 로딩이 아니면 검은 화면
    if (false == m_bUseSpecialLoading)
    {
        vColor = { 0.f, 0.f, 0.f, 1.f };
    }

    if (FAILED(m_pShaderCom->Bind_RawValue(
        "g_fAlpha",
        &fAlpha,
        sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue(
        "g_vColor",
        &vColor,
        sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue(
        "g_fIntensity",
        &fIntensity,
        sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

unique_ptr<Loading_BackGround_UI> Loading_BackGround_UI::Create(
    ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext)
{
    unique_ptr<Loading_BackGround_UI> pInstance =
        unique_ptr<Loading_BackGround_UI>(
            new Loading_BackGround_UI(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Loading_BackGround_UI");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Loading_BackGround_UI::Clone(void* pArg)
{
    shared_ptr<Loading_BackGround_UI> pInstance =
        shared_ptr<Loading_BackGround_UI>(
            new Loading_BackGround_UI(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Loading_BackGround_UI");
        return nullptr;
    }

    return pInstance;
}