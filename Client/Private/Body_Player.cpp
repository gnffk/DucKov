#include "Body_Player.h"
#include "Player.h"
#include "OBB_Collider.h"
#include "GameInstance.h"
#include "Player_FSM.h"

Body_Player::Body_Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: PartObject{ pDevice, pContext }
{
}

Body_Player::Body_Player(const Body_Player& Prototype)
	: PartObject{ Prototype }
{
}

Body_Player::~Body_Player()
{
}

const _float4x4* Body_Player::Get_SocketMatrixPtr(const _char* pSocketName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pSocketName);

}

HRESULT Body_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Body_Player::Initialize(void* pArg)
{
	auto	pDesc = static_cast<BODY_PLAYER_DESC*>(pArg);


	m_pParentState = pDesc->pParentState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void Body_Player::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Body_Player::Update(_float fTimeDelta)
{
	uint32_t targetAnim = 0;
	_bool isLoop = true;
	_float blendDuration = 0.15f;

	if (nullptr != m_pParentState)
	{
		switch (*m_pParentState)
		{
		case Player_FSM::PLAYER_STATE::IDLE:
			targetAnim = 0;
			blendDuration = 0.18f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::HAND_UP:
			targetAnim = 1;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::RIGHT_WALK:
			targetAnim = 2;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::LEFT_WALK:
			targetAnim = 3;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::WALK:
			targetAnim = 4;
			blendDuration = 0.12f;
			nextAnimPlus = 0.5f;
			break;

		case Player_FSM::PLAYER_STATE::WALK_BACKWARD:
			targetAnim = 5;
			blendDuration = 0.1f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::TPOSE:
			targetAnim = 6;
			blendDuration = 0.15f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::HAND_UP_AND_BACKWARD:
			targetAnim = 7;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::HAND_UP_AND_WALK:
			targetAnim = 8;
			blendDuration = 0.12f;
			nextAnimPlus = 0.7f;
			break;

		case Player_FSM::PLAYER_STATE::HAND_UP_AND_RIGHT:
			targetAnim = 9;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::HAND_UP_AND_LEFT:
			targetAnim = 10;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::ROLL:
			targetAnim = 11;
			isLoop = false;
			blendDuration = 0.06f;
			nextAnimPlus = 2.f;
			break;

		case Player_FSM::PLAYER_STATE::HIT:
			targetAnim = 12;
			isLoop = false;
			blendDuration = 0.05f;
			nextAnimPlus = 1.f;
			break;

		case Player_FSM::PLAYER_STATE::HIT_REVERSE:
			targetAnim = 13;
			isLoop = false;
			blendDuration = 0.05f;
			nextAnimPlus = 1.f;
			break;


		case Player_FSM::PLAYER_STATE::RUN:
			targetAnim = 4;
			blendDuration = 0.05f;
			nextAnimPlus = 1.f;
			break;

		default:
			targetAnim = 0;
			blendDuration = 0.18f;
			nextAnimPlus = 1.f;
			break;
		}
	}

	nextAnim = targetAnim;

	if (m_pModelCom->Get_CurAnimationIndex() != nextAnim)
	{
		m_pModelCom->Set_Animation(nextAnim, isLoop, blendDuration);
	}

	else {
		m_pModelCom->Play_Animation(fTimeDelta* nextAnimPlus);
	}


	__super::Update(fTimeDelta);
}

void Body_Player::Late_Update(_float fTimeDelta)
{

	Make_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Body_Player));

	__super::Late_Update(fTimeDelta);
}

HRESULT Body_Player::Render()
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

	for (uint32_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", (uint32_t)i, (uint32_t)ETOUI(TEXTURETYPE::DIFFUSE), 0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT Body_Player::Ready_Components()
{
	__super::Clear_Compnent();

	_wstring m_ModelComponentName = TEXT("Prototype_Com_Model_") + Object_INFO.m_strPrototypeBaseName;


	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), m_ModelComponentName));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;


	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_AnimFbx")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;


	m_pOBBCom = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_OBB_Collider")));
	if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), m_pOBBCom)))
		return E_FAIL;


	return S_OK;
}


unique_ptr<Body_Player> Body_Player::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Body_Player>(new Body_Player(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Body_Player");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Body_Player::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Body_Player(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Body_Player");
		return nullptr;
	}

	return pInstance;
}

