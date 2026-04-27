#include "AABB_Collider.h"

AABB_Collider::AABB_Collider(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext) : BaseCollider{pDevice, pContext}
{
	m_eColliderID = ColliderType::AABB;
	m_eColor = ColliderColor::GREEN;
}

AABB_Collider::~AABB_Collider()
{
}

HRESULT AABB_Collider::Intersect(BaseCollider* pCollider)
{
	return true;
}

HRESULT AABB_Collider::Intersect(_vector vPos, _vector vDir, float& pOutDist)
{
	return m_boudingBox.Intersects(vPos, vDir,OUT pOutDist);
}



HRESULT AABB_Collider::Initialize_Prototype()
{
    m_boudingBox.Extents = { 0.5f,0.5f,0.5f };
    m_Extend = { 0.5f,0.5f,0.5f };
    m_Center = { 0.f,0.f,0.f };
    return S_OK;

}

HRESULT AABB_Collider::Initialize(void* pArg) {

	__super::Initialize(pArg);


	return S_OK;

}
void AABB_Collider::Update(float Timedelta) {

    switch (m_eColor) {
    case  ColliderColor::GREEN:
    {
        XMStoreFloat4(&m_color, { 0.f, 255.f, 0.f, 0.f });
    }
    break;
    case  ColliderColor::RED:
    {
        XMStoreFloat4(&m_color, { 255.f, 0.f, 0.f, 0.f });
  
    }
    break;
    case  ColliderColor::BLACK:
    {
        XMStoreFloat4(&m_color, { 0.f, 0.f, 0.f, 0.f });
     
    }
    break;
    }

    XMFLOAT4X4 worldFloat = m_Owner->GetTransform()->GetWorldMatrix();
    XMMATRIX world = XMLoadFloat4x4(&worldFloat);

    XMVECTOR localCenter = XMLoadFloat3(&m_Center);

    XMVECTOR worldCenter = XMVector3TransformCoord(localCenter, world);

    XMStoreFloat3(&m_boudingBox.Center, worldCenter);
    m_boudingBox.Extents = m_Extend;

}
HRESULT AABB_Collider::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch) {
	__super::Render(m_batch);


    _vector world = XMVectorSet(0.f,0.f,0.f,1.f);

    if (nullptr != m_Owner) {
        world = m_Owner->GetTransform()->Get_State(STATE::POSITION);
  
    }
    
    XMFLOAT3 corners[8];
    m_boudingBox.GetCorners(corners);


    VertexPositionColor v[8];
    for (int i = 0; i < 8; ++i)
    {
        v[i].position = corners[i];
        v[i].color = m_color;
    }


    // Front
    m_batch->DrawLine(v[0], v[1]);
    m_batch->DrawLine(v[1], v[2]);
    m_batch->DrawLine(v[2], v[3]);
    m_batch->DrawLine(v[3], v[0]);

    // Back
    m_batch->DrawLine(v[4], v[5]);
    m_batch->DrawLine(v[5], v[6]);
    m_batch->DrawLine(v[6], v[7]);
    m_batch->DrawLine(v[7], v[4]);

    // Connect
    m_batch->DrawLine(v[0], v[4]);
    m_batch->DrawLine(v[1], v[5]);
    m_batch->DrawLine(v[2], v[6]);
    m_batch->DrawLine(v[3], v[7]);


    GUI_ColliderExtend();
    return S_OK;
}

unique_ptr<AABB_Collider> AABB_Collider::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) {
	auto		pInstance = unique_ptr<AABB_Collider>(new AABB_Collider(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : AABB_Collider");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> AABB_Collider::Clone(void* pArg) {
	auto		pInstance = shared_ptr<AABB_Collider>(new AABB_Collider(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : AABB_Collider");
		return nullptr;
	}

	return pInstance;
}