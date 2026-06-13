#include "Engine_Macro.h"
#include "Monster.h"
#include "GameInstance.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"
Monster::Monster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: ContainerObject{ pDevice, pContext }

{
}

Monster::Monster(const Monster& Prototype)
	: ContainerObject{ Prototype }
{
}

Monster::~Monster()
{
	//a.clear();
}


HRESULT Monster::Initialize_Prototype()
{

	return S_OK;
}

HRESULT Monster::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void Monster::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(Monster));
	for (int type = 0; type < (int)COLLIDER::COLLIDER_END; ++type)
	{
		auto& colliderList = m_pColliderComs[type];

		for (size_t i = 0; i < colliderList.size(); ++i)
		{

			CGameInstance::Get().Add_Collider(colliderList[i]->Get_GroupTag(), colliderList[i].get());

		}
	}

}

void Monster::Update(_float fTimeDelta)
{

	__super::Update(fTimeDelta);
}

void Monster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT Monster::Render()
{
	


	return S_OK;
}

HRESULT Monster::Ready_Components()
{
	__super::Clear_Compnent();

	m_ModelComponentName = TEXT("Prototype_Com_Model_") + Object_INFO.m_strPrototypeBaseName;


	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), m_ModelComponentName));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;
	

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_AnimFbx")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;
	
	auto m_pCollider = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_OBB_Collider")));
	m_pCollider->SetOwner(SHARED_THIS(Monster).get());
	if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), m_pCollider)))
		return E_FAIL;
	m_pCollider->Set_GroupTag(L"Monster");
	m_pColliderComs[(int)COLLIDER::COLLIDER_OBB].push_back(m_pCollider);
	

	

	return S_OK;
}

HRESULT Monster::Set_Model(wstring m_strPrototypeBaseName) {
	//Object_INFO.m_strPrototypeBaseName = m_strPrototypeBaseName;
	//m_ModelComponentName = TEXT("Prototype_Com_Model_") + Object_INFO.m_strPrototypeBaseName;


	//m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::MAPEDITOR), m_ModelComponentName));
	//if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
	//	return E_FAIL;

	return S_OK;
}




shared_ptr<Prototype> Monster::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Monster(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Monster");
		return nullptr;
	}

	return pInstance;
}
