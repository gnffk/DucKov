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


   vertices = make_shared<vector<VTXTEX>>();
   indices = make_shared<vector<uint16_t>>();


    VTXTEX v;

    v.vPosition = _float3(-0.5f, 0.5f, 0.f);
    v.vTexcoord = _float2(0.f, 0.f);
    (*vertices).emplace_back(v);

    v.vPosition = _float3(0.5f, 0.5f, 0.f);
    v.vTexcoord = _float2(1.f, 0.f);
    (*vertices).emplace_back(v);


    v.vPosition = _float3(0.5f, -0.5f, 0.f);
    v.vTexcoord = _float2(1.f, 1.f);
    (*vertices).emplace_back(v);

    v.vPosition = _float3(-0.5f, -0.5f, 0.f);
    v.vTexcoord = _float2(0.f, 1.f);
    (*vertices).emplace_back(v);

    (*indices).emplace_back(0);
    (*indices).emplace_back(1);
    (*indices).emplace_back(2);

    (*indices).emplace_back(0);
    (*indices).emplace_back(2);
    (*indices).emplace_back(3);


     m_iNumVertexBuffers = 1;
     m_iNumVertices = (UINT)vertices->size();
     m_iVertexStride = sizeof(VTXTEX);
     
     m_iNumIndices = (UINT)indices->size();
     m_iIndexStride = 2;
     m_eIndexFormat = DXGI_FORMAT_R16_UINT;
     m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;





        D3D11_BUFFER_DESC           VertexBufferDesc{};
        VertexBufferDesc.ByteWidth = static_cast<UINT>(vertices->size() * sizeof(VTXTEX));
        VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        VertexBufferDesc.StructureByteStride = m_iVertexStride;
        VertexBufferDesc.CPUAccessFlags = 0;
        VertexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA          VertexInitialData{};
        VertexInitialData.pSysMem = vertices->data();

        if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, m_pVB.GetAddressOf()))) {

            return E_FAIL;
        }





        D3D11_BUFFER_DESC           IndexBufferDesc{};
        IndexBufferDesc.ByteWidth = static_cast<UINT>(indices->size() * sizeof(uint16_t));
        IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        IndexBufferDesc.StructureByteStride = m_iIndexStride;
        IndexBufferDesc.CPUAccessFlags = 0;
        IndexBufferDesc.MiscFlags = 0;



        D3D11_SUBRESOURCE_DATA          IndexInitialData{};
        IndexInitialData.pSysMem = indices->data();

        if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, m_pIB.GetAddressOf())))
            return E_FAIL;


        return S_OK;
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

