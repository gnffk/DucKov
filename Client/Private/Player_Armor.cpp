#include "Player_Armor.h"
#include "OBB_Collider.h"
#include "GameInstance.h"
#include "BaseCollider.h"
Player_Armor::Player_Armor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: PartObject{ pDevice, pContext }

{
}

Player_Armor::Player_Armor(const Player_Armor& Prototype)
	: PartObject{ Prototype }
{
}

Player_Armor::~Player_Armor()
{

}


HRESULT Player_Armor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Player_Armor::Initialize(void* pArg)
{
	auto		pDesc = static_cast<Player_Armor::ARMOR_DESC*>(pArg);

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

void Player_Armor::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Player_Armor::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	_vector vSocketPos = SocketMatrix.r[3];

	_matrix SocketPosMatrix = XMMatrixIdentity();
	SocketPosMatrix.r[3] = vSocketPos;

	



	Make_CombinedWorldMatrix(SocketPosMatrix);

}

void Player_Armor::Late_Update(_float fTimeDelta)
{


	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Player_Armor));

	__super::Late_Update(fTimeDelta);
}

HRESULT Player_Armor::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;

	if (currentArmorType == "Default")
		return S_OK;

	auto iterModel = m_pModelMapCom.find(currentArmorType);

	if (iterModel == m_pModelMapCom.end())
		return S_OK;

	if (iterModel->second == nullptr)
		return S_OK;

	auto pModel = iterModel->second;

	uint32_t iNumMeshes = pModel->Get_NumMeshes();

	for (uint32_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(pModel->Bind_Materials(m_pShaderCom,"g_DiffuseTexture",i,ETOUI(TEXTURETYPE::DIFFUSE),0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		pModel->Render(i);
	}

	return S_OK;
}

HRESULT Player_Armor::Ready_Components()
{

	for (int32_t i = 0; i < 3; ++i)
	{
		string armorType = "Armor" + to_string(i);
		string armorModelName = "Prototype_Com_Model_" + armorType;

		wstring armorModelNameW =
			wstring(armorModelName.begin(), armorModelName.end());

		wstring componentNameW =L"Com_Model_" + wstring(armorType.begin(), armorType.end());

		auto pModel =dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),armorModelNameW));

		if (pModel == nullptr)
			return E_FAIL;

		m_pModelMapCom[armorType] = pModel;

		if (FAILED(__super::Add_Component(componentNameW.c_str(), pModel)))
			return E_FAIL;
	}




	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_FBX_Tex")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	//m_pColliderCom = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_OBB_Collider")));
	//if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), m_pColliderCom)))
	//	return E_FAIL;






	return S_OK;
}

unique_ptr<Player_Armor> Player_Armor::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Player_Armor>(new Player_Armor(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Player_Armor");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Player_Armor::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Player_Armor(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Player_Armor");
		return nullptr;
	}

	return pInstance;
}

