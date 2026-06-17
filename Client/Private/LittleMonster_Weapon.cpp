#include "LittleMonster_Weapon.h"
#include "OBB_Collider.h"
#include "GameInstance.h"
#include "BaseCollider.h"
LittleMonster_Weapon::LittleMonster_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: PartObject{ pDevice, pContext }

{
}

LittleMonster_Weapon::LittleMonster_Weapon(const LittleMonster_Weapon& Prototype)
	: PartObject{ Prototype }
{
}

LittleMonster_Weapon::~LittleMonster_Weapon()
{

}


HRESULT LittleMonster_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT LittleMonster_Weapon::Initialize(void* pArg)
{
	auto		pDesc = static_cast<LittleMonster_Weapon::WEAPON_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void LittleMonster_Weapon::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void LittleMonster_Weapon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
//#ifdef _DEBUG
//	GUI_WeaponDebug();
//#endif

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

void LittleMonster_Weapon::Late_Update(_float fTimeDelta)
{


	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(LittleMonster_Weapon));

	__super::Late_Update(fTimeDelta);
}

HRESULT LittleMonster_Weapon::Render()
{
	if (!m_bVisible)
		return E_FAIL;


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

HRESULT LittleMonster_Weapon::Ready_Components()
{
	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_LittleMonsterWeapon")));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_FBX_Tex")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

unique_ptr<LittleMonster_Weapon> LittleMonster_Weapon::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<LittleMonster_Weapon>(new LittleMonster_Weapon(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : LittleMonster_Weapon");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> LittleMonster_Weapon::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new LittleMonster_Weapon(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : LittleMonster_Weapon");
		return nullptr;
	}

	return pInstance;
}
void LittleMonster_Weapon::Attack()
{

	if (m_fFireTimer > 0.f)
		return;

	m_fFireTimer = m_fFireCoolTime;

	_matrix ParentMatrix = XMLoadFloat4x4(m_pParentMatrix);

	_vector vDir = ParentMatrix.r[2];

	vDir = XMVectorSetY(vDir, 0.f);
	vDir = XMVector3Normalize(vDir);
}

#ifdef _DEBUG
void LittleMonster_Weapon::GUI_WeaponDebug()
{
	if (false == m_bShowWeaponDebug)
		return;

	string strWindowName = "LittleMonster Weapon Debug##";
	strWindowName += to_string(reinterpret_cast<size_t>(this));

	if (ImGui::Begin(strWindowName.c_str(), &m_bShowWeaponDebug))
	{
		ImGui::Text("LittleMonster Weapon Local Transform");
		ImGui::Separator();

		ImGui::DragFloat3(
			"Local Position",
			reinterpret_cast<float*>(&m_vLocalPos),
			0.005f,
			-5.f,
			5.f
		);

		ImGui::DragFloat3(
			"Local Rotation Degree",
			reinterpret_cast<float*>(&m_vLocalRot),
			0.1f,
			-360.f,
			360.f
		);

		ImGui::DragFloat3(
			"Local Scale",
			reinterpret_cast<float*>(&m_vLocalScale),
			0.01f,
			0.01f,
			10.f
		);

		ImGui::Separator();
		ImGui::Text("Muzzle Setting");

		ImGui::DragFloat3(
			"Muzzle Local Position",
			reinterpret_cast<float*>(&m_vMuzzleLocalPos),
			0.01f,
			-10.f,
			10.f
		);

		ImGui::Text(
			"Muzzle World Position : %.3f, %.3f, %.3f",
			m_vMuzzleWorldPos.x,
			m_vMuzzleWorldPos.y,
			m_vMuzzleWorldPos.z
		);

		ImGui::Text(
			"Bullet Dir : %.3f, %.3f, %.3f",
			m_vBulletDir.x,
			m_vBulletDir.y,
			m_vBulletDir.z
		);

		ImGui::Separator();
		ImGui::Text("Attack Setting");

		ImGui::DragFloat(
			"Fire CoolTime",
			&m_fFireCoolTime,
			0.01f,
			0.01f,
			5.f
		);

		ImGui::Text("Fire Timer : %.3f", m_fFireTimer);

		ImGui::Separator();

		if (ImGui::Button("Reset Weapon Local"))
		{
			m_vLocalPos = { -0.06f, 0.15f, 0.27f };
			m_vLocalRot = { 120.f, -148.5f, -14.5f };
			m_vLocalScale = { 1.f, 1.f, 1.f };
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset Muzzle"))
		{
			m_vMuzzleLocalPos = { 1.f, 0.f, -1.f };
		}

		ImGui::Separator();

		if (ImGui::Button("Copy Current Values"))
		{
			char szBuffer[512] = {};

			sprintf_s(
				szBuffer,
				"m_vLocalPos = { %.4ff, %.4ff, %.4ff };\n"
				"m_vLocalRot = { %.4ff, %.4ff, %.4ff };\n"
				"m_vLocalScale = { %.4ff, %.4ff, %.4ff };\n"
				"m_vMuzzleLocalPos = { %.4ff, %.4ff, %.4ff };",
				m_vLocalPos.x,
				m_vLocalPos.y,
				m_vLocalPos.z,
				m_vLocalRot.x,
				m_vLocalRot.y,
				m_vLocalRot.z,
				m_vLocalScale.x,
				m_vLocalScale.y,
				m_vLocalScale.z,
				m_vMuzzleLocalPos.x,
				m_vMuzzleLocalPos.y,
				m_vMuzzleLocalPos.z
			);

			ImGui::SetClipboardText(szBuffer);
		}
	}

	ImGui::End();
}
#endif