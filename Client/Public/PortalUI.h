#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Client)

class Portal;

class PortalUI final : public UIObject
{
private:
    PortalUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    PortalUI(const PortalUI& Prototype);

public:
    virtual ~PortalUI();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_Owner(shared_ptr<GameObject> pOwner) {
        m_pOwner = pOwner;
    }

    weak_ptr<GameObject> GetOwner() {
        return m_pOwner;
    }

private:
    void Update_PortalPrompt(_float fTimeDelta);
    void Try_MoveLevel();

private:
    _bool WorldToScreen(_fvector vWorldPos, _float2& vOutScreenPos);

private:
    weak_ptr<GameObject> m_pOwner;

private:
    _bool m_bVisible = false;
    _bool m_bChangingLevel = false;

private:
    _float2 m_vTextPos = {};
    _float2 m_vBackPos = {};
    _float2 m_vBackSize = { 150.f, 42.f };

private:
    _float3 m_vOwnerUIOffset = { 0.f, 2.2f, 0.f };

public:
    static unique_ptr<PortalUI> Create(
        ComPtr<ID3D11Device> pDevice,
        ComPtr<ID3D11DeviceContext> pContext);

    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END