#include "FindUI.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "Player.h"
#include "InvenUI.h"
#include <fstream>
#include <sstream>


FindUI::FindUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : UIObject{ pDevice, pContext }
{
}

FindUI::FindUI(const FindUI& Prototype)
    : UIObject{ Prototype }
{
}

FindUI::~FindUI()
{
}

HRESULT FindUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT FindUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_UIRects.clear();
    m_UITexts.clear();


    // 상자 배경
    if (FAILED(Add_UIRect(TEXT("BackGround"), TEXT("BackGround"), TEXT("Prototype_Com_Texture_UI_BaseRectCustom"), { 640.f, 360.f }, { 420.f, 360.f }, 0.0f)))
        return E_FAIL;
    // 상자 배경2
    if (FAILED(Add_UIRect(TEXT("BackGround1"), TEXT("BackGround1"), TEXT("Prototype_Com_Texture_UI_BaseRectCustom"), { 640.f, 360.f }, { 420.f, 360.f }, 0.0f)))
        return E_FAIL;


    // 찾기 배경
    if (FAILED(Add_UIRect(TEXT("Prompt_BackGround"), TEXT("Prompt_BackGround"), TEXT("Prototype_Com_Texture_UI_BaseRectCustom"), { 660.f, 360.f }, { 420.f, 360.f }, 0.0f)))
        return E_FAIL;
    // Owner 위에 뜨는 찾기 텍스트
    if (FAILED(Add_UIText(TEXT("Text_FindPrompt"), TEXT("Font_Default"), TEXT("[F] 찾기"), { 0.f, 0.f }, 0.65f, { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;

    // Owner 위에 뜨는 찾기 텍스트
    if (FAILED(Add_UIText(TEXT("Text_BoxPrompt"), TEXT("Font_Default"), TEXT("상자"), { 0.f, 0.f }, 0.65f, { 1.f, 1.f, 1.f, 1.f })))
        return E_FAIL;



    if (FAILED(Ready_LootSlots()))
        return E_FAIL;

    if (FAILED(Ready_LootItems()))
        return E_FAIL;

    Load_UIRects(TEXT("../../Resources/Data/UI/FindUI.txt"));
    Load_UITexts(TEXT("../../Resources/Data/UI/FindUI_Text.txt"));

    return S_OK;
}

HRESULT FindUI::Ready_LootSlots()
{
    const int iColCount = 5;


    const float fStartX = 910.f;
    const float fStartY = 120.f;

    const float fSlotSize = 58.f;
    const float fGap = 8.f;


    for (int x = 0; x < iColCount; ++x)
    {
        const int iIndex =  x;

        wchar_t szSlotKey[64]{};
        swprintf_s(szSlotKey, TEXT("LootSlot_%02d"), iIndex);

        _float2 vPos =
        {
            fStartX + x * (fSlotSize + fGap),
            fStartY +  (fSlotSize + fGap)
        };

        if (FAILED(Add_UIRect(szSlotKey, szSlotKey, TEXT("Prototype_Com_Texture_UI_BaseRectItemRect"), vPos, { fSlotSize, fSlotSize },  0.0f)))
            return E_FAIL;

        INV_SLOT Slot{};
        Slot.strSlotRectKey = szSlotKey;
        Slot.eKind =SLOT_KIND::BAG;
        Slot.iItemIndex = -1;
        Slot.fNormalAlpha = 0.20f;
        Slot.fHoverAlpha = 0.65f;

        m_InventorySlots.push_back(Slot);
    }
   

    return S_OK;
}

HRESULT FindUI::Ready_LootItems()
{

    // 예시 아이템 1 
    if (FAILED(Add_UIRect( TEXT("LootItem_Gun1"), TEXT("LootItem_Gun1"), TEXT("Prototype_Com_Texture_UI_Item_Gun1"), { 0.f, 0.f },{ 48.f, 48.f }, 0.0f)))
        return E_FAIL;

    INV_ITEM ItemGun1{};
    ItemGun1.strItemName = TEXT("Gun1");
    ItemGun1.strIconRectKey = TEXT("LootItem_Gun1");
    ItemGun1.strTextureTag = TEXT("Prototype_Com_Texture_UI_Item_Gun1");
    ItemGun1.eEquipKind = SLOT_KIND::GUN;
    ItemGun1.vIconSize = { 48.f, 48.f };
    ItemGun1.strEquipModelKey = "Gun1";



    m_InventoryItems.push_back(ItemGun1);

    // 아이템을 Loot Slot에 배치
    if (m_InventorySlots.size() > 0)
        m_InventorySlots[0].iItemIndex = 0;


    // 예시 아이템 1 
    if (FAILED(Add_UIRect(TEXT("LootItem_Gun2"), TEXT("LootItem_Gun2"), TEXT("Prototype_Com_Texture_UI_Item_Gun2"), { 0.f, 0.f }, { 48.f, 48.f }, 0.0f)))
        return E_FAIL;

    INV_ITEM ItemGun2{};
    ItemGun2.strItemName = TEXT("Gun2");
    ItemGun2.strIconRectKey = TEXT("LootItem_Gun2");
    ItemGun2.strTextureTag = TEXT("Prototype_Com_Texture_UI_Item_Gun2");
    ItemGun2.eEquipKind = SLOT_KIND::GUN;
    ItemGun2.vIconSize = { 48.f, 48.f };
    ItemGun2.strEquipModelKey = "Gun2";



    m_InventoryItems.push_back(ItemGun2);

    // 아이템을 Loot Slot에 배치
    if (m_InventorySlots.size() > 0)
        m_InventorySlots[1].iItemIndex = 1;


    // 예시 아이템 1 
    if (FAILED(Add_UIRect(TEXT("LootItem_Gun3"), TEXT("LootItem_Gun3"), TEXT("Prototype_Com_Texture_UI_Item_Gun3"), { 0.f, 0.f }, { 48.f, 48.f }, 0.0f)))
        return E_FAIL;

    INV_ITEM ItemGun3{};
    ItemGun3.strItemName = TEXT("Gun3");
    ItemGun3.strIconRectKey = TEXT("LootItem_Gun3");
    ItemGun3.strTextureTag = TEXT("Prototype_Com_Texture_UI_Item_Gun3");
    ItemGun3.eEquipKind = SLOT_KIND::GUN;
    ItemGun3.vIconSize = { 48.f, 48.f };
    ItemGun3.strEquipModelKey = "Gun3";



    m_InventoryItems.push_back(ItemGun3);

    // 아이템을 Loot Slot에 배치
    if (m_InventorySlots.size() > 0)
        m_InventorySlots[2].iItemIndex = 2;






    return S_OK;
}

void FindUI::Priority_Update(_float fTimeDelta)
{
}

void FindUI::Update(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    Set_CollidingOwner(pOwner->GetCollision());
    Update_FindPrompt(fTimeDelta);
    Update_LootBox(fTimeDelta);

//#ifdef _DEBUG
//    GUI_FindUI();
//#endif




}

void FindUI::Late_Update(_float fTimeDelta)
{
   
}

HRESULT FindUI::Render()
{
 
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    if (!m_bBoxOpen)
    {
        if (FAILED(Render_UIRect_ByKey(TEXT("Prompt_BackGround"))))
            return E_FAIL;
    }

    // 상자 UI
    if (m_bBoxOpen)
    {
        for (auto& Pair : m_UIRects)
        {
            const wstring& Key = Pair.first;

            if (Key == TEXT("Prompt_BackGround"))
                continue;

            if (Is_ItemIconKey(Key))
                continue;

            if (FAILED(Render_UIRect_ByKey(Key)))
                return E_FAIL;
        }

        for (auto& Item : m_InventoryItems)
        {
            if (FAILED(Render_UIRect_ByKey(Item.strIconRectKey)))
                return E_FAIL;
        }
    }

    for (auto& Pair : m_UITexts)
    {
        UI_TEXT& Text = Pair.second;

        if (false == Text.bVisible)
            continue;

        CGameInstance::Get().Draw_Text(Text.strFontTag, Text.strText.c_str(), Text.vPos,  Text.fScale,  XMLoadFloat4(&Text.vColor),  0.f,   _float2(0.f, 0.f) );
    }

    return S_OK;
}
void FindUI::GUI_FindUI()
{
#ifdef _DEBUG

    string strWindowName = "InvenUI Editor##";
    strWindowName += to_string(reinterpret_cast<size_t>(this));

    if (ImGui::Begin(strWindowName.c_str()))
    {


        if (ImGui::Button("Save UI"))
        {
            Save_UIRects(TEXT("../../Resources/Data/UI/FindUI.txt"));
            Save_UITexts(TEXT("../../Resources/Data/UI/FindUI_Text.txt"));
        }

        ImGui::SameLine();

        if (ImGui::Button("Load UI"))
        {
            Load_UIRects(TEXT("../../Resources/Data/UI/FindUI.txt"));
            Load_UITexts(TEXT("../../Resources/Data/UI/FindUI_Text.txt"));
        }

        ImGui::Separator();

        ImGui::Text("Find Prompt Local Setting");

        ImGui::DragFloat2(
            "Find Text Local Offset",
            reinterpret_cast<float*>(&m_vFindTextLocalOffset),
            1.f,
            -500.f,
            500.f
        );

        ImGui::DragFloat2(
            "Find Back Local Offset",
            reinterpret_cast<float*>(&m_vFindBackLocalOffset),
            1.f,
            -500.f,
            500.f
        );

        ImGui::DragFloat2(
            "Find Back Size",
            reinterpret_cast<float*>(&m_vFindBackSize),
            1.f,
            1.f,
            500.f
        );

        if (ImGui::Button("Reset Find Prompt Local"))
        {
            m_vFindTextLocalOffset = { -35.f, -12.f };
            m_vFindBackLocalOffset = { 0.f, 0.f };
            m_vFindBackSize = { 120.f, 42.f };
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
HRESULT FindUI::Render_UIRect(UI_RECT& UI)
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

HRESULT FindUI::Ready_Components()
{
    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_VtxPosTex"), TEXT("Com_Shader"), m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_VIBuffer_Rect"), TEXT("Com_VIBuffer"), m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT FindUI::Add_UIRect(const wstring& UIName, const wstring& strName, const wstring& strTextureTag, const _float2& vPos, const _float2& vSize, _float fDepth)
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

HRESULT FindUI::Add_UIText(const wstring& TextName, const wstring& FontTag, const wstring& Text, const _float2& vPos, _float fScale, const _float4& vColor)
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


HRESULT FindUI::Render_UIRect_ByKey(const wstring& strKey)
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

void FindUI::Update_FindPrompt(_float fTimeDelta)
{
    auto iterFindText = m_UITexts.find(TEXT("Text_FindPrompt"));
    auto iterBoxText = m_UITexts.find(TEXT("Text_BoxPrompt"));
    auto iterBack = m_UIRects.find(TEXT("Prompt_BackGround"));
    
    if (iterFindText != m_UITexts.end())
        iterFindText->second.bVisible = false;



    if (iterBack != m_UIRects.end())
        iterBack->second.bVisible = false;

    if (m_bBoxOpen)
        return;

 

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pTransform = pOwner->GetTransform();

    if (pTransform == nullptr)
        return;

    _vector vOwnerPos = pTransform->Get_State(STATE::POSITION);

    // Owner 머리 위로 살짝 띄우기
    vOwnerPos += XMVectorSet( m_vOwnerUIOffset.x, m_vOwnerUIOffset.y, m_vOwnerUIOffset.z, 0.f );

    _float2 vScreenPos{}; 

    if (WorldToScreen(vOwnerPos, vScreenPos))
    {
        m_bFindTextOnScreen = true;

     
        if (iterFindText != m_UITexts.end())
        {
            iterFindText->second.vPos =
            {
                vScreenPos.x + m_vFindTextLocalOffset.x,
                vScreenPos.y + m_vFindTextLocalOffset.y
            };

            iterFindText->second.bVisible = true;
        }

        if (iterBack != m_UIRects.end())
        {
            iterBack->second.vPos =
            {
                vScreenPos.x + m_vFindBackLocalOffset.x,
                vScreenPos.y + m_vFindBackLocalOffset.y
            };

            iterBack->second.vSize = m_vFindBackSize;
            iterBack->second.bVisible = true;
        }


        if (iterBoxText != m_UITexts.end())
        {
     
            iterBoxText->second.bVisible = false;
        }

 
    }
    else
    {
        m_bFindTextOnScreen = false;
    }
    if (CGameInstance::Get().Key_Down(DIK_F))
    {
        if (m_bBoxOpen == false)
        {
            CGameInstance::Get().PlaySoundOne(
                L"EFFECT_BOX_OPEN",
                CHANNELID::EFFECT,
                0.65f
            );

        }
        m_bBoxOpen = true;

        auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");

        if (pPlayer == nullptr)
            return;

        auto pPlayerObj = static_pointer_cast<Player>(pPlayer);

        if (pPlayerObj == nullptr)
            return;

        pPlayerObj->InvenSet(true);

        if (iterFindText != m_UITexts.end())
            iterFindText->second.bVisible = false;
        if (iterBack != m_UIRects.end())
        {
            iterBack->second.bVisible = false;
        }

        if (iterBoxText != m_UITexts.end())
        {

            iterBoxText->second.bVisible = true;
        }
    }


}

void FindUI::Update_LootBox(_float fTimeDelta)
{
    m_vMouseUIPos = Get_MouseUIPos();

    if (false == m_bBoxOpen)
    {
        m_iHoverSlot = -1;

        auto iterBack = m_UIRects.find(TEXT("BackGround"));
        if (iterBack != m_UIRects.end())
            iterBack->second.bVisible = false;

        for (auto& Slot : m_InventorySlots)
        {
            auto iterSlot = m_UIRects.find(Slot.strSlotRectKey);
            if (iterSlot != m_UIRects.end())
                iterSlot->second.bVisible = false;
        }

        for (auto& Item : m_InventoryItems)
        {
            auto iterItem = m_UIRects.find(Item.strIconRectKey);
            if (iterItem != m_UIRects.end())
                iterItem->second.bVisible = false;
        }

        return;
    }

    auto iterBack = m_UIRects.find(TEXT("BackGround"));
    if (iterBack != m_UIRects.end())
        iterBack->second.bVisible = true;

    for (auto& Slot : m_InventorySlots)
    {
        auto iterSlot = m_UIRects.find(Slot.strSlotRectKey);
        if (iterSlot != m_UIRects.end())
            iterSlot->second.bVisible = true;
    }

    m_iPrevHoverSlot = m_iHoverSlot;
    m_iHoverSlot = Find_Slot_ByMouse(m_vMouseUIPos);

    if (m_iHoverSlot != -1 && m_iHoverSlot != m_iPrevHoverSlot)
    {
        CGameInstance::Get().PlaySoundOne(
            L"EFFECT_SLOT_HOVER",
            CHANNELID::EFFECT,
            0.35f
        );
    }
    Update_SlotHover();
    Update_ItemIconPosition();

    if (CGameInstance::Get().Mouse_Down(MOUSEKEYSTATE::DIM_LB))
    {
        if (m_iHoverSlot != -1)
            Farm_Item(m_iHoverSlot);
    }
}

int FindUI::Find_Slot_ByMouse(const _float2& vMousePos)
{
    for (int i = 0; i < static_cast<int>(m_InventorySlots.size()); ++i)
    {
        INV_SLOT& Slot = m_InventorySlots[i];

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

void FindUI::Update_SlotHover()
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
void FindUI::Update_ItemIconPosition()
{
    for (auto& Item : m_InventoryItems)
    {
        auto iterItemRect = m_UIRects.find(Item.strIconRectKey);

        if (iterItemRect == m_UIRects.end())
            continue;

        iterItemRect->second.bVisible = false;
    }

    for (auto& Slot : m_InventorySlots)
    {
        if (Slot.iItemIndex == -1)
            continue;

        if (Slot.iItemIndex < 0 || Slot.iItemIndex >= static_cast<int>(m_InventoryItems.size()))
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
}
void FindUI::Farm_Item(int iSlotIndex)
{
    if (iSlotIndex < 0 || iSlotIndex >= static_cast<int>(m_InventorySlots.size()))
        return;

    INV_SLOT& Slot = m_InventorySlots[iSlotIndex];

    if (Slot.iItemIndex == -1)
        return;

    int iItemIndex = Slot.iItemIndex;

    if (iItemIndex < 0 || iItemIndex >= static_cast<int>(m_InventoryItems.size()))
        return;

    auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(),L"PlayerTag", L"Player");

    if (pPlayer == nullptr)
        return;

    auto pPlayerObj = static_pointer_cast<Player>(pPlayer);

    if (pPlayerObj == nullptr)
        return;

    auto& UIs = pPlayerObj->GetUIs();

    auto iterUI = UIs.find("InvenUI");

    if (iterUI == UIs.end())
        return;

    auto pInvenUI = static_pointer_cast<InvenUI>(iterUI->second);

    if (pInvenUI == nullptr)
        return;

    if (FAILED(pInvenUI->Add_Item(m_InventoryItems[iItemIndex])))
    {
      
        return;
    }

    CGameInstance::Get().PlaySoundOne(
        L"EFFECT_LOOT_GET",
        CHANNELID::EFFECT,
        0.65f
    );

    Slot.iItemIndex = -1;

    auto iterItemRect = m_UIRects.find(m_InventoryItems[iItemIndex].strIconRectKey);

    if (iterItemRect != m_UIRects.end())
        iterItemRect->second.bVisible = false;
}

unique_ptr<FindUI> FindUI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    unique_ptr<FindUI> pInstance = unique_ptr<FindUI>(new FindUI(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : FindUI");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> FindUI::Clone(void* pArg)
{
    shared_ptr<FindUI> pInstance = shared_ptr<FindUI>(new FindUI(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : FindUI");
        return nullptr;
    }

    return pInstance;
}


HRESULT FindUI::Save_UIRects(const wstring& strFilePath)
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
HRESULT FindUI::Load_UIRects(const wstring& strFilePath)
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
HRESULT FindUI::Save_UITexts(const wstring& strFilePath)
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
HRESULT FindUI::Load_UITexts(const wstring& strFilePath)
{
    ifstream ifs(strFilePath);

    // UI 데이터 파일이 없어도 기본 Add_UIText 값으로 진행
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

        // 구버전:
        // TextName | FontTag | Text | PosX | PosY | Visible
        //
        // 신버전:
        // TextName | FontTag | Text | PosX | PosY | Scale | Visible | R | G | B | A
        if (Tokens.size() < 6)
            continue;

        wstring TextName = CGameInstance::Get().StringToWString(Tokens[0]);

        auto iter = m_UITexts.find(TextName);

        // Initialize에서 Add_UIText로 만든 Text가 아니면 무시
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

            // 핵심:
            // Owner 위에 뜨는 찾기 프롬프트는 저장된 위치/Visible을 쓰면 안 됨.
            // 매 프레임 Update_FindPrompt에서 WorldToScreen으로 갱신해야 함.
            if (TextName == TEXT("Text_FindPrompt"))
            {
                Text.vPos = { 0.f, 0.f };
                Text.bVisible = false;
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

_float2 FindUI::Get_MouseUIPos()
{
    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);

    return _float2(
        static_cast<float>(pt.x),
        static_cast<float>(pt.y)
    );
}

_bool FindUI::Is_PointInRect(const _float2& vPoint, const UI_RECT& Rect)
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

_bool FindUI::Is_ItemIconKey(const wstring& strKey)
{
    return strKey.find(TEXT("LootItem_")) == 0;
}
HRESULT FindUI::Set_LootItems(const vector<INV_ITEM>& LootItems)
{
    m_InventoryItems.clear();

    for (auto& Slot : m_InventorySlots)
        Slot.iItemIndex = -1;

    for (uint32_t i = 0; i < LootItems.size(); ++i)
    {
        if (i >= m_InventorySlots.size())
            break;

        const INV_ITEM& SrcItem = LootItems[i];

        wchar_t szIconKey[128]{};
        swprintf_s(szIconKey, TEXT("LootItem_%u"), i);

        if (FAILED(Add_UIRect(
            szIconKey,
            szIconKey,
            SrcItem.strTextureTag,
            { 0.f, 0.f },
            SrcItem.vIconSize,
            0.0f)))
            return E_FAIL;

        INV_ITEM NewItem = SrcItem;
        NewItem.strIconRectKey = szIconKey;

        m_InventoryItems.push_back(NewItem);
        m_InventorySlots[i].iItemIndex = static_cast<int>(i);
    }

    Update_ItemIconPosition();

    return S_OK;
}
void FindUI::Set_CollidingOwner(_bool bColliding)
{
    m_bCollidingOwner = bColliding;
    auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");

    if (pPlayer == nullptr)
        return;

    auto pPlayerObj = static_pointer_cast<Player>(pPlayer);

    if (pPlayerObj == nullptr)
        return;

    pPlayerObj->InvenSet(true);

    if (false == m_bCollidingOwner) {
        pPlayerObj->InvenSet(false);

        if (m_bBoxOpen)
        {
            CGameInstance::Get().PlaySoundOne(
                L"EFFECT_BOX_CLOSE",
                CHANNELID::EFFECT,
                0.55f
            );
        }

        m_bBoxOpen = false;
    }
}
_bool FindUI::WorldToScreen(_fvector vWorldPos, _float2& vOutScreenPos)
{
    _float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

    if (vViewportSize.x <= 0.f || vViewportSize.y <= 0.f)
        return false;

    _float4x4 ViewMatrix{};
    _float4x4 ProjMatrix{};

    CGameInstance::Get().Get_MainCamerwaViewMatrix(ViewMatrix);
    CGameInstance::Get().Get_MainCamerwaProjectionMatrix(ProjMatrix);

    _matrix matView = XMLoadFloat4x4(&ViewMatrix);
    _matrix matProj = XMLoadFloat4x4(&ProjMatrix);

    _vector vClip = XMVector3TransformCoord(vWorldPos, matView);
    vClip = XMVector3TransformCoord(vClip, matProj);

    float x = XMVectorGetX(vClip);
    float y = XMVectorGetY(vClip);
    float z = XMVectorGetZ(vClip);

    if (z < 0.f || z > 1.f)
        return false;

    vOutScreenPos.x = (x * 0.5f + 0.5f) * vViewportSize.x;
    vOutScreenPos.y = (-y * 0.5f + 0.5f) * vViewportSize.y;

    return true;
}
