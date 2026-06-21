#include "Boss_Weapon.h"
#include "OBB_Collider.h"
#include "GameInstance.h"
#include "BaseCollider.h"
#include "Bullet.h"
Boss_Weapon::Boss_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: PartObject{ pDevice, pContext }

{
}

Boss_Weapon::Boss_Weapon(const Boss_Weapon& Prototype)
	: PartObject{ Prototype }
{
}

Boss_Weapon::~Boss_Weapon()
{

}


HRESULT Boss_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Boss_Weapon::Initialize(void* pArg)
{
	auto		pDesc = static_cast<Boss_Weapon::WEAPON_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_vLocalPos = { -0.06f, 0.15f, 0.27f };
	m_vLocalRot = { 360.f, -148.5f, -14.5f };
	m_vLocalScale = { 1.f, 1.f, 1.f };
	m_vMuzzleLocalPos = { 0.3f, 0.f, -0.3f };
	return S_OK;
}

void Boss_Weapon::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Boss_Weapon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	_vector vSocketPos = SocketMatrix.r[3];

	_matrix SocketPosMatrix = XMMatrixIdentity();
	SocketPosMatrix.r[3] = vSocketPos;

	_matrix LocalWeaponMatrix =
		XMMatrixScaling(m_vLocalScale.x, m_vLocalScale.y, m_vLocalScale.z) *
		XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(m_vLocalRot.x),
			XMConvertToRadians(m_vLocalRot.y),
			XMConvertToRadians(m_vLocalRot.z)
		) *
		XMMatrixTranslation(
			m_vLocalPos.x,
			m_vLocalPos.y,
			m_vLocalPos.z
		);

	_matrix ChildMatrix = LocalWeaponMatrix * SocketPosMatrix;

	Make_CombinedWorldMatrix(ChildMatrix);

	_float4x4 WeaponWorldFloat4x4 = __super::GetCombined();
	_matrix WeaponWorld = XMLoadFloat4x4(&WeaponWorldFloat4x4);

	_vector vMuzzleLocal = XMVectorSet(m_vMuzzleLocalPos.x, m_vMuzzleLocalPos.y, m_vMuzzleLocalPos.z, 1.f);

	_vector vMuzzleWorld = XMVector3TransformCoord(vMuzzleLocal, WeaponWorld);

	XMStoreFloat3(&m_vMuzzleWorldPos, vMuzzleWorld);

	if (m_fFireTimer > 0.f)
		m_fFireTimer -= fTimeDelta;

}

void Boss_Weapon::Late_Update(_float fTimeDelta)
{


	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Boss_Weapon));

	__super::Late_Update(fTimeDelta);
}

HRESULT Boss_Weapon::Render()
{
	if (!m_bVisible)
		return S_OK;


	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;



	uint32_t	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (uint32_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", (uint32_t)i, (uint32_t)ETOUI(TEXTURETYPE::DIFFUSE), 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		m_pModelCom->Render(i);
	}



	return S_OK;
}

HRESULT Boss_Weapon::Ready_Components()
{
	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_Gun2")));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_FBX_Tex")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

unique_ptr<Boss_Weapon> Boss_Weapon::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Boss_Weapon>(new Boss_Weapon(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Boss_Weapon");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Boss_Weapon::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Boss_Weapon(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Boss_Weapon");
		return nullptr;
	}

	return pInstance;
}
void Boss_Weapon::Fire_Bullet()
{

	if (m_fFireTimer > 0.f)
		return;

	m_fFireTimer = m_fFireCoolTime;

	_matrix ParentMatrix = XMLoadFloat4x4(m_pParentMatrix);

	_vector vDir = ParentMatrix.r[2];

	vDir = XMVectorSetY(vDir, 0.f);
	vDir = XMVector3Normalize(vDir);

	XMStoreFloat3(&m_vBulletDir, vDir);


	Bullet::BULLET_DESC Desc{};
	Desc.vStartPos = m_vMuzzleWorldPos;
	Desc.vDir = m_vBulletDir;
	Desc.fSpeed = 60.f;
	Desc.m_bCollider = false;
	Desc.ContainerObject = true;
	Desc.m_strName = TEXT("Bullet");
	Desc.m_strPrototypeObjectName = TEXT("Prototype_GameObject_Bullet");
	Desc.m_strPrototypeBaseName = TEXT("Prototype_GameObject_Bullet");
	Desc.pCameraType = 0;
	Desc.fSpeedPerSec = 20.f;
	Desc.fRotationPerSec = 20.f;

	CGameInstance::Get().Add_GameObject_toLayer(
		CGameInstance::Get().Get_Level(),
		TEXT("Prototype_GameObject_Bullet"),
		CGameInstance::Get().Get_Level(),
		TEXT("Layer_Bullet"),
		&Desc
	);
}
