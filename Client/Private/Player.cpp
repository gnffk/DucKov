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

	m_pPlayerFSM->SetOwner(SHARED_THIS(Player));
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
	if (FAILED(Roll(fTimeDelta))) {
		return;
	}
	Shift(fTimeDelta);
	m_pPlayerFSM->Update(fTimeDelta);

	__super::Update(fTimeDelta);


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


	_vector TurnAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);


	_vector vCurrentLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));


	_vector cross = XMVector3Cross(vCurrentLook, vDir);
	_float dot = XMVectorGetX(XMVector3Dot(vCurrentLook, vDir));

	if (dot < 0.98f) {
		if (XMVectorGetY(cross) < 0.f) {
			m_pTransformCom->Turn(-TurnAxis, fTimeDelta * 5.f);
		}
		else {
			m_pTransformCom->Turn(TurnAxis, fTimeDelta * 5.f);
		}
		
	}
	else if (dot < 0.999f)
	{
		if (XMVectorGetY(cross) < 0.f) {
			m_pTransformCom->Turn(-TurnAxis, fTimeDelta );
		}
		else {
			m_pTransformCom->Turn(TurnAxis, fTimeDelta );
		}
	}
	
}

void Player::KeyBoardLook(_float fTimeDelta) {
	

	_vector vMoveDir = XMVectorZero();
	_float4 fCameraPos;
	CGameInstance::Get().Get_MainCameraPosition(fCameraPos);

	_vector	vPlayerPos, vCameraPos, vCameraDir;
	vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
	vCameraPos = XMLoadFloat4(&fCameraPos);
	vCameraPos = XMVectorSetY(vCameraPos, 0.0f);
	vCameraDir = vCameraPos - vPlayerPos;

	vCameraDir = XMVector3Normalize(vCameraDir);

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vRight = XMVector3Cross(vUp, vCameraDir);
	vRight = XMVector3Normalize(vRight);

	if (CGameInstance::Get().Key_Pressing(DIK_A))
	{
		vMoveDir += vRight;
	}

	if (CGameInstance::Get().Key_Pressing(DIK_D))
	{
		vMoveDir -= vRight;
	}

	if (CGameInstance::Get().Key_Pressing(DIK_W))
	{
		vMoveDir -= vCameraDir;
	}

	if (CGameInstance::Get().Key_Pressing(DIK_S))
	{
		vMoveDir += vCameraDir;
	}

	_vector TurnAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);



	vMoveDir = XMVector3Normalize(vMoveDir);


	_vector vCurrentLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

	_vector cross = XMVector3Cross(vCurrentLook, vMoveDir);
	_float dot = XMVectorGetX(XMVector3Dot(vCurrentLook, vMoveDir));


	if (dot < 0.98f) {

		if (m_fSpeedFloat < 0.f)
		{
			m_fSpeedFloat = 0.f;

		}
		else {
			m_fSpeedFloat -= 0.001f;
		}

		if (XMVectorGetY(cross) < 0.f) {
			m_pTransformCom->Turn(-TurnAxis, fTimeDelta * 1.5f);
		}
		else {
			m_pTransformCom->Turn(TurnAxis, fTimeDelta * 1.5f);
		}

	}
	else if (dot < 0.999f)
	{
		if (m_fSpeedFloat < 0.f)
		{
			m_fSpeedFloat = 0.f;

		}
		else {
			m_fSpeedFloat -= 0.001f;
		}

		if (XMVectorGetY(cross) < 0.f) {
			m_pTransformCom->Turn(-TurnAxis, fTimeDelta);
		}
		else {
			m_pTransformCom->Turn(TurnAxis, fTimeDelta);
		}
	}
	else {
		if (m_fSpeedFloat > 2.f)
		{
			m_fSpeedFloat = 2.f;

		}
		else {
			m_fSpeedFloat += 0.1f;
		}
	}


	if (!XMVector3Equal(vMoveDir, XMVectorZero()))
	{
		m_pTransformCom->Move(vMoveDir, fTimeDelta * m_fSpeedFloat);

		dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::WALK);
	}
	else
	{
		dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::IDLE);
	}


	
}

