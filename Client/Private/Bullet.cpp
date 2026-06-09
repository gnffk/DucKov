#include "Engine_Macro.h"
#include "Bullet.h"
#include "GameInstance.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"
#include "Sphere_Collider.h"
Bullet::Bullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Bullet::Bullet(const Bullet& Prototype)
	: GameObject{ Prototype }
{
}

Bullet::~Bullet()
{
	//a.clear();
}
0.1f

HRESULT Bullet::Initialize_Prototype()
{

	return S_OK;
}

HRESULT Bullet::Initialize(void* pArg)
{
	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pDesc != nullptr)
	{
		m_vDir = pDesc->vDir;
		m_fSpeed = pDesc->fSpeed;

		_matrix matWorld =
			XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			XMMatrixTranslation(
				pDesc->vStartPos.x,
				pDesc->vStartPos.y,
				pDesc->vStartPos.z
			);

		_float4x4 World{};
		XMStoreFloat4x4(&World, matWorld);

		m_pTransformCom->Set_WorldMatrix(World);
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void Bullet::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Bullet));


	for (int type = 0; type < (int)COLLIDER::COLLIDER_END; ++type)
	{
		auto& colliderList = m_pColliderComs[type];

		for (size_t i = 0; i < colliderList.size(); ++i)
		{

			CGameInstance::Get().Add_Collider(colliderList[i]->Get_GroupTag(), colliderList[i].get());

		}
	}



}

void Bullet::Update(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vDir = XMLoadFloat3(&m_vDir);

	vDir = XMVector3Normalize(vDir);

	vPos += vDir * m_fSpeed * fTimeDelta;

	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void Bullet::Late_Update(_float fTimeDelta)
{

}

HRESULT Bullet::Render()
{


	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

	_float4x4 World = m_pTransformCom->GetWorldMatrix();




	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;



	uint32_t	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (uint32_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", (uint32_t)i, (uint32_t)ETOUI(TEXTURETYPE::DIFFUSE), 0)))
			return E_FAIL;


		//if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_Normals, 0)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT Bullet::Ready_Components()
{
	__super::Clear_Compnent();


	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_Bullet")));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;


	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_FBX_Tex")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	auto pOBBCom =dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_OBB_Collider")));

	pOBBCom->Set_Tag(COLLIDER::COLLIDER_OBB);
	pOBBCom->Set_GroupTag(L"Bullet");

	if (FAILED(__super::Add_Component(TEXT("Com_Collider"), m_pShaderCom)))
		return E_FAIL;

	pOBBCom->SetOwner(SHARED_THIS(Bullet).get());

	m_pColliderComs[(int)COLLIDER::COLLIDER_OBB].push_back(pOBBCom);

	return S_OK;
}


unique_ptr<Bullet> Bullet::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Bullet>(new Bullet(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Bullet");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Bullet::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Bullet(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Bullet");
		return nullptr;
	}

	return pInstance;
}
