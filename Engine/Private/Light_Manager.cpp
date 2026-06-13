#include "Light_Manager.h"
#include "Light.h"

Light_Manager::Light_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{

}

Light_Manager::~Light_Manager()
{
}

HRESULT Light_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
    auto pLight = Light::Create(m_pDevice, m_pContext, LightDesc);
    if (nullptr == pLight)
        return E_FAIL;

    m_Lights.push_back(pLight);

    return S_OK;
}

HRESULT Light_Manager::Render(shared_ptr<class Shader> pShader, shared_ptr<class VIBuffer_Rect> pVIBuffer)
{
    for (auto& pLight : m_Lights)
    {
        pLight->Render(pShader, pVIBuffer);
    }

    return S_OK;
}

unique_ptr<Light_Manager>  Light_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return  unique_ptr<Light_Manager>(new Light_Manager(pDevice, pContext));
}
