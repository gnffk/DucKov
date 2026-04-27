#include "OBB_Collider.h"

OBB_Collider::OBB_Collider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext) : BaseCollider{ pDevice, pContext }
{
    m_eColliderID = ColliderType::OBB;
    m_eColor = ColliderColor::GREEN;
}

OBB_Collider::~OBB_Collider()
{
}

HRESULT OBB_Collider::Intersect(BaseCollider* pCollider)
{
    return true;
}

HRESULT OBB_Collider::Intersect(_vector vPos, _vector vDir, float& pOutDist)
{
    return m_boudingBox.Intersects(vPos, vDir, OUT pOutDist);
}



HRESULT OBB_Collider::Initialize_Prototype()
{

    m_boudingBox.Extents = {0.5f,0.5f,0.5f };

    return S_OK;

}

HRESULT OBB_Collider::Initialize(void* pArg) {

    __super::Initialize(pArg);


    return S_OK;

}
void OBB_Collider::Update(float Timedelta) {
    XMFLOAT4X4 worldFloat = m_Owner->GetTransform()->GetWorldMatrix(); 
    XMMATRIX world = XMLoadFloat4x4(&worldFloat); 
    
    XMVECTOR scale, rotQuat, trans; 
    
    XMMatrixDecompose(&scale, &rotQuat, &trans, world); 
    XMStoreFloat3(&m_boudingBox.Center, trans); 
    XMStoreFloat4(&m_boudingBox.Orientation, rotQuat);



}
HRESULT OBB_Collider::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch) {
    __super::Render(m_batch);

    XMVECTOR color = XMVectorSet(0.f, 0.f, 0.f, 0.f);

    switch (m_eColor)
    {
    case ColliderColor::GREEN:
        color = XMVectorSet(0.f, 1.f, 0.f, 1.f);
        break;
    case ColliderColor::RED:
        color = XMVectorSet(1.f, 0.f, 0.f, 1.f);
        break;
    case ColliderColor::BLACK:
        color = XMVectorSet(0.f, 0.f, 0.f, 1.f);
        break;
    }


    XMFLOAT3 corners[8];
    m_boudingBox.GetCorners(corners);


    VertexPositionColor v[8];
    for (int i = 0; i < 8; ++i)
    {
        v[i].position = corners[i];

    }


    // Bottom
    m_batch->DrawLine(v[0], v[1]);
    m_batch->DrawLine(v[1], v[2]);
    m_batch->DrawLine(v[2], v[3]);
    m_batch->DrawLine(v[3], v[0]);

    // Top
    m_batch->DrawLine(v[4], v[5]);
    m_batch->DrawLine(v[5], v[6]);
    m_batch->DrawLine(v[6], v[7]);
    m_batch->DrawLine(v[7], v[4]);

    // Side
    m_batch->DrawLine(v[0], v[4]);
    m_batch->DrawLine(v[1], v[5]);
    m_batch->DrawLine(v[2], v[6]);
    m_batch->DrawLine(v[3], v[7]);

    return S_OK;
}

unique_ptr<OBB_Collider> OBB_Collider::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) {
    auto		pInstance = unique_ptr<OBB_Collider>(new OBB_Collider(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : OBB_Collider");
        return nullptr;
    }

    return pInstance;
}


shared_ptr<Prototype> OBB_Collider::Clone(void* pArg) {
    auto		pInstance = shared_ptr<OBB_Collider>(new OBB_Collider(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : OBB_Collider");
        return nullptr;
    }

    return pInstance;
}