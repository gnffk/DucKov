#include "Collider_Manager.h"
#include "BaseCollider.h"


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
    
    // 1. Effect ¼¼ÆÃ
    m_effect->SetWorld(matWorld);
    m_effect->SetView(matView);
    m_effect->SetProjection(matProj);
    m_effect->Apply(m_pDeviceContext.Get());

    // 2. InputLayout
    m_pDeviceContext->IASetInputLayout(m_inputLayout.Get());

    // 3. Begin
    m_batch->Begin();


    for (auto& ColliderGroup : m_Colliders) {
        for (auto& Collider : ColliderGroup.second) {

            Collider->Render(m_batch);
       }
    }

    m_batch->End();
    m_Colliders.clear();
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