#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"


VIBuffer_Particle_Point::VIBuffer_Particle_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : VIBuffer_Instance{ pDevice, pContext }
{
}

VIBuffer_Particle_Point::~VIBuffer_Particle_Point()
{
}

HRESULT VIBuffer_Particle_Point::Initialize_Prototype(void* pArg)
{
    auto        pDesc = static_cast<PARTICLE_INSTANCE_DESC*>(pArg);

    m_iNumVertexBuffers = 2;
    m_iNumVertices = 1;
    m_iVertexStride = sizeof(VTXPOS);
    m_iNumIndices = 0;
    m_iIndexStride = 0;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    m_iInstanceVertexStride = sizeof(VTXINSTANCE_PARTICLE);
    m_iIndexCountPerInstance = m_iNumIndices;
    m_iNumInstances = pDesc->iNumInstances;

#pragma region VERTEX_BUFFER

    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;

    unique_ptr<VTXPOS[]> pVertices = make_unique<VTXPOS[]>(m_iNumVertices);

    pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);

    D3D11_SUBRESOURCE_DATA          VertexInitialData{};
    VertexInitialData.pSysMem = pVertices.get();

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER
    /*   D3D11_BUFFER_DESC           IndexBufferDesc{};
       IndexBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
       IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
       IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
       IndexBufferDesc.StructureByteStride = m_iIndexStride;
       IndexBufferDesc.CPUAccessFlags = 0;
       IndexBufferDesc.MiscFlags = 0;

       unique_ptr<uint16_t[]> pIndices = make_unique<uint16_t[]>(m_iNumIndices);

       pIndices[0] = 0;
       pIndices[1] = 1;
       pIndices[2] = 2;

       pIndices[3] = 0;
       pIndices[4] = 2;
       pIndices[5] = 3;

       D3D11_SUBRESOURCE_DATA          IndexInitialData{};
       IndexInitialData.pSysMem = pIndices.get();

       if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
           return E_FAIL;*/

#pragma endregion

