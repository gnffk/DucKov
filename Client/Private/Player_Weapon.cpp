#include "Player_Weapon.h"
#include "OBB_Collider.h"
#include "GameInstance.h"
#include "BaseCollider.h"
#include "Bullet.h"
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
		XMMatrixTranslation(
			m_vLocalPos.x,
			m_vLocalPos.y,
			m_vLocalPos.z
		);

	_matrix ChildMatrix = LocalWeaponMatrix * SocketPosMatrix;

	Make_CombinedWorldMatrix(ChildMatrix);

	_float4x4 WeaponWorldFloat4x4 = __super::GetCombined();
	_matrix WeaponWorld = XMLoadFloat4x4(&WeaponWorldFloat4x4);

	_vector vMuzzleLocal = XMVectorSet(m_vMuzzleLocalPos.x,m_vMuzzleLocalPos.y,m_vMuzzleLocalPos.z,1.f);

	_vector vMuzzleWorld =XMVector3TransformCoord(vMuzzleLocal, WeaponWorld);

	XMStoreFloat3(&m_vMuzzleWorldPos, vMuzzleWorld);

	if (m_fFireTimer > 0.f)
		m_fFireTimer -= fTimeDelta;



	Update_MouseRecoil(fTimeDelta, CGameInstance::Get().Mouse_Pressing(MOUSEKEYSTATE::DIM_LB));
}

void Player_Weapon::Late_Update(_float fTimeDelta)
{


	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Player_Weapon));

	__super::Late_Update(fTimeDelta);
}

HRESULT Player_Weapon::Render()
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
void Player_Weapon::Fire_Bullet()
{

	if (m_fFireTimer > 0.f)
		return;

	m_fFireTimer = m_fFireCoolTime;

	_matrix ParentMatrix = XMLoadFloat4x4(m_pParentMatrix);

	_vector vDir = ParentMatrix.r[2];

	vDir = XMVectorSetY(vDir, 0.f);
	vDir = XMVector3Normalize(vDir);

	XMStoreFloat3(&m_vBulletDir, vDir);

	Apply_MouseRecoil(vDir);


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
#ifdef _DEBUG

void Player_Weapon::GUI_PlayerWeapon()
{
	if (ImGui::Begin("Player Weapon Editor"))
	{
		ImGui::SeparatorText("Weapon Local Transform");

		ImGui::DragFloat3("Local Position", reinterpret_cast<float*>(&m_vLocalPos), 0.01f, -100.f, 100.f);
		ImGui::DragFloat3("Local Rotation", reinterpret_cast<float*>(&m_vLocalRot), 0.5f, -360.f, 360.f);
		ImGui::DragFloat3("Local Scale", reinterpret_cast<float*>(&m_vLocalScale), 0.01f, 0.01f, 100.f);

		ImGui::SeparatorText("Muzzle");

		ImGui::DragFloat3("Muzzle Local Position", reinterpret_cast<float*>(&m_vMuzzleLocalPos), 0.01f, -10.f, 10.f);
	
		_float4x4 WeaponWorldFloat4x4 = __super::GetCombined();

		_matrix WeaponWorld = XMLoadFloat4x4(&WeaponWorldFloat4x4);

		// 기본 발사 위치 = Weapon 현재 위치
		_vector vWeaponPos = WeaponWorld.r[3];

		// 총구 Local Offset 적용
		_vector vMuzzleLocal = XMVectorSet(m_vMuzzleLocalPos.x,m_vMuzzleLocalPos.y,m_vMuzzleLocalPos.z,1.f);

		_vector vMuzzleWorld = XMVector3TransformCoord(vMuzzleLocal, WeaponWorld);

		XMStoreFloat3(&m_vMuzzleWorldPos, vMuzzleWorld);

		ImGui::Text("Bullet Dir : %.3f, %.3f, %.3f",
			m_vBulletDir.x,
			m_vBulletDir.y,
			m_vBulletDir.z);

		if (ImGui::Button("Test Fire"))
		{
			Fire_Bullet();
		}

		if (ImGui::Button("Reset"))
		{
			m_vLocalPos = { 0.f, 0.f, 0.f };
			m_vLocalRot = { 90.f, 0.f, 0.f };
			m_vLocalScale = { 1.f, 1.f, 1.f };
			m_vMuzzleLocalPos = { 0.f, 0.f, 1.f };
		}
	}

	ImGui::End();
}

#endif


_bool Player_Weapon::Make_ScreenDirFromWorldDir(const _float3& vWorldPos,_vector vWorldDir,_float2& vOutScreenDir)
{
	_float2 vViewportSize = CGameInstance::Get().Get_ViewportSize();

	_float fWidth = vViewportSize.x;
	_float fHeight = vViewportSize.y;

	_float4x4 ViewMatrix{};
	_float4x4 ProjMatrix{};

	CGameInstance::Get().Get_MainCamerwaViewMatrix(ViewMatrix);
	CGameInstance::Get().Get_MainCamerwaProjectionMatrix(ProjMatrix);

	_matrix matView = XMLoadFloat4x4(&ViewMatrix);
	_matrix matProj = XMLoadFloat4x4(&ProjMatrix);
	_matrix matWorld = XMMatrixIdentity();

	_vector vStart = XMLoadFloat3(&vWorldPos);
	_vector vDir = XMVector3Normalize(vWorldDir);

	// 총알 방향으로 살짝 앞쪽 점
	_vector vEnd = vStart + vDir * 10.f;

	_vector vScreenStart = XMVector3Project(vStart,0.f,0.f,fWidth,fHeight,0.f,1.f,matProj,matView,matWorld);

	_vector vScreenEnd = XMVector3Project(vEnd,0.f,0.f,fWidth,fHeight,0.f,1.f,matProj,matView,matWorld
	);

	_float3 vS0{};
	_float3 vS1{};

	XMStoreFloat3(&vS0, vScreenStart);
	XMStoreFloat3(&vS1, vScreenEnd);

	_float fDX = vS1.x - vS0.x;
	_float fDY = vS1.y - vS0.y;

	_float fLen = sqrtf(fDX * fDX + fDY * fDY);

	if (fLen <= 0.001f)
	{
		// 화면상 방향이 거의 안 잡히면 위쪽으로 반동
		vOutScreenDir = { 0.f, -1.f };
		return false;
	}

	vOutScreenDir.x = fDX / fLen;
	vOutScreenDir.y = fDY / fLen;

	return true;
}

void Player_Weapon::Apply_MouseRecoil(_vector vBulletDir)
{
	_float2 vScreenDir{};

	Make_ScreenDirFromWorldDir(m_vMuzzleWorldPos, vBulletDir, vScreenDir);

	_float2 vSideDir{};
	vSideDir.x = -vScreenDir.y;
	vSideDir.y = vScreenDir.x;

	_float fPower =
		m_fMouseRecoilPower +
		CGameInstance::Get().Random(-m_fMouseRecoilPowerRandom, m_fMouseRecoilPowerRandom);

	_float fSidePower =
		CGameInstance::Get().Random(-m_fMouseRecoilSideRandom, m_fMouseRecoilSideRandom);

	_float2 vKick{};
	vKick.x = vScreenDir.x * fPower + vSideDir.x * fSidePower;
	vKick.y = vScreenDir.y * fPower + vSideDir.y * fSidePower;

	POINT ptCursor{};
	GetCursorPos(&ptCursor);

	if (!m_bMouseRecoilActive)
	{
		m_ptMouseRecoilBase = ptCursor;

		m_vMouseRecoilTargetOffset = { 0.f, 0.f };
		m_vMouseRecoilAppliedOffset = { 0.f, 0.f };
		m_vPrevAppliedMouseRecoil = { 0.f, 0.f };

		m_bMouseRecoilActive = true;
	}
	else
	{
		// 유저가 반동 중에도 마우스를 움직였으면 기준점 갱신
		m_ptMouseRecoilBase.x = LONG(ptCursor.x - m_vPrevAppliedMouseRecoil.x);
		m_ptMouseRecoilBase.y = LONG(ptCursor.y - m_vPrevAppliedMouseRecoil.y);
	}

	// 여기서 바로 마우스를 움직이지 않고 목표값만 증가
	m_vMouseRecoilTargetOffset.x += vKick.x;
	m_vMouseRecoilTargetOffset.y += vKick.y;

	_float fLen = sqrtf(
		m_vMouseRecoilTargetOffset.x * m_vMouseRecoilTargetOffset.x +
		m_vMouseRecoilTargetOffset.y * m_vMouseRecoilTargetOffset.y
	);

	if (fLen > m_fMouseRecoilMaxOffset)
	{
		_float fRatio = m_fMouseRecoilMaxOffset / fLen;

		m_vMouseRecoilTargetOffset.x *= fRatio;
		m_vMouseRecoilTargetOffset.y *= fRatio;
	}
}
void Player_Weapon::Update_MouseRecoil(_float fTimeDelta, _bool bIsShooting)
{
	if (!m_bMouseRecoilActive)
		return;

	POINT ptCursor{};
	GetCursorPos(&ptCursor);

	if (!bIsShooting)
	{
		m_ptMouseRecoilBase = ptCursor;

		m_vMouseRecoilTargetOffset = { 0.f, 0.f };
		m_vMouseRecoilAppliedOffset = { 0.f, 0.f };
		m_vPrevAppliedMouseRecoil = { 0.f, 0.f };

		m_bMouseRecoilActive = false;
		return;
	}

	// 유저가 마우스를 직접 움직였을 수 있으므로 기준점 갱신
	m_ptMouseRecoilBase.x = LONG(ptCursor.x - m_vPrevAppliedMouseRecoil.x);
	m_ptMouseRecoilBase.y = LONG(ptCursor.y - m_vPrevAppliedMouseRecoil.y);

	// 총을 누르고 있을 때만 반동 복귀
	if (bIsShooting)
	{
		_float fTargetLen = sqrtf(
			m_vMouseRecoilTargetOffset.x * m_vMouseRecoilTargetOffset.x +
			m_vMouseRecoilTargetOffset.y * m_vMouseRecoilTargetOffset.y
		);

		if (fTargetLen > 0.001f)
		{
			_float fRecover = m_fMouseRecoilRecoverSpeed * fTimeDelta;

			if (fRecover >= fTargetLen)
			{
				m_vMouseRecoilTargetOffset = { 0.f, 0.f };
			}
			else
			{
				m_vMouseRecoilTargetOffset.x -=
					m_vMouseRecoilTargetOffset.x / fTargetLen * fRecover;

				m_vMouseRecoilTargetOffset.y -=
					m_vMouseRecoilTargetOffset.y / fTargetLen * fRecover;
			}
		}
	}

	// 현재 적용된 반동이 목표 반동까지 서서히 따라감
	_float2 vDelta{};
	vDelta.x = m_vMouseRecoilTargetOffset.x - m_vMouseRecoilAppliedOffset.x;
	vDelta.y = m_vMouseRecoilTargetOffset.y - m_vMouseRecoilAppliedOffset.y;

	_float fDeltaLen = sqrtf(vDelta.x * vDelta.x + vDelta.y * vDelta.y);

	if (fDeltaLen > 0.001f)
	{
		_float fMove = m_fMouseRecoilKickSpeed * fTimeDelta;

		if (fMove >= fDeltaLen)
		{
			m_vMouseRecoilAppliedOffset = m_vMouseRecoilTargetOffset;
		}
		else
		{
			m_vMouseRecoilAppliedOffset.x += vDelta.x / fDeltaLen * fMove;
			m_vMouseRecoilAppliedOffset.y += vDelta.y / fDeltaLen * fMove;
		}
	}

	m_vPrevAppliedMouseRecoil = m_vMouseRecoilAppliedOffset;

	SetCursorPos(
		LONG(m_ptMouseRecoilBase.x + m_vMouseRecoilAppliedOffset.x),
		LONG(m_ptMouseRecoilBase.y + m_vMouseRecoilAppliedOffset.y)
	);

	_float fAppliedLen = sqrtf(
		m_vMouseRecoilAppliedOffset.x * m_vMouseRecoilAppliedOffset.x +
		m_vMouseRecoilAppliedOffset.y * m_vMouseRecoilAppliedOffset.y
	);

	_float fTargetLen = sqrtf(
		m_vMouseRecoilTargetOffset.x * m_vMouseRecoilTargetOffset.x +
		m_vMouseRecoilTargetOffset.y * m_vMouseRecoilTargetOffset.y
	);

	if (fAppliedLen <= 0.5f && fTargetLen <= 0.5f)
	{
		m_vMouseRecoilAppliedOffset = { 0.f, 0.f };
		m_vMouseRecoilTargetOffset = { 0.f, 0.f };
		m_vPrevAppliedMouseRecoil = { 0.f, 0.f };
		m_bMouseRecoilActive = false;
	}
}