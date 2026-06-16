#include "Player_State_UI.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"


Player_State_UI::Player_State_UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext): UIObject{ pDevice, pContext }
{
}

Player_State_UI::Player_State_UI(const Player_State_UI& Prototype) : UIObject{ Prototype }
{
}

Player_State_UI::~Player_State_UI()
{
}

HRESULT Player_State_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Player_State_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_UIRects.clear();


    // HP 배경
    if (FAILED(Add_UIRect( TEXT("HP_Back"), TEXT("HP_Back"),TEXT("Prototype_Com_Texture_Graph"),m_vHPBarPos, m_vHPBarSize,  0.0f)))
        return E_FAIL;

    // 데미지 잔상 바
    if (FAILED(Add_UIRect(TEXT("HP_Damage"), TEXT("HP_Damage"), TEXT("Prototype_Com_Texture_Graph"), m_vHPBarPos, m_vHPBarSize, 0.0f)))
        return E_FAIL;

    // 실제 HP 바
    if (FAILED(Add_UIRect( TEXT("HP_Fill"),TEXT("HP_Fill"),  TEXT("Prototype_Com_Texture_Graph"), m_vHPBarPos,  m_vHPBarSize,  0.0f)))
        return E_FAIL;

    // 기본 색상

    m_UIRects[TEXT("HP_Back")].vColor = { 0.05f, 0.05f, 0.05f, 1.f };
    m_UIRects[TEXT("HP_Back")].fAlpha = 0.75f;

    m_UIRects[TEXT("HP_Damage")].vColor = { 1.f, 0.75f, 0.15f, 1.f };
    m_UIRects[TEXT("HP_Damage")].fAlpha = 0.9f;

    m_UIRects[TEXT("HP_Fill")].vColor = { 0.9f, 0.05f, 0.05f, 1.f };
    m_UIRects[TEXT("HP_Fill")].fAlpha = 1.f;

    return S_OK;
}

void Player_State_UI::Priority_Update(_float fTimeDelta)
{
}

void Player_State_UI::Update(_float fTimeDelta)
{
    Update_HPBar(fTimeDelta);
}

void Player_State_UI::Late_Update(_float fTimeDelta)
{
}

HRESULT Player_State_UI::Render()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    // 순서 중요
    if (FAILED(Render_UIRect_ByKey(TEXT("HP_Back"))))
        return E_FAIL;

    if (FAILED(Render_UIRect_ByKey(TEXT("HP_Damage"))))
        return E_FAIL;

    if (FAILED(Render_UIRect_ByKey(TEXT("HP_Fill"))))
        return E_FAIL;


    return S_OK;
}

HRESULT Player_State_UI::Ready_Components()
{
    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_VtxPosTex"), TEXT("Com_Shader"), m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component( CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Rect"), TEXT("Com_VIBuffer"),m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT Player_State_UI::Add_UIRect(const wstring& UIName,const wstring& strName,const wstring& strTextureTag, const _float2& vPos,const _float2& vSize,_float fDepth)
{
    UI_RECT UI{};

    UI.strName = strName;
    UI.strTextureTag = strTextureTag;
    UI.vPos = vPos;
    UI.vSize = vSize;
    UI.fDepth = fDepth;
    UI.iTextureIndex = 0;
    UI.bVisible = true;
    UI.fAlpha = 1.f;
    UI.vColor = { 1.f, 1.f, 1.f, 1.f };

    if (FAILED(__super::Add_Component(
        CGameInstance::Get().Get_Level(),
        strTextureTag.c_str(),
        UI.strName,
        UI.pTexture)))
        return E_FAIL;

    m_UIRects[UIName] = UI;

    return S_OK;
}

HRESULT Player_State_UI::Render_UIRect_ByKey(const wstring& strKey)
{
    auto iter = m_UIRects.find(strKey);
    if (iter == m_UIRects.end())
        return S_OK;

    UI_RECT& UI = iter->second;

    if (false == UI.bVisible)
        return S_OK;

    if (nullptr == UI.pTexture)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha",&UI.fAlpha, sizeof(float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor",&UI.vColor,sizeof(_float4))))
        return E_FAIL;

    if (FAILED(Render_UIRect(UI)))
        return E_FAIL;

    return S_OK;
}

