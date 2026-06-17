#include "Player_Mouse.h"
#include "GameInstance.h"

Player_Mouse::Player_Mouse( ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext): UIObject{ pDevice, pContext }
{
}

Player_Mouse::Player_Mouse(const Player_Mouse& Prototype) : UIObject{ Prototype }
{
}

Player_Mouse::~Player_Mouse()
{
}

HRESULT Player_Mouse::Ready_Components()
{
    m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_VtxPosTex")));
    if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
        return E_FAIL;

    m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Rect>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_VIBuffer_Rect")));
    if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), m_pVIBufferCom)))
        return E_FAIL;

    m_pTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Texture_MouseMarker") ) );

    if (FAILED(__super::Add_Component(TEXT("Com_Texture"), m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT Player_Mouse::Initialize_Prototype()
{

    return S_OK;
}

HRESULT Player_Mouse::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fMarkerSize = 50.f;


    return S_OK;
}

void Player_Mouse::Priority_Update(_float fTimeDelta)
{
}

void Player_Mouse::Update(_float fTimeDelta)
{
    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);

    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    float fX = static_cast<float>(pt.x) - vViewportSize.x * 0.5f;
    float fY = -static_cast<float>(pt.y) + vViewportSize.y * 0.5f;

    m_pTransformCom->Set_Scale(m_fMarkerSize, m_fMarkerSize, 1.f);

    m_pTransformCom->Set_State(STATE::POSITION,XMVectorSet(fX, fY, 0.f, 1.f));
}
void Player_Mouse::Late_Update(_float fTimeDelta)
{
   
}
HRESULT Player_Mouse::Render()
{
    if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom || nullptr == m_pTextureCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    // 중요: 이전 UI 색상 값 초기화
    _float4 vMouseColor = { 1.f, 1.f, 1.f, 1.f };
    float fMouseAlpha = 1.f;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vMouseColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fMouseAlpha, sizeof(float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

unique_ptr<Player_Mouse> Player_Mouse::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = unique_ptr<Player_Mouse>( new Player_Mouse(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Player_Mouse");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Player_Mouse::Clone(void* pArg)
{
    shared_ptr<Player_Mouse> pInstance =shared_ptr<Player_Mouse>(new Player_Mouse(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Player_Mouse");
        return nullptr;
    }

    return pInstance;
}