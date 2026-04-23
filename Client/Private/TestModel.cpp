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

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);

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
	


		_float4x4 View, Proj;
		CGameInstance::Get().Get_MainCameraMatrix(View,Proj);

	_float4x4 World = m_pTransformCom->GetWorldMatrix();




	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT TestModel::Ready_Components()
{


	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Model_Duck")));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Shader_Vtx_FBX_Tex")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), TEXT("Prototype_Component_Texture_Logo")));
	if (FAILED(__super::Add_Component(TEXT("Com_Texture"), m_pTextureCom)))
		return E_FAIL;




	

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