HRESULT Player_State_UI::Render_UIRect(UI_RECT& UI)
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float fX = UI.vPos.x - vViewportSize.x * 0.5f;
    _float fY = -UI.vPos.y + vViewportSize.y * 0.5f;

    m_pTransformCom->Set_Scale(UI.vSize.x, UI.vSize.y, 1.f);
    m_pTransformCom->Set_State( STATE::POSITION,XMVectorSet(fX, fY, UI.fDepth, 1.f));

    if (FAILED(m_pTransformCom->Bind_ShaderResource( m_pShaderCom,"g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(UI.pTexture->Bind_ShaderResource( m_pShaderCom,"g_Texture",UI.iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}
void Player_State_UI::Set_HP(float fCurHP, float fMaxHP)
{
    m_fMaxHP = max(fMaxHP, 1.f);
    m_fCurHP = ClampFloat(fCurHP, 0.f, m_fMaxHP);
}

void Player_State_UI::Update_HPBar(_float fTimeDelta)
{
    float fTargetRatio = 0.f;

    if (m_fMaxHP > 0.f)
        fTargetRatio = m_fCurHP / m_fMaxHP;

    fTargetRatio = ClampFloat(fTargetRatio, 0.f, 1.f);

    const float fHpSpeed = 18.f;

    m_fHpRatio = LerpFloat( m_fHpRatio, fTargetRatio, fTimeDelta * fHpSpeed);

    if (fTargetRatio < m_fDamageRatio)
    {
        const float fDamageSpeed = 3.5f;

        m_fDamageRatio = LerpFloat( m_fDamageRatio,fTargetRatio, fTimeDelta * fDamageSpeed );
    }
    else
    {
        m_fDamageRatio = fTargetRatio;
    }

    Update_HPBarVisual();
}

void Player_State_UI::Update_HPBarVisual()
{
    auto iterBack = m_UIRects.find(TEXT("HP_Back"));
    if (iterBack == m_UIRects.end())
        return;

    UI_RECT& BackRect = iterBack->second;

    _float2 vBasePos = BackRect.vPos;
    _float2 vBaseSize = BackRect.vSize;

    Set_BarRatio(TEXT("HP_Damage"), vBasePos, vBaseSize, m_fDamageRatio);
    Set_BarRatio(TEXT("HP_Fill"), vBasePos, vBaseSize, m_fHpRatio);
}
void Player_State_UI::Set_BarRatio( const wstring& strKey,const _float2& vBasePos, const _float2& vBaseSize,float fRatio)
{
    fRatio = ClampFloat(fRatio, 0.f, 1.f);

    auto iter = m_UIRects.find(strKey);
    if (iter == m_UIRects.end())
        return;

    UI_RECT& Bar = iter->second;

    float fNewWidth = vBaseSize.x * fRatio;
    float fLeft = vBasePos.x - vBaseSize.x * 0.5f;

    Bar.vPos.x = fLeft + fNewWidth * 0.5f;
    Bar.vPos.y = vBasePos.y;

    Bar.vSize.x = fNewWidth;
    Bar.vSize.y = vBaseSize.y;

    Bar.bVisible = fRatio > 0.001f;
}
float Player_State_UI::ClampFloat(float fValue, float fMin, float fMax)
{
    if (fValue < fMin)
        return fMin;

    if (fValue > fMax)
        return fMax;

    return fValue;
}

float Player_State_UI::LerpFloat(float fStart, float fEnd, float fRatio)
{
    fRatio = ClampFloat(fRatio, 0.f, 1.f);

    return fStart + (fEnd - fStart) * fRatio;
}

unique_ptr<Player_State_UI> Player_State_UI::Create( ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext)
{
    unique_ptr<Player_State_UI> pInstance =unique_ptr<Player_State_UI>( new Player_State_UI(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Player_State_UI");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Player_State_UI::Clone(void* pArg)
{
    shared_ptr<Player_State_UI> pInstance =shared_ptr<Player_State_UI>(new Player_State_UI(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Player_State_UI");
        return nullptr;
    }

    return pInstance;
}

