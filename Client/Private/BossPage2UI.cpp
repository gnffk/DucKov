#include "BossPage2UI.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"

NS_BEGIN(Client)

BossPage2UI::BossPage2UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : UIObject{ pDevice, pContext }
{
}

BossPage2UI::BossPage2UI(const BossPage2UI& Prototype)
    : UIObject{ Prototype }
{
}

BossPage2UI::~BossPage2UI()
{
}

HRESULT BossPage2UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT BossPage2UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_UIRects.clear();

    _float2 vDummyPos = { 0.f, 0.f };

    // 보스 이름 이미지
    // 이 텍스처 프로토타입은 따로 등록해야 함.
    if (FAILED(Add_UIRect(TEXT("BossName"),TEXT("BossName"),TEXT("Prototype_Com_Texture_BossPage2_Name"),vDummyPos,m_vNameSize,0.0f)))
        return E_FAIL;

    // HP 배경
    if (FAILED(Add_UIRect(TEXT("HP_Back"),TEXT("HP_Back"),TEXT("Prototype_Com_Texture_Graph"),vDummyPos,m_vHPBarSize,0.0f)))
        return E_FAIL;

    // 데미지 잔상 바
    if (FAILED(Add_UIRect(TEXT("HP_Damage"),TEXT("HP_Damage"),TEXT("Prototype_Com_Texture_Graph"),vDummyPos,m_vHPBarSize,0.0f)))
        return E_FAIL;

    // 실제 HP 바
    if (FAILED(Add_UIRect(TEXT("HP_Fill"),TEXT("HP_Fill"),TEXT("Prototype_Com_Texture_Graph"),vDummyPos,m_vHPBarSize,0.0f)))
        return E_FAIL;

    m_UIRects[TEXT("BossName")].vColor = { 1.f, 1.f, 1.f, 1.f };

    m_UIRects[TEXT("HP_Back")].vColor = { 0.05f, 0.05f, 0.05f, 1.f };
    m_UIRects[TEXT("HP_Back")].fAlpha = 0.75f;

    m_UIRects[TEXT("HP_Damage")].vColor = { 1.f, 0.75f, 0.15f, 1.f };
    m_UIRects[TEXT("HP_Damage")].fAlpha = 0.9f;

    m_UIRects[TEXT("HP_Fill")].vColor = { 0.85f, 0.05f, 0.05f, 1.f };
    m_UIRects[TEXT("HP_Fill")].fAlpha = 1.f;

    Set_AllVisible(false);
    Set_AllAlpha(0.f);

    return S_OK;
}

void BossPage2UI::Priority_Update(_float fTimeDelta)
{
}

void BossPage2UI::Update(_float fTimeDelta)
{
    Update_Appear(fTimeDelta);
    Update_Layout();
    Update_HPBar(fTimeDelta);

//#ifdef _DEBUG
//    GUI_BossPage2UI();
//#endif
}

void BossPage2UI::Late_Update(_float fTimeDelta)
{
}

HRESULT BossPage2UI::Render()
{
    if (m_eUIState == UI_STATE::HIDDEN)
        return S_OK;

    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    // 렌더 순서 중요
    if (FAILED(Render_UIRect_ByKey(TEXT("BossName"))))
        return E_FAIL;

    if (FAILED(Render_UIRect_ByKey(TEXT("HP_Back"))))
        return E_FAIL;

    if (FAILED(Render_UIRect_ByKey(TEXT("HP_Damage"))))
        return E_FAIL;

    if (FAILED(Render_UIRect_ByKey(TEXT("HP_Fill"))))
        return E_FAIL;

    return S_OK;
}

HRESULT BossPage2UI::Ready_Components()
{
    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_MainUI"),TEXT("Com_Shader"),m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component( CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_VIBuffer_Rect"),TEXT("Com_VIBuffer"),m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT BossPage2UI::Add_UIRect(const wstring& UIName,const wstring& strName,const wstring& strTextureTag,const _float2& vPos, const _float2& vSize,_float fDepth)
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

    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(),strTextureTag.c_str(),UI.strName,UI.pTexture)))
        return E_FAIL;

    m_UIRects[UIName] = UI;

    return S_OK;
}



