#include "VIBuffer_Mesh_Instance.h"

#include "GameInstance.h"


VIBuffer_Mesh_Instance::VIBuffer_Mesh_Instance( ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext): VIBuffer_Instance{ pDevice, pContext }
{
}

VIBuffer_Mesh_Instance::VIBuffer_Mesh_Instance(const VIBuffer_Mesh_Instance& Prototype): VIBuffer_Instance{ Prototype }
    , vertices{ Prototype.vertices }
    , indices{ Prototype.indices }
    , m_iMaterialIndex{ Prototype.m_iMaterialIndex }
    , m_iInstanceStride{ Prototype.m_iInstanceStride }
{
    // Static VB / IB는 공유해도 됨.
    // 하지만 InstanceVB는 Clone마다 따로 가져야 함.
    m_pInstanceVB = nullptr;
    m_iNumInstances = 0;
    m_iInstanceCapacity = 0;
}

VIBuffer_Mesh_Instance::~VIBuffer_Mesh_Instance()
{
}

HRESULT VIBuffer_Mesh_Instance::Initialize(shared_ptr<vector<VTXMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices, uint32_t materialIndex, _fmatrix PreTransformMatrix)
{
    vertices = pvertices;
    indices = pindices;
    m_iMaterialIndex = materialIndex;

    m_iNumVertexBuffers = 2;
    m_iNumVertices = static_cast<UINT>(vertices->size());
    m_iVertexStride = sizeof(VTXMESH);

    m_iNumIndices = static_cast<UINT>(indices->size());

    // 이거 기존 코드에서 2로 되어 있었는데 잘못됨.
    // DXGI_FORMAT_R32_UINT면 uint32_t라서 4바이트.
    m_iIndexStride = sizeof(uint32_t);

    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        XMStoreFloat3(
            &(*vertices)[i].vPosition,
            XMVector3TransformCoord(
                XMLoadFloat3(&(*vertices)[i].vPosition),
                PreTransformMatrix));

        XMStoreFloat3(
            &(*vertices)[i].vNormal,
            XMVector3TransformNormal(
                XMLoadFloat3(&(*vertices)[i].vNormal),
                PreTransformMatrix));
    }

    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = static_cast<UINT>(vertices->size() * sizeof(VTXMESH));
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = vertices->data();

    if (FAILED(m_pDevice->CreateBuffer(
        &VertexBufferDesc,
        &VertexInitialData,
        m_pVB.GetAddressOf())))
    {
        return E_FAIL;
    }

    D3D11_BUFFER_DESC IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = static_cast<UINT>(indices->size() * sizeof(uint32_t));
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA IndexInitialData{};
    IndexInitialData.pSysMem = indices->data();

    if (FAILED(m_pDevice->CreateBuffer(
        &IndexBufferDesc,
        &IndexInitialData,
        m_pIB.GetAddressOf())))
    {
        return E_FAIL;
    }

    // 처음에는 최소 1개짜리 Instance Buffer를 만들어둠.
    if (FAILED(Create_InstanceBuffer(2000)))
        return E_FAIL;

    return S_OK;
}

HRESULT VIBuffer_Mesh_Instance::Create_InstanceBuffer(uint32_t iCapacity)
{
    if (iCapacity == 0)
        iCapacity = 1;

    m_pInstanceVB.Reset();

    m_iInstanceCapacity = iCapacity;
    m_iNumInstances = 0;

    D3D11_BUFFER_DESC InstanceBufferDesc{};
    InstanceBufferDesc.ByteWidth =sizeof(INSTANCE_MESH_DATA) * m_iInstanceCapacity;

    InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    InstanceBufferDesc.MiscFlags = 0;
    InstanceBufferDesc.StructureByteStride = sizeof(INSTANCE_MESH_DATA);

    if (FAILED(m_pDevice->CreateBuffer(
        &InstanceBufferDesc,
        nullptr,
        m_pInstanceVB.GetAddressOf())))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT VIBuffer_Mesh_Instance::Update_InstanceBuffer(const vector<_float4x4>& InstanceWorlds)
{
    uint32_t iInstanceCount = static_cast<uint32_t>(InstanceWorlds.size());

    if (iInstanceCount == 0)
    {
        m_iNumInstances = 0;
        return S_OK;
    }

    if (m_iInstanceCapacity < iInstanceCount)
    {
        uint32_t iNewCapacity = max(iInstanceCount, m_iInstanceCapacity * 2);

        if (FAILED(Create_InstanceBuffer(iNewCapacity)))
            return E_FAIL;
    }

    D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

    if (FAILED(m_pContext->Map(
        m_pInstanceVB.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &MappedSubResource)))
    {
        return E_FAIL;
    }

    memcpy(MappedSubResource.pData,InstanceWorlds.data(),sizeof(_float4x4) * iInstanceCount);

    m_pContext->Unmap(m_pInstanceVB.Get(), 0);

    m_iNumInstances = iInstanceCount;

    return S_OK;
}

HRESULT VIBuffer_Mesh_Instance::Bind_Resources()
{
    ID3D11Buffer* pVertexBuffers[] =
    {
        m_pVB.Get(),          // Slot 0 : VTXMESH
        m_pInstanceVB.Get()   // Slot 1 : INSTANCE_MESH_DATA
    };

    UINT iStrides[] =
    {
        sizeof(VTXMESH),
        sizeof(INSTANCE_MESH_DATA)
    };

    UINT iOffsets[] =
    {
        0,
        0
    };

    m_pContext->IASetVertexBuffers(
        0,
        2,
        pVertexBuffers,
        iStrides,
        iOffsets);

    m_pContext->IASetIndexBuffer(
        m_pIB.Get(),
        m_eIndexFormat,
        0);

    m_pContext->IASetPrimitiveTopology(
        m_ePrimitiveType);

    return S_OK;
}

HRESULT VIBuffer_Mesh_Instance::Render()
{
    if (m_iNumInstances == 0)
        return S_OK;

    m_pContext->DrawIndexedInstanced(m_iNumIndices, m_iNumInstances, 0, 0, 0);

    return S_OK;
}

shared_ptr<VIBuffer_Mesh_Instance> VIBuffer_Mesh_Instance::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,shared_ptr<vector<VTXMESH>> pvertices,shared_ptr<vector<uint32_t>> pindices,uint32_t materialIndex,_fmatrix PreTransformMatrix)
{
    auto pInstance = shared_ptr<VIBuffer_Mesh_Instance>( new VIBuffer_Mesh_Instance(pDevice, pContext));

    if (FAILED(pInstance->Initialize( pvertices, pindices, materialIndex,PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created : VIBuffer_Mesh_Instance");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> VIBuffer_Mesh_Instance::Clone(void* pArg)
{
    auto pInstance = shared_ptr<VIBuffer_Mesh_Instance>( new VIBuffer_Mesh_Instance(*this));

    uint32_t iCapacity = 1;

    if (pArg != nullptr)
        iCapacity = *reinterpret_cast<uint32_t*>(pArg);

    if (FAILED(pInstance->Create_InstanceBuffer(iCapacity)))
    {
        MSG_BOX("Failed to Cloned : VIBuffer_Mesh_Instance");
        return nullptr;
    }

    return pInstance;
}