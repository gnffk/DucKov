#include "VIBuffer_Instance.h"

VIBuffer_Instance::VIBuffer_Instance(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : VIBuffer{ pDevice, pContext }
{
}

VIBuffer_Instance::~VIBuffer_Instance()
{
}

HRESULT VIBuffer_Instance::Initialize_Prototype(void* pArg)
{
    return S_OK;
}

HRESULT VIBuffer_Instance::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT VIBuffer_Instance::Bind_Resources()
{
    ID3D11Buffer* pVertexBuffers[] = {
         m_pVB.Get(),
         m_pVBInstance.Get(),
    };

    uint32_t       iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceVertexStride
    };

    uint32_t        iOffSets[] = {
        0,
        0,
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffSets);
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

    return S_OK;
}

HRESULT VIBuffer_Instance::Render()
{
    m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstances, 0, 0, 0);

    return S_OK;
}
