#include "Collider_Manager.h"
#include "BaseCollider.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"

#include "GameInstance.h"
#include "Layer.h"
#include "Mesh.h"
Collider_Manager::~Collider_Manager()
{
}
HRESULT Collider_Manager::Initialize(ComPtr<ID3D11Device> p_Device, ComPtr<ID3D11DeviceContext> p_DeviceContext) {
    m_pDevice = p_Device;
    m_pDeviceContext = p_DeviceContext;
    // Batch
    m_batch = make_shared<PrimitiveBatch<VertexPositionColor>>(m_pDeviceContext.Get());

    // Effect
    m_effect = make_shared<BasicEffect>(m_pDevice.Get());
    m_effect->SetVertexColorEnabled(true);

    // InputLayout
    void const* shaderByteCode = nullptr;
    size_t byteCodeLength = 0;

    m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    m_pDevice->CreateInputLayout(
        VertexPositionColor::InputElements,
        VertexPositionColor::InputElementCount,
        shaderByteCode,
        byteCodeLength,
        m_inputLayout.GetAddressOf()
    );

	return S_OK;
}

HRESULT Collider_Manager::Add_Collider(wstring GroupTag, BaseCollider* pCollider)
{
    m_Colliders[GroupTag].emplace_back(pCollider);
    //if(FAILED(Find_Collider(GroupTag, pCollider)))
    //    return E_FAIL;

	return S_OK;
}

_bool Collider_Manager::Intersect(BaseCollider* pCollider, BaseCollider* sCollider)
{
    if (pCollider == nullptr || sCollider == nullptr)
        return false;


    // pCollider AABB
    if (pCollider->Get_Tag() == COLLIDER::COLLIDER_AABB) {
        auto pAABB = dynamic_cast<AABB_Collider*>(pCollider);
        switch (sCollider->Get_Tag())
        {
            case COLLIDER::COLLIDER_AABB:
            {
           
                auto sAABB =dynamic_cast<AABB_Collider*>(sCollider);

                if (pAABB == nullptr || sAABB == nullptr)
                    return false;

                if (pAABB->Get_BoudingBox().Intersects(
                    sAABB->Get_BoudingBox()))
                {
                    return true;
                }
            }
            break;

            case COLLIDER::COLLIDER_OBB:
            {
                auto sOBB =
                    dynamic_cast<OBB_Collider*>(sCollider);

                if (pAABB == nullptr || sOBB == nullptr)
                    return false;

                if (pAABB->Get_BoudingBox().Intersects(
                    sOBB->Get_BoudingBox()))
                {
                    return true;
                }
            }
            break;

            case COLLIDER::COLLIDER_SPHERE:
            {
           
            }
            break;
        }
    }
    

    // pCollider OBB
    if (pCollider->Get_Tag() == COLLIDER::COLLIDER_OBB) {
        auto pOBB = dynamic_cast<OBB_Collider*>(pCollider);
        switch (sCollider->Get_Tag())
        {
        case COLLIDER::COLLIDER_AABB:
        {

            auto sAABB = dynamic_cast<AABB_Collider*>(sCollider);

            if (pOBB == nullptr || sAABB == nullptr)
                return false;

            if (pOBB->Get_BoudingBox().Intersects(
                sAABB->Get_BoudingBox()))
            {
                return true;
            }
        }
        break;

        case COLLIDER::COLLIDER_OBB:
        {
            auto sOBB =
                dynamic_cast<OBB_Collider*>(sCollider);

            if (pOBB == nullptr || sOBB == nullptr)
                return false;

            if (pOBB->Get_BoudingBox().Intersects(sOBB->Get_BoudingBox()))
            {
                return true;
            }
        }
        break;

        case COLLIDER::COLLIDER_SPHERE:
        {
      
        }
        break;
        }
    }


    // pCollider SPhere
    if (pCollider->Get_Tag() == COLLIDER::COLLIDER_AABB) {
      
    }

    return false;
}
HRESULT Collider_Manager::Find_Collider(wstring GroupTag, BaseCollider* pCollider)
{
    if (pCollider == nullptr)
        return E_FAIL;

    auto& vec = m_Colliders[GroupTag];

    auto it = std::find_if(vec.begin(), vec.end(),
        [pCollider](BaseCollider* other)
        {
            if (other == nullptr)
                return false;

            return other->Get_Tag() == pCollider->Get_Tag();
        });

    if (it != vec.end())
        return E_FAIL; 


    return S_OK;
}

void Collider_Manager::Primitive_Update(float Timedelta)
{

    for (auto& ColliderGroup : m_Colliders) {
        for (auto& Collider : ColliderGroup.second) {
            Collider->SetColliderColor(ColliderColor::GREEN);
        }
    }
}

void Collider_Manager::Update(float Timedelta)
{
    for (auto& ColliderGroup : m_Colliders) {
        for (auto& Collider : ColliderGroup.second) {
         
            Collider->Update(Timedelta);
        }
    }
}

HRESULT Collider_Manager::Clear()
{
    m_Colliders.clear();

    return S_OK;
}

