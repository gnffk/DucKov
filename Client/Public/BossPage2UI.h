#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class BossPage2UI final : public UIObject
{
private:
    BossPage2UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    BossPage2UI(const BossPage2UI& Prototype);

public:
    virtual ~BossPage2UI();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_HP(_float fCurHP, _float fMaxHP);
    void Start_Appear();
    void Start_Disappear();
    void Set_BossNameTexture(const wstring& strTextureTag);

private:
    struct UI_RECT
    {
        wstring strName;
        wstring strTextureTag;

        _float2 vPos{};
        _float2 vSize{};

        _float fDepth = 0.f;
        _bool bVisible = true;

        uint32_t iTextureIndex = 0;

        _float fAlpha = 1.f;
        _float4 vColor = { 1.f, 1.f, 1.f, 1.f };

        shared_ptr<Texture> pTexture;
    };

private:
    HRESULT Ready_Components();

    HRESULT Add_UIRect(
        const wstring& UIName,
        const wstring& strName,
        const wstring& strTextureTag,
        const _float2& vPos,
        const _float2& vSize,
        _float fDepth
    );

#ifdef _DEBUG
private:
    void GUI_BossPage2UI();
#endif

    HRESULT Render_UIRect_ByKey(const wstring& strKey);
    HRESULT Render_UIRect(UI_RECT& UI);

private:
    void Update_Appear(_float fTimeDelta);
    void Update_HPBar(_float fTimeDelta);
    void Update_HPBarVisual();
    void Set_BarRatio(const wstring& strKey, const _float2& vBasePos, const _float2& vBaseSize, _float fRatio);

    void Update_Layout();
    void Set_AllAlpha(_float fAlpha);
    void Set_AllVisible(_bool bVisible);

    _float ClampFloat(_float fValue, _float fMin, _float fMax);
    _float LerpFloat(_float fStart, _float fEnd, _float fRatio);
    _float SmoothStep(_float fRatio);

private:
    shared_ptr<Shader>        m_pShaderCom = nullptr;
    shared_ptr<VIBuffer_Rect> m_pVIBufferCom = nullptr;

    map<wstring, UI_RECT> m_UIRects;

private:
    enum class UI_STATE
    {
        HIDDEN,
        APPEAR,
        SHOW,
        DISAPPEAR
    };

    UI_STATE m_eUIState = UI_STATE::HIDDEN;

    _float m_fAppearTimer = 0.f;
    _float m_fAppearDuration = 1.0f;
    _float m_fDisappearDuration = 0.5f;
    _float m_fAlpha = 0.f;

private:
    _float m_fCurHP = 100.f;
    _float m_fMaxHP = 100.f;

    _float m_fHpRatio = 1.f;
    _float m_fDamageRatio = 1.f;

    _float m_fHPBarPaddingX = 4.f;
    _float m_fHPBarPaddingY = 4.f;

private:
    _float2 m_vNameSize = { 40.f, 40.f };
    _float2 m_vHPBarSize = { 339.f, 30.f };

    _float2 m_vNamePos = {};
    _float2 m_vHPBarPos = {};

private:
    _float m_fNameTargetY = 31.f;
    _float m_fHPBarTargetY = 33.f;
    _float m_fSlideStartOffsetY = -35.f;

    _float m_fNameOffsetX = -204.f;
    _float m_fHPBarOffsetX = 0.f;

public:
    static unique_ptr<BossPage2UI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END