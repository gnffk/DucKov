#include "InvenUI.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include <fstream>
#include <sstream>


InvenUI::InvenUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : UIObject{ pDevice, pContext }
{
}

InvenUI::InvenUI(const InvenUI& Prototype)
    : UIObject{ Prototype }
{
}

InvenUI::~InvenUI()
{
}

HRESULT InvenUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT InvenUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_UIRects.clear();


    if (FAILED(Add_UIRect(TEXT("Base_BackGround"), TEXT("Base_BackGround"), TEXT("Prototype_Com_Texture_Icon"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

  

    return S_OK;
}

void InvenUI::Priority_Update(_float fTimeDelta)
{
}

void InvenUI::Update(_float fTimeDelta)
{
#ifdef _DEBUG
    GUI_MainUI();
#endif
}

void InvenUI::Late_Update(_float fTimeDelta)
{
    CGameInstance::Get().Add_RenderObject(RENDERGROUP::UI, SHARED_THIS(InvenUI));
}

HRESULT InvenUI::Render()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    for (auto& Pair : m_UIRects)
    {
        UI_RECT& UI = Pair.second;

        if (false == UI.bVisible)
            continue;

        if (nullptr == UI.pTexture)
            continue;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &UI.fAlpha, sizeof(float))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &UI.vColor, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(Render_UIRect(UI)))
            return E_FAIL;
    }

    return S_OK;
}

void InvenUI::GUI_MainUI()
{
#ifdef _DEBUG

    if (ImGui::Begin("MainUI Editor"))
    {
        ImGui::Text("UI Rect Count : %d", static_cast<int>(m_UIRects.size()));

        if (ImGui::Button("Save UI"))
        {
            Save_UIRects(TEXT("../../Resources/Data/UI/MainUI.txt"));
        }

        ImGui::SameLine();

        if (ImGui::Button("Load UI"))
        {
            Load_UIRects(TEXT("../../Resources/Data/UI/MainUI.txt"));
        }


        ImGui::Separator();



        int i = 0;

        for (auto& Pair : m_UIRects)
        {
            const wstring& UIName = Pair.first;
            UI_RECT& UI = Pair.second;

            string strName(UIName.begin(), UIName.end());

            string strHeader = "UI Rect ";
            strHeader += to_string(i);
            strHeader += " : ";
            strHeader += strName;

            if (ImGui::CollapsingHeader(strHeader.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushID(i);

                bool bVisible = UI.bVisible;
                if (ImGui::Checkbox("Visible", &bVisible))
                {
                    UI.bVisible = bVisible;
                }

                ImGui::DragFloat2("Position", reinterpret_cast<float*>(&UI.vPos), 1.f, -5000.f, 5000.f);

                ImGui::DragFloat2("Size", reinterpret_cast<float*>(&UI.vSize), 1.f, 0.f, 5000.f);

                ImGui::DragFloat("Depth", &UI.fDepth, 0.001f, 0.f, 1.f);
                ImGui::SliderFloat("Alpha", &UI.fAlpha, 0.f, 1.f);
                ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&UI.vColor));
                int iTexIndex = static_cast<int>(UI.iTextureIndex);
                if (ImGui::InputInt("Texture Index", &iTexIndex))
                {
                    if (iTexIndex < 0)
                        iTexIndex = 0;

                    UI.iTextureIndex = static_cast<int>(iTexIndex);
                }

                ImGui::Text("Texture Prototype");

                string strTextureTag(
                    UI.strTextureTag.begin(),
                    UI.strTextureTag.end());

                ImGui::Text("%s", strTextureTag.c_str());

                if (ImGui::Button("Reset"))
                {
                    UI.vPos = { 50.f, 50.f };
                    UI.vSize = { 64.f, 64.f };
                    UI.fDepth = 0.f;
                    UI.bVisible = true;
                    UI.iTextureIndex = 0;
                }

                ImGui::PopID();
            }

            ImGui::Separator();

            ++i;
        }
    }

    ImGui::End();

#endif
}

HRESULT InvenUI::Render_UIRect(UI_RECT& UI)
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

HRESULT InvenUI::Ready_Components()
{
    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_VtxPosTex"), TEXT("Com_Shader"), m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Rect"), TEXT("Com_VIBuffer"), m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT InvenUI::Add_UIRect(const wstring& UIName, const wstring& strName, const wstring& strTextureTag, const _float2& vPos, const _float2& vSize, _float fDepth)
{
    UI_RECT UI{};
    UI.strName = strName;
    UI.strTextureTag = strTextureTag;
    UI.vPos = vPos;
    UI.vSize = vSize;
    UI.fDepth = fDepth;
    UI.iTextureIndex = 0;
    UI.bVisible = true;




    if (FAILED(__super::Add_Component(
        CGameInstance::Get().Get_Level(),
        strTextureTag.c_str(),
        UI.strName,
        UI.pTexture)))
        return E_FAIL;

    m_UIRects[UIName] = (UI);

    return S_OK;
}

unique_ptr<InvenUI> InvenUI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    unique_ptr<InvenUI> pInstance = unique_ptr<InvenUI>(new InvenUI(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : InvenUI");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> InvenUI::Clone(void* pArg)
{
    shared_ptr<InvenUI> pInstance = shared_ptr<InvenUI>(new InvenUI(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : InvenUI");
        return nullptr;
    }

    return pInstance;
}


HRESULT InvenUI::Save_UIRects(const wstring& strFilePath)
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
            << UI.fAlpha
            << UI.vColor.x << "|"
            << UI.vColor.y << "|"
            << UI.vColor.z << "|"
            << UI.vColor.w
            << endl;
    }

    ofs.close();

    return S_OK;
}

HRESULT InvenUI::Load_UIRects(const wstring& strFilePath)
{
    ifstream ifs(strFilePath);

    if (!ifs.is_open())
        return E_FAIL;

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

        // Initialize에서 만든 UI가 아니면 무시
        if (iter == m_UIRects.end())
            continue;

        UI_RECT& UI = iter->second;

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
        UI.iTextureIndex = static_cast<int>(stoi(Tokens[8]));

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

    ifs.close();

    return S_OK;
}