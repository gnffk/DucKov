#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL UIObject abstract : public GameObject
{
public:
	typedef struct tagUIObjectDesc : public GameObject::GAMEOBJECT_DESC
	{
		_float			fX, fY, fSizeX, fSizeY;
	}UIOBJECT_DESC;

    typedef struct tagUIRect
    {
        wstring strName = L"";
        wstring strTextureTag = L"";

        shared_ptr<Texture> pTexture = nullptr;

        _float2 vPos = {};
        _float2 vSize = {};
        _float  fDepth = 0.f;
        _float4 vColor = { 1.f,1.f,1.f,1.f };
        float fAlpha = 1.f;
        uint32_t iTextureIndex = 0;
        _bool bVisible = true;

    } UI_RECT;

    typedef struct tagUIText
    {
        wstring strFontTag;
        wstring strText;
        
        _float2 vPos = { 0.f, 0.f };
        _float fScale = 1.f;
        _float4 vColor = { 1.f, 1.f, 1.f, 1.f };
        _bool bVisible = true;

    } UI_TEXT;

    enum class SLOT_KIND
    {
        BAG,
        GUN,
        MELEE,
        HEAD,
        CLOTHES,
        END
    };

    struct INV_SLOT
    {
        wstring     strSlotRectKey;   // m_UIRects에 있는 박스 UI 이름
        SLOT_KIND   eKind = SLOT_KIND::END;

        int         iItemIndex = -1;  // 이 슬롯에 들어있는 아이템 번호
        float       fNormalAlpha = 0.20f;
        float       fHoverAlpha = 0.65f;
    };

    struct INV_ITEM
    {
        wstring     strItemName;
        wstring     strIconRectKey;
        wstring     strTextureTag;

        SLOT_KIND   eEquipKind = SLOT_KIND::END;

        _float2     vIconSize = { 48.f, 48.f };
    };

protected:
	UIObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~UIObject();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float				m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float4x4			m_ViewMatrix{}, m_ProjMatrix{};

protected:
	void Update_Transform();



};

NS_END