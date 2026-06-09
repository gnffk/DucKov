#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class MainUI final : public UIObject
{
private:
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

private:
    MainUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    MainUI(const MainUI& Prototype);

public:
    virtual ~MainUI();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    
    void    GUI_MainUI();

private:
    shared_ptr<Shader>        m_pShaderCom = nullptr;
    shared_ptr<VIBuffer_Rect> m_pVIBufferCom = nullptr;

    map<wstring, UI_RECT> m_UIRects;

private:
    HRESULT Ready_Components();
    HRESULT Add_UIRect(const wstring& UIName, const wstring& strName,const wstring& strTextureTag,const _float2& vPos,const _float2& vSize, _float fDepth);
    HRESULT Render_UIRect(UI_RECT& UI);

    //Name|TextureTag|PosX|PosY|SizeX|SizeY|Depth|Visible|TextureIndex|Alpha
    HRESULT Save_UIRects(const wstring& strFilePath);
    HRESULT Load_UIRects(const wstring& strFilePath);

public:
    static unique_ptr<MainUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END