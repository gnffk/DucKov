#include "Collider_Manager.h"
#include "BaseCollider.h"
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
    if(FAILED(Find_Collider(GroupTag, pCollider)))
        return E_FAIL;

	return S_OK;
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

    m_Colliders[GroupTag].push_back(pCollider);
    return S_OK;
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

void Collider_Manager::MousePicking(XMVECTOR rayOrigin, XMVECTOR rayDir, uint32_t LevelIndex) {
    for (auto& Layer : CGameInstance::Get().Find_Layer_Lists(LevelIndex)) {
        auto& gameObjects = Layer.second->Get_GameObjects();
        float minDist = FLT_MAX;
        for (auto gameObject : gameObjects) {
            auto& Components = gameObject->GetComponents();
            auto iter = Components.find(L"Com_Model");

            if (iter != Components.end() && iter->second != nullptr)
            {
                auto model = dynamic_pointer_cast<Model>(iter->second);
                for (auto& Mesh : model->GetMeshes()) {
                    auto& Meshnondata = Mesh->GetNonAnimMesh();
                    auto& Meshanimdata = Mesh->GetAnimMesh();
                    auto& indices = Mesh->GetIndices();
                    if (Meshnondata != nullptr) {
                        for (size_t i = 0; i < indices->size(); i += 3)
                        {
                            XMVECTOR v0 =
                                XMLoadFloat3(
                                    &(*Meshnondata)[(*indices)[i]].vPosition);

                            XMVECTOR v1 =
                                XMLoadFloat3(
                                    &(*Meshnondata)[(*indices)[i + 1]].vPosition);

                            XMVECTOR v2 =
                                XMLoadFloat3(
                                    &(*Meshnondata)[(*indices)[i + 2]].vPosition);

                            _float4x4 matWord;

                            CGameInstance::Get().GetWorldMatrix(matWord);
                            v0 = XMVector3TransformCoord(v0, XMLoadFloat4x4(&matWord));
                            v1 = XMVector3TransformCoord(v1, XMLoadFloat4x4(&matWord));
                            v2 = XMVector3TransformCoord(v2, XMLoadFloat4x4(&matWord));

                            float dist = 1000.f;

                            if (TriangleTests::Intersects(
                                rayOrigin,
                                rayDir,
                                v0,
                                v1,
                                v2,
                                dist))
                            {
                                if (dist < minDist)
                                {
                                    minDist = dist;

                                    CGameInstance::Get().SetSeletObject(gameObject.get());
                                }
                            }
                        }
                    }
                    else {
                        for (size_t i = 0; i < indices->size(); i += 3)
                        {
                            XMVECTOR v0 =XMLoadFloat3(&(*Meshanimdata)[(*indices)[i]].vPosition);

                            XMVECTOR v1 =XMLoadFloat3(&(*Meshanimdata)[(*indices)[i + 1]].vPosition);

                            XMVECTOR v2 =XMLoadFloat3(&(*Meshanimdata)[(*indices)[i + 2]].vPosition);

                            _float4x4 matWord;
                            matWord = gameObject->GetTransform()->GetWorldMatrix();
                      
                            v0 = XMVector3TransformCoord(v0, XMLoadFloat4x4(&matWord));
                            v1 = XMVector3TransformCoord(v1, XMLoadFloat4x4(&matWord));
                            v2 = XMVector3TransformCoord(v2, XMLoadFloat4x4(&matWord));

                            float dist{100};

                            if (TriangleTests::Intersects(
                                rayOrigin,
                                rayDir,
                                v0,
                                v1,
                                v2,
                                dist))
                            {
                                if (dist < minDist)
                                {
                                    minDist = dist;

                                    CGameInstance::Get().SetSeletObject(gameObject.get());
                                    break;
                                }
                            }
                          
                        }
                    }


                }
            }

        }
    }
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