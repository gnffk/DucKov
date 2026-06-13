#include "BossMonster.h"
#include "GameInstance.h"
#include "BossMonsterFSM.h"
#include "Boss_Weapon.h"
#include "BossPattern.h"
NS_BEGIN(Client)

BossMonster::BossMonster(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext): Monster{ pDevice, pContext }
{
}

BossMonster::BossMonster(const BossMonster& Prototype): Monster{ Prototype }
	, m_fMaxHP{ Prototype.m_fMaxHP }
	, m_fHP{ Prototype.m_fHP }
	, m_fAttackPower{ Prototype.m_fAttackPower }
{
}

BossMonster::~BossMonster()
{
}

const _float4x4* BossMonster::Get_SocketMatrixPtr(const _char* pSocketName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pSocketName);

}

void BossMonster::SetCurrentNavMesh()
{
	if (nullptr != m_pNavigationCom)
	{
		m_pNavigationCom->Set_CurrentCell(m_pTransformCom->Get_State(STATE::POSITION));

		m_pTransformCom->Set_State(STATE::POSITION, m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));
	}

}

HRESULT BossMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT BossMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_PartObjects()))
		return E_FAIL;


	return S_OK;
}

HRESULT BossMonster::Update_Animation(_float fTimeDelta)
{
	uint32_t targetAnim = 0;
	_bool isLoop = true;
	_float blendDuration = 0.15f;


	if (-1 != m_iState)
	{
		switch (m_iState)
		{
		case BossMonsterFSM::BOSS_STATE::IDLE:
			targetAnim = 0;
			blendDuration = 0.18f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::HAND_UP:
			targetAnim = 1;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::RIGHT_WALK:
			targetAnim = 2;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::LEFT_WALK:
			targetAnim = 3;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::WALK:
			targetAnim = 4;
			blendDuration = 0.12f;
			nextAnimPlus = 0.5f;
			break;

		case BossMonsterFSM::BOSS_STATE::WALK_BACKWARD:
			targetAnim = 5;
			blendDuration = 0.1f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::TPOSE:
			targetAnim = 6;
			blendDuration = 0.15f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::HAND_UP_AND_BACKWARD:
			targetAnim = 7;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::HAND_UP_AND_WALK:
			targetAnim = 8;
			blendDuration = 0.12f;
			nextAnimPlus = 0.7f;
			break;

		case BossMonsterFSM::BOSS_STATE::HAND_UP_AND_RIGHT:
			targetAnim = 9;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::HAND_UP_AND_LEFT:
			targetAnim = 10;
			blendDuration = 0.12f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::ROLL:
			targetAnim = 11;
			isLoop = false;
			blendDuration = 0.06f;
			nextAnimPlus = 2.f;
			break;

		case BossMonsterFSM::BOSS_STATE::HIT:
			targetAnim = 12;
			isLoop = false;
			blendDuration = 0.05f;
			nextAnimPlus = 1.f;
			break;

		case BossMonsterFSM::BOSS_STATE::HIT_REVERSE:
			targetAnim = 13;
			isLoop = false;
			blendDuration = 0.05f;
			nextAnimPlus = 1.f;
			break;


		case BossMonsterFSM::BOSS_STATE::RUN:
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
		m_pModelCom->Play_Animation(fTimeDelta * nextAnimPlus);
	}

	return S_OK;
}

HRESULT BossMonster::Ready_PartObjects()
{

	// FSM---------------------------------------------------------------------------

	if ((m_pMonsterFSM = BossMonsterFSM::Create(&m_iState)) == nullptr) {
		return E_FAIL;
	}

	m_pMonsterFSM->SetOwner(SHARED_THIS(BossMonster));
	
	// NavMesh -----------------------------------------------------------------------
	Navigation::NAVIGATION_DESC		NaviDesc{ 1 };

	m_pNavigationCom = dynamic_pointer_cast<Navigation>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Navigation"), &NaviDesc));
	if (FAILED(__super::Add_Component(TEXT("Com_Navigation"), m_pNavigationCom)))
		return E_FAIL;

	// BossPattern ------------------------------------------------------------------
	m_pBossPattern = BossPattern::Create(SHARED_THIS(BossMonster));

	
	Boss_Weapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentState = &m_iState;
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pSocketMatrix = this->Get_SocketMatrixPtr("MeleeWeaponSocket");
	WeaponDesc.ObjectType = ETOUI(OBJECTTYPE::OBEJCT_PART);
	WeaponDesc.m_strName = L"ObjectPart";
	WeaponDesc.pParentState = &m_iState;
	WeaponDesc.m_strPrototypeObjectName = L"";
	WeaponDesc.m_strPrototypeBaseName = L"SK_CustomBody";
	WeaponDesc.pCameraType = ETOUI(CAMERA::NONE);
	WeaponDesc.fSpeedPerSec = 5.f;
	WeaponDesc.fRotationPerSec = 1.f;
	
	if (FAILED(__super::Add_PartObject(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossPlayer_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT BossMonster::Ready_UI()
{

	return S_OK;
}

void BossMonster::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	m_pMonsterFSM->Priority_Update(fTimeDelta);
}

void BossMonster::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	

	if (m_pBossPattern)
		m_pBossPattern->Update(fTimeDelta);


	if (m_pMonsterFSM)
		m_pMonsterFSM->Update(fTimeDelta);
	SetCurrentNavMesh();


	Update_Animation(fTimeDelta);
}

void BossMonster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pMonsterFSM->Late_Update(fTimeDelta);
}

HRESULT BossMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;



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
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (uint32_t)i)))
			return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_Normals, 0)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		m_pModelCom->Render(i);
	}



	return S_OK;
}

