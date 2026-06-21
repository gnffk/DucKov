#include "InvenUI.h"
#include "Player.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "UIObject.h"
#include "MainUI.h"
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
    m_UITexts.clear();
    m_InventorySlots.clear();
    m_InventoryItems.clear();
 

    if (FAILED(Add_UIRect(TEXT("BackGround1"), TEXT("BackGround1"), TEXT("Prototype_Com_Texture_UI_BaseRectCustom"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("BackGround2"), TEXT("BackGround2"), TEXT("Prototype_Com_Texture_UI_BaseRectCustom1"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("BackGround3"), TEXT("BackGround3"), TEXT("Prototype_Com_Texture_UI_BaseRectCustom1"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("Gun1_Box"), TEXT("Gun1_Box"), TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("Gun2_Box"), TEXT("Gun2_Box"), TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("melee_Box"), TEXT("melee_Box"), TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("Clothes_Box"), TEXT("Clothes_Box"), TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("Head_Box"), TEXT("Head_Box"), TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;


    if (FAILED(Add_UIRect(TEXT("Gun1"), TEXT("Gun1"), TEXT("Prototype_Com_Texture_Inven_Gun"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("Gun2"), TEXT("Gun2"), TEXT("Prototype_Com_Texture_Inven_Gun"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("melee"), TEXT("melee"), TEXT("Prototype_Com_Texture_Inven_Swords"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("Clothes"), TEXT("Clothes"), TEXT("Prototype_Com_Texture_Inven_Clothes"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;

    if (FAILED(Add_UIRect(TEXT("Head"), TEXT("Head"), TEXT("Prototype_Com_Texture_Inven_Head"), { 50.f, 50.f }, { 64.f, 64.f }, 0.0f)))
        return E_FAIL;




    if (FAILED(Add_UIText(TEXT("Text_Equip"), TEXT("Font_Default"),TEXT("장비"),  { 1100.f, 0.f }, 1.f,  { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;

    if (FAILED(Add_UIText(TEXT("Text_Bag"), TEXT("Font_Default"),TEXT("가방"),  { 1100.f, 0.f }, 1.f,  { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;

    if (FAILED(Add_UIText(TEXT("Text_Gun1"), TEXT("Font_Default"),TEXT("총기"),  { 1100.f, 0.f }, 1.f,  { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;
    if (FAILED(Add_UIText(TEXT("Text_Gun2"), TEXT("Font_Default"),TEXT("총기"),  { 1100.f, 0.f }, 1.f,  { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;
    if (FAILED(Add_UIText(TEXT("Text_melee"), TEXT("Font_Default"),TEXT("근접 무기"),  { 1100.f, 0.f }, 1.f,  { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;
    if (FAILED(Add_UIText(TEXT("Text_Clothes"), TEXT("Font_Default"),TEXT("신체"),  { 1100.f, 0.f }, 1.f,  { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;
    if (FAILED(Add_UIText(TEXT("Text_Head"), TEXT("Font_Default"),TEXT("머리"),  { 1100.f, 0.f }, 1.f,  { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;

    // 가방 5x5 슬롯 생성
    if (FAILED(Ready_InventorySlots()))
        return E_FAIL;

    // 아이템 생성
    if (FAILED(Ready_InventoryItems()))
        return E_FAIL;

    Load_UIRects(TEXT("../../Resources/Data/UI/InvenUI.txt"));
    Load_UITexts(TEXT("../../Resources/Data/UI/InvenUI_Text.txt"));

    return S_OK;
}

void InvenUI::Priority_Update(_float fTimeDelta)
{
}

void InvenUI::Update(_float fTimeDelta)
{

    Update_InvenAnimation(fTimeDelta);

    // 완전히 닫혀 있으면 인벤토리 입력 처리 X
    if (m_eAnimState == INVEN_ANIM_STATE::OPEN)
    {
        Update_Inventory(fTimeDelta);
    }
    else if (m_eAnimState == INVEN_ANIM_STATE::OPENING || m_eAnimState == INVEN_ANIM_STATE::CLOSING)
    {
        // 애니메이션 중에는 위치만 정리
        // 드래그/클릭은 막고, 아이콘 위치만 슬롯에 맞춰둠
        Update_ItemIconPosition();
    }
//#ifdef _DEBUG
//    GUI_InvenUI();
//#endif




}

void InvenUI::Late_Update(_float fTimeDelta)
{
   
}

HRESULT InvenUI::Render()
{
    if (m_eAnimState == INVEN_ANIM_STATE::CLOSED)
        return S_OK;

    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

  

    // 1. 아이템 아이콘 제외하고 일반 UI 먼저 렌더
    for (auto& Pair : m_UIRects)
    {
        const wstring& Key = Pair.first;

        if (Is_ItemIconKey(Key))
            continue;

        if (FAILED(Render_UIRect_ByKey(Key)))
            return E_FAIL;
    }

    // 2. 아이템 아이콘을 나중에 렌더
    for (auto& Item : m_InventoryItems)
    {
        if (FAILED(Render_UIRect_ByKey(Item.strIconRectKey)))
            return E_FAIL;
    }

    // 3. 글자는 제일 마지막
    for (auto& Pair : m_UITexts)
    {
        UI_TEXT& Text = Pair.second;

        if (false == Text.bVisible)
            continue;

        Text.fScale = std::clamp(Text.fScale, 0.01f, 10.f);

        Text.vColor.x = std::clamp(Text.vColor.x, 0.f, 1.f);
        Text.vColor.y = std::clamp(Text.vColor.y, 0.f, 1.f);
        Text.vColor.z = std::clamp(Text.vColor.z, 0.f, 1.f);
        Text.vColor.w = std::clamp(Text.vColor.w, 0.f, 1.f);
        _float2 vAnimTextPos =
        {
            Text.vPos.x + m_vAnimOffset.x,
            Text.vPos.y + m_vAnimOffset.y
        };

        _float4 vAnimTextColor = Text.vColor;
        vAnimTextColor.w *= m_fAnimAlpha;

        CGameInstance::Get().Draw_Text(Text.strFontTag,Text.strText.c_str(), vAnimTextPos,Text.fScale, XMLoadFloat4(&vAnimTextColor),0.f, _float2(0.f, 0.f) );
    }

    return S_OK;
}
void InvenUI::GUI_InvenUI()
{
#ifdef _DEBUG

    string strWindowName = "InvenUI Editor##";
    strWindowName += to_string(reinterpret_cast<size_t>(this));

    if (ImGui::Begin(strWindowName.c_str()))
    {
        if (ImGui::Button("Save UI"))
        {
            Save_UIRects(TEXT("../../Resources/Data/UI/InvenUI.txt"));
            Save_UITexts(TEXT("../../Resources/Data/UI/InvenUI_Text.txt"));
        }

        ImGui::SameLine();

        if (ImGui::Button("Load UI"))
        {
            Load_UIRects(TEXT("../../Resources/Data/UI/InvenUI.txt"));
            Load_UITexts(TEXT("../../Resources/Data/UI/InvenUI_Text.txt"));
        }

        ImGui::Separator();

        // =========================================================
        // UI RECT EDITOR
        // =========================================================
        ImGui::Text("UI Rect Count : %d", static_cast<int>(m_UIRects.size()));

        int iRect = 0;

        for (auto& Pair : m_UIRects)
        {
            const wstring& UIName = Pair.first;
            UI_RECT& UI = Pair.second;

            string strName = CGameInstance::Get().WStringToString(UIName);

            string strHeader = "UI Rect ";
            strHeader += to_string(iRect);
            strHeader += " : ";
            strHeader += strName;

            ImGui::PushID(("Rect_" + strName).c_str());

            if (ImGui::CollapsingHeader(strHeader.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Visible", reinterpret_cast<bool*>(&UI.bVisible));

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

                    UI.iTextureIndex = static_cast<int>(iTexIndex);
                }

                string strTextureTag = CGameInstance::Get().WStringToString(UI.strTextureTag);

                ImGui::Text("Texture Prototype");
                ImGui::Text("%s", strTextureTag.c_str());

                if (ImGui::Button("Reset Rect"))
                {
                    UI.vPos = { 50.f, 50.f };
                    UI.vSize = { 64.f, 64.f };
                    UI.fDepth = 0.f;
                    UI.bVisible = true;
                    UI.iTextureIndex = 0;
                    UI.fAlpha = 1.f;
                    UI.vColor = { 1.f, 1.f, 1.f, 1.f };
                }
            }

            ImGui::PopID();

            ImGui::Separator();

            ++iRect;
        }

        // =========================================================
        // FONT TEXT EDITOR
        // =========================================================
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Font Text Count : %d", static_cast<int>(m_UITexts.size()));

        int iText = 0;

        for (auto& Pair : m_UITexts)
        {
            const wstring& TextName = Pair.first;
            UI_TEXT& Text = Pair.second;

            string strName = CGameInstance::Get().WStringToString(TextName);

            string strHeader = "Font Text ";
            strHeader += to_string(iText);
            strHeader += " : ";
            strHeader += strName;

            ImGui::PushID(("Text_" + strName).c_str());

            if (ImGui::CollapsingHeader(strHeader.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Visible", reinterpret_cast<bool*>(&Text.bVisible));

                ImGui::DragFloat2(
                    "Text Position",
                    reinterpret_cast<float*>(&Text.vPos),
                    1.f,
                    -5000.f,
                    5000.f
                );

                ImGui::DragFloat(
                    "Text Scale",
                    &Text.fScale,
                    0.01f,
                    0.01f,
                    10.f
                );

                ImGui::ColorEdit4(
                    "Text Color",
                    reinterpret_cast<float*>(&Text.vColor)
                );

                string strFontTag = CGameInstance::Get().WStringToString(Text.strFontTag);
                string strText = CGameInstance::Get().WStringToString(Text.strText);

                char szFontTag[256]{};
                char szText[256]{};

                strcpy_s(szFontTag, strFontTag.c_str());
                strcpy_s(szText, strText.c_str());

                if (ImGui::InputText("Font Tag", szFontTag, sizeof(szFontTag)))
                {
                    Text.strFontTag = CGameInstance::Get().StringToWString(szFontTag);
                }

                if (ImGui::InputText("Text", szText, sizeof(szText)))
                {
                    Text.strText = CGameInstance::Get().StringToWString(szText);
                }

                if (ImGui::Button("Reset Text"))
                {
                    Text.vPos = { 1100.f, 0.f };
                    Text.fScale = 1.f;
                    Text.vColor = { 1.f, 1.f, 1.f, 1.f };
                    Text.bVisible = true;
                }
            }

            ImGui::PopID();

            ImGui::Separator();

            ++iText;
        }
    }

    ImGui::End();

#endif
}
HRESULT InvenUI::Render_UIRect(UI_RECT& UI)
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    _float fX = UI.vPos.x + m_vAnimOffset.x - vViewportSize.x * 0.5f;
    _float fY = -(UI.vPos.y + m_vAnimOffset.y) + vViewportSize.y * 0.5f;

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

HRESULT InvenUI::Add_UIRect(const wstring& UIName,const wstring& strName,const wstring& strTextureTag,const _float2& vPos,const _float2& vSize, _float fDepth)
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

HRESULT InvenUI::Add_UIText(const wstring& TextName, const wstring& FontTag, const wstring& Text,const _float2& vPos,_float fScale,const _float4& vColor)
{
    UI_TEXT Desc{};

    Desc.strFontTag = FontTag;
    Desc.strText = Text;
    Desc.vPos = vPos;
    Desc.fScale = fScale;
    Desc.vColor = vColor;
    Desc.bVisible = true;

    m_UITexts[TextName] = Desc;

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
HRESULT InvenUI::Save_UITexts(const wstring& strFilePath)
{
    ofstream ofs(strFilePath);

    if (!ofs.is_open())
        return E_FAIL;

    for (auto& Pair : m_UITexts)
    {
        const wstring& TextName = Pair.first;
        const UI_TEXT& Text = Pair.second;

        ofs
            << CGameInstance::Get().WStringToString(TextName) << "|"
            << CGameInstance::Get().WStringToString(Text.strFontTag) << "|"
            << CGameInstance::Get().WStringToString(Text.strText) << "|"
            << Text.vPos.x << "|"
            << Text.vPos.y << "|"
            << Text.fScale << "|"
            << static_cast<int>(Text.bVisible) << "|"
            << Text.vColor.x << "|"
            << Text.vColor.y << "|"
            << Text.vColor.z << "|"
            << Text.vColor.w
            << endl;
    }

    ofs.close();

    return S_OK;
}
HRESULT InvenUI::Load_UITexts(const wstring& strFilePath)
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

        // 구버전 최소 형식:
        // TextName | FontTag | Text | PosX | PosY | Visible
        if (Tokens.size() < 6)
            continue;

        wstring TextName = CGameInstance::Get().StringToWString(Tokens[0]);

        auto iter = m_UITexts.find(TextName);

        // Initialize에서 만든 Text가 아니면 무시
        if (iter == m_UITexts.end())
            continue;

        UI_TEXT& Text = iter->second;

        try
        {
            Text.strFontTag = CGameInstance::Get().StringToWString(Tokens[1]);
            Text.strText = CGameInstance::Get().StringToWString(Tokens[2]);

            Text.vPos =
            {
                stof(Tokens[3]),
                stof(Tokens[4])
            };

            // 신버전:
            // 0 TextName
            // 1 FontTag
            // 2 Text
            // 3 PosX
            // 4 PosY
            // 5 Scale
            // 6 Visible
            // 7 ColorR
            // 8 ColorG
            // 9 ColorB
            // 10 ColorA
            if (Tokens.size() >= 11)
            {
                Text.fScale = stof(Tokens[5]);
                Text.bVisible = stoi(Tokens[6]) != 0;

                Text.vColor =
                {
                    stof(Tokens[7]),
                    stof(Tokens[8]),
                    stof(Tokens[9]),
                    stof(Tokens[10])
                };
            }
            else
            {
                // 구버전 호환
                Text.fScale = 1.f;
                Text.bVisible = stoi(Tokens[5]) != 0;
                Text.vColor = { 1.f, 1.f, 1.f, 1.f };
            }

            Text.fScale = std::clamp(Text.fScale, 0.01f, 10.f);

            Text.vColor.x = std::clamp(Text.vColor.x, 0.f, 1.f);
            Text.vColor.y = std::clamp(Text.vColor.y, 0.f, 1.f);
            Text.vColor.z = std::clamp(Text.vColor.z, 0.f, 1.f);
            Text.vColor.w = std::clamp(Text.vColor.w, 0.f, 1.f);
        }
        catch (...)
        {
            continue;
        }
    }

    ifs.close();

    return S_OK;
}


HRESULT InvenUI::Add_Item(const INV_ITEM& Item)
{
    int iEmptySlot = Find_EmptyBagSlot();

    if (iEmptySlot == -1)
    {
        // 인벤토리 가방칸이 꽉 참
        return E_FAIL;
    }

    if (Item.strTextureTag.empty())
        return E_FAIL;

    wchar_t szIconKey[128]{};

    swprintf_s(szIconKey,TEXT("Item_Loot_%u"),m_iDynamicItemSerial++);

    if (FAILED(Add_UIRect(szIconKey,szIconKey,Item.strTextureTag,{ 0.f, 0.f },Item.vIconSize, 0.0f)))
        return E_FAIL;

    auto iterIconRect = m_UIRects.find(szIconKey);

    if (iterIconRect != m_UIRects.end())
    {
        iterIconRect->second.bVisible = false;
        iterIconRect->second.fAlpha = 1.f;
        iterIconRect->second.vColor = { 1.f, 1.f, 1.f, 1.f };
    }

    INV_ITEM NewItem{};

    NewItem.strItemName = Item.strItemName;
    NewItem.strIconRectKey = szIconKey;
    NewItem.strTextureTag = Item.strTextureTag;
    NewItem.eEquipKind = Item.eEquipKind;
    NewItem.vIconSize = Item.vIconSize;
    NewItem.strEquipModelKey = Item.strEquipModelKey;

    int iNewItemIndex = static_cast<int>(m_InventoryItems.size());

    m_InventoryItems.push_back(NewItem);

    m_InventorySlots[iEmptySlot].iItemIndex = iNewItemIndex;

    Update_ItemIconPosition();

    return S_OK;
}

int InvenUI::Find_EmptyBagSlot() const
{
    for (int i = 0; i < static_cast<int>(m_InventorySlots.size()); ++i)
    {
        const INV_SLOT& Slot = m_InventorySlots[i];

        if (Slot.eKind != SLOT_KIND::BAG)
            continue;

        if (Slot.iItemIndex == -1)
            return i;
    }

    return -1;
}

HRESULT InvenUI::Ready_InventorySlots()
{
  
    m_InventorySlots.push_back({ TEXT("Gun1_Box"),    SLOT_KIND::GUN });
    m_InventorySlots.push_back({ TEXT("Gun2_Box"),    SLOT_KIND::GUN });
    m_InventorySlots.push_back({ TEXT("melee_Box"),   SLOT_KIND::MELEE });
    m_InventorySlots.push_back({ TEXT("Head_Box"),    SLOT_KIND::HEAD });
    m_InventorySlots.push_back({ TEXT("Clothes_Box"), SLOT_KIND::CLOTHES });

    // 가방 5x5
    const int iColCount = 5;
    const int iRowCount = 5;

    const float fStartX = 85.f;
    const float fStartY = 365.f;
    const float fSlotSize = 58.f;
    const float fGap = 8.f;

    for (int y = 0; y < iRowCount; ++y)
    {
        for (int x = 0; x < iColCount; ++x)
        {
            int iIndex = y * iColCount + x;

            wchar_t szKey[64]{};
            swprintf_s(szKey, TEXT("BagSlot_%02d"), iIndex);

            _float2 vPos =
            {
                fStartX + x * (fSlotSize + fGap),
                fStartY + y * (fSlotSize + fGap)
            };

            if (FAILED(Add_UIRect(szKey,szKey,TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"),vPos,{ fSlotSize, fSlotSize }, 0.0f)))
                return E_FAIL;

            INV_SLOT Slot{};
            Slot.strSlotRectKey = szKey;
            Slot.eKind = SLOT_KIND::BAG;
            Slot.iItemIndex = -1;
            Slot.fNormalAlpha = 0.20f;
            Slot.fHoverAlpha = 0.65f;

            m_InventorySlots.push_back(Slot);
        }
    }

    return S_OK;
}

HRESULT InvenUI::Ready_InventoryItems()
{
    //// 예시 아이템 1개
    //if (FAILED(Add_UIRect(TEXT("Item_Bag01"), TEXT("Item_Bag01"), TEXT("Prototype_Com_Texture_Heart"), { 0.f, 0.f }, { 48.f, 48.f },0.0f)))
    //    return E_FAIL;

    //INV_ITEM Item{};
    //Item.strItemName = TEXT("Lv3_Bag");
    //Item.strIconRectKey = TEXT("Item_Bag01");
    //Item.eEquipKind = SLOT_KIND::GUN;
    //Item.vIconSize = { 48.f, 48.f };

    //m_InventoryItems.push_back(Item);

    //// 아이템을 일단 첫 번째 가방 슬롯에 넣기
    //// 장비 슬롯 5개 이후부터 BagSlot_00임
    //const int iFirstBagSlotIndex = 5;
    //m_InventorySlots[iFirstBagSlotIndex].iItemIndex = 0;

    return S_OK;
}

void InvenUI::Update_Inventory(_float fTimeDelta)
{
    m_vMouseUIPos = Get_MouseUIPos();

    m_iHoverSlot = Find_Slot_ByMouse(m_vMouseUIPos);

    Update_SlotHover();

    if (CGameInstance::Get().Mouse_Down(MOUSEKEYSTATE::DIM_LB))
    {
        if (m_iHoverSlot != -1)
            Begin_DragItem(m_iHoverSlot);
    }

    if (m_bDraggingItem)
    {
        if (CGameInstance::Get().Mouse_Up(MOUSEKEYSTATE::DIM_LB))
        {
            End_DragItem();
        }
    }

    Update_ItemIconPosition();
}

_float2 InvenUI::Get_MouseUIPos()
{
    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);

    return _float2(
        static_cast<float>(pt.x),
        static_cast<float>(pt.y)
    );
}

_bool InvenUI::Is_PointInRect(const _float2& vPoint, const UI_RECT& Rect)
{
    float fHalfX = Rect.vSize.x * 0.5f;
    float fHalfY = Rect.vSize.y * 0.5f;

    if (vPoint.x < Rect.vPos.x - fHalfX)
        return false;

    if (vPoint.x > Rect.vPos.x + fHalfX)
        return false;

    if (vPoint.y < Rect.vPos.y - fHalfY)
        return false;

    if (vPoint.y > Rect.vPos.y + fHalfY)
        return false;

    return true;
}

int InvenUI::Find_Slot_ByMouse(const _float2& vMousePos)
{
    for (int i = 0; i < static_cast<int>(m_InventorySlots.size()); ++i)
    {
        const INV_SLOT& Slot = m_InventorySlots[i];

        auto iter = m_UIRects.find(Slot.strSlotRectKey);
        if (iter == m_UIRects.end())
            continue;

        UI_RECT& SlotRect = iter->second;

        if (false == SlotRect.bVisible)
            continue;

        if (Is_PointInRect(vMousePos, SlotRect))
            return i;
    }

    return -1;
}

void InvenUI::Update_SlotHover()
{
    for (int i = 0; i < static_cast<int>(m_InventorySlots.size()); ++i)
    {
        INV_SLOT& Slot = m_InventorySlots[i];

        auto iter = m_UIRects.find(Slot.strSlotRectKey);
        if (iter == m_UIRects.end())
            continue;

        UI_RECT& SlotRect = iter->second;

        if (i == m_iHoverSlot)
            SlotRect.fAlpha = Slot.fHoverAlpha;
        else
            SlotRect.fAlpha = Slot.fNormalAlpha;
    }
}

void InvenUI::Begin_DragItem(int iSlotIndex)
{
    if (iSlotIndex < 0 || iSlotIndex >= static_cast<int>(m_InventorySlots.size()))
        return;

    INV_SLOT& Slot = m_InventorySlots[iSlotIndex];

    if (Slot.iItemIndex == -1)
        return;

    m_bDraggingItem = true;
    m_iDragSrcSlot = iSlotIndex;
    m_iDraggingItem = Slot.iItemIndex;

    // 드래그 중에는 원래 슬롯에서 잠깐 비운다.
    Slot.iItemIndex = -1;
}
void InvenUI::End_DragItem()
{
    if (false == m_bDraggingItem)
        return;

    int iTargetSlot = Find_Slot_ByMouse(m_vMouseUIPos);

    int iSrcSlot = m_iDragSrcSlot;
    int iDraggingItem = m_iDraggingItem;

    auto Restore_DragItem = [&]()
        {
            if (iSrcSlot != -1 && iDraggingItem != -1)
                m_InventorySlots[iSrcSlot].iItemIndex = iDraggingItem;
        };

    // 안전 검사
    if (iSrcSlot < 0 || iSrcSlot >= static_cast<int>(m_InventorySlots.size()))
    {
        m_bDraggingItem = false;
        m_iDragSrcSlot = -1;
        m_iDraggingItem = -1;
        Update_ItemIconPosition();
        return;
    }

    if (iDraggingItem < 0 || iDraggingItem >= static_cast<int>(m_InventoryItems.size()))
    {
        m_bDraggingItem = false;
        m_iDragSrcSlot = -1;
        m_iDraggingItem = -1;
        Update_ItemIconPosition();
        return;
    }

    // 아무 슬롯에도 안 놓으면 원래 자리로 복귀
    if (iTargetSlot == -1)
    {
        Restore_DragItem();

        m_bDraggingItem = false;
        m_iDragSrcSlot = -1;
        m_iDraggingItem = -1;

        Update_ItemIconPosition();
        return;
    }

    if (iTargetSlot < 0 || iTargetSlot >= static_cast<int>(m_InventorySlots.size()))
    {
        Restore_DragItem();

        m_bDraggingItem = false;
        m_iDragSrcSlot = -1;
        m_iDraggingItem = -1;

        Update_ItemIconPosition();
        return;
    }

    // 타겟 슬롯에 드래그 아이템을 넣을 수 없으면 복귀
    if (false == Can_PlaceItem(iTargetSlot, iDraggingItem))
    {
        Restore_DragItem();

        m_bDraggingItem = false;
        m_iDragSrcSlot = -1;
        m_iDraggingItem = -1;

        Update_ItemIconPosition();
        return;
    }

    int iTargetItem = m_InventorySlots[iTargetSlot].iItemIndex;

    // 서로 교환할 경우, 타겟 아이템이 원래 슬롯으로 들어갈 수 있는지도 검사해야 함
    if (iTargetItem != -1)
    {
        if (false == Can_PlaceItem(iSrcSlot, iTargetItem))
        {
            Restore_DragItem();

            m_bDraggingItem = false;
            m_iDragSrcSlot = -1;
            m_iDraggingItem = -1;

            Update_ItemIconPosition();
            return;
        }
    }

    // 변경 전 상태 저장
    int iPrevSrcItem = iDraggingItem;
    int iPrevTargetItem = iTargetItem;

    // 실제 이동/교환
    m_InventorySlots[iTargetSlot].iItemIndex = iDraggingItem;
    m_InventorySlots[iSrcSlot].iItemIndex = iTargetItem;

    // 장비 슬롯 변화 반영
    if (Is_EquipSlot(iSrcSlot))
    {
        Notify_EquipChanged(
            iSrcSlot,
            iPrevSrcItem,
            m_InventorySlots[iSrcSlot].iItemIndex
        );
    }

    if (Is_EquipSlot(iTargetSlot))
    {
        Notify_EquipChanged(
            iTargetSlot,
            iPrevTargetItem,
            m_InventorySlots[iTargetSlot].iItemIndex
        );
    }

    m_bDraggingItem = false;
    m_iDragSrcSlot = -1;
    m_iDraggingItem = -1;

    Update_ItemIconPosition();
}
_bool InvenUI::Can_PlaceItem(int iSlotIndex, int iItemIndex)
{
    if (iSlotIndex < 0 || iSlotIndex >= static_cast<int>(m_InventorySlots.size()))
        return false;

    if (iItemIndex < 0 || iItemIndex >= static_cast<int>(m_InventoryItems.size()))
        return false;

    const INV_SLOT& Slot = m_InventorySlots[iSlotIndex];
    const INV_ITEM& Item = m_InventoryItems[iItemIndex];

    // 가방 슬롯에는 모든 아이템 가능
    if (Slot.eKind == SLOT_KIND::BAG)
        return true;

    // 장비 슬롯은 타입이 맞아야 가능
    if (Slot.eKind == Item.eEquipKind)
        return true;

    return false;
}
void InvenUI::Update_ItemIconPosition()
{
    // 1. 일단 모든 아이템 아이콘 숨김
    for (auto& Item : m_InventoryItems)
    {
        auto iter = m_UIRects.find(Item.strIconRectKey);
        if (iter == m_UIRects.end())
            continue;

        iter->second.bVisible = false;
    }

    // 2. 특정 UI Rect 보이기/숨기기
    auto Set_RectVisible = [&](const wstring& strKey, _bool bVisible)
        {
            auto iter = m_UIRects.find(strKey);
            if (iter == m_UIRects.end())
                return;

            iter->second.bVisible = bVisible;
        };

    // 3. 특정 슬롯에 아이템이 들어있는지 확인
    auto Is_SlotFilled = [&](const wstring& strSlotKey) -> _bool
        {
            for (const auto& Slot : m_InventorySlots)
            {
                if (Slot.strSlotRectKey == strSlotKey)
                    return Slot.iItemIndex != -1;
            }

            return false;
        };

    // 4. 장비 슬롯에 아이템이 들어갔으면 기본 그림 숨김
    //    비어 있으면 다시 기본 그림 보임
    Set_RectVisible(TEXT("Gun1"), !Is_SlotFilled(TEXT("Gun1_Box")));
    Set_RectVisible(TEXT("Gun2"), !Is_SlotFilled(TEXT("Gun2_Box")));
    Set_RectVisible(TEXT("melee"), !Is_SlotFilled(TEXT("melee_Box")));
    Set_RectVisible(TEXT("Head"), !Is_SlotFilled(TEXT("Head_Box")));
    Set_RectVisible(TEXT("Clothes"), !Is_SlotFilled(TEXT("Clothes_Box")));

    // 5. 슬롯에 들어있는 아이템 아이콘 위치 갱신
    for (auto& Slot : m_InventorySlots)
    {
        if (Slot.iItemIndex == -1)
            continue;

        INV_ITEM& Item = m_InventoryItems[Slot.iItemIndex];

        auto iterSlotRect = m_UIRects.find(Slot.strSlotRectKey);
        auto iterItemRect = m_UIRects.find(Item.strIconRectKey);

        if (iterSlotRect == m_UIRects.end())
            continue;

        if (iterItemRect == m_UIRects.end())
            continue;

        UI_RECT& SlotRect = iterSlotRect->second;
        UI_RECT& ItemRect = iterItemRect->second;

        ItemRect.vPos = SlotRect.vPos;
        ItemRect.vSize = Item.vIconSize;
        ItemRect.fAlpha = 1.f;
        ItemRect.vColor = { 1.f, 1.f, 1.f, 1.f };
        ItemRect.bVisible = true;
    }

    // 6. 드래그 중인 아이템은 마우스 따라다니게
    if (m_bDraggingItem && m_iDraggingItem != -1)
    {
        INV_ITEM& Item = m_InventoryItems[m_iDraggingItem];

        auto iterItemRect = m_UIRects.find(Item.strIconRectKey);
        if (iterItemRect != m_UIRects.end())
        {
            UI_RECT& ItemRect = iterItemRect->second;

            ItemRect.vPos = m_vMouseUIPos;
            ItemRect.vSize = Item.vIconSize;
            ItemRect.fAlpha = 0.9f;
            ItemRect.vColor = { 1.f, 1.f, 1.f, 1.f };
            ItemRect.bVisible = true;
        }
    }
}
_bool InvenUI::Is_ItemIconKey(const wstring& strKey)
{
    return strKey.find(TEXT("Item_")) == 0;
}

HRESULT InvenUI::Render_UIRect_ByKey(const wstring& strKey)
{
    auto iter = m_UIRects.find(strKey);
    if (iter == m_UIRects.end())
        return S_OK;

    UI_RECT& UI = iter->second;

    if (false == UI.bVisible)
        return S_OK;

    if (nullptr == UI.pTexture)
        return S_OK;

    float fFinalAlpha = UI.fAlpha * m_fAnimAlpha;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fFinalAlpha, sizeof(float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &UI.vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(Render_UIRect(UI)))
        return E_FAIL;

    return S_OK;
}

_bool InvenUI::Is_EquipSlot(int iSlotIndex)
{
    if (iSlotIndex < 0 || iSlotIndex >= static_cast<int>(m_InventorySlots.size()))
        return false;

    SLOT_KIND eKind = m_InventorySlots[iSlotIndex].eKind;

    return eKind != SLOT_KIND::BAG;
}

void InvenUI::Notify_EquipChanged(int iSlotIndex, int iPrevItemIndex, int iNewItemIndex)
{
    if (false == Is_EquipSlot(iSlotIndex))
        return;

    SLOT_KIND eSlotKind = m_InventorySlots[iSlotIndex].eKind;
    const wstring& strSlotKey = m_InventorySlots[iSlotIndex].strSlotRectKey;

    auto pPlayer =
        CGameInstance::Get().Find_Object(
            CGameInstance::Get().Get_Level(),
            L"PlayerTag",
            L"Player"
        );

    if (pPlayer == nullptr)
        return;

    auto pPlayerObj = static_pointer_cast<Player>(pPlayer);

    if (pPlayerObj == nullptr)
        return;

    // ==========================
    // 무기 슬롯 처리
    // ==========================
    if (eSlotKind == SLOT_KIND::GUN)
    {
        int iWeaponSlotNumber = -1;

        if (strSlotKey == TEXT("Gun1_Box"))
            iWeaponSlotNumber = 1;
        else if (strSlotKey == TEXT("Gun2_Box"))
            iWeaponSlotNumber = 2;

        if (iWeaponSlotNumber == -1)
            return;

        // Player의 무기 슬롯 데이터 갱신
        if (iNewItemIndex != -1)
        {
            INV_ITEM& NewItem = m_InventoryItems[iNewItemIndex];
            pPlayerObj->Set_WeaponSlot(iWeaponSlotNumber, NewItem);
        }
        else
        {
            pPlayerObj->Clear_WeaponSlot(iWeaponSlotNumber);
        }

        // MainUI의 1, 2번 아이콘 갱신
        auto& UIs = pPlayerObj->GetUIs();

        auto iterMainUI = UIs.find("MainUI");

        if (iterMainUI != UIs.end())
        {
            auto pMainUI = static_pointer_cast<MainUI>(iterMainUI->second);

            if (pMainUI != nullptr)
            {
                if (iNewItemIndex != -1)
                {
                    INV_ITEM& NewItem = m_InventoryItems[iNewItemIndex];

                    pMainUI->Set_WeaponSlotIcon(
                        iWeaponSlotNumber,
                        NewItem.strTextureTag
                    );
                }
                else
                {
                    pMainUI->Clear_WeaponSlotIcon(iWeaponSlotNumber);
                }
            }
        }

        return;
    }
    // ==========================
    // 방어구, 머리, 근접무기 등 기존 처리
    // ==========================
    if (iPrevItemIndex != -1)
    {
        INV_ITEM& PrevItem = m_InventoryItems[iPrevItemIndex];
        pPlayerObj->Unequip_Item(eSlotKind, PrevItem);
    }

    if (iNewItemIndex != -1)
    {
        INV_ITEM& NewItem = m_InventoryItems[iNewItemIndex];
        pPlayerObj->Equip_Item(eSlotKind, NewItem);
    }
}
void InvenUI::Toggle_Inven()
{
    InvenSet(!m_bInventoryOpen);
}

void InvenUI::InvenSet(_bool bInvenSet)
{
    if (m_bInventoryOpen == bInvenSet)
        return;

    m_bInventoryOpen = bInvenSet;

    if (m_bInventoryOpen)
    {
        Open_Inven();
    }
    else
    {
        Close_Inven();
    }
}
void InvenUI::Open_Inven()
{
    m_eAnimState = INVEN_ANIM_STATE::OPENING;
    m_fAnimRatio = 0.f;
    m_fAnimAlpha = 0.f;
    m_vAnimOffset.x = -m_fSlideDistance;
    m_vAnimOffset.y = 0.f;
}

void InvenUI::Close_Inven()
{
    if (m_bDraggingItem)
    {
        if (m_iDragSrcSlot != -1 && m_iDraggingItem != -1)
        {
            m_InventorySlots[m_iDragSrcSlot].iItemIndex = m_iDraggingItem;
        }

        m_bDraggingItem = false;
        m_iDragSrcSlot = -1;
        m_iDraggingItem = -1;

        // 추가
        Update_ItemIconPosition();
    }

    m_eAnimState = INVEN_ANIM_STATE::CLOSING;
    m_fAnimRatio = 1.f;
    m_fAnimAlpha = 1.f;
    m_vAnimOffset = { 0.f, 0.f };
}

void InvenUI::Update_InvenAnimation(_float fTimeDelta)
{
    if (m_eAnimState == INVEN_ANIM_STATE::OPENING)
    {
        m_fAnimRatio += fTimeDelta * m_fAnimSpeed;

        if (m_fAnimRatio >= 1.f)
        {
            m_fAnimRatio = 1.f;
            m_eAnimState = INVEN_ANIM_STATE::OPEN;
        }
    }
    else if (m_eAnimState == INVEN_ANIM_STATE::CLOSING)
    {
        m_fAnimRatio -= fTimeDelta * m_fAnimSpeed;

        if (m_fAnimRatio <= 0.f)
        {
            m_fAnimRatio = 0.f;
            m_eAnimState = INVEN_ANIM_STATE::CLOSED;
        }
    }

    m_fAnimRatio = ClampFloat(m_fAnimRatio, 0.f, 1.f);

    float fEase = EaseOutCubic(m_fAnimRatio);

    // 0일 때 왼쪽 밖, 1일 때 원래 위치
    m_vAnimOffset.x = -m_fSlideDistance * (1.f - fEase);
    m_vAnimOffset.y = 0.f;

    // 같이 사악 페이드 인/아웃
    m_fAnimAlpha = fEase;
}

float InvenUI::ClampFloat(float fValue, float fMin, float fMax)
{
    if (fValue < fMin)
        return fMin;

    if (fValue > fMax)
        return fMax;

    return fValue;
}

float InvenUI::EaseOutCubic(float fRatio)
{
    fRatio = ClampFloat(fRatio, 0.f, 1.f);

    float fInv = 1.f - fRatio;

    return 1.f - fInv * fInv * fInv;
}