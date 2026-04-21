#include "BaseCollider.h"

BaseCollider::BaseCollider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext) : Component{ pDevice, pContext }
{
}

BaseCollider::~BaseCollider()
{
}

HRESULT BaseCollider::Initialize_Prototype() {

	return S_OK;
}
HRESULT BaseCollider::Initialize(void* pArg) {
	return S_OK;
}


HRESULT BaseCollider::Bind_Resources()
{


    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB.Get(),
        // m_pVBInstance.Get(), 
    };

    uint32_t       iVertexStrides[] = {
        m_iVertexStride,
    };

    uint32_t        iOffSets[] = {
        0,

    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffSets);
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);




    return S_OK;
}

HRESULT BaseCollider::Render()
{
    if (nullptr == m_pContext)
        return E_FAIL;



    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);


    return S_OK;
}
