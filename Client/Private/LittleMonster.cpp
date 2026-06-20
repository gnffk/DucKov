#include "LittleMonster.h"
#include "GameInstance.h"
#include "LittleMonsterFSM.h"
#include "LittleMonster_Pattern.h"
#include "LittleMonster_Weapon.h"
#include "LittleMonster_StateUI.h"
#include "BaseCollider.h"
#include "InteractBox.h"
#include "Particle_Blood.h"

NS_BEGIN(Client)

LittleMonster::LittleMonster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : Monster{ pDevice, pContext }
{
}

LittleMonster::LittleMonster(const LittleMonster& Prototype) : Monster{ Prototype }
, m_fMaxHP{ Prototype.m_fMaxHP }
, m_fHP{ Prototype.m_fHP }
, m_fAttackPower{ Prototype.m_fAttackPower }
{
}

LittleMonster::~LittleMonster()
{
}

const _float4x4* LittleMonster::Get_SocketMatrixPtr(const _char* pSocketName)
{
	return m_pModelCom->Get_BoneMatrixPtr(pSocketName);

}

void LittleMonster::SetCurrentNavMesh()
{
	if (nullptr != m_pNavigationCom)
	{
		m_pNavigationCom->Set_CurrentCell(m_pTransformCom->Get_State(STATE::POSITION));

		m_pTransformCom->Set_State(STATE::POSITION, m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));
	}

}

HRESULT LittleMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT LittleMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;


	return S_OK;
}

