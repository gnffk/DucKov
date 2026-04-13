#include "VIBuffer.h"

VIBuffer::VIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : Component{ pDevice, pContext }
{
}

VIBuffer::~VIBuffer()
{

}


HRESULT VIBuffer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT VIBuffer::Initialize(void* pArg)
{

    return S_OK;
}

HRESULT VIBuffer::Bind_Resources()
{

    for (auto& mesh : *Meshes_VIBuffers) {
        ID3D11Buffer* pVertexBuffers[] = {
            mesh.m_pVB.Get(),
       // m_pVBInstance.Get(), 
        };

        uint32_t       iVertexStrides[] = {
             mesh.m_iVertexStride,
        };

        uint32_t        iOffSets[] = {
            0,

        };

        m_pContext->IASetVertexBuffers(0, mesh.m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffSets);
        m_pContext->IASetIndexBuffer(mesh.m_pIB.Get(), mesh.m_eIndexFormat, 0);
        m_pContext->IASetPrimitiveTopology(mesh.m_ePrimitiveType);

    
    }
   

    return S_OK;
}

HRESULT VIBuffer::Render()
{
    if (nullptr == m_pContext)
        return E_FAIL;

 
    for (auto& mesh : *Meshes_VIBuffers) {
 

        m_pContext->DrawIndexed(mesh.m_iNumIndices, 0, 0);
    }
   

    return S_OK;
}