#pragma region INSTANCE_BUFFER

    m_InstanceBufferDesc.ByteWidth = m_iNumInstances * m_iInstanceVertexStride;
    m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
    m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_InstanceBufferDesc.MiscFlags = 0;

    // m_pInstanceData = make_shared<VTXINSTANCE_PARTICLE[]>(m_iNumInstances);
    m_pInstanceData = shared_ptr<VTXINSTANCE_PARTICLE[]>(new VTXINSTANCE_PARTICLE[m_iNumInstances]);
    m_pSpeeds = shared_ptr<_float[]>(new _float[m_iNumInstances]);
    m_isLoop = pDesc->isLoop;
    m_vPivot = pDesc->vPivot;

    for (size_t i = 0; i < m_iNumInstances; i++)
    {
        _float      fScale = CGameInstance::Get().Random(pDesc->vSize.x, pDesc->vSize.y);
        m_pSpeeds[i] = CGameInstance::Get().Random(pDesc->vSpeed.x, pDesc->vSpeed.y);

        m_pInstanceData[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
        m_pInstanceData[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
        m_pInstanceData[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
        m_pInstanceData[i].vTranslation = _float4(
            CGameInstance::Get().Random((pDesc->vCenter.x - pDesc->vRange.x) * 0.5f, (pDesc->vCenter.x + pDesc->vRange.x) * 0.5f),
            CGameInstance::Get().Random((pDesc->vCenter.y - pDesc->vRange.y) * 0.5f, (pDesc->vCenter.y + pDesc->vRange.y) * 0.5f),
            CGameInstance::Get().Random((pDesc->vCenter.z - pDesc->vRange.z) * 0.5f, (pDesc->vCenter.z + pDesc->vRange.z) * 0.5f),
            1.f);

        m_pInstanceData[i].vLifeTime = _float2(
            CGameInstance::Get().Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y),
            0.f);

    }


#pragma endregion

    return S_OK;
}

HRESULT VIBuffer_Particle_Point::Initialize(void* pArg)
{

    D3D11_SUBRESOURCE_DATA          InstanceInitialData{};
    InstanceInitialData.pSysMem = m_pInstanceData.get();

    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

HRESULT VIBuffer_Particle_Point::Bind_Resources()
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

    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

    return S_OK;
}

HRESULT VIBuffer_Particle_Point::Render()
{
    m_pContext->DrawInstanced(1, m_iNumInstances, 0, 0);

    return S_OK;
}
void VIBuffer_Particle_Point::Drop(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE            MappedSubResource{};

    if (FAILED(m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource)))
        return;

    auto        pVertices = static_cast<VTXINSTANCE_PARTICLE*>(MappedSubResource.pData);

    for (uint32_t i = 0; i < m_iNumInstances; i++)
    {
        pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
        pVertices[i].vLifeTime.y += fTimeDelta;

        if (true == m_isLoop &&
            pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            pVertices[i].vTranslation.y = m_pInstanceData[i].vTranslation.y;
            pVertices[i].vLifeTime.y = 0.f;
        }
    }

    m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void VIBuffer_Particle_Point::Rise(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE            MappedSubResource{};

    if (FAILED(m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource)))
        return;

    auto        pVertices = static_cast<VTXINSTANCE_PARTICLE*>(MappedSubResource.pData);

    for (uint32_t i = 0; i < m_iNumInstances; i++)
    {
        pVertices[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta;
        pVertices[i].vLifeTime.y += fTimeDelta;

        if (true == m_isLoop &&
            pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            pVertices[i].vTranslation.y = m_pInstanceData[i].vTranslation.y;
            pVertices[i].vLifeTime.y = 0.f;
        }
    }

    m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void VIBuffer_Particle_Point::Spread(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE            MappedSubResource{};

    if (FAILED(m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource)))
        return;

    auto        pVertices = static_cast<VTXINSTANCE_PARTICLE*>(MappedSubResource.pData);

    for (uint32_t i = 0; i < m_iNumInstances; i++)
    {
        // pVertices[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta;
        pVertices[i].vLifeTime.y += fTimeDelta;

        _vector     vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f);

        XMStoreFloat4(&pVertices[i].vTranslation,
            XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeeds[i] * fTimeDelta);




        if (true == m_isLoop &&
            pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            pVertices[i].vTranslation = m_pInstanceData[i].vTranslation;
            pVertices[i].vLifeTime.y = 0.f;
        }
    }

    m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void VIBuffer_Particle_Point::Smoke_Stay(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

    if (FAILED(m_pContext->Map(
        m_pVBInstance.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &MappedSubResource)))
    {
        return;
    }

    auto pVertices =
        static_cast<VTXINSTANCE_PARTICLE*>(MappedSubResource.pData);

    for (uint32_t i = 0; i < m_iNumInstances; ++i)
    {
        // 이미 수명 끝난 파티클은 그대로 둠
        if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
            continue;

        // 위치는 고정하고 시간만 증가
        pVertices[i].vLifeTime.y += fTimeDelta;

        // 연기 느낌으로 아주 살짝만 위로 뜨게 하고 싶으면 사용
        // 완전히 고정하고 싶으면 이 줄도 빼면 됨.
        pVertices[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta * 0.08f;
    }

    m_pContext->Unmap(m_pVBInstance.Get(), 0);
}
unique_ptr<VIBuffer_Particle_Point> VIBuffer_Particle_Point::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    auto		pInstance = unique_ptr<VIBuffer_Particle_Point>(new VIBuffer_Particle_Point(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Created : VIBuffer_Particle_Point");
        return nullptr;
    }

    return pInstance;
}



shared_ptr<Prototype> VIBuffer_Particle_Point::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<VIBuffer_Particle_Point>(new VIBuffer_Particle_Point(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : VIBuffer_Particle_Point");
        return nullptr;
    }

    return pInstance;
}

