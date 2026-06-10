#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Client)

class Player_Mouse final : public UIObject
{
private:
    Player_Mouse(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    Player_Mouse(const Player_Mouse& Prototype);

public:
    virtual ~Player_Mouse();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    HRESULT Ready_Components();

    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    _vector Get_MouseWorldPos() const { return XMLoadFloat3(&m_vMouseWorldPos); }
    _vector Get_MouseDir() const { return XMLoadFloat3(&m_vMouseDir); }

private:
    _float3 m_vMouseWorldPos = {};
    _float3 m_vMouseDir = { 0.f, 0.f, 1.f };


private:
    shared_ptr<Shader> m_pShaderCom = nullptr;
    shared_ptr<VIBuffer_Rect> m_pVIBufferCom = nullptr;
    shared_ptr<Texture> m_pTextureCom = nullptr;

private:
    _float2 m_vMouseScreenPos = {};

    _float m_fMarkerSize = 0.5f;


public:
    static unique_ptr<Player_Mouse> Create(
        ComPtr<ID3D11Device> pDevice,
        ComPtr<ID3D11DeviceContext> pContext);

    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END