HRESULT BossPage2UI::Render_UIRect_ByKey(const wstring& strKey)
{
    auto iter = m_UIRects.find(strKey);

    if (iter == m_UIRects.end())
        return S_OK;

    UI_RECT& UI = iter->second;

    if (false == UI.bVisible)
        return S_OK;

    if (nullptr == UI.pTexture)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &UI.fAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &UI.vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(Render_UIRect(UI)))
        return E_FAIL;

    return S_OK;
}

HRESULT BossPage2UI::Render_UIRect(UI_RECT& UI)
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float fX = UI.vPos.x - vViewportSize.x * 0.5f;
    _float fY = -UI.vPos.y + vViewportSize.y * 0.5f;

    m_pTransformCom->Set_Scale(UI.vSize.x, UI.vSize.y, 1.f);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(fX, fY, UI.fDepth, 1.f));

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(UI.pTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture", UI.iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

void BossPage2UI::Start_Appear()
{
    m_eUIState = UI_STATE::APPEAR;
    m_fAppearTimer = 0.f;
    m_fAlpha = 0.f;

    Set_AllVisible(true);
    Set_AllAlpha(0.f);
}

void BossPage2UI::Start_Disappear()
{
    m_eUIState = UI_STATE::DISAPPEAR;
    m_fAppearTimer = 0.f;
}

void BossPage2UI::Update_Appear(_float fTimeDelta)
{
    if (m_eUIState == UI_STATE::HIDDEN)
        return;

    if (m_eUIState == UI_STATE::SHOW)
    {
        m_fAlpha = 1.f;
        Set_AllAlpha(m_fAlpha);
        return;
    }

    m_fAppearTimer += fTimeDelta;

    if (m_eUIState == UI_STATE::APPEAR)
    {
        _float fRatio = m_fAppearTimer / m_fAppearDuration;
        fRatio = ClampFloat(fRatio, 0.f, 1.f);

        m_fAlpha = SmoothStep(fRatio);

        if (fRatio >= 1.f)
        {
            m_fAlpha = 1.f;
            m_eUIState = UI_STATE::SHOW;
        }

        Set_AllAlpha(m_fAlpha);
    }
    else if (m_eUIState == UI_STATE::DISAPPEAR)
    {
        _float fRatio = m_fAppearTimer / m_fDisappearDuration;
        fRatio = ClampFloat(fRatio, 0.f, 1.f);

        m_fAlpha = 1.f - SmoothStep(fRatio);

        if (fRatio >= 1.f)
        {
            m_fAlpha = 0.f;
            m_eUIState = UI_STATE::HIDDEN;
            Set_AllVisible(false);
        }

        Set_AllAlpha(m_fAlpha);
    }
}

void BossPage2UI::Update_Layout()
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float fCenterX = vViewportSize.x * 0.5f;


    _float fSlideOffset = (1.f - m_fAlpha) * m_fSlideStartOffsetY;

    m_vNamePos =
    {
        fCenterX + m_fNameOffsetX,
        m_fNameTargetY + fSlideOffset
    };

    m_vHPBarPos =
    {
        fCenterX + m_fHPBarOffsetX,
        m_fHPBarTargetY + fSlideOffset
    };

    auto iterName = m_UIRects.find(TEXT("BossName"));
    if (iterName != m_UIRects.end())
    {
        iterName->second.vPos = m_vNamePos;
        iterName->second.vSize = m_vNameSize;
    }

    auto iterBack = m_UIRects.find(TEXT("HP_Back"));
    if (iterBack != m_UIRects.end())
    {
        iterBack->second.vPos = m_vHPBarPos;
        iterBack->second.vSize = m_vHPBarSize;
    }
}

void BossPage2UI::Set_HP(_float fCurHP, _float fMaxHP)
{
    m_fMaxHP = max(fMaxHP, 1.f);
    m_fCurHP = ClampFloat(fCurHP, 0.f, m_fMaxHP);
}

