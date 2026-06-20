#include "VIBuffer_Particle_Rect.h"
#include "GameInstance.h"
#include "Particle_System.h"

VIBuffer_Particle_Rect::VIBuffer_Particle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : VIBuffer_Instance{ pDevice, pContext }
{
}

VIBuffer_Particle_Rect::~VIBuffer_Particle_Rect()
{
}

HRESULT VIBuffer_Particle_Rect::Initialize_Prototype(void* pArg)
{
    auto        pDesc = static_cast<PARTICLE_INSTANCE_DESC*>(pArg);

    m_iNumVertexBuffers = 2;
    m_iNumVertices = 4;
    m_iVertexStride = sizeof(VTXTEX);
    m_iNumIndices = 6;
    m_iIndexStride = 2;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
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

    unique_ptr<VTXTEX[]> pVertices = make_unique<VTXTEX[]>(m_iNumVertices);

    pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = _float2(0.f, 0.f);

    pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = _float2(1.f, 0.f);

    pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = _float2(1.f, 1.f);

    pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = _float2(0.f, 1.f);

    D3D11_SUBRESOURCE_DATA          VertexInitialData{};
    VertexInitialData.pSysMem = pVertices.get();

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
        return E_FAIL;

#pragma endregion

#pragma region INSTANCE_BUFFER
    D3D11_BUFFER_DESC           InstanceBufferDesc{};
    InstanceBufferDesc.ByteWidth = m_iNumInstances * m_iInstanceVertexStride;
    InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
    InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    InstanceBufferDesc.MiscFlags = 0;

    // m_pInstanceData = make_shared<VTXINSTANCE_PARTICLE[]>(m_iNumInstances);
    m_pInstanceData = shared_ptr<VTXINSTANCE_PARTICLE[]>(new VTXINSTANCE_PARTICLE[m_iNumInstances]);
    m_pSpeeds = shared_ptr<_float[]>(new _float[m_iNumInstances]);
    m_isLoop = pDesc->isLoop;

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

    D3D11_SUBRESOURCE_DATA          InstanceInitialData{};
    InstanceInitialData.pSysMem = m_pInstanceData.get();

    if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
        return E_FAIL;

#pragma endregion

    return S_OK;
}

HRESULT VIBuffer_Particle_Rect::Initialize(void* pArg)
{
    m_vVelocity.resize(m_iNumInstances);
    m_vActive.resize(m_iNumInstances, false);

    for (uint32_t i = 0; i < m_iNumInstances; ++i)
    {
        m_pInstanceData[i].vLifeTime = _float2(1.f, 1.f); // 이미 죽은 상태
    }

    return S_OK;
}

void VIBuffer_Particle_Rect::Drop(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE            MappedSubResource{};

    if (FAILED(m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource)))
        return;

    auto        pVertices = static_cast<VTXINSTANCE_PARTICLE*>(MappedSubResource.pData);

    for (uint32_t i = 0; i < m_iNumInstances; i++)
    {
        pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
        pVertices[i].vLifeTime.y += fTimeDelta;

        if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
        {
            pVertices[i].vTranslation.y = m_pInstanceData[i].vTranslation.y;
            pVertices[i].vLifeTime.y = 0.f;
        }
    }

    m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void VIBuffer_Particle_Rect::Add_Spray(const _float3& vSpawnPos, uint32_t iCount)
{
    uint32_t iSpawned = 0;

    for (uint32_t i = 0; i < m_iNumInstances; ++i)
    {
        if (iSpawned >= iCount)
            break;

        if (m_vActive[i])
            continue;

        m_vActive[i] = true;

        _float fScale = CGameInstance::Get().Random(0.5f,1.f);

        m_pInstanceData[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
        m_pInstanceData[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
        m_pInstanceData[i].vLook = _float4(0.f, 0.f, fScale, 0.f);

        m_pInstanceData[i].vTranslation = _float4(vSpawnPos.x,vSpawnPos.y,vSpawnPos.z,1.f);

        m_pInstanceData[i].vLifeTime = _float2(CGameInstance::Get().Random(0.4f, 0.8f),0.f);

        _float randX = CGameInstance::Get().Random(-1.f, 1.f);
        _float randY = CGameInstance::Get().Random(0.5f, 2.0f);
        _float randZ = CGameInstance::Get().Random(-1.f, 1.f);

        _vector vDir = XMVectorSet(randX, randY, randZ, 0.f);
        vDir = XMVector3Normalize(vDir);

        _float fSpeed = CGameInstance::Get().Random(2.f, 6.f);

        XMStoreFloat3(&m_vVelocity[i], vDir * fSpeed);

        ++iSpawned;
    }
}

void VIBuffer_Particle_Rect::Update_Spray(_float fTimeDelta)
{
    const _float fGravity = 4.f;

    for (uint32_t i = 0; i < m_iNumInstances; ++i)
    {
        if (!m_vActive[i])
            continue;

        m_pInstanceData[i].vLifeTime.y += fTimeDelta;

        if (m_pInstanceData[i].vLifeTime.y >= m_pInstanceData[i].vLifeTime.x)
        {
            m_pInstanceData[i].vLifeTime.y = m_pInstanceData[i].vLifeTime.x;
            m_vActive[i] = false;

            // 수명 끝난 파티클은 안 보이게 처리
            m_pInstanceData[i].vRight = _float4(0.f, 0.f, 0.f, 0.f);
            m_pInstanceData[i].vUp = _float4(0.f, 0.f, 0.f, 0.f);
            m_pInstanceData[i].vLook = _float4(0.f, 0.f, 0.f, 0.f);

            continue;
        }

        m_vVelocity[i].y -= fGravity * fTimeDelta;

        m_pInstanceData[i].vTranslation.x += m_vVelocity[i].x * fTimeDelta;
        m_pInstanceData[i].vTranslation.y += m_vVelocity[i].y * fTimeDelta;
        m_pInstanceData[i].vTranslation.z += m_vVelocity[i].z * fTimeDelta;
    }

    D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

    if (FAILED(m_pContext->Map( m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource)))
    {
        return;
    }

    memcpy( MappedSubResource.pData,  m_pInstanceData.get(),sizeof(VTXINSTANCE_PARTICLE) * m_iNumInstances);

    m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

unique_ptr<VIBuffer_Particle_Rect> VIBuffer_Particle_Rect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    auto		pInstance = unique_ptr<VIBuffer_Particle_Rect>(new VIBuffer_Particle_Rect(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Created : VIBuffer_Particle_Rect");
        return nullptr;
    }

    return pInstance;
}



shared_ptr<Prototype> VIBuffer_Particle_Rect::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<VIBuffer_Particle_Rect>(new VIBuffer_Particle_Rect(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : VIBuffer_Particle_Rect");
        return nullptr;
    }

    return pInstance;
}