void Collider_Manager::Render() {

    m_pDeviceContext->RSSetState(nullptr);

    m_pDeviceContext->OMSetBlendState(
        nullptr,
        nullptr,
        0xFFFFFFFF);

    m_pDeviceContext->OMSetDepthStencilState(
        nullptr,
        0);

    m_pDeviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    _float4x4  View, Proj;
    CGameInstance::Get().Get_MainCameraMatrix(View, Proj);
   

    _matrix matView = XMLoadFloat4x4(&View);
    _matrix matProj = XMLoadFloat4x4(&Proj);
    XMMATRIX matWorld = XMMatrixIdentity();
    

    m_effect->SetWorld(matWorld);
    m_effect->SetView(matView);
    m_effect->SetProjection(matProj);
    m_effect->Apply(m_pDeviceContext.Get());


    m_pDeviceContext->IASetInputLayout(m_inputLayout.Get());


    m_batch->Begin();


    for (auto& ColliderGroup : m_Colliders) {
        for (auto& Collider : ColliderGroup.second) {

            Collider->Render(m_batch);
       }
    }

    m_batch->End();
    m_Colliders.clear();
}

void Collider_Manager::MousePicking(
    XMVECTOR rayOrigin,
    XMVECTOR rayDir,
    uint32_t LevelIndex)
{
    float globalMinDist = FLT_MAX;
    GameObject* pickedObject = nullptr;

    auto& layers =
        CGameInstance::Get().Find_Layer_Lists(LevelIndex);

    for (auto& layer : layers)
    {
        auto& gameObjects =
            layer.second->Get_GameObjects();

        for (auto& gameObject : gameObjects)
        {
            auto& components =
                gameObject->GetComponents();

            // =====================================================
            // Broad Phase
            // =====================================================

            bool broadHit = false;

            auto aabbIter = components.find(L"Com_AABBCollider");

            if (aabbIter != components.end() &&aabbIter->second != nullptr)
            {
                auto collider =
                    dynamic_pointer_cast<AABB_Collider>(aabbIter->second);

                float dist = 0.f;

                if (collider &&
                    collider->Intersect(rayOrigin, rayDir, dist))
                {
                    broadHit = true;
                }
            }

            auto obbIter =
                components.find(L"Com_OBBCollider");

            if (!broadHit &&
                obbIter != components.end() &&
                obbIter->second != nullptr)
            {
                auto collider =
                    dynamic_pointer_cast<OBB_Collider>(obbIter->second);

                float dist = 0.f;

                if (collider &&
                    collider->Intersect(rayOrigin, rayDir, dist))
                {
                    broadHit = true;
                }
            }

            auto sphereIter =
                components.find(L"Com_SphereCollider");

 /*           if (!broadHit &&
                sphereIter != components.end() &&
                sphereIter->second != nullptr)
            {
                auto collider =
                    dynamic_pointer_cast<SphereCollider>(sphereIter->second);

                if (collider &&
                    collider->Intersects(rayOrigin, rayDir))
                {
                    broadHit = true;
                }
            }*/

            if (!broadHit)
                continue;

            // =====================================================
            // Narrow Phase
            // =====================================================

            auto modelIter =
                components.find(L"Com_Model");

            if (modelIter == components.end() ||
                modelIter->second == nullptr)
                continue;

            auto model =
                dynamic_pointer_cast<Model>(modelIter->second);

            if (!model)
                continue;

            _float4x4 worldMatrix =
                gameObject->GetTransform()->GetWorldMatrix();

            XMMATRIX matWorld =
                XMLoadFloat4x4(&worldMatrix);

            for (auto& mesh : model->GetMeshes())
            {
                auto& indices = mesh->GetIndices();

                auto TestTriangles = [&](auto& vertices)
                    {
                        for (size_t i = 0; i < indices->size(); i += 3)
                        {
                            XMVECTOR v0 =
                                XMLoadFloat3(
                                    &vertices[(*indices)[i]].vPosition);

                            XMVECTOR v1 =
                                XMLoadFloat3(
                                    &vertices[(*indices)[i + 1]].vPosition);

                            XMVECTOR v2 =
                                XMLoadFloat3(
                                    &vertices[(*indices)[i + 2]].vPosition);

                            v0 = XMVector3TransformCoord(v0, matWorld);
                            v1 = XMVector3TransformCoord(v1, matWorld);
                            v2 = XMVector3TransformCoord(v2, matWorld);

                            float dist = 0.f;

                            if (TriangleTests::Intersects(
                                rayOrigin,
                                rayDir,
                                v0,
                                v1,
                                v2,
                                dist))
                            {
                                if (dist < globalMinDist)
                                {
                                    globalMinDist = dist;
                                    pickedObject = gameObject.get();
                                }
                            }
                        }
                    };

                auto& nonAnimMesh =
                    mesh->GetNonAnimMesh();

                if (nonAnimMesh)
                {
                    TestTriangles(*nonAnimMesh);
                }
                else
                {
                    auto& animMesh =
                        mesh->GetAnimMesh();

                    if (animMesh)
                    {
                        TestTriangles(*animMesh);
                    }
                }
            }
        }
    }

    CGameInstance::Get().SetSeletObject(pickedObject);
}
unique_ptr<Collider_Manager> Collider_Manager::Create(ComPtr<ID3D11Device> p_Device, ComPtr<ID3D11DeviceContext> p_DeviceContext) {
	auto		pInstance = unique_ptr<Collider_Manager>(new Collider_Manager());

	if (FAILED(pInstance->Initialize(p_Device, p_DeviceContext)))
	{
		MSG_BOX("Failed to Created : Collider_Manager");
		return nullptr;
	}

	return pInstance;
}