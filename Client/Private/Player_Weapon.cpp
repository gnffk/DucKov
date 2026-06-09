#include "Player_Weapon.h"
#include "OBB_Collider.h"
#include "GameInstance.h"
#include "BaseCollider.h"
Player_Weapon::Player_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: PartObject{ pDevice, pContext }

{
}

Player_Weapon::Player_Weapon(const Player_Weapon& Prototype)
	: PartObject{ Prototype }
{
}

Player_Weapon::~Player_Weapon()
{

}


HRESULT Player_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Player_Weapon::Initialize(void* pArg)
{
	auto		pDesc = static_cast<Player_Weapon::WEAPON_DESC*>(pArg);

	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	        

	//m_pTransformCom->Rotation(360.f, -148.5f, -14.5);
	//m_pTransformCom->Set_State(STATE::POSITION, {-0.06f, 0.15f,0.27f});


	return S_OK;
}

void Player_Weapon::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Player_Weapon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
#ifdef _DEBUG
	GUI_PlayerWeapon();
#endif
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
		XMMatrixTranslation(m_vLocalPos.x, m_vLocalPos.y, m_vLocalPos.z);

	_matrix ChildMatrix = LocalWeaponMatrix * SocketPosMatrix;

	Make_CombinedWorldMatrix(ChildMatrix);

}

void Player_Weapon::Late_Update(_float fTimeDelta)
{


	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Player_Weapon));

	__super::Late_Update(fTimeDelta);
}

HRESULT Player_Weapon::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;



	uint32_t	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", (uint32_t)i, (uint32_t)ETOUI(TEXTURETYPE::DIFFUSE), 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		m_pModelCom->Render(i);
	}



	return S_OK;
}

HRESULT Player_Weapon::Ready_Components()
{
	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_Gun2")));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_FBX_Tex")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	m_pColliderCom = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_OBB_Collider")));
	if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

unique_ptr<Player_Weapon> Player_Weapon::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Player_Weapon>(new Player_Weapon(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Player_Weapon");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Player_Weapon::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Player_Weapon(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Player_Weapon");
		return nullptr;
	}

	return pInstance;
}

#ifdef _DEBUG

void Player_Weapon::GUI_PlayerWeapon()
{
	if (ImGui::Begin("Player Weapon Editor"))
	{
		static _float3 vPos = { 0.f, 0.f, 0.f };
		static _float3 vRot = { 90.f, 0.f, 0.f };
		static _float3 vScale = { 1.f, 1.f, 1.f };

		ImGui::SeparatorText("Weapon Local Transform");

		ImGui::DragFloat3("Local Position", reinterpret_cast<float*>(&vPos), 0.01f, -100.f, 100.f);
		ImGui::DragFloat3("Local Rotation", reinterpret_cast<float*>(&vRot), 0.5f, -360.f, 360.f);
		ImGui::DragFloat3("Local Scale", reinterpret_cast<float*>(&vScale), 0.01f, 0.01f, 100.f);

		if (ImGui::Button("Reset"))
		{
			vPos = { 0.f, 0.f, 0.f };
			vRot = { 90.f, 0.f, 0.f };
			vScale = { 1.f, 1.f, 1.f };
		}

		m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

		_matrix matWorld = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

		matWorld *= XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(vRot.x),
			XMConvertToRadians(vRot.y),
			XMConvertToRadians(vRot.z)
		);

		matWorld *= XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

		_float4x4 World;
		XMStoreFloat4x4(&World, matWorld);

		m_pTransformCom->Set_WorldMatrix(World);
	}

	ImGui::End();
}

#endif