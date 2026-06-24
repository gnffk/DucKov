#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class Loading_BackGround_UI final : public UIObject
{
public:
    struct LOADING_UI_DESC : public UIObject::UIOBJECT_DESC
    {
        LEVEL eNextLevel = LEVEL::END;
    };

private:
    Loading_BackGround_UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    Loading_BackGround_UI(const Loading_BackGround_UI& Prototype);

public:
    virtual ~Loading_BackGround_UI();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    HRESULT Ready_Components();
    HRESULT Bind_ShaderResources();

private:
    shared_ptr<Shader>          m_pShaderCom = nullptr;
    shared_ptr<Texture>         m_pTextureCom = nullptr;
    shared_ptr<VIBuffer_Rect>   m_pVIBufferCom = nullptr;

private:
    LEVEL m_eNextLevel = LEVEL::END;

private:
    _bool m_bUseSpecialLoading = false;

private:
    _float m_fFrameAcc = 0.f;
    _float m_fFrameInterval = 1.f / 30.f;

    uint32_t m_iFrameIndex = 0;
    uint32_t m_iFrameCount = 90;
private:
    _bool m_bAnimationFinished = false;

public:
    _bool Is_AnimationFinished() const
    {
        return m_bAnimationFinished;
    }
public:
    static unique_ptr<Loading_BackGround_UI> Create(
        ComPtr<ID3D11Device> pDevice,
        ComPtr<ID3D11DeviceContext> pContext);

    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END