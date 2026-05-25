#include "Player.h"


#include "Body_Player.h"
#include "BaseCollider.h"
#include "OBB_Collider.h"
#include "AABB_Collider.h"
#include "Player_FSM.h"
#include "GameInstance.h"

Player::Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: ContainerObject{ pDevice, pContext }
{
}

Player::Player(const Player& Prototype)
	: ContainerObject{ Prototype }
{
}

Player::~Player()
{
}

HRESULT Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Player::Initialize(void* pArg)
{

	GameObject::GAMEOBJECT_DESC desPlayer{};
	desPlayer.ObjectType = ETOUI(OBJECTTYPE::OBJECT_PLAYER);
	desPlayer.m_strName = L"Player";
	desPlayer.ContainerObject = true;
	desPlayer.m_strPrototypeObjectName = L"Prototype_GameObject_Player";
	desPlayer.m_strPrototypeBaseName = L"SK_Player";
	desPlayer.pCameraType = ETOUI(CAMERA::NONE);
	desPlayer.fSpeedPerSec = 5.f;
	desPlayer.fRotationPerSec = 180.f;

	if (FAILED(__super::Initialize(&desPlayer)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pCollider->SetOwner(SHARED_THIS(Player).get());


	return S_OK;
}

void Player::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	
	CGameInstance::Get().Add_Collider(L"Player", m_pCollider.get());
	
	m_pPlayerFSM->Priority_Update(fTimeDelta);

}

void Player::Update(_float fTimeDelta)
{
	
	MouseLook( fTimeDelta);

	_vector vMoveDir = XMVectorZero();

	if (CGameInstance::Get().Key_Pressing(DIK_A))
	{
		vMoveDir += XMVectorSet(-1.f, 0.f, -1.f, 0.f);
	}

	if (CGameInstance::Get().Key_Pressing(DIK_D))
	{
		vMoveDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
	}

	if (CGameInstance::Get().Key_Pressing(DIK_W))
	{
		vMoveDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
	}

	if (CGameInstance::Get().Key_Pressing(DIK_S))
	{
		vMoveDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
	}

	if (!XMVector3Equal(vMoveDir, XMVectorZero()))
	{
		m_pTransformCom->Move(vMoveDir, fTimeDelta);

		dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)
			->Change_State(Player_FSM::WALK);
	}
	else
	{
		dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)
			->Change_State(Player_FSM::IDLE);
	}
	__super::Update(fTimeDelta);



	m_pPlayerFSM->Update(fTimeDelta);
}

void Player::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	Collider_Obstacle(fTimeDelta);
	m_pPlayerFSM->Late_Update(fTimeDelta);
}

void Player::MouseLook(_float fTimeDelta) {

	POINT pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	float width = CGameInstance::Get().Get_ViewportSize().x;
	float height = CGameInstance::Get().Get_ViewportSize().y;

	// Screen -> NDC
	float px = (2.f * pt.x / width) - 1.f;
	float py = 1.f - (2.f * pt.y / height);

	// View / Projection
	_float4x4 view, proj;

	CGameInstance::Get().Get_MainCamerwaViewMatrix(view);
	CGameInstance::Get().Get_MainCamerwaProjectionMatrix(proj);

	XMMATRIX matView = XMLoadFloat4x4(&view);
	XMMATRIX matProj = XMLoadFloat4x4(&proj);

	XMMATRIX invView = XMMatrixInverse(nullptr, matView);
	XMMATRIX invProj = XMMatrixInverse(nullptr, matProj);

	// Near
	XMVECTOR vNear = XMVectorSet(px, py, 0.f, 1.f);
	vNear = XMVector3TransformCoord(vNear, invProj);
	vNear = XMVector3TransformCoord(vNear, invView);

	// Far
	XMVECTOR vFar = XMVectorSet(px, py, 1.f, 1.f);
	vFar = XMVector3TransformCoord(vFar, invProj);
	vFar = XMVector3TransformCoord(vFar, invView);

	// Ray
	XMVECTOR vRayOrigin = vNear;

	XMVECTOR vRayDir =
		XMVector3Normalize(vFar - vNear);

	// y = 0 Plane
	float dirY = XMVectorGetY(vRayDir);

	if (fabs(dirY) < 0.0001f)
		return;

	float t =
		-XMVectorGetY(vRayOrigin) / dirY;

	XMVECTOR vHitPos =
		vRayOrigin + vRayDir * t;

	// Direction
	XMVECTOR vPlayerPos =
		m_pTransformCom->Get_State(STATE::POSITION);

	XMVECTOR vDir =
		XMVector3Normalize(vHitPos - vPlayerPos);

	vDir = XMVectorSetY(vDir, 0.f);
	vDir = XMVector3Normalize(vDir);

	// Yaw
	float yaw =
		atan2f(
			XMVectorGetX(vDir),
			XMVectorGetZ(vDir));

	yaw = XMConvertToDegrees(yaw);

	// Rotation
	m_pTransformCom->Rotation(0.f, yaw, 0.f);
}
HRESULT Player::Render()
{
	return S_OK;
}

HRESULT Player::Ready_Components()
{
	m_pCollider = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_OBB_Collider")));
	m_pCollider->Set_Tag(COLLIDER::COLLIDER_OBB);
	if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), m_pCollider)))
		return E_FAIL;


	return S_OK;
}

HRESULT Player::Ready_PartObjects()
{

	// FSM---------------------------------------------------------------------------


	if ((m_pPlayerFSM = Player_FSM::Create(&m_iState)) == nullptr) {
		return E_FAIL;
	}

	Body_Player::BODY_PLAYER_DESC		BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	BodyDesc.ObjectType = ETOUI(OBJECTTYPE::OBEJCT_PART);
	BodyDesc.m_strName =L"ObjectPart";
	BodyDesc.pParentState = &m_iState;
	BodyDesc.m_strPrototypeObjectName = L"Prototype_GameObject_Body_Player";
	BodyDesc.m_strPrototypeBaseName =L"SK_Player";
	BodyDesc.pCameraType = ETOUI(CAMERA::NONE);
	BodyDesc.fSpeedPerSec = 5.f;
	BodyDesc.fRotationPerSec = 1.f;

	if (FAILED(__super::Add_PartObject(CGameInstance::Get().Get_Level(), BodyDesc.m_strPrototypeObjectName,
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;

	/*Weapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pParentState = &m_iState;
	WeaponDesc.pSocketMatrix = dynamic_pointer_cast<CBody_Player>(m_PartObjects[TEXT("Part_Body")])->Get_SocketMatrixPtr("SWORD");

	if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;*/






	return S_OK;
}

_bool Player::Collider_Obstacle(_float fTimeDelta)
{

	auto ColliderGroup = CGameInstance::Get().GetColliderGroups(L"Obstacle");
	if (ColliderGroup != nullptr) {
		for (auto Collider : *ColliderGroup)
		{
			if (CGameInstance::Get().Intersect(m_pCollider.get(),Collider))
			{
				auto a = static_cast<OBB_Collider*>(m_pCollider.get())->Get_BoudingBox();

				auto b =static_cast<OBB_Collider*>(Collider)->Get_BoudingBox();

			
				Collider->SetColliderColor(
					ColliderColor::RED);

				m_pCollider->SetColliderColor(
					ColliderColor::RED);
			}
		}
	}

	return _bool();
}

unique_ptr<Player> Player::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Player>(new Player(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Player");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Player::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Player(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Player");
		return nullptr;
	}

	return pInstance;
}

