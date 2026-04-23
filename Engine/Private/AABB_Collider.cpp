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
    
    return S_OK;

}

HRESULT AABB_Collider::Initialize(void* pArg) {

	__super::Initialize(pArg);


	return S_OK;

}
void AABB_Collider::Update(float Timedelta) {

}
HRESULT AABB_Collider::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch) {
	__super::Render(m_batch);

    XMFLOAT3 min = { -0.5f, -0.5f, -0.5f };
    XMFLOAT3 max = { 0.5f, 0.5f, 0.5f };
    XMVECTOR color = XMVectorSet(0.f, 0.f, 0.f, 0.f);

    switch (m_eColor) {
    case  ColliderColor::GREEN:
    {
        color = XMVectorSet(0.f, 255.f, 0.f, 0.f);
    }
    break;
    case  ColliderColor::RED:
    {
        color = XMVectorSet(255.f, 0.f, 0.f, 0.f);
    }
    break;
    case  ColliderColor::BLACK:
    {
        color = XMVectorSet(0.f, 0.f, 0.f, 0.f);
    }
    break;
    }

    _vector world = XMVectorSet(0.f,0.f,0.f,1.f);

    if (nullptr != m_Owner) {
        world = m_Owner->GetTransform()->Get_State(STATE::POSITION);
  
    }
    
    VertexPositionColor v[8] =
    {
        {{min.x, min.y, min.z}, color},
        {{max.x, min.y, min.z}, color},
        {{max.x, max.y, min.z}, color},
        {{min.x, max.y, min.z}, color},

        {{min.x, min.y, max.z}, color},
        {{max.x, min.y, max.z},color},
        {{max.x, max.y, max.z},color},
        {{min.x, max.y, max.z}, color},
    };

    for (int i = 0; i < 8; ++i)
    {
        XMVECTOR pos = XMLoadFloat3(&v[i].position);

        pos = XMVectorAdd(pos, world);

        XMStoreFloat3(&v[i].position, pos);
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