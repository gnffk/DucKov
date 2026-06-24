#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class MainMenu_UI final : public UIObject
{
public:
    struct UI_RECT
    {
        wstring strName;
        wstring strTextureTag;

        _float2 vPos = {};
        _float2 vSize = {};
        _float fDepth = 0.f;

        int32_t iTextureIndex = 0;
        _bool bVisible = true;

        _float fAlpha = 1.f;
        _float4 vColor = { 1.f, 1.f, 1.f, 1.f };

        shared_ptr<Texture> pTexture = nullptr;
    };

    struct UI_TEXT
    {
        wstring strFontTag;
        wstring strText;

        _float2 vPos = {};
        _float fScale = 1.f;
        _float4 vColor = { 1.f, 1.f, 1.f, 1.f };

        _bool bVisible = true;
    };

private:
    MainMenu_UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    MainMenu_UI(const MainMenu_UI& Prototype);

public:
    virtual ~MainMenu_UI();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    HRESULT Ready_Components();
    HRESULT Ready_UI();

private:
    HRESULT Add_UIRect(
        const wstring& UIName,
        const wstring& strName,
        const wstring& strTextureTag,
        const _float2& vPos,
        const _float2& vSize,
        _float fDepth);

    HRESULT Add_UIText(
        const wstring& TextName,
        const wstring& FontTag,
        const wstring& Text,
        const _float2& vPos,
        _float fScale,
        const _float4& vColor);

private:
    HRESULT Render_UIRect(UI_RECT& UI);
    HRESULT Render_UIRect_ByKey(const wstring& strKey);

private:
    void Update_Button();
    void Update_SettingPanel();

private:
    _float2 Get_MouseUIPos();
    _bool Is_PointInRect(const _float2& vPoint, const UI_RECT& Rect);

private:
    void Click_Start();
    void Click_Setting();
    void Click_Exit();

private:
#ifdef _DEBUG
    void GUI_MainMenuUI();
#endif

private:
    HRESULT Save_UIRects(const wstring& strFilePath);
    HRESULT Load_UIRects(const wstring& strFilePath);
    HRESULT Save_UITexts(const wstring& strFilePath);
    HRESULT Load_UITexts(const wstring& strFilePath);

private:
    shared_ptr<Shader> m_pShaderCom = nullptr;
    shared_ptr<VIBuffer_Rect> m_pVIBufferCom = nullptr;

private:
    map<wstring, UI_RECT> m_UIRects;
    map<wstring, UI_TEXT> m_UITexts;

private:
    _float2 m_vMouseUIPos = {};
    int32_t m_iHoverButton = -1;

private:
    _bool m_bSettingOpen = false;

public:
    static unique_ptr<MainMenu_UI> Create(
        ComPtr<ID3D11Device> pDevice,
        ComPtr<ID3D11DeviceContext> pContext);

    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END