void BossPage2UI::Update_HPBar(_float fTimeDelta)
{
    _float fTargetRatio = 0.f;

    if (m_fMaxHP > 0.f)
        fTargetRatio = m_fCurHP / m_fMaxHP;

    fTargetRatio = ClampFloat(fTargetRatio, 0.f, 1.f);

    const _float fHpSpeed = 18.f;

    m_fHpRatio = LerpFloat(
        m_fHpRatio,
        fTargetRatio,
        fTimeDelta * fHpSpeed
    );

    if (fTargetRatio < m_fDamageRatio)
    {
        const _float fDamageSpeed = 3.5f;

        m_fDamageRatio = LerpFloat(
            m_fDamageRatio,
            fTargetRatio,
            fTimeDelta * fDamageSpeed
        );
    }
    else
    {
        m_fDamageRatio = fTargetRatio;
    }

    Update_HPBarVisual();
}

void BossPage2UI::Update_HPBarVisual()
{
    auto iterBack = m_UIRects.find(TEXT("HP_Back"));

    if (iterBack == m_UIRects.end())
        return;

    UI_RECT& BackRect = iterBack->second;

    _float2 vBasePos = BackRect.vPos;
    _float2 vBaseSize = BackRect.vSize;

    _float2 vInnerPos = vBasePos;

    _float2 vInnerSize =
    {
        vBaseSize.x - m_fHPBarPaddingX * 2.f,
        vBaseSize.y - m_fHPBarPaddingY * 2.f
    };

    if (vInnerSize.x < 1.f)
        vInnerSize.x = 1.f;

    if (vInnerSize.y < 1.f)
        vInnerSize.y = 1.f;

    Set_BarRatio(TEXT("HP_Damage"), vInnerPos, vInnerSize, m_fDamageRatio);
    Set_BarRatio(TEXT("HP_Fill"), vInnerPos, vInnerSize, m_fHpRatio);
}

void BossPage2UI::Set_BarRatio(
    const wstring& strKey,
    const _float2& vBasePos,
    const _float2& vBaseSize,
    _float fRatio)
{
    fRatio = ClampFloat(fRatio, 0.f, 1.f);

    auto iter = m_UIRects.find(strKey);

    if (iter == m_UIRects.end())
        return;

    UI_RECT& Bar = iter->second;

    _float fNewWidth = vBaseSize.x * fRatio;
    _float fLeft = vBasePos.x - vBaseSize.x * 0.5f;

    Bar.vPos.x = fLeft + fNewWidth * 0.5f;
    Bar.vPos.y = vBasePos.y;

    Bar.vSize.x = fNewWidth;
    Bar.vSize.y = vBaseSize.y;

    Bar.bVisible = fRatio > 0.001f;
}

void BossPage2UI::Set_AllAlpha(_float fAlpha)
{
    fAlpha = ClampFloat(fAlpha, 0.f, 1.f);

    for (auto& Pair : m_UIRects)
    {
        Pair.second.fAlpha = fAlpha;
    }

    // 기존 개별 알파를 살리고 싶으면 여기서 따로 조절
    auto iterBack = m_UIRects.find(TEXT("HP_Back"));
    if (iterBack != m_UIRects.end())
        iterBack->second.fAlpha = fAlpha * 0.75f;

    auto iterDamage = m_UIRects.find(TEXT("HP_Damage"));
    if (iterDamage != m_UIRects.end())
        iterDamage->second.fAlpha = fAlpha * 0.9f;
}

void BossPage2UI::Set_AllVisible(_bool bVisible)
{
    for (auto& Pair : m_UIRects)
    {
        Pair.second.bVisible = bVisible;
    }
}

void BossPage2UI::Set_BossNameTexture(const wstring& strTextureTag)
{
    auto iter = m_UIRects.find(TEXT("BossName"));

    if (iter == m_UIRects.end())
        return;

    UI_RECT& UI = iter->second;

    UI.strTextureTag = strTextureTag;

    UI.pTexture = dynamic_pointer_cast<Texture>(
        CGameInstance::Get().Clone_Prototype(
            CGameInstance::Get().Get_Level(),
            strTextureTag
        )
    );

    UI.iTextureIndex = 0;
}

_float BossPage2UI::ClampFloat(_float fValue, _float fMin, _float fMax)
{
    if (fValue < fMin)
        return fMin;

    if (fValue > fMax)
        return fMax;

    return fValue;
}

_float BossPage2UI::LerpFloat(_float fStart, _float fEnd, _float fRatio)
{
    fRatio = ClampFloat(fRatio, 0.f, 1.f);

    return fStart + (fEnd - fStart) * fRatio;
}

