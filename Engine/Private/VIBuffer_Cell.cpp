#include "VIBuffer_Cell.h"

VIBuffer_Cell::VIBuffer_Cell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : VIBuffer{ pDevice, pContext }
{
}

VIBuffer_Cell::~VIBuffer_Cell()
{

}


HRESULT VIBuffer_Cell::Initialize_Prototype(const _float3* pPoints)
{
    m_iNumVertexBuffers = 1;
    m_iNumVertices = 3;
    m_iVertexStride = sizeof(VTXPOS);
    m_iNumIndices = 4;
    m_iIndexStride = 2;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

#pragma region VERTEX_BUFFER
    /*
     UINT ByteWidth;
     D3D11_USAGE Usage;
     UINT BindFlags;
     UINT CPUAccessFlags;
     UINT MiscFlags;
     UINT StructureByteStride;
    */
    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;

    VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
    memcpy(pVertices, pPoints, sizeof(VTXPOS) * m_iNumVertices);

    D3D11_SUBRESOURCE_DATA          VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER
    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;

    uint16_t* pIndices = new uint16_t[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(uint16_t) * m_iNumIndices);

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;
    pIndices[3] = 0;

    D3D11_SUBRESOURCE_DATA          IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

#pragma endregion

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT VIBuffer_Cell::Initialize(void* pArg)
{

    return S_OK;
}

unique_ptr<VIBuffer_Cell> VIBuffer_Cell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints)
{
    auto		pInstance = unique_ptr<VIBuffer_Cell>(new VIBuffer_Cell(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(pPoints)))
    {
        MSG_BOX("Failed to Created : VIBuffer_Cell");
        return nullptr;
    }

    return pInstance;
}



shared_ptr<Prototype> VIBuffer_Cell::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<VIBuffer_Cell>(new VIBuffer_Cell(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : VIBuffer_Cell");
        return nullptr;
    }

    return pInstance;
}

