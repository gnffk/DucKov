#include "VIBuffer_Cube.h"

VIBuffer_Cube::VIBuffer_Cube(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : VIBuffer{ pDevice, pContext }
{
}

VIBuffer_Cube::~VIBuffer_Cube()
{

}


HRESULT VIBuffer_Cube::Initialize_Prototype()
{
    m_iNumVertexBuffers = 1;
    m_iNumVertices = 8;
    m_iVertexStride = sizeof(VTXCUBE);
    m_iNumIndices = 36;
    m_iIndexStride = 2;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;

    VTXCUBE* pVertices = new VTXCUBE[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXCUBE) * m_iNumVertices);

    pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
    pVertices[0].vTexcoord = pVertices[0].vPosition;

    pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
    pVertices[1].vTexcoord = pVertices[1].vPosition;

    pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
    pVertices[2].vTexcoord = pVertices[2].vPosition;

    pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
    pVertices[3].vTexcoord = pVertices[3].vPosition;

    pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
    pVertices[4].vTexcoord = pVertices[4].vPosition;

    pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
    pVertices[5].vTexcoord = pVertices[5].vPosition;

    pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
    pVertices[6].vTexcoord = pVertices[6].vPosition;

    pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
    pVertices[7].vTexcoord = pVertices[7].vPosition;

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

    pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
    pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

    pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
    pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;	// Y+

    pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
    pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

    pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
    pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

    pIndices[24] = 7; pIndices[25] = 6; pIndices[26] = 5;
    pIndices[27] = 7; pIndices[28] = 5; pIndices[29] = 4;

    pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
    pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

    D3D11_SUBRESOURCE_DATA          IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

#pragma endregion

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);

    return S_OK;
}

HRESULT VIBuffer_Cube::Initialize(void* pArg)
{

    return S_OK;
}

unique_ptr<VIBuffer_Cube> VIBuffer_Cube::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto		pInstance = unique_ptr<VIBuffer_Cube>(new VIBuffer_Cube(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : VIBuffer_Cube");
        return nullptr;
    }

    return pInstance;
}



shared_ptr<Prototype> VIBuffer_Cube::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<VIBuffer_Cube>(new VIBuffer_Cube(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : VIBuffer_Cube");
        return nullptr;
    }

    return pInstance;
}

