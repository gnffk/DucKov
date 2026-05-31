#include "Sphere_Collider.h"

Sphere_Collider::Sphere_Collider(
    ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext)
    : BaseCollider{ pDevice, pContext }
{
    m_sTag = COLLIDER::COLLIDER_SPHERE;
    m_eColliderID = ColliderType::Sphere;
    m_eColor = ColliderColor::GREEN;
}

Sphere_Collider::~Sphere_Collider()
{
}

HRESULT Sphere_Collider::Intersect(_vector vPos, _vector vDir, float& pOutDist)
{
    return m_boudingSphere.Intersects(vPos, vDir, OUT pOutDist);
}

HRESULT Sphere_Collider::Initialize_Prototype()
{
    m_boudingSphere.Center = { 0.f, 0.f, 0.f };
    m_boudingSphere.Radius = 0.5f;

    m_Extend = { 0.5f, 0.5f, 0.5f };
    m_Center = { 0.f, 0.f, 0.f };

    return S_OK;
}

HRESULT Sphere_Collider::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void Sphere_Collider::Update(float fTimeDelta)
{
    if (m_Owner == nullptr)
        return;

    XMFLOAT4X4 worldFloat = m_Owner->GetTransform()->GetWorldMatrix();
    XMMATRIX world = XMLoadFloat4x4(&worldFloat);

    XMVECTOR scale, rotQuat, trans;
    XMMatrixDecompose(&scale, &rotQuat, &trans, world);

    XMVECTOR localCenter = XMLoadFloat3(&m_Center);
    XMVECTOR worldCenter = XMVector3TransformCoord(localCenter, world);

    XMStoreFloat3(&m_boudingSphere.Center, worldCenter);

    float sx = XMVectorGetX(scale);
    float sy = XMVectorGetY(scale);
    float sz = XMVectorGetZ(scale);

    float maxScale = max(sx, max(sy, sz));

    m_boudingSphere.Radius = m_Extend.x * maxScale;
}

HRESULT Sphere_Collider::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> m_batch)
{
    __super::Render(m_batch);

    switch (m_eColor)
    {
    case ColliderColor::GREEN:
        XMStoreFloat4(&m_color, { 0.f, 255.f, 0.f, 1.f });
        break;

    case ColliderColor::RED:
        XMStoreFloat4(&m_color, { 255.f, 0.f, 0.f, 1.f });
        break;

    case ColliderColor::BLACK:
        XMStoreFloat4(&m_color, { 0.f, 0.f, 0.f, 1.f });
        break;
    }

    constexpr int iSegmentCount = 32;

    XMFLOAT3 center = m_boudingSphere.Center;
    float radius = m_boudingSphere.Radius;

    auto DrawCircle = [&](int axis)
        {
            for (int i = 0; i < iSegmentCount; ++i)
            {
                float angle0 = XM_2PI * i / iSegmentCount;
                float angle1 = XM_2PI * (i + 1) / iSegmentCount;

                XMFLOAT3 p0 = center;
                XMFLOAT3 p1 = center;

                switch (axis)
                {
                case 0: // XY
                    p0.x += cosf(angle0) * radius;
                    p0.y += sinf(angle0) * radius;
                    p1.x += cosf(angle1) * radius;
                    p1.y += sinf(angle1) * radius;
                    break;

                case 1: // XZ
                    p0.x += cosf(angle0) * radius;
                    p0.z += sinf(angle0) * radius;
                    p1.x += cosf(angle1) * radius;
                    p1.z += sinf(angle1) * radius;
                    break;

                case 2: // YZ
                    p0.y += cosf(angle0) * radius;
                    p0.z += sinf(angle0) * radius;
                    p1.y += cosf(angle1) * radius;
                    p1.z += sinf(angle1) * radius;
                    break;
                }

                VertexPositionColor v0{ p0, m_color };
                VertexPositionColor v1{ p1, m_color };

                m_batch->DrawLine(v0, v1);
            }
        };

    DrawCircle(0);
    DrawCircle(1);
    DrawCircle(2);

    return S_OK;
}

unique_ptr<Sphere_Collider> Sphere_Collider::Create(
    ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = unique_ptr<Sphere_Collider>(
        new Sphere_Collider(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Sphere_Collider");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Sphere_Collider::Clone(void* pArg)
{
    auto pInstance = shared_ptr<Sphere_Collider>(
        new Sphere_Collider(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Sphere_Collider");
        return nullptr;
    }

    return pInstance;
}