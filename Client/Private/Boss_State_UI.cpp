#include "Boss_State_UI.h"
#include "BossMonster.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "LittleMonster.h"
#include <fstream>
#include <sstream>

Boss_State_UI::Boss_State_UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : UIObject{ pDevice, pContext }
{
}

Boss_State_UI::Boss_State_UI(const Boss_State_UI& Prototype) : UIObject{ Prototype }
{
}

Boss_State_UI::~Boss_State_UI()
{
}

HRESULT Boss_State_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Boss_State_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_UIRects.clear();


    // HP 배경
    if (FAILED(Add_UIRect(TEXT("HP_Back"), TEXT("HP_Back"), TEXT("Prototype_Com_Texture_Graph"), m_vHPBarPos, m_vHPBarSize, 0.0f)))
        return E_FAIL;

    // 데미지 잔상 바
    if (FAILED(Add_UIRect(TEXT("HP_Damage"), TEXT("HP_Damage"), TEXT("Prototype_Com_Texture_Graph"), m_vHPBarPos, m_vHPBarSize, 0.0f)))
        return E_FAIL;

    // 실제 HP 바
    if (FAILED(Add_UIRect(TEXT("HP_Fill"), TEXT("HP_Fill"), TEXT("Prototype_Com_Texture_Graph"), m_vHPBarPos, m_vHPBarSize, 0.0f)))
        return E_FAIL;

    // 기본 색상

    m_UIRects[TEXT("HP_Back")].vColor = { 0.05f, 0.05f, 0.05f, 1.f };
    m_UIRects[TEXT("HP_Back")].fAlpha = 0.75f;

    m_UIRects[TEXT("HP_Damage")].vColor = { 1.f, 0.75f, 0.15f, 1.f };
    m_UIRects[TEXT("HP_Damage")].fAlpha = 0.9f;

    m_UIRects[TEXT("HP_Fill")].vColor = { 0.9f, 0.05f, 0.05f, 1.f };
    m_UIRects[TEXT("HP_Fill")].fAlpha = 1.f;

    Load_UIRects(TEXT("../../Resources/Data/UI/LittleMonster_State_UI.txt"));
    m_vBillboardHPSize = { 0.8f, 0.14f };
    m_vBillboardOffset = { 0.f, 1.5f, 0.f };

    // 빌보드에서는 padding도 월드 단위라 작아야 함
    m_fHPBarPaddingX = 0.03f;
    m_fHPBarPaddingY = 0.015f;
    return S_OK;
}

void Boss_State_UI::Priority_Update(_float fTimeDelta)
{
}

void Boss_State_UI::Update(_float fTimeDelta)
{
    Update_FollowOwner();
    Update_HPBar(fTimeDelta);

#ifdef _DEBUG
    GUI_BossStateUI();
#endif
}

void Boss_State_UI::Late_Update(_float fTimeDelta)
{
}

HRESULT Boss_State_UI::Render()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    if (false == m_bVisible)
        return S_OK;

    if (FAILED(Render_BillboardRect(TEXT("HP_Back"))))
        return E_FAIL;

    if (FAILED(Render_BillboardRect(TEXT("HP_Damage"))))
        return E_FAIL;

    if (FAILED(Render_BillboardRect(TEXT("HP_Fill"))))
        return E_FAIL;

    return S_OK;
}