void BossMonster::Turn_To_Direction(const _float3& vDirection,_float fTimeDelta)
{
	if (m_pTransformCom == nullptr)
		return;

	_vector vTargetDir =XMLoadFloat3(&vDirection);

	vTargetDir = XMVectorSetY(vTargetDir, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vTargetDir)) <= 0.000001f)
		return;

	vTargetDir = XMVector3Normalize(vTargetDir);

	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

	vLook = XMVectorSetY(vLook, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
		return;

	vLook = XMVector3Normalize(vLook);

	// 현재 Look과 목표 방향 사이의 각도 계산
	_float fDot = XMVectorGetX(XMVector3Dot(vLook, vTargetDir));

	fDot =max(-1.f, min(1.f, fDot));

	_float fAngle = XMConvertToDegrees(acosf(fDot));

	// 거의 같은 방향이면 더 돌릴 필요 없음
	if (fAngle <= 1.f)
		return;

	// 어느 방향으로 돌지 결정
	_vector vCross = XMVector3Cross(vLook, vTargetDir);

	_float fTurnSign = XMVectorGetY(vCross);

	if (fTurnSign >= 0.f)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f),fTimeDelta);
	}
	else
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, -1.f, 0.f, 0.f),fTimeDelta);
	}
}
void BossMonster::Turn_To_Position(const _float3& vPosition,_float fTimeDelta)
{
	if (m_pTransformCom == nullptr)
		return;

	_vector vMyPosition = m_pTransformCom->Get_State(STATE::POSITION);

	_vector vTargetPosition = XMLoadFloat3(&vPosition);

	_vector vDir = vTargetPosition - vMyPosition;

	vDir = XMVectorSetY(vDir, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vDir)) <= 0.000001f)
		return;

	_float3 vDirFloat{};
	XMStoreFloat3(&vDirFloat, vDir);

	Turn_To_Direction(vDirFloat, fTimeDelta);
}

void BossMonster::Move_Direction(const _float3& vDirection,_float fTimeDelta,_float fSpeedScale)
{
	if (m_pTransformCom == nullptr)
		return;

	_vector vMoveDir =XMLoadFloat3(&vDirection);

	vMoveDir = XMVectorSetY(vMoveDir, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vMoveDir)) <= 0.000001f)
		return;

	vMoveDir = XMVector3Normalize(vMoveDir);

	m_pTransformCom->Go_Direction(vMoveDir,fTimeDelta,m_pNavigationCom,fSpeedScale);
}
void BossMonster::Move_Forward(_float fTimeDelta,_float fSpeedScale)
{
	if (m_pTransformCom == nullptr)
		return;

	_vector vLook =m_pTransformCom->Get_State(STATE::LOOK);

	vLook = XMVectorSetY(vLook, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
		return;

	vLook =XMVector3Normalize(vLook);

	m_pTransformCom->Go_Direction(vLook,fTimeDelta,m_pNavigationCom,fSpeedScale);
}

unique_ptr<BossMonster> BossMonster::Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<BossMonster> pInstance =unique_ptr<BossMonster>(new BossMonster(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : BossMonster");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> BossMonster::Clone(void* pArg)
{
	shared_ptr<BossMonster> pInstance =
		shared_ptr<BossMonster>(new BossMonster(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : BossMonster");
		return nullptr;
	}

	return pInstance;
}

NS_END