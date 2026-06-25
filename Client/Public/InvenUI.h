#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class InvenUI final : public UIObject
{
private:

private:
    InvenUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    InvenUI(const InvenUI& Prototype);

public:
    virtual ~InvenUI();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    void    GUI_InvenUI();

private:
    shared_ptr<Shader>        m_pShaderCom = nullptr;
    shared_ptr<VIBuffer_Rect> m_pVIBufferCom = nullptr;
private:
    map<wstring, UI_TEXT> m_UITexts;
    map<wstring, UI_RECT> m_UIRects;

private:
    HRESULT Ready_Components();
    HRESULT Add_UIRect(const wstring& UIName, const wstring& strName, const wstring& strTextureTag, const _float2& vPos, const _float2& vSize, _float fDepth);
    HRESULT Render_UIRect(UI_RECT& UI);

    //Name|TextureTag|PosX|PosY|SizeX|SizeY|Depth|Visible|TextureIndex|Alpha
    HRESULT Save_UIRects(const wstring& strFilePath);
    HRESULT Load_UIRects(const wstring& strFilePath);

private:
    HRESULT Add_UIText(const wstring& TextName, const wstring& FontTag, const wstring& Text, const _float2& vPos,_float fScale = 1.f, const _float4& vColor = { 1.f, 1.f, 1.f, 1.f });

    HRESULT Save_UITexts(const wstring& strFilePath);
    HRESULT Load_UITexts(const wstring& strFilePath);


public:
    HRESULT Add_Item(const INV_ITEM& Item);
private:
    int Find_EmptyBagSlot() const;

private:
    uint32_t m_iDynamicItemSerial = 0;
private:
    HRESULT Ready_InventorySlots();
    HRESULT Ready_InventoryItems();


    void Update_Inventory(_float fTimeDelta);
    void Update_SlotHover();
    void Update_ItemIconPosition();

    _float2 Get_MouseUIPos();
    _bool Is_PointInRect(const _float2& vPoint, const UI_RECT& Rect);
    int Find_Slot_ByMouse(const _float2& vMousePos);

    void Begin_DragItem(int iSlotIndex);
    void End_DragItem();

    _bool Can_PlaceItem(int iSlotIndex, int iItemIndex);

    HRESULT Render_UIRect_ByKey(const wstring& strKey);
    _bool Is_EquipSlot(int iSlotIndex);
    void Notify_EquipChanged(int iSlotIndex, int iPrevItemIndex, int iNewItemIndex);
    _bool Is_ItemIconKey(const wstring& strKey);

public:
    PLAYER_INVENTORY_SAVE Make_SaveData() const;
    void Apply_SaveData(const PLAYER_INVENTORY_SAVE& Data);

private:
    void Refresh_EquipSlots_ForSaveLoad();

private:
    vector<INV_SLOT> m_InventorySlots;
    vector<INV_ITEM> m_InventoryItems;

    int     m_iHoverSlot = -1;
    int     m_iDragSrcSlot = -1;
    int     m_iDraggingItem = -1;

    _bool   m_bDraggingItem = false;
    _float2 m_vMouseUIPos = { 0.f, 0.f };


private:
    INVEN_ANIM_STATE m_eAnimState = INVEN_ANIM_STATE::CLOSED;

    float   m_fAnimRatio = 0.f;       // 0 ´ÝÈû, 1 ¿­¸²
    float   m_fAnimSpeed = 7.5f;
    float   m_fSlideDistance = 650.f;

    _float2 m_vAnimOffset = { 0.f, 0.f };
    float   m_fAnimAlpha = 0.f;

private:

    void Open_Inven();
    void Close_Inven();

    void Update_InvenAnimation(_float fTimeDelta);
    float ClampFloat(float fValue, float fMin, float fMax);
    float EaseOutCubic(float fRatio);

public:
    void Toggle_Inven();
    void InvenSet(_bool bInvenSet);
    _bool GetInven() const { return m_bInventoryOpen; }


private:
    _bool m_bInventoryOpen = false;

public:
    static unique_ptr<InvenUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END