#include "Engine_Macro.h"
#include "TestModel.h"
#include "GameInstance.h"

TestModel::TestModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

TestModel::TestModel(const TestModel& Prototype)
	: GameObject{ Prototype }
{
}

TestModel::~TestModel()
{
	int a = 10;
}


HRESULT TestModel::Initialize_Prototype()
{

	return S_OK;
}

HRESULT TestModel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void TestModel::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(TestModel));
}

void TestModel::Update(_float fTimeDelta)
{

}

void TestModel::Late_Update(_float fTimeDelta)
{

}

HRESULT TestModel::Render()
{
	_float4x4		IdentityMatrix = {};
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &IdentityMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &IdentityMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &IdentityMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	
	for (auto m_pVIBufferCom : m_pVIBufferComs) {
		if (FAILED(m_pVIBufferCom->Bind_Resources()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT TestModel::Ready_Components()
{

	//auto VIBufferCom = dynamic_pointer_cast<Component>(m_pVIBufferCom);
	//if (FAILED(__super::Add_Component(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_VIBuffer_Fbx_TestModel"),
	//	TEXT("Com_VIBuffer"), VIBufferCom)))
	//	return E_FAIL;
	//m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Fbx>(VIBufferCom);

	//auto VIBufferCom = dynamic_pointer_cast<Component>(m_pVIBufferCom);
	//if (FAILED(__super::Add_Component(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_VIBuffer_Rect"),
	//	TEXT("Com_VIBuffer"), VIBufferCom)))
	//	return E_FAIL;
	//m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Rect>(VIBufferCom);

	auto ModelCom = dynamic_pointer_cast<Component>(m_pModelCom);
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Model_Duck"),
		TEXT("Com_Model"), ModelCom)))
		return E_FAIL;
	m_pModelCom = dynamic_pointer_cast<Model>(ModelCom);

	auto ShaderCom = dynamic_pointer_cast<Component>(m_pShaderCom);
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Shader_Vtx_FBX_Tex"),
		TEXT("Com_Shader"), ShaderCom)))
		return E_FAIL;
	m_pShaderCom = dynamic_pointer_cast<Shader>(ShaderCom);
	



	for (int i = 0; i < m_pModelCom->GetMeshNames().size(); i++) {
		shared_ptr<Component> DummyVIBufferCom;

		auto pVIBufferCom = dynamic_pointer_cast<Component>(DummyVIBufferCom);

 		std::wstring FindTag= L"Prototype_Component_VIBuffer_Mesh_" + m_pModelCom->GetMeshNames()[i];
		std::wstring TagGameObject= L"Com_" + m_pModelCom->GetMeshNames()[i];
		if (FAILED(__super::Add_Component(ETOUI(LEVEL::MAPEDITOR), FindTag,
			TagGameObject, pVIBufferCom)))
			return E_FAIL;

		if (pVIBufferCom == nullptr) {
			return E_FAIL;
		}
		m_pVIBufferComs.emplace_back(dynamic_pointer_cast<VIBuffer_Mesh>(pVIBufferCom));
	}
	

	return S_OK;
}

unique_ptr<TestModel> TestModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<TestModel>(new TestModel(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TestModel");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> TestModel::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new TestModel(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TestModel");
		return nullptr;
	}

	return pInstance;
}