HRESULT LittleMonster::Update_Animation(_float fTimeDelta)
{
	uint32_t targetAnim = 0;
	_bool isLoop = true;
	_float blendDuration = 0.15f;
	_float animSpeed = 1.f;

	if (-1 != m_iState)
	{
		switch (m_iState)
		{
		case LittleMonsterFSM::LITTLEMONSTER_STATE::IDLE:
			targetAnim = 0;
			isLoop = true;
			blendDuration = 0.18f;
			animSpeed = 1.f;
			break;

		case LittleMonsterFSM::LITTLEMONSTER_STATE::WALK:
			targetAnim =1;
			isLoop = true;
			blendDuration = 0.12f;
			animSpeed = 1.f;
			break;

		case LittleMonsterFSM::LITTLEMONSTER_STATE::ATTACK:

			targetAnim = 2;
			isLoop = false;
			blendDuration = 0.08f;
			animSpeed = 1.2f;
			break;

		case LittleMonsterFSM::LITTLEMONSTER_STATE::ROLL:
			targetAnim = 3;
			isLoop = false;
			blendDuration = 0.06f;
			animSpeed = 2.f;
			break;

		default:
			targetAnim = 0;
			isLoop = true;
			blendDuration = 0.18f;
			animSpeed = 1.f;
			break;
		}
	}

	nextAnim = targetAnim;
	nextAnimPlus = animSpeed;

	if (m_pModelCom->Get_CurAnimationIndex() != nextAnim)
	{
		m_pModelCom->Set_Animation(nextAnim, isLoop, blendDuration);
	}
	else
	{
		m_pModelCom->Play_Animation(fTimeDelta * nextAnimPlus);
	}

	return S_OK;
}
HRESULT LittleMonster::Ready_PartObjects()
{

	// FSM---------------------------------------------------------------------------

	if ((m_pMonsterFSM = LittleMonsterFSM::Create(&m_iState)) == nullptr) {
		return E_FAIL;
	}

	m_pMonsterFSM->SetOwner(SHARED_THIS(LittleMonster));

	// NavMesh -----------------------------------------------------------------------
	Navigation::NAVIGATION_DESC		NaviDesc{ 1 };

	m_pNavigationCom = dynamic_pointer_cast<Navigation>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Navigation"), &NaviDesc));
	if (FAILED(__super::Add_Component(TEXT("Com_Navigation"), m_pNavigationCom)))
		return E_FAIL;

	// LittleMonster_Pattern ------------------------------------------------------------------
	m_pLittleMonsterPattern = LittleMonster_Pattern::Create(SHARED_THIS(LittleMonster));


	LittleMonster_Weapon::WEAPON_DESC		WeaponDesc{};
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

	if (FAILED(__super::Add_PartObject(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LittleMonster_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;






	return S_OK;
}

HRESULT LittleMonster::Ready_UI()
{

	UIObject::UIOBJECT_DESC DesLittleMonsterStateUI{};
	DesLittleMonsterStateUI.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
	DesLittleMonsterStateUI.m_strName = L"LittleMonsterStateUI";
	DesLittleMonsterStateUI.m_strPrototypeObjectName = L"Prototype_GameObject_LittleMonster_StateUI";
	DesLittleMonsterStateUI.m_strPrototypeBaseName = L"LittleMonsterStateUI";
	DesLittleMonsterStateUI.pCameraType = ETOUI(CAMERA::NONE);
	DesLittleMonsterStateUI.fSpeedPerSec = 5.f;
	DesLittleMonsterStateUI.fRotationPerSec = 1.f;
	DesLittleMonsterStateUI.fSizeX = 1.f;
	DesLittleMonsterStateUI.fSizeY = 1.f;
	DesLittleMonsterStateUI.fX = 1.f;
	DesLittleMonsterStateUI.fY = 1.f;

	m_pUI.emplace("LittleMonsterStateUI", dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_LittleMonster_StateUI"), &DesLittleMonsterStateUI)));
	static_pointer_cast<LittleMonster_StateUI>(m_pUI["LittleMonsterStateUI"])->SetOwner(SHARED_THIS(LittleMonster));
	static_pointer_cast<LittleMonster_StateUI>(m_pUI["LittleMonsterStateUI"])->Set_HP(m_fHP, m_fMaxHP);
	return S_OK;
}

void LittleMonster::Spawn_DieBox()
{
	if (m_pTransformCom == nullptr)
		return;

	_float3 vSpawnPos{};
	XMStoreFloat3(&vSpawnPos,m_pTransformCom->Get_State(STATE::POSITION));

	// 박스가 바닥에 너무 박히거나 뜨면 여기 y값 보정
	// vSpawnPos.y += 0.2f;

	InteractBox::INTERACTBOX_DESC Desc{};
	Desc.eInteractType = InteractBox::InteractType::Die;
	Desc.vSpawnPos = vSpawnPos;

	Desc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_STATIC);
	Desc.m_strName = L"DieBox";
	Desc.m_strPrototypeObjectName = L"Prototype_GameObject_InteractBox";
	Desc.m_strPrototypeBaseName = L"SM_Die";
	Desc.pCameraType = ETOUI(CAMERA::NONE);
	Desc.fSpeedPerSec = 0.f;
	Desc.fRotationPerSec = 0.f;

	CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_InteractBox"), CGameInstance::Get().Get_Level(), L"Interact", &Desc);
}

_bool LittleMonster::Collider_Bullet(_float fTimeDelta)
{
	auto ColliderGroup = CGameInstance::Get().GetColliderGroups(L"Bullet");
	auto& ColliderPlayer = m_pColliderComs[(int)COLLIDER::COLLIDER_OBB].front();
	if (ColliderGroup != nullptr && ColliderPlayer != nullptr) {
	
		for (auto Collider : *ColliderGroup)
		{
			if (CGameInstance::Get().Intersect(ColliderPlayer.get(), Collider))
			{
				_float3 vHitPos{};

				auto pBulletOwner = Collider->GetOwner();

				if (pBulletOwner != nullptr && pBulletOwner->GetTransform() != nullptr)
				{
					XMStoreFloat3(&vHitPos,pBulletOwner->GetTransform()->Get_State(STATE::POSITION));
				}
				else
				{
					XMStoreFloat3(&vHitPos,m_pTransformCom->Get_State(STATE::POSITION));

					vHitPos.y += 1.0f;
				}

				Take_Damage(20.f, vHitPos);

				if (pBulletOwner != nullptr) {
					pBulletOwner->Set_Dead();
		
				}

				Collider->SetColliderColor(ColliderColor::RED);
				ColliderPlayer->SetColliderColor(ColliderColor::RED);

				break;
			}


		}
		

	}

	return _bool();
}

void LittleMonster::Spawn_BloodEffect(const _float3& vSpawnPos)
{
	Particle_System::PARTICLE_SPAWN_DESC Desc{};
	Desc.vSpawnPos = vSpawnPos;
	Desc.iCount = 10;
	Desc.fPower = 1.f;

	CGameInstance::Get().Add_Particle(PARTICLE_TYPE::BLOOD, &Desc);
}

void LittleMonster::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	m_pMonsterFSM->Priority_Update(fTimeDelta);
}

void LittleMonster::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	if (m_pLittleMonsterPattern)
		m_pLittleMonsterPattern->Update(fTimeDelta);


	if (m_pMonsterFSM)
		m_pMonsterFSM->Update(fTimeDelta);
	SetCurrentNavMesh();


	Update_Animation(fTimeDelta);
}

void LittleMonster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	for (auto& Pair : m_pUI)
	{
		const string& strUIName = Pair.first;
		auto pUIObject = Pair.second;

		if (nullptr == pUIObject)
			continue;

	

		CGameInstance::Get().Add_UIObject(L"LittleMonster", static_pointer_cast<UIObject>(pUIObject));
	}
	Collider_Bullet(fTimeDelta);
	m_pMonsterFSM->Late_Update(fTimeDelta);
}

HRESULT LittleMonster::Render()
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

void LittleMonster::Turn_To_Direction(const _float3& vDirection, _float fTimeDelta)
{
	if (m_pTransformCom == nullptr)
		return;

	_vector vTargetDir = XMLoadFloat3(&vDirection);

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

	fDot = max(-1.f, min(1.f, fDot));

	_float fAngle = XMConvertToDegrees(acosf(fDot));

	// 거의 같은 방향이면 더 돌릴 필요 없음
	if (fAngle <= 1.f)
		return;

	// 어느 방향으로 돌지 결정
	_vector vCross = XMVector3Cross(vLook, vTargetDir);

	_float fTurnSign = XMVectorGetY(vCross);


	if (fDot < -0.999f)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

	}
	else if (fDot < 0.98f)
	{
		if (fTurnSign < 0.f)
			m_pTransformCom->Turn(XMVectorSet(0.f, -1.f, 0.f, 0.f), fTimeDelta * 1.5f);
		else
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.5f);


	}


}
void LittleMonster::Turn_To_Position(const _float3& vPosition, _float fTimeDelta)
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

void LittleMonster::Move_Direction(const _float3& vDirection, _float fTimeDelta, _float fSpeedScale)
{
	if (m_pTransformCom == nullptr)
		return;

	_vector vMoveDir = XMLoadFloat3(&vDirection);

	vMoveDir = XMVectorSetY(vMoveDir, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vMoveDir)) <= 0.000001f)
		return;

	vMoveDir = XMVector3Normalize(vMoveDir);

	m_pTransformCom->Go_Direction(vMoveDir, fTimeDelta, m_pNavigationCom, fSpeedScale);
}
void LittleMonster::Move_Forward(_float fTimeDelta, _float fSpeedScale)
{
	if (m_pTransformCom == nullptr)
		return;

	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

	vLook = XMVectorSetY(vLook, 0.f);

	if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
		return;

	vLook = XMVector3Normalize(vLook);

	m_pTransformCom->Go_Direction(vLook, fTimeDelta, m_pNavigationCom, fSpeedScale);
}

void LittleMonster::Take_Damage(_float fDamage)
{
	_float3 vBloodPos{};
	XMStoreFloat3(&vBloodPos, m_pTransformCom->Get_State(STATE::POSITION));

	vBloodPos.y += 1.0f;

	Take_Damage(fDamage, vBloodPos);
}

void LittleMonster::Take_Damage(_float fDamage, const _float3& vHitPos)
{
	if (Get_Dead())
		return;

	if (fDamage <= 0.f)
		return;

	m_fHP -= fDamage;

	if (m_fHP < 0.f)
		m_fHP = 0.f;

	Update_HP_UI();

	Spawn_BloodEffect(vHitPos);

	if (m_fHP <= 0.f)
	{
		Spawn_DieBox();
		Set_Dead();
	}
}
void LittleMonster::Update_HP_UI()
{
	auto iter = m_pUI.find("LittleMonsterStateUI");

	if (iter == m_pUI.end())
		return;

	auto pStateUI = dynamic_pointer_cast<LittleMonster_StateUI>(iter->second);

	if (pStateUI == nullptr)
		return;

	pStateUI->Set_HP(m_fHP, m_fMaxHP);
}

unique_ptr<LittleMonster> LittleMonster::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<LittleMonster> pInstance = unique_ptr<LittleMonster>(new LittleMonster(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : LittleMonster");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> LittleMonster::Clone(void* pArg)
{
	shared_ptr<LittleMonster> pInstance =
		shared_ptr<LittleMonster>(new LittleMonster(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : LittleMonster");
		return nullptr;
	}

	return pInstance;
}

NS_END