HRESULT Player::Roll(_float fTimeDelta)
{
	if (CGameInstance::Get().Key_Down(DIK_SPACE) && false == m_isRolling)
	{
		_vector vMoveDir = XMVectorZero();

		_float4 fCameraPos;
		CGameInstance::Get().Get_MainCameraPosition(fCameraPos);

		_vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
		_vector vCameraPos = XMLoadFloat4(&fCameraPos);
		vCameraPos = XMVectorSetY(vCameraPos, 0.f);

		_vector vCameraDir = XMVector3Normalize(vCameraPos - vPlayerPos);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vCameraDir));

		if (CGameInstance::Get().Key_Pressing(DIK_A))
			vMoveDir += vRight;

		if (CGameInstance::Get().Key_Pressing(DIK_D))
			vMoveDir -= vRight;

		if (CGameInstance::Get().Key_Pressing(DIK_W))
			vMoveDir -= vCameraDir;

		if (CGameInstance::Get().Key_Pressing(DIK_S))
			vMoveDir += vCameraDir;

		// 키 안 누르고 구르면 현재 바라보는 방향으로
		if (XMVector3Equal(vMoveDir, XMVectorZero()))
			vMoveDir = m_pTransformCom->Get_State(STATE::LOOK);


		m_vRollDir = XMVector3Normalize(vMoveDir);

		// Roll 방향으로 즉시 바라보게 만들기
		_float fX = XMVectorGetX(m_vRollDir);
		_float fZ = XMVectorGetZ(m_vRollDir);

		// 기본 Look이 +Z라면 이 공식
		_float fYaw = XMConvertToDegrees(atan2f(fX, fZ));

		m_pTransformCom->Rotation(0.f, fYaw, 0.f);

		m_isRolling = true;
		m_fRollTimer = m_fRollDuration;

		dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::ROLL);


	}
	if (true == m_isRolling)
	{
		m_fRollTimer -= fTimeDelta;

		if (m_fRollTimer <= 0.f)
		{
			m_isRolling = false;
			dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::IDLE);
		}
		else
		{
			dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::ROLL);
			m_pPlayerFSM->Update(fTimeDelta);
			__super::Update(fTimeDelta);
			return E_FAIL;
		}
	}

	return S_OK;
}

void Player::Shift(_float fTimeDelta)
{
	if (CGameInstance::Get().Key_Down(DIK_LSHIFT))
		m_bShift = !m_bShift;

	if (!m_bShift) {

		m_fSpeedFloat = 1.f;
		_vector vMoveDir = XMVectorZero();
		_float4 fCameraPos;
		CGameInstance::Get().Get_MainCameraPosition(fCameraPos);

		_vector	vPlayerPos, vCameraPos, vCameraDir;
		vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
		vCameraPos = XMLoadFloat4(&fCameraPos);
		vCameraPos = XMVectorSetY(vCameraPos, 0.0f);
		vCameraDir = vCameraPos - vPlayerPos;

		vCameraDir = XMVector3Normalize(vCameraDir);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vRight = XMVector3Cross(vUp, vCameraDir);
		vRight = XMVector3Normalize(vRight);

		if (CGameInstance::Get().Key_Pressing(DIK_A))
		{
			vMoveDir += vRight;
		}

		if (CGameInstance::Get().Key_Pressing(DIK_D))
		{
			vMoveDir -= vRight;
		}

		if (CGameInstance::Get().Key_Pressing(DIK_W))
		{
			vMoveDir -= vCameraDir;
		}

		if (CGameInstance::Get().Key_Pressing(DIK_S))
		{
			vMoveDir += vCameraDir;
		}

		if (!XMVector3Equal(vMoveDir, XMVectorZero()))
		{
			m_pTransformCom->Move(vMoveDir, fTimeDelta);
			dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::HAND_UP_AND_WALK);
		}
		else
		{
			dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::IDLE);
		}

		MouseLook(fTimeDelta);
	}
	else if (m_bShift && (CGameInstance::Get().Key_Pressing(DIK_A) || CGameInstance::Get().Key_Pressing(DIK_S) || CGameInstance::Get().Key_Pressing(DIK_D) || CGameInstance::Get().Key_Pressing(DIK_W))) {
		KeyBoardLook(fTimeDelta);
	}
	else {
		m_bShift = false;
		m_fSpeedFloat = 1.f;
	}
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
	//BodyDesc.m_strPrototypeBaseName =L"SK_Player";
	BodyDesc.m_strPrototypeBaseName =L"SK_CharacterModel_Duck_Jeff";
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

