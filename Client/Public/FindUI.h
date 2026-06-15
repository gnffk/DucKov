#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class FindUI final : public UIObject
{
private:
  

private:
    FindUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    FindUI(const FindUI& Prototype);

public:
    virtual ~FindUI();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    void    GUI_FindUI();

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
    HRESULT Add_UIText(const wstring& TextName, const wstring& FontTag, const wstring& Text, const _float2& vPos, _float fScale = 1.f, const _float4& vColor = { 1.f, 1.f, 1.f, 1.f });

    HRESULT Save_UITexts(const wstring& strFilePath);
    HRESULT Load_UITexts(const wstring& strFilePath);


    void Update_SlotHover();
    void Update_ItemIconPosition();

    _float2 Get_MouseUIPos();
    _bool Is_PointInRect(const _float2& vPoint, const UI_RECT& Rect);
    int Find_Slot_ByMouse(const _float2& vMousePos);

    void Begin_DragItem(int iSlotIndex);
    void End_DragItem();

    _bool Can_PlaceItem(int iSlotIndex, int iItemIndex);

    HRESULT Render_UIRect_ByKey(const wstring& strKey);

    _bool Is_ItemIconKey(const wstring& strKey);
private:
    vector<INV_SLOT> m_InventorySlots;
    vector<INV_ITEM> m_InventoryItems;

    int     m_iHoverSlot = -1;
    int     m_iDragSrcSlot = -1;
    int     m_iDraggingItem = -1;

    _bool   m_bDraggingItem = false;
    _float2 m_vMouseUIPos = { 0.f, 0.f };

    private:
        weak_ptr<GameObject> m_pOwner;
        weak_ptr<GameObject> m_pPlayer;

        _bool m_bCollidingOwner = false;
        _bool m_bBoxOpen = false;

        _float3 m_vOwnerUIOffset = { 0.f, 2.f, 0.f };

        _float2 m_vFindTextScreenPos = { 0.f, 0.f };
        _bool   m_bFindTextOnScreen = false;

public:
    void Set_Owner(shared_ptr<GameObject> pOwner) { m_pOwner = pOwner; }
    void Set_Player(shared_ptr<GameObject> pPlayer) { m_pPlayer = pPlayer; }

    void Set_CollidingOwner(_bool bColliding)
    {
        m_bCollidingOwner = bColliding;

        if (false == m_bCollidingOwner)
            m_bBoxOpen = false;
    }

    weak_ptr<GameObject> GetOwner() { return m_pPlayer; }
    
private:
    _bool WorldToScreen(_fvector vWorldPos, _float2& vOutScreenPos);
    void Update_FindPrompt(_float fTimeDelta);
    void Update_LootBox(_float fTimeDelta);

    HRESULT Ready_LootSlots();
    HRESULT Ready_LootItems();

    void Farm_Item(int iSlotIndex);


public:
    static unique_ptr<FindUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END