_float BossPage2UI::SmoothStep(_float fRatio)
{
    fRatio = ClampFloat(fRatio, 0.f, 1.f);

    return fRatio * fRatio * (3.f - 2.f * fRatio);
}

unique_ptr<BossPage2UI> BossPage2UI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    unique_ptr<BossPage2UI> pInstance = unique_ptr<BossPage2UI>(new BossPage2UI(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : BossPage2UI");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> BossPage2UI::Clone(void* pArg)
{
    shared_ptr<BossPage2UI> pInstance = shared_ptr<BossPage2UI>(new BossPage2UI(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : BossPage2UI");
        return nullptr;
    }

    return pInstance;
}

#ifdef _DEBUG
void BossPage2UI::GUI_BossPage2UI()
{
    string strWindowName = "Boss Page2 UI Editor##";
    strWindowName += to_string(reinterpret_cast<size_t>(this));

    if (ImGui::Begin(strWindowName.c_str()))
    {
        ImGui::Text("Boss Page2 UI Layout");

        ImGui::Separator();

        ImGui::Text("Position");

        ImGui::DragFloat("Name Y", &m_fNameTargetY, 1.f, -500.f, 1000.f);
        ImGui::DragFloat("HP Bar Y", &m_fHPBarTargetY, 1.f, -500.f, 1000.f);

        ImGui::DragFloat("Name Offset X", &m_fNameOffsetX, 1.f, -1000.f, 1000.f);
        ImGui::DragFloat("HP Bar Offset X", &m_fHPBarOffsetX, 1.f, -1000.f, 1000.f);

        ImGui::Separator();

        ImGui::Text("Size");

        ImGui::DragFloat2(
            "Name Size",
            reinterpret_cast<float*>(&m_vNameSize),
            1.f,
            1.f,
            2000.f
        );

        ImGui::DragFloat2(
            "HP Bar Size",
            reinterpret_cast<float*>(&m_vHPBarSize),
            1.f,
            1.f,
            2000.f
        );

        ImGui::Separator();

        ImGui::Text("Appear Effect");

        ImGui::DragFloat("Appear Duration", &m_fAppearDuration, 0.05f, 0.05f, 10.f);
        ImGui::DragFloat("Disappear Duration", &m_fDisappearDuration, 0.05f, 0.05f, 10.f);
        ImGui::DragFloat("Slide Start Offset Y", &m_fSlideStartOffsetY, 1.f, -300.f, 300.f);

        ImGui::Separator();

        ImGui::Text("HP Test");

        if (ImGui::Button("Full HP"))
        {
            Set_HP(m_fMaxHP, m_fMaxHP);
        }

        ImGui::SameLine();

        if (ImGui::Button("Damage 10"))
        {
            Set_HP(m_fCurHP - 10.f, m_fMaxHP);
        }

        ImGui::SameLine();

        if (ImGui::Button("Heal 10"))
        {
            Set_HP(m_fCurHP + 10.f, m_fMaxHP);
        }

        ImGui::DragFloat("Current HP", &m_fCurHP, 1.f, 0.f, m_fMaxHP);
        ImGui::DragFloat("Max HP", &m_fMaxHP, 1.f, 1.f, 99999.f);

        ImGui::Separator();

        if (ImGui::Button("Start Appear"))
        {
            Start_Appear();
        }

        ImGui::SameLine();

        if (ImGui::Button("Start Disappear"))
        {
            Start_Disappear();
        }

        ImGui::SameLine();

        if (ImGui::Button("Force Show"))
        {
            m_eUIState = UI_STATE::SHOW;
            m_fAlpha = 1.f;
            Set_AllVisible(true);
            Set_AllAlpha(1.f);
        }

        ImGui::SameLine();

        if (ImGui::Button("Force Hide"))
        {
            m_eUIState = UI_STATE::HIDDEN;
            m_fAlpha = 0.f;
            Set_AllVisible(false);
            Set_AllAlpha(0.f);
        }

        ImGui::Separator();

        ImGui::Text("Current Alpha : %.2f", m_fAlpha);
        ImGui::Text("Name Pos : %.1f, %.1f", m_vNamePos.x, m_vNamePos.y);
        ImGui::Text("HP Pos : %.1f, %.1f", m_vHPBarPos.x, m_vHPBarPos.y);
    }

    ImGui::End();
}
#endif


NS_END