HRESULT Boss_State_UI::Render_BillboardRect(const wstring& strKey)
{
    auto iter = m_UIRects.find(strKey);
    if (iter == m_UIRects.end())
        return S_OK;

    UI_RECT& UI = iter->second;

    if (false == UI.bVisible)
        return S_OK;

    if (nullptr == UI.pTexture)
        return S_OK;

    _float4x4 ViewMatrix{};
    _float4x4 ProjMatrix{};

    CGameInstance::Get().Get_MainCamerwaViewMatrix(ViewMatrix);
    CGameInstance::Get().Get_MainCamerwaProjectionMatrix(ProjMatrix);

    _matrix matView = XMLoadFloat4x4(&ViewMatrix);
    _matrix matProj = XMLoadFloat4x4(&ProjMatrix);

    _matrix matInvView =
        XMMatrixInverse(nullptr, matView);

    _vector vCamRight =
        XMVector3Normalize(matInvView.r[0]);

    _vector vCamUp =
        XMVector3Normalize(matInvView.r[1]);

    _vector vCamLook =
        XMVector3Normalize(matInvView.r[2]);

    _vector vCenter =
        XMLoadFloat3(&m_vBillboardCenter);



    // UI.vPos는 빌보드 내부 로컬 오프셋으로 사용
    vCenter += vCamRight * UI.vPos.x;
    vCenter += vCamUp * UI.vPos.y;

    // HP_Back, HP_Damage, HP_Fill이 같은 평면에 겹치지 않도록 깊이 분리
    vCenter -= vCamLook * UI.fDepth;

    _matrix matWorld = XMMatrixIdentity();

    matWorld.r[0] = vCamRight * UI.vSize.x;
    matWorld.r[1] = vCamUp * UI.vSize.y;
    matWorld.r[2] = vCamLook;
    matWorld.r[3] = XMVectorSetW(vCenter, 1.f);

    _float4x4 World{};
    _float4x4 View{};
    _float4x4 Proj{};

    XMStoreFloat4x4(&World, matWorld);
    XMStoreFloat4x4(&View, matView);
    XMStoreFloat4x4(&Proj, matProj);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &UI.fAlpha, sizeof(float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &UI.vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(UI.pTexture->Bind_ShaderResource(m_pShaderCom, "g_Texture", UI.iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

void Boss_State_UI::Set_BarRatio_Billboard(const wstring& strKey, const _float2& vBaseSize, float fRatio)
{
    fRatio = ClampFloat(fRatio, 0.f, 1.f);

    auto iter = m_UIRects.find(strKey);
    if (iter == m_UIRects.end())
        return;

    UI_RECT& Bar = iter->second;

    float fNewWidth = vBaseSize.x * fRatio;
    float fLeft = -vBaseSize.x * 0.5f;

    // 빌보드 로컬 좌표 기준
    Bar.vPos.x = fLeft + fNewWidth * 0.5f;
    Bar.vPos.y = 0.f;

    Bar.vSize.x = fNewWidth;
    Bar.vSize.y = vBaseSize.y;

    Bar.bVisible = fRatio > 0.001f;
}

void Boss_State_UI::Update_HPBarVisual_Billboard()
{
    auto iterBack = m_UIRects.find(TEXT("HP_Back"));
    if (iterBack == m_UIRects.end())
        return;

    UI_RECT& Back = iterBack->second;

    Back.vPos = { 0.f, 0.f };
    Back.vSize = m_vBillboardHPSize;
    Back.fDepth = 0.000f;
    Back.bVisible = true;

    _float2 vInnerSize =
    {
        m_vBillboardHPSize.x - m_fHPBarPaddingX * 2.f,
        m_vBillboardHPSize.y - m_fHPBarPaddingY * 2.f
    };

    if (vInnerSize.x < 0.01f)
        vInnerSize.x = 0.01f;

    if (vInnerSize.y < 0.01f)
        vInnerSize.y = 0.01f;

    Set_BarRatio_Billboard(TEXT("HP_Damage"), vInnerSize, m_fDamageRatio);
    Set_BarRatio_Billboard(TEXT("HP_Fill"), vInnerSize, m_fHpRatio);

    auto iterDamage = m_UIRects.find(TEXT("HP_Damage"));
    if (iterDamage != m_UIRects.end())
    {
        iterDamage->second.fDepth = 0.01f;
        iterDamage->second.bVisible = m_fDamageRatio > 0.001f;
    }

    auto iterFill = m_UIRects.find(TEXT("HP_Fill"));
    if (iterFill != m_UIRects.end())
    {
        iterFill->second.fDepth = 0.05f;
        iterFill->second.bVisible = m_fHpRatio > 0.001f;
    }
}

HRESULT Boss_State_UI::Ready_Components()
{
    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_VtxPosTex"), TEXT("Com_Shader"), m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Rect"), TEXT("Com_VIBuffer"), m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT Boss_State_UI::Add_UIRect(const wstring& UIName, const wstring& strName, const wstring& strTextureTag, const _float2& vPos, const _float2& vSize, _float fDepth)
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

    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(), strTextureTag.c_str(), UI.strName, UI.pTexture)))
        return E_FAIL;

    m_UIRects[UIName] = UI;

    return S_OK;
}

HRESULT Boss_State_UI::Render_UIRect_ByKey(const wstring& strKey)
{
    auto iter = m_UIRects.find(strKey);
    if (iter == m_UIRects.end())
        return S_OK;

    UI_RECT& UI = iter->second;

    if (false == UI.bVisible)
        return S_OK;

    if (nullptr == UI.pTexture)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &UI.fAlpha, sizeof(float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &UI.vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(Render_UIRect(UI)))
        return E_FAIL;

    return S_OK;
}

HRESULT Boss_State_UI::Render_UIRect(UI_RECT& UI)
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
void Boss_State_UI::Set_HP(float fCurHP, float fMaxHP)
{
    m_fMaxHP = max(fMaxHP, 1.f);
    m_fCurHP = ClampFloat(fCurHP, 0.f, m_fMaxHP);
}

void Boss_State_UI::Update_HPBar(_float fTimeDelta)
{
    float fTargetRatio = 0.f;

    if (m_fMaxHP > 0.f)
        fTargetRatio = m_fCurHP / m_fMaxHP;

    fTargetRatio = ClampFloat(fTargetRatio, 0.f, 1.f);

    const float fHpSpeed = 18.f;

    m_fHpRatio = LerpFloat(m_fHpRatio, fTargetRatio, fTimeDelta * fHpSpeed);

    if (fTargetRatio < m_fDamageRatio)
    {
        const float fDamageSpeed = 3.5f;

        m_fDamageRatio = LerpFloat(m_fDamageRatio, fTargetRatio, fTimeDelta * fDamageSpeed);
    }
    else
    {
        m_fDamageRatio = fTargetRatio;
    }

    Update_HPBarVisual_Billboard();
}

void Boss_State_UI::Update_HPBarVisual()
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
void Boss_State_UI::Set_BarRatio(const wstring& strKey, const _float2& vBasePos, const _float2& vBaseSize, float fRatio)
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
float Boss_State_UI::ClampFloat(float fValue, float fMin, float fMax)
{
    if (fValue < fMin)
        return fMin;

    if (fValue > fMax)
        return fMax;

    return fValue;
}

float Boss_State_UI::LerpFloat(float fStart, float fEnd, float fRatio)
{
    fRatio = ClampFloat(fRatio, 0.f, 1.f);

    return fStart + (fEnd - fStart) * fRatio;
}


void Boss_State_UI::Update_FollowOwner()
{
    auto pOwner = m_pOwner;

    if (pOwner == nullptr)
    {
        m_bVisible = false;

        for (auto& Pair : m_UIRects)
            Pair.second.bVisible = false;

        return;
    }

    auto pTransform = pOwner->GetTransform();

    if (pTransform == nullptr)
    {
        m_bVisible = false;

        for (auto& Pair : m_UIRects)
            Pair.second.bVisible = false;

        return;
    }

    _vector vOwnerPos =
        pTransform->Get_State(STATE::POSITION);

    vOwnerPos += XMVectorSet(
        m_vBillboardOffset.x,
        m_vBillboardOffset.y,
        m_vBillboardOffset.z,
        0.f
    );

    XMStoreFloat3(&m_vBillboardCenter, vOwnerPos);

    m_bVisible = true;

    for (auto& Pair : m_UIRects)
        Pair.second.bVisible = true;
}
unique_ptr<Boss_State_UI> Boss_State_UI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    unique_ptr<Boss_State_UI> pInstance = unique_ptr<Boss_State_UI>(new Boss_State_UI(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Boss_State_UI");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Boss_State_UI::Clone(void* pArg)
{
    shared_ptr<Boss_State_UI> pInstance = shared_ptr<Boss_State_UI>(new Boss_State_UI(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Boss_State_UI");
        return nullptr;
    }

    return pInstance;
}

#ifdef _DEBUG
void Boss_State_UI::GUI_BossStateUI()
{
    string strWindowName = "Player State UI Editor##";
    strWindowName += to_string(reinterpret_cast<size_t>(this));

    if (ImGui::Begin(strWindowName.c_str()))
    {
        if (ImGui::Button("Save UI"))
        {
            Save_UIRects(TEXT("../../Resources/Data/UI/BossMonster_State_UI.txt"));
        }

        ImGui::SameLine();

        if (ImGui::Button("Load UI"))
        {
            Load_UIRects(TEXT("../../Resources/Data/UI/BossMonster_State_UI.txt"));
        }

        ImGui::Separator();
        ImGui::Text("Follow Player Setting");

        ImGui::DragFloat3(
            "World Offset",
            reinterpret_cast<float*>(&m_vWorldOffset),
            0.05f,
            -10.f,
            10.f
        );

        ImGui::DragFloat2(
            "Screen Offset",
            reinterpret_cast<float*>(&m_vScreenOffset),
            1.f,
            -500.f,
            500.f
        );

        ImGui::DragFloat2(
            "HP Bar Size",
            reinterpret_cast<float*>(&m_vHPBarSize),
            1.f,
            1.f,
            500.f
        );
        ImGui::Separator();

        ImGui::Text("HP Test");

        ImGui::DragFloat("Current HP", &m_fCurHP, 1.f, 0.f, m_fMaxHP);
        ImGui::DragFloat("Max HP", &m_fMaxHP, 1.f, 1.f, 9999.f);

        if (ImGui::Button("Full HP"))
        {
            Set_HP(m_fMaxHP, m_fMaxHP);
        }

        ImGui::SameLine();

        if (ImGui::Button("Damage 10"))
        {
            Set_HP(m_fCurHP - 10.f, m_fMaxHP);


            _vector vPos = m_pOwner->GetTransform()->Get_State(STATE::POSITION);

            _float3 vPosFloat3;

            XMStoreFloat3(&vPosFloat3, vPos);

            static_pointer_cast<BossMonster>(m_pOwner)->Take_Damage(10.f, vPosFloat3);


        }

        ImGui::SameLine();

        if (ImGui::Button("Heal 10"))
        {
            Set_HP(m_fCurHP + 10.f, m_fMaxHP);
        }

        ImGui::Separator();

        ImGui::Text("Important");
        ImGui::Text("Edit HP_Back Position / Size.");
        ImGui::Text("HP_Damage and HP_Fill follow HP_Back automatically.");

        ImGui::Separator();

        int i = 0;

        for (auto& Pair : m_UIRects)
        {
            const wstring& UIName = Pair.first;
            UI_RECT& UI = Pair.second;

            string strName = CGameInstance::Get().WStringToString(UIName);

            string strHeader = "UI Rect ";
            strHeader += to_string(i);
            strHeader += " : ";
            strHeader += strName;

            ImGui::PushID(("PlayerStateUI_" + strName).c_str());

            if (ImGui::CollapsingHeader(strHeader.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Visible", reinterpret_cast<bool*>(&UI.bVisible));

                bool bBarFollowObject =
                    UIName == TEXT("HP_Damage") ||
                    UIName == TEXT("HP_Fill");

                if (bBarFollowObject)
                {
                    ImGui::Text("Position / Size follows HP_Back.");
                    ImGui::BeginDisabled();

                    ImGui::DragFloat2(
                        "Position",
                        reinterpret_cast<float*>(&UI.vPos),
                        1.f,
                        -5000.f,
                        5000.f
                    );

                    ImGui::DragFloat2(
                        "Size",
                        reinterpret_cast<float*>(&UI.vSize),
                        1.f,
                        0.f,
                        5000.f
                    );

                    ImGui::EndDisabled();
                }
                else
                {
                    ImGui::DragFloat2(
                        "Position",
                        reinterpret_cast<float*>(&UI.vPos),
                        1.f,
                        -5000.f,
                        5000.f
                    );

                    ImGui::DragFloat2(
                        "Size",
                        reinterpret_cast<float*>(&UI.vSize),
                        1.f,
                        0.f,
                        5000.f
                    );
                }

                ImGui::DragFloat(
                    "Depth",
                    &UI.fDepth,
                    0.001f,
                    0.f,
                    1.f
                );

                ImGui::SliderFloat(
                    "Alpha",
                    &UI.fAlpha,
                    0.f,
                    1.f
                );

                ImGui::ColorEdit4(
                    "Color",
                    reinterpret_cast<float*>(&UI.vColor)
                );

                int iTexIndex = static_cast<int>(UI.iTextureIndex);

                if (ImGui::InputInt("Texture Index", &iTexIndex))
                {
                    if (iTexIndex < 0)
                        iTexIndex = 0;

                    UI.iTextureIndex = static_cast<uint32_t>(iTexIndex);
                }

                string strTextureTag =
                    CGameInstance::Get().WStringToString(UI.strTextureTag);

                ImGui::Text("Texture Prototype");
                ImGui::Text("%s", strTextureTag.c_str());

                if (ImGui::Button("Reset Rect"))
                {
                    UI.fDepth = 0.f;
                }

                if (ImGui::Button("Reset Basic Value"))
                {
                    UI.fDepth = 0.f;
                    UI.bVisible = true;
                    UI.iTextureIndex = 0;
                    UI.fAlpha = 1.f;
                    UI.vColor = { 1.f, 1.f, 1.f, 1.f };

                    if (UIName == TEXT("HP_Back"))
                    {
                        UI.vPos = { 180.f, 50.f };
                        UI.vSize = { 260.f, 24.f };
                        UI.vColor = { 0.05f, 0.05f, 0.05f, 1.f };
                        UI.fAlpha = 0.75f;
                    }
                    else if (UIName == TEXT("HP_Damage"))
                    {
                        UI.vColor = { 1.f, 0.75f, 0.15f, 1.f };
                        UI.fAlpha = 0.9f;
                    }
                    else if (UIName == TEXT("HP_Fill"))
                    {
                        UI.vColor = { 0.9f, 0.05f, 0.05f, 1.f };
                        UI.fAlpha = 1.f;
                    }
                }
            }

            ImGui::PopID();
            ImGui::Separator();

            ++i;
        }
    }

    ImGui::End();
}
#endif

HRESULT Boss_State_UI::Save_UIRects(const wstring& strFilePath)
{
    ofstream ofs(strFilePath);

    if (!ofs.is_open())
        return E_FAIL;

    for (auto& Pair : m_UIRects)
    {
        const wstring& strName = Pair.first;
        const UI_RECT& UI = Pair.second;

        ofs
            << CGameInstance::Get().WStringToString(strName) << "|"
            << CGameInstance::Get().WStringToString(UI.strTextureTag) << "|"
            << UI.vPos.x << "|"
            << UI.vPos.y << "|"
            << UI.vSize.x << "|"
            << UI.vSize.y << "|"
            << UI.fDepth << "|"
            << static_cast<int>(UI.bVisible) << "|"
            << UI.iTextureIndex << "|"
            << UI.fAlpha << "|"
            << UI.vColor.x << "|"
            << UI.vColor.y << "|"
            << UI.vColor.z << "|"
            << UI.vColor.w
            << endl;
    }

    ofs.close();

    return S_OK;
}

HRESULT Boss_State_UI::Load_UIRects(const wstring& strFilePath)
{
    ifstream ifs(strFilePath);

    if (!ifs.is_open())
        return S_OK;

    string strLine;

    while (getline(ifs, strLine))
    {
        if (strLine.empty())
            continue;

        stringstream ss(strLine);
        string token;
        vector<string> Tokens;

        while (getline(ss, token, '|'))
            Tokens.push_back(token);

        if (Tokens.size() < 9)
            continue;

        wstring strName =
            CGameInstance::Get().StringToWString(Tokens[0]);

        auto iter = m_UIRects.find(strName);

        if (iter == m_UIRects.end())
            continue;

        UI_RECT& UI = iter->second;

        try
        {
            UI.strTextureTag =
                CGameInstance::Get().StringToWString(Tokens[1]);

            UI.vPos =
            {
                stof(Tokens[2]),
                stof(Tokens[3])
            };

            UI.vSize =
            {
                stof(Tokens[4]),
                stof(Tokens[5])
            };

            UI.fDepth = stof(Tokens[6]);
            UI.bVisible = stoi(Tokens[7]) != 0;
            UI.iTextureIndex = static_cast<uint32_t>(stoi(Tokens[8]));

            if (Tokens.size() >= 10)
                UI.fAlpha = stof(Tokens[9]);
            else
                UI.fAlpha = 1.f;

            if (Tokens.size() >= 14)
            {
                UI.vColor =
                {
                    stof(Tokens[10]),
                    stof(Tokens[11]),
                    stof(Tokens[12]),
                    stof(Tokens[13])
                };
            }
            else
            {
                UI.vColor = { 1.f, 1.f, 1.f, 1.f };
            }
        }
        catch (...)
        {
            continue;
        }
    }

    ifs.close();

    return S_OK;
}