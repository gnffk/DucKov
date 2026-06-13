#include "Light.h"

#include "GameInstance.h"

Light::Light(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{

}

HRESULT Light::Initialize(const LIGHT_DESC& LightDesc)
{
    m_LightDesc = LightDesc;

    return S_OK;
}

HRESULT Light::Render(shared_ptr<Shader> pShader, shared_ptr<VIBuffer_Rect> pVIBuffer)
{
    uint32_t       iPassIndex = {};

    if (LIGHT::DIRECTIONAL == m_LightDesc.eType)
    {
        if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof m_LightDesc.vDirection)))
            return E_FAIL;

        iPassIndex = ETOUI(DEFERRED::DIRECTIONAL);
    }
    else
    {
        if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof m_LightDesc.vPosition)))
            return E_FAIL;
        if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof m_LightDesc.fRange)))
            return E_FAIL;

        iPassIndex = ETOUI(DEFERRED::POINT);
    }

    if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof m_LightDesc.vDiffuse)))
        return E_FAIL;
    if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof m_LightDesc.vDiffuse)))
        return E_FAIL;
    if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof m_LightDesc.vSpecular)))
        return E_FAIL;


    if (FAILED(pShader->Begin(iPassIndex)))
        return E_FAIL;

    if (FAILED(pVIBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

shared_ptr<Light> Light::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const LIGHT_DESC& LightDesc)
{
    auto pInstance = shared_ptr<Light>(new Light(pDevice, pContext));

    if (FAILED(pInstance->Initialize(LightDesc)))
        MSG_BOX("Failed to Created : Light");

    return pInstance;
}

