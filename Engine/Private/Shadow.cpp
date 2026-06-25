#include "Shadow.h"
#include "GameInstance.h"

Shadow::Shadow()
{
}

Shadow::~Shadow()
{
}

const _float4x4* Shadow::Get_ShadowLightTransform(D3DTS eState)
{
    return &m_LightMatrices[ETOUI(eState)];
}



HRESULT Shadow::Add_Shadow_Light(const SHADOW_LIGHT_DESC& LightDesc)
{
    XMStoreFloat4x4(&m_LightMatrices[ETOUI(D3DTS::VIEW)],
        XMMatrixLookAtLH(XMLoadFloat4(&LightDesc.vEye), XMLoadFloat4(&LightDesc.vAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    _float      fAspect = CGameInstance::Get().Get_ViewportSize().x / CGameInstance::Get().Get_ViewportSize().y;

    XMStoreFloat4x4(&m_LightMatrices[ETOUI(D3DTS::PROJ)],
        XMMatrixPerspectiveFovLH(LightDesc.fFovy, fAspect, LightDesc.fNear, LightDesc.fFar));

    return S_OK;
}

HRESULT Shadow::Set_ShadowLightDesc(const SHADOW_LIGHT_DESC& ShadowLightDesc)
{
    m_ShadowLightDesc = ShadowLightDesc;

    _vector vEye = XMLoadFloat4(&m_ShadowLightDesc.vEye);
    _vector vAt = XMLoadFloat4(&m_ShadowLightDesc.vAt);
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

    XMStoreFloat4x4(&m_LightMatrices[ETOUI(D3DTS::VIEW)], XMMatrixLookAtLH(vEye, vAt, vUp));

    XMStoreFloat4x4(&m_LightMatrices[ETOUI(D3DTS::PROJ)],XMMatrixPerspectiveFovLH(
            m_ShadowLightDesc.fFovy,
            static_cast<float>(g_iMaxWidth) / static_cast<float>(g_iMaxHeight),
            m_ShadowLightDesc.fNear,
            m_ShadowLightDesc.fFar
        )
    );

    return S_OK;
}

unique_ptr<Shadow> Shadow::Create()
{
    return unique_ptr<Shadow>(new Shadow());
}
