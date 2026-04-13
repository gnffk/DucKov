#include "VIBuffer_Rect.h"

VIBuffer_Rect::VIBuffer_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : VIBuffer{ pDevice, pContext }
{
}

VIBuffer_Rect::~VIBuffer_Rect()
{

}


HRESULT VIBuffer_Rect::Initialize_Prototype()
{
#pragma region mesh 바인딩
    Meshes_VIBuffers = make_shared<vector<Mesh_VIBUFFER>>();
    Meshes_VIBuffers->reserve(1);
    Mesh_VIBUFFER Meshes{};
    Meshes.vertices = make_shared<vector<VTXTEX>>();
    Meshes.indices = make_shared<vector<uint16_t>>();
   
#pragma region VERTEX_BUFFER

    VTXTEX v;

    v.vPosition = _float3(-0.5f, 0.5f, 0.f);
    v.vTexcoord = _float2(0.f, 0.f);
    (*Meshes.vertices).emplace_back(v);

    v.vPosition = _float3(0.5f, 0.5f, 0.f);
    v.vTexcoord = _float2(1.f, 0.f);
    (*Meshes.vertices).emplace_back(v);


    v.vPosition = _float3(0.5f, -0.5f, 0.f);
    v.vTexcoord = _float2(1.f, 1.f);
    (*Meshes.vertices).emplace_back(v);

    v.vPosition = _float3(-0.5f, -0.5f, 0.f);
    v.vTexcoord = _float2(0.f, 1.f);
    (*Meshes.vertices).emplace_back(v);
#pragma endregion


#pragma region INDEX_BUFFER
    (*Meshes.indices).emplace_back(0);
    (*Meshes.indices).emplace_back(1);
    (*Meshes.indices).emplace_back(2);

    (*Meshes.indices).emplace_back(0);
    (*Meshes.indices).emplace_back(2);
    (*Meshes.indices).emplace_back(3);

#pragma endregion


    Meshes.m_iNumVertexBuffers = 1;
    Meshes.m_iNumVertices = (UINT)Meshes.vertices->size();
    Meshes.m_iVertexStride = sizeof(VTXTEX);

    Meshes.m_iNumIndices = (UINT)Meshes.indices->size();
    Meshes.m_iIndexStride = 2;
    Meshes.m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    Meshes.m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    (*Meshes_VIBuffers).emplace_back(Meshes);


#pragma endregion mesh 바인딩
    

#pragma region mesh Buffer Create
    for (auto& mesh : *Meshes_VIBuffers) {
#pragma region VERTEX_BUFFER

        D3D11_BUFFER_DESC           VertexBufferDesc{};
        VertexBufferDesc.ByteWidth = static_cast<UINT>(mesh.vertices->size() * sizeof(VTXTEX));
        VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        VertexBufferDesc.StructureByteStride = mesh.m_iVertexStride;
        VertexBufferDesc.CPUAccessFlags = 0;
        VertexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA          VertexInitialData{};
        VertexInitialData.pSysMem = mesh.vertices->data();

        if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, mesh.m_pVB.GetAddressOf()))) {

            return E_FAIL;
        }


#pragma endregion


#pragma region INDEX_BUFFER
        D3D11_BUFFER_DESC           IndexBufferDesc{};
        IndexBufferDesc.ByteWidth = static_cast<UINT>(mesh.indices->size() * sizeof(uint16_t));
        IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        IndexBufferDesc.StructureByteStride = mesh.m_iIndexStride;
        IndexBufferDesc.CPUAccessFlags = 0;
        IndexBufferDesc.MiscFlags = 0;



        D3D11_SUBRESOURCE_DATA          IndexInitialData{};
        IndexInitialData.pSysMem = mesh.indices->data();

        if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, mesh.m_pIB.GetAddressOf())))
            return E_FAIL;

#pragma endregion
    }

#pragma endregion mesh Buffer Create
}

HRESULT VIBuffer_Rect::Initialize(void* pArg)
{

    return S_OK;
}

unique_ptr<VIBuffer_Rect> VIBuffer_Rect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto		pInstance = unique_ptr<VIBuffer_Rect>(new VIBuffer_Rect(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : VIBuffer_Rect");
        return nullptr;
    }

    return pInstance;
}



shared_ptr<Prototype> VIBuffer_Rect::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<VIBuffer_Rect>(new VIBuffer_Rect(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : VIBuffer_Rect");
        return nullptr;
    }

    return pInstance;
}

