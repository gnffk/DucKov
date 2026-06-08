#include "Engine_Macro.h"
#include "Tree.h"
#include "GameInstance.h"

Tree::Tree(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Tree::Tree(const Tree& Prototype)
	: GameObject{ Prototype }
{
}

Tree::~Tree()
{
	//a.clear();
}


HRESULT Tree::Initialize_Prototype()
{

	return S_OK;
}

HRESULT Tree::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 테스트용: 원점에 나무 하나 찍기
	Add_Tree(
		_float3{ 0.f, 0.f, 0.f },
		_float3{ 1.f, 1.f, 1.f },
		0.f);

	return S_OK;

}

void Tree::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Tree));



}

void Tree::Update(_float fTimeDelta)
{



}

void Tree::Late_Update(_float fTimeDelta)
{

}

HRESULT Tree::Render()
{
	if (m_pModelCom == nullptr || m_pShaderCom == nullptr)
		return E_FAIL;

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;

	uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (uint32_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom,"g_DiffuseTexture",i,ETOUI(TEXTURETYPE::DIFFUSE),0)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}
HRESULT Tree::Ready_Components()
{
	__super::Clear_Compnent();

	m_pModelCom =dynamic_pointer_cast<InstanceModel>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Model_Tree")));

	if (m_pModelCom == nullptr)
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;


	m_pShaderCom =dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_Vtx_Instance_Tex")));

	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


unique_ptr<Tree> Tree::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Tree>(new Tree(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Tree");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Tree::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Tree(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Tree");
		return nullptr;
	}

	return pInstance;
}

HRESULT Tree::Add_Tree(const _float3& vPosition)
{
	return Add_Tree(vPosition,_float3{ 1.f, 1.f, 1.f },0.f);
}

HRESULT Tree::Add_Tree(const _float3& vPosition,const _float3& vScale,_float fYaw)
{
	if (m_pModelCom == nullptr)
		return E_FAIL;

	return m_pModelCom->Add_Instance(vPosition,vScale,fYaw);
}

HRESULT Tree::Erase_Tree(const _float3& vCenter,_float fRadius)
{
	if (m_pModelCom == nullptr)
		return E_FAIL;

	return m_pModelCom->Remove_Instance_InRadius(vCenter,fRadius);
}

HRESULT Tree::Clear_Trees()
{
	if (m_pModelCom == nullptr)
		return E_FAIL;

	return m_pModelCom->Clear_Instances();
}

uint32_t Tree::Get_TreeCount() const
{
	if (m_pModelCom == nullptr)
		return 0;

	return m_pModelCom->Get_InstanceCount();
}