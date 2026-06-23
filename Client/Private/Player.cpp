#include "Player.h"

#include "MainUI.h"
#include "InvenUI.h"
#include "Body_Player.h"
#include "BaseCollider.h"
#include "OBB_Collider.h"
#include "AABB_Collider.h"
#include "Player_FSM.h"
#include "GameInstance.h"
#include "Player_Weapon.h"
#include "Player_State_UI.h"
#include "Particle_System.h"
#include "Player_Armor.h"
#include "Player_Helmat.h"
#include "UIObject.h"
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

	if (FAILED(Ready_UI()))
		return E_FAIL;



	return S_OK;
}

void Player::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	
	for (int type = 0; type < (int)COLLIDER::COLLIDER_END; ++type)
	{
		auto& colliderList = m_pColliderComs[type];

		for (size_t i = 0; i < colliderList.size(); ++i)
		{

			CGameInstance::Get().Add_Collider(colliderList[i]->Get_GroupTag(), colliderList[i].get());

		}
	}
	m_pPlayerFSM->Priority_Update(fTimeDelta);
	
}

void Player::Update(_float fTimeDelta)
{

	if (m_bHit)
	{
		m_fHitTime += fTimeDelta;

		if (m_fHitTime >= m_fHitDuration)
		{
			m_bHit = false;
			m_fHitTime = 0.f;
		}
	}

	if (CGameInstance::Get().Key_Down(DIK_1))
	{
		Switch_WeaponSlot(1);
	}

	if (CGameInstance::Get().Key_Down(DIK_2))
	{
		Switch_WeaponSlot(2);
	}


	if (CGameInstance::Get().Key_Down(DIK_I))
	{
		m_bInventoryOpen = !m_bInventoryOpen;
		
	}

	static_pointer_cast<InvenUI>(m_pUI["InvenUI"])->InvenSet(m_bInventoryOpen);
	if (FAILED(Roll(fTimeDelta))) {
		return;
	}
	Shift(fTimeDelta);
	m_pPlayerFSM->Update(fTimeDelta);

	m_pTransformCom->Set_State(STATE::POSITION,m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));

	__super::Update(fTimeDelta);

}

void Player::Late_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Player));

	for (auto& Pair : m_pUI)
	{
		const string& strUIName = Pair.first;
		auto pUIObject = Pair.second;

		if (nullptr == pUIObject)
			continue;

		// 인벤토리가 닫혀 있을 때는 InvenUI만 숨김
		if (!m_bInventoryOpen)
		{
			if (strUIName == "InvenUI")
				continue;
		}

		// 인벤토리가 열려 있을 때는 Mouse와 InvenUI만 보임
		else
		{
			if (strUIName != "Player_Mouse" && strUIName != "InvenUI")
				continue;
		}

		CGameInstance::Get().Add_UIObject(L"Player",static_pointer_cast<UIObject>(pUIObject));
	}
	__super::Late_Update(fTimeDelta);
	Collider_Obstacle(fTimeDelta);
	Collider_Box(fTimeDelta);
	m_pPlayerFSM->Late_Update(fTimeDelta);
}

void Player::MouseLook(_float fTimeDelta)
{
	POINT pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	float width = CGameInstance::Get().Get_ViewportSize().x;
	float height = CGameInstance::Get().Get_ViewportSize().y;

	if (width <= 0.f || height <= 0.f)
		return;

	float px = (2.f * pt.x / width) - 1.f;
	float py = 1.f - (2.f * pt.y / height);

	_float4x4 view, proj;

	CGameInstance::Get().Get_MainCamerwaViewMatrix(view);
	CGameInstance::Get().Get_MainCamerwaProjectionMatrix(proj);

	XMMATRIX matView = XMLoadFloat4x4(&view);
	XMMATRIX matProj = XMLoadFloat4x4(&proj);

	XMMATRIX invView = XMMatrixInverse(nullptr, matView);
	XMMATRIX invProj = XMMatrixInverse(nullptr, matProj);

	XMVECTOR vNear = XMVectorSet(px, py, 0.f, 1.f);
	vNear = XMVector3TransformCoord(vNear, invProj);
	vNear = XMVector3TransformCoord(vNear, invView);

	XMVECTOR vFar = XMVectorSet(px, py, 1.f, 1.f);
	vFar = XMVector3TransformCoord(vFar, invProj);
	vFar = XMVector3TransformCoord(vFar, invView);

	XMVECTOR vRayOrigin = vNear;
	XMVECTOR vRayDir = XMVector3Normalize(vFar - vNear);

	float dirY = XMVectorGetY(vRayDir);
	if (fabsf(dirY) < 0.0001f)
		return;

	XMVECTOR vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);

	// 핵심:
	// 기존에는 y = 0 평면에 ray를 쐈음.
	// 이제는 Player 현재 Y 높이의 평면에 ray를 쏜다.
	float fPlayerY = XMVectorGetY(vPlayerPos);

	float t = (fPlayerY - XMVectorGetY(vRayOrigin)) / dirY;

	if (t < 0.f)
		return;

	XMVECTOR vHitPos = vRayOrigin + vRayDir * t;

	XMVECTOR vDir = vHitPos - vPlayerPos;
	vDir = XMVectorSetY(vDir, 0.f);

	if (XMVector3Equal(vDir, XMVectorZero()))
		return;

	vDir = XMVector3Normalize(vDir);

	XMVECTOR vCurrentLook = m_pTransformCom->Get_State(STATE::LOOK);
	vCurrentLook = XMVectorSetY(vCurrentLook, 0.f);

	if (XMVector3Equal(vCurrentLook, XMVectorZero()))
		return;

	vCurrentLook = XMVector3Normalize(vCurrentLook);

	XMVECTOR TurnAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	XMVECTOR cross = XMVector3Cross(vCurrentLook, vDir);
	float dot = XMVectorGetX(XMVector3Dot(vCurrentLook, vDir));

	dot = std::clamp(dot, -1.f, 1.f);

	if (dot < 0.98f)
	{
		if (XMVectorGetY(cross) < 0.f)
			m_pTransformCom->Turn(-TurnAxis, fTimeDelta * 5.f);
		else
			m_pTransformCom->Turn(TurnAxis, fTimeDelta * 5.f);
	}
	else if (dot < 0.999f)
	{
		if (XMVectorGetY(cross) < 0.f)
			m_pTransformCom->Turn(-TurnAxis, fTimeDelta);
		else
			m_pTransformCom->Turn(TurnAxis, fTimeDelta);
	}
}
void Player::KeyBoardLook(_float fTimeDelta)
{
	_vector vMoveDir = XMVectorZero();

	_float4 fCameraPos{};
	CGameInstance::Get().Get_MainCameraPosition(fCameraPos);

	_vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vCameraPos = XMLoadFloat4(&fCameraPos);

	// 이동 방향은 XZ 평면 기준
	vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);
	vCameraPos = XMVectorSetY(vCameraPos, 0.f);

	_vector vCameraDir = vCameraPos - vPlayerPos;

	if (XMVector3Equal(vCameraDir, XMVectorZero()))
		return;

	vCameraDir = XMVector3Normalize(vCameraDir);

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vRight = XMVector3Cross(vUp, vCameraDir);

	if (XMVector3Equal(vRight, XMVectorZero()))
		return;

	vRight = XMVector3Normalize(vRight);

	if (CGameInstance::Get().Key_Pressing(DIK_A))
		vMoveDir += vRight;

	if (CGameInstance::Get().Key_Pressing(DIK_D))
		vMoveDir -= vRight;

	if (CGameInstance::Get().Key_Pressing(DIK_W))
		vMoveDir -= vCameraDir;

	if (CGameInstance::Get().Key_Pressing(DIK_S))
		vMoveDir += vCameraDir;

	// 여기서 먼저 검사해야 함
	if (XMVector3Equal(vMoveDir, XMVectorZero()))
	{
		m_fSpeedFloat = 1.f;
		dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::IDLE);
		return;
	}

	// TURN 계산 전에 반드시 Normalize
	vMoveDir = XMVector3Normalize(vMoveDir);

	_vector vCurrentLook = m_pTransformCom->Get_State(STATE::LOOK);

	// 현재 Look도 XZ 평면 기준으로 맞춤
	vCurrentLook = XMVectorSetY(vCurrentLook, 0.f);

	if (XMVector3Equal(vCurrentLook, XMVectorZero()))
		vCurrentLook = vMoveDir;
	else
		vCurrentLook = XMVector3Normalize(vCurrentLook);

	_vector vTurnAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_float dot = XMVectorGetX(XMVector3Dot(vCurrentLook, vMoveDir));
	dot = std::clamp(dot, -1.f, 1.f);

	_vector cross = XMVector3Cross(vCurrentLook, vMoveDir);
	float fCrossY = XMVectorGetY(cross);

	// 거의 반대 방향이면 cross가 0에 가까워져서 회전 방향 판단이 안 됨
	// 이때는 임의로 오른쪽 방향으로 돌림
	if (dot < -0.999f)
	{
		m_pTransformCom->Turn(vTurnAxis, fTimeDelta * 1.f);
		m_fSpeedFloat = 0.5f;
	}
	else if (dot < 0.98f)
	{
		if (fCrossY < 0.f)
			m_pTransformCom->Turn(-vTurnAxis, fTimeDelta * 1.5f);
		else
			m_pTransformCom->Turn(vTurnAxis, fTimeDelta * 1.5f);

		m_fSpeedFloat -= 0.01f;
		if (m_fSpeedFloat < 0.5f)
			m_fSpeedFloat = 0.5f;
	}
	else
	{
		m_fSpeedFloat += 0.1f;
		if (m_fSpeedFloat > 2.f)
			m_fSpeedFloat = 2.f;
	}

	m_pTransformCom->Go_Direction(
		vMoveDir,
		fTimeDelta,
		m_pNavigationCom,
		m_fSpeedFloat
	);

	dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::RUN);
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

		vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);
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


		_float fX = XMVectorGetX(m_vRollDir);
		_float fZ = XMVectorGetZ(m_vRollDir);

	
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

		vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);
		vCameraPos = XMVectorSetY(vCameraPos, 0.f);

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
			m_pTransformCom->Go_Direction(
				vMoveDir,
				fTimeDelta,
				m_pNavigationCom,
				m_fSpeedFloat
			);

			dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::HAND_UP_AND_WALK);
				
		}
		else
		{
			if (CGameInstance::Get().Mouse_Pressing(MOUSEKEYSTATE::DIM_RB))
				dynamic_pointer_cast<Player_FSM>(m_pPlayerFSM)->Change_State(Player_FSM::HAND_UP);
			else
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
#ifdef _DEBUG
	m_pNavigationCom->Render();
	IMGUI_DEBUGRENDER();
#endif
	return S_OK;
}

#ifdef _DEBUG
void Player::IMGUI_DEBUGRENDER()
{
	//-----------------------------------------------Collider-------------------------------------------------
	ImGui::Begin("Collider");

	static int selectedCollider = -1;

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 10.f));

	ImGui::SeparatorText("Create Collider");

	auto CreateUniqueColliderName = [&](const wstring& baseName)
		{
			wstring finalName = baseName;

			int index = 1;

			while (true)
			{
				auto iter =
					m_Components.find(finalName);

				if (iter == m_Components.end())
					break;

				finalName =
					baseName + L"_" + to_wstring(index);

				++index;
			}

			return finalName;
		};

	if (ImGui::Selectable("AABB Collider", selectedCollider == 0))
	{
		selectedCollider = 0;

		auto pAABBCom =
			dynamic_pointer_cast<BaseCollider>(
				CGameInstance::Get().Clone_Prototype(
					CGameInstance::Get().Get_Level(),
					TEXT("Prototype_Com_AABB_Collider")));
		pAABBCom->Set_Tag(COLLIDER::COLLIDER_AABB);
		wstring componentName =
			CreateUniqueColliderName(L"Com_AABBCollider");


		__super::Add_Component(componentName, pAABBCom);

		pAABBCom->SetOwner(SHARED_THIS(Player).get());
		pAABBCom->Set_GroupTag(L"Player");
		m_pColliderComs[(int)COLLIDER::COLLIDER_AABB].push_back(pAABBCom);
	}

	if (ImGui::Selectable("OBB Collider", selectedCollider == 1))
	{
		selectedCollider = 1;

		auto pOBBCom =
			dynamic_pointer_cast<BaseCollider>(
				CGameInstance::Get().Clone_Prototype(
					CGameInstance::Get().Get_Level(),
					TEXT("Prototype_Com_OBB_Collider")));

		pOBBCom->Set_Tag(COLLIDER::COLLIDER_OBB);
		pOBBCom->Set_GroupTag(L"Player");
		wstring componentName =
			CreateUniqueColliderName(L"Com_OBBCollider");

		__super::Add_Component(
			componentName,
			pOBBCom);

		pOBBCom->SetOwner(
			SHARED_THIS(Player).get());

		m_pColliderComs[(int)COLLIDER::COLLIDER_OBB]
			.push_back(pOBBCom);
	}


	if (ImGui::Selectable("Sphere Collider", selectedCollider == 2))
	{
		selectedCollider = 2;

		auto pSphereCom =
			dynamic_pointer_cast<BaseCollider>(
				CGameInstance::Get().Clone_Prototype(
					CGameInstance::Get().Get_Level(),
					TEXT("Prototype_Com_Sphere_Collider")));
		pSphereCom->Set_Tag(COLLIDER::COLLIDER_SPHERE);
		wstring componentName =
			CreateUniqueColliderName(L"Com_Sphere_Collider");


		__super::Add_Component(
			componentName,
			pSphereCom);

		pSphereCom->SetOwner(
			SHARED_THIS(Player).get());
		pSphereCom->Set_GroupTag(L"Player");
		m_pColliderComs[(int)COLLIDER::COLLIDER_SPHERE]
			.push_back(pSphereCom);
	}

	ImGui::Spacing();
	ImGui::Separator();

	ImGui::TextDisabled("Click a collider type to add it.");


	// =====================================================
// Collider Settings
// =====================================================

	ImGui::Spacing();
	ImGui::SeparatorText("Collider Settings");
	for (int type = 0; type < (int)COLLIDER::COLLIDER_END; ++type)
	{
		auto& colliderList = m_pColliderComs[type];

		for (size_t i = 0; i < colliderList.size(); ++i)
		{
			ImGui::PushID(type * 1000 + (int)i);

			auto& collider = colliderList[i];

			if (collider == nullptr)
			{
				ImGui::PopID();
				continue;
			}

			string colliderTypeName;

			switch ((COLLIDER)type)
			{
			case COLLIDER::COLLIDER_AABB:
				colliderTypeName = "AABB";
				break;

			case COLLIDER::COLLIDER_OBB:
				colliderTypeName = "OBB";
				break;

			case COLLIDER::COLLIDER_SPHERE:
				colliderTypeName = "SPHERE";
				break;
			}

			string headerName =
				colliderTypeName + " Collider " + std::to_string(i);

			if (ImGui::CollapsingHeader(headerName.c_str()))
			{
				collider->GUI_ColliderExtend();
			}

			ImGui::PopID();
		}
	}
	ImGui::PopStyleVar(2);

	ImGui::End();

	// =====================================================
  // Navigation Change Debug GUI
  // =====================================================
	ImGui::Begin("Player Navigation");

	static _float3 vBossWarpPos = { 438.f, 37.f, 220 };
	static int iBossStartCell = 1;

	ImGui::DragFloat3(
		"Boss Warp Pos",
		reinterpret_cast<float*>(&vBossWarpPos),
		0.1f
	);

	ImGui::InputInt("Boss Start Cell", &iBossStartCell);

	if (iBossStartCell < 0)
		iBossStartCell = 0;

	if (ImGui::Button("Change To Boss Navigation + Warp", ImVec2(300.f, 40.f)))
	{
		Change_Navigation_AndWarp(L"Prototype_Component_Navigation_Boss",vBossWarpPos,static_cast<uint32_t>(iBossStartCell));
	}

	if (ImGui::Button("Change To Normal Navigation", ImVec2(300.f, 40.f)))
	{
		Change_Navigation(
			L"Prototype_Component_Navigation",
			1
		);
	}

	ImGui::End();
}
#endif
HRESULT Player::Ready_Components()
{
	auto m_pCollider = dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_OBB_Collider")));
	m_pCollider->SetOwner(SHARED_THIS(Player).get());
	if (FAILED(__super::Add_Component(TEXT("Com_OBBCollider"), m_pCollider)))
		return E_FAIL;
	m_pCollider->Set_GroupTag(L"Player");
	m_pCollider->Set_Center(_float3{0.f, 0.6f, 0.f});
	m_pCollider->Set_Extend(_float3{0.3f, 0.6f, 0.3f});

	
	m_pColliderComs[(int)COLLIDER::COLLIDER_OBB].push_back(m_pCollider);

	Navigation::NAVIGATION_DESC		NaviDesc{ 1 };

	m_pNavigationCom = dynamic_pointer_cast<Navigation>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Navigation"), &NaviDesc));
	if (FAILED(__super::Add_Component(TEXT("Com_Navigation"), m_pNavigationCom)))
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
	BodyDesc.m_strPrototypeBaseName =L"SK_CustomBody";
	BodyDesc.pCameraType = ETOUI(CAMERA::NONE);
	BodyDesc.fSpeedPerSec = 5.f;
	BodyDesc.fRotationPerSec = 180.f;

	if (FAILED(__super::Add_PartObject(CGameInstance::Get().Get_Level(), BodyDesc.m_strPrototypeObjectName,
		TEXT("Part_Body"), &BodyDesc)))
		return E_FAIL;



	Player_Weapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pParentState = &m_iState;
	WeaponDesc.pSocketMatrix = dynamic_pointer_cast<Body_Player>(m_PartObjects[TEXT("Part_Body")])->Get_SocketMatrixPtr("MeleeWeaponSocket");
	WeaponDesc.ObjectType = ETOUI(OBJECTTYPE::OBEJCT_PART);
	WeaponDesc.m_strName = L"ObjectPart";
	WeaponDesc.pParentState = &m_iState;
	WeaponDesc.m_strPrototypeObjectName = L"";
	WeaponDesc.m_strPrototypeBaseName = L"SK_CustomBody";
	WeaponDesc.pCameraType = ETOUI(CAMERA::NONE);
	WeaponDesc.fSpeedPerSec = 5.f;
	WeaponDesc.fRotationPerSec = 1.f;

	if (FAILED(__super::Add_PartObject(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Weapon"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;



	Player_Armor::ARMOR_DESC		ArmorDesc{};
	ArmorDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	ArmorDesc.pParentState = &m_iState;
	ArmorDesc.pSocketMatrix = dynamic_pointer_cast<Body_Player>(m_PartObjects[TEXT("Part_Body")])->Get_SocketMatrixPtr("ArmorSocket");
	ArmorDesc.ObjectType = ETOUI(OBJECTTYPE::OBEJCT_PART);
	ArmorDesc.m_strName = L"ObjectPart";
	ArmorDesc.pParentState = &m_iState;
	ArmorDesc.m_strPrototypeObjectName = L"";
	ArmorDesc.m_strPrototypeBaseName = L"SK_CustomBody";
	ArmorDesc.pCameraType = ETOUI(CAMERA::NONE);
	ArmorDesc.fSpeedPerSec = 5.f;
	ArmorDesc.fRotationPerSec = 1.f;

	if (FAILED(__super::Add_PartObject(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Armor"),
		TEXT("Part_Armor"), &ArmorDesc)))
		return E_FAIL;

	Player_Helmat::HELMAT_DESC		HelmatDesc{};
	HelmatDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HelmatDesc.pParentState = &m_iState;
	HelmatDesc.pSocketMatrix = dynamic_pointer_cast<Body_Player>(m_PartObjects[TEXT("Part_Body")])->Get_SocketMatrixPtr("HelmatSocket");
	HelmatDesc.ObjectType = ETOUI(OBJECTTYPE::OBEJCT_PART);
	HelmatDesc.m_strName = L"ObjectPart";
	HelmatDesc.pParentState = &m_iState;
	HelmatDesc.m_strPrototypeObjectName = L"";
	HelmatDesc.m_strPrototypeBaseName = L"SK_CustomBody";
	HelmatDesc.pCameraType = ETOUI(CAMERA::NONE);
	HelmatDesc.fSpeedPerSec = 5.f;
	HelmatDesc.fRotationPerSec = 1.f;

	if (FAILED(__super::Add_PartObject(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Helmat"),
		TEXT("Part_Helmat"), &HelmatDesc)))
		return E_FAIL;




	return S_OK;
}

HRESULT Player::Ready_UI()
{
	_vector vPos = XMVectorSet(455.f, 33.f, 104.f, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);

	m_pPlayerFSM->SetOwner(SHARED_THIS(Player));

	if (nullptr != m_pNavigationCom)
	{
		m_pNavigationCom->Set_CurrentCell(m_pTransformCom->Get_State(STATE::POSITION));

		m_pTransformCom->Set_State(STATE::POSITION, m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));
	}


	// UI는 오로지 UI_Manager안에서 관리하기 위해 Layer에 넣지 말기
	UIObject::UIOBJECT_DESC DesUI{};
	DesUI.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
	DesUI.m_strName = L"MainUI";
	DesUI.m_strPrototypeObjectName = L"Prototype_GameObject_MainUI";
	DesUI.m_strPrototypeBaseName = L"MainUI";
	DesUI.pCameraType = ETOUI(CAMERA::NONE);
	DesUI.fSpeedPerSec = 5.f;
	DesUI.fRotationPerSec = 1.f;
	DesUI.fSizeX = 1.f;
	DesUI.fSizeY = 1.f;
	DesUI.fX = 1.f;
	DesUI.fY = 1.f;	

	m_pUI.emplace("MainUI", dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_MainUI"), &DesUI)));
	// 추가
	static_pointer_cast<MainUI>(m_pUI["MainUI"])->Set_HP(m_fHP, m_fMaxHP);

	UIObject::UIOBJECT_DESC DesPlayerMouseUI{};
	DesPlayerMouseUI.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
	DesPlayerMouseUI.m_strName = L"Player_Mouse";
	DesPlayerMouseUI.m_strPrototypeObjectName = L"Prototype_GameObject_Player_Mouse";
	DesPlayerMouseUI.m_strPrototypeBaseName = L"Player_Mouse";
	DesPlayerMouseUI.pCameraType = ETOUI(CAMERA::NONE);
	DesPlayerMouseUI.fSpeedPerSec = 5.f;
	DesPlayerMouseUI.fRotationPerSec = 1.f;
	DesPlayerMouseUI.fSizeX = 1.f;
	DesPlayerMouseUI.fSizeY = 1.f;
	DesPlayerMouseUI.fX = 1.f;
	DesPlayerMouseUI.fY = 1.f;

	m_pUI.emplace("Player_Mouse", dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_Mouse"), &DesPlayerMouseUI)));

	UIObject::UIOBJECT_DESC DesPlayerInven{};
	DesPlayerInven.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
	DesPlayerInven.m_strName = L"InvenUI";
	DesPlayerInven.m_strPrototypeObjectName = L"Prototype_GameObject_InvenUI";
	DesPlayerInven.m_strPrototypeBaseName = L"InvenUI";
	DesPlayerInven.pCameraType = ETOUI(CAMERA::NONE);
	DesPlayerInven.fSpeedPerSec = 5.f;
	DesPlayerInven.fRotationPerSec = 1.f;
	DesPlayerInven.fSizeX = 1.f;
	DesPlayerInven.fSizeY = 1.f;
	DesPlayerInven.fX = 1.f;
	DesPlayerInven.fY = 1.f;

	m_pUI.emplace("InvenUI", dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_InvenUI"), &DesPlayerInven)));

	UIObject::UIOBJECT_DESC DesPlayerStateUI{};
	DesPlayerStateUI.ObjectType = ETOUI(OBJECTTYPE::OBJECT_UI);
	DesPlayerStateUI.m_strName = L"PlayerStateUI";
	DesPlayerStateUI.m_strPrototypeObjectName = L"Prototype_GameObject_Player_State_UI";
	DesPlayerStateUI.m_strPrototypeBaseName = L"PlayerStateUI";
	DesPlayerStateUI.pCameraType = ETOUI(CAMERA::NONE);
	DesPlayerStateUI.fSpeedPerSec = 5.f;
	DesPlayerStateUI.fRotationPerSec = 1.f;
	DesPlayerStateUI.fSizeX = 1.f;
	DesPlayerStateUI.fSizeY = 1.f;
	DesPlayerStateUI.fX = 1.f;
	DesPlayerStateUI.fY = 1.f;

	m_pUI.emplace("PlayerStateUI", dynamic_pointer_cast<GameObject>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_Player_State_UI"), &DesPlayerStateUI)));
	static_pointer_cast<Player_State_UI>(m_pUI["PlayerStateUI"])->Set_HP(m_fHP, m_fMaxHP);


	return S_OK;
}

_bool Player::Collider_Obstacle(_float fTimeDelta)
{

	auto ColliderGroup = CGameInstance::Get().GetColliderGroups(L"Obstacle");
	auto ColliderPlayer= CGameInstance::Get().GetColliderGroups(L"Player");
	if (ColliderGroup != nullptr && ColliderPlayer != nullptr) {
		for (auto _PlayerCollider : *ColliderPlayer) {
			for (auto Collider : *ColliderGroup)
			{
				if (CGameInstance::Get().Intersect(_PlayerCollider, Collider))
				{

					Collider->SetColliderColor(ColliderColor::RED);

					_PlayerCollider->SetColliderColor(ColliderColor::RED);
				}


			}
		}
	
	}

	return _bool();
}
_bool Player::Collider_Box(_float fTimeDelta)
{
	auto ColliderGroup = CGameInstance::Get().GetColliderGroups(L"InteractBox");
	auto ColliderPlayer = CGameInstance::Get().GetColliderGroups(L"Player");

	_bool bHitInteractBox = false;

	if (ColliderGroup != nullptr && ColliderPlayer != nullptr)
	{
		for (auto _PlayerCollider : *ColliderPlayer)
		{
			for (auto Collider : *ColliderGroup)
			{
				if (CGameInstance::Get().Intersect(_PlayerCollider, Collider))
				{
					bHitInteractBox = true;

					Collider->SetColliderColor(ColliderColor::RED);
					_PlayerCollider->SetColliderColor(ColliderColor::RED);
				}
			}
		}
	}


	if (m_bPrevHitInteractBox == true && bHitInteractBox == false)
	{
		InvenSet(false);
	}

	m_bPrevHitInteractBox = bHitInteractBox;

	return bHitInteractBox;
}
_bool Player::Collider_HitAttack(_float fTimeDelta)
{
	auto ColliderGroup = CGameInstance::Get().GetColliderGroups(L"HitAttack_for_Player");
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
					XMStoreFloat3(&vHitPos, pBulletOwner->GetTransform()->Get_State(STATE::POSITION));
				}
				else
				{
					XMStoreFloat3(&vHitPos, m_pTransformCom->Get_State(STATE::POSITION));

					vHitPos.y += 1.0f;
				}

				Take_Damage(10.f, vHitPos);

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

void Player::Take_Damage(_float fDamage, const _float3& vHitPos)
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

	m_bHit = true;
	m_fHitTime = 0.f;

	if (m_fHP <= 0.f)
	{
	
		//Set_Dead();
	}
}


void Player::Spawn_BloodEffect(const _float3& vSpawnPos)
{
	Particle_System::PARTICLE_SPAWN_DESC Desc{};
	Desc.vSpawnPos = vSpawnPos;
	Desc.iCount = 20;
	Desc.fPower = 1.f;

	CGameInstance::Get().Add_Particle(PARTICLE_TYPE::BLOOD, &Desc);
}

void Player::Update_HP_UI()
{
	auto iterState = m_pUI.find("PlayerStateUI");

	if (iterState != m_pUI.end())
	{
		auto pStateUI = dynamic_pointer_cast<Player_State_UI>(iterState->second);

		if (pStateUI != nullptr)
			pStateUI->Set_HP(m_fHP, m_fMaxHP);
	}

	// 2. 메인 화면 HP UI
	auto iterMain = m_pUI.find("MainUI");

	if (iterMain != m_pUI.end())
	{
		auto pMainUI = dynamic_pointer_cast<MainUI>(iterMain->second);

		if (pMainUI != nullptr)
			pMainUI->Set_HP(m_fHP, m_fMaxHP);
	}
}
void Player::Equip_Item(Engine::UIObject::SLOT_KIND eSlotKind, const Engine::UIObject::INV_ITEM& Item)
{
	if (eSlotKind == Engine::UIObject::SLOT_KIND::CLOTHES)
	{
		auto iter = m_PartObjects.find(TEXT("Part_Armor"));

		if (iter == m_PartObjects.end())
			return;

		auto pArmor = dynamic_pointer_cast<Player_Armor>(iter->second);

		if (pArmor == nullptr)
			return;

		pArmor->Set_ArmorType(Item.strEquipModelKey);
	}

	if (eSlotKind == Engine::UIObject::SLOT_KIND::HEAD)
	{
		auto iter = m_PartObjects.find(TEXT("Part_Helmat"));

		if (iter == m_PartObjects.end())
			return;

		auto pArmor = dynamic_pointer_cast<Player_Helmat>(iter->second);

		if (pArmor == nullptr)
			return;

		pArmor->Set_HelmatType(Item.strEquipModelKey);
	}

	if (eSlotKind == Engine::UIObject::SLOT_KIND::GUN)
	{
		auto iter = m_PartObjects.find(TEXT("Part_Weapon"));

		if (iter == m_PartObjects.end())
			return;

		auto pWeapon = dynamic_pointer_cast<Player_Weapon>(iter->second);

		if (pWeapon == nullptr)
			return;

		pWeapon->Set_WeaponType(Item.strEquipModelKey);
	}
}

void Player::Unequip_Item(Engine::UIObject::SLOT_KIND eSlotKind, const Engine::UIObject::INV_ITEM& Item)
{
	if (eSlotKind == Engine::UIObject::SLOT_KIND::CLOTHES)
	{
		auto iter = m_PartObjects.find(TEXT("Part_Armor"));

		if (iter == m_PartObjects.end())
			return;

		auto pArmor = dynamic_pointer_cast<Player_Armor>(iter->second);

		if (pArmor == nullptr)
			return;

		pArmor->Set_ArmorType("Default");
	}
	if (eSlotKind == Engine::UIObject::SLOT_KIND::HEAD)
	{
		auto iter = m_PartObjects.find(TEXT("Part_Helmat"));

		if (iter == m_PartObjects.end())
			return;

		auto pArmor = dynamic_pointer_cast<Player_Helmat>(iter->second);

		if (pArmor == nullptr)
			return;

		pArmor->Set_HelmatType("Default");
	}
	if (eSlotKind == Engine::UIObject::SLOT_KIND::GUN)
	{
		auto iter = m_PartObjects.find(TEXT("Part_Weapon"));

		if (iter == m_PartObjects.end())
			return;

		auto pWeapon = dynamic_pointer_cast<Player_Weapon>(iter->second);

		if (pWeapon == nullptr)
			return;

		pWeapon->Set_WeaponType("Default");
	}
}

void Player::Set_WeaponSlot(int iSlotNumber, const Engine::UIObject::INV_ITEM& Item)
{
	// iSlotNumber는 1 또는 2
	if (iSlotNumber < 1 || iSlotNumber > 2)
		return;

	int iIndex = iSlotNumber - 1;

	m_strWeaponSlotKey[iIndex] = Item.strEquipModelKey;

	// 현재 들고 있는 슬롯에 새 무기를 넣은 경우 바로 갱신
	if (m_iCurrentWeaponSlot == iIndex)
	{
		Switch_WeaponSlot(iSlotNumber);
	}

	// 아직 아무 무기도 안 들고 있으면 첫 장착 무기를 바로 들게 함
	if (m_iCurrentWeaponSlot == -1)
	{
		Switch_WeaponSlot(iSlotNumber);
	}
}

void Player::Clear_WeaponSlot(int iSlotNumber)
{
	if (iSlotNumber < 1 || iSlotNumber > 2)
		return;

	int iIndex = iSlotNumber - 1;

	m_strWeaponSlotKey[iIndex] = "Default";

	if (m_iCurrentWeaponSlot == iIndex)
	{
		auto iter = m_PartObjects.find(TEXT("Part_Weapon"));

		if (iter == m_PartObjects.end())
			return;

		auto pWeapon = dynamic_pointer_cast<Player_Weapon>(iter->second);

		if (pWeapon == nullptr)
			return;

		pWeapon->Set_WeaponType("Default");

		m_iCurrentWeaponSlot = -1;
	}
}

void Player::Switch_WeaponSlot(int iSlotNumber)
{
	if (iSlotNumber < 1 || iSlotNumber > 2)
		return;

	int iIndex = iSlotNumber - 1;

	const string& strWeaponKey = m_strWeaponSlotKey[iIndex];

	auto iter = m_PartObjects.find(TEXT("Part_Weapon"));

	if (iter == m_PartObjects.end())
		return;

	auto pWeapon = dynamic_pointer_cast<Player_Weapon>(iter->second);

	if (pWeapon == nullptr)
		return;

	// 빈 슬롯이면 손에 든 무기 제거
	if (strWeaponKey == "Default")
	{
		pWeapon->Set_WeaponType("Default");
		m_iCurrentWeaponSlot = -1;
		return;
	}

	pWeapon->Set_WeaponType(strWeaponKey);
	m_iCurrentWeaponSlot = iIndex;
}

HRESULT Player::Change_Navigation(const wstring& strNavigationPrototypeTag, uint32_t iStartCellIndex)
{
	Navigation::NAVIGATION_DESC NaviDesc{ iStartCellIndex };

	auto pNewNavigation =dynamic_pointer_cast<Navigation>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),strNavigationPrototypeTag.c_str(),&NaviDesc));

	if (nullptr == pNewNavigation)
		return E_FAIL;

	// 기존 m_pNavigationCom을 새 Navigation으로 교체
	m_pNavigationCom = pNewNavigation;

	// 이미 "Com_Navigation"이라는 컴포넌트 키가 있으므로 Add_Component 다시 하면 실패할 수 있음.
	// 따라서 map에 직접 덮어쓰기.
	m_Components[TEXT("Com_Navigation")] = m_pNavigationCom;

	m_strCurrentNavigationTag = strNavigationPrototypeTag;

	// 현재 플레이어 위치 기준으로 새 Navigation의 Cell 다시 설정
	if (nullptr != m_pTransformCom)
	{
		_vector vCurPos = m_pTransformCom->Get_State(STATE::POSITION);

		m_pNavigationCom->Set_CurrentCell(vCurPos);

		m_pTransformCom->Set_State(STATE::POSITION,m_pNavigationCom->SetUp_OnNavigation(vCurPos));
	}

	GameObject::GAMEOBJECT_DESC descBossCutCamera{};

	descBossCutCamera.ObjectType = ETOUI(OBJECTTYPE::OBJECT_CAMERA);
	descBossCutCamera.m_strName = L"Boss_CutScene_Camera";
	descBossCutCamera.m_strPrototypeObjectName = L"Prototype_GameObject_PlayerCamera";
	descBossCutCamera.m_strPrototypeBaseName = L"Prototype_GameObject_PlayerCamera";
	descBossCutCamera.pCameraType = ETOUI(CAMERA::SUB2);
	descBossCutCamera.fSpeedPerSec = 10.f;
	descBossCutCamera.fRotationPerSec = 0.1f;

	CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(),TEXT("Prototype_GameObject_PlayerCamera"),CGameInstance::Get().Get_Level(),L"CameraTag",&descBossCutCamera);

	CGameInstance::Get().Add_Camera(
		ETOUI(CAMERA::SUB2),
		dynamic_pointer_cast<Camera>(
			CGameInstance::Get().Find_Object(
				CGameInstance::Get().Get_Level(),
				L"CameraTag",
				L"Boss_CutScene_Camera"
			)
		)
	);

	return S_OK;
}

HRESULT Player::Change_Navigation_AndWarp(const wstring& strNavigationPrototypeTag,const _float3& vWarpPos,uint32_t iStartCellIndex)
{
	Navigation::NAVIGATION_DESC NaviDesc{ iStartCellIndex };

	auto pNewNavigation = dynamic_pointer_cast<Navigation>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),strNavigationPrototypeTag.c_str(),&NaviDesc));

	if (pNewNavigation == nullptr)
		return E_FAIL;

	m_pNavigationCom = pNewNavigation;

	// 기존 Navigation 컴포넌트 교체
	m_Components[TEXT("Com_Navigation")] = m_pNavigationCom;

	if (m_pTransformCom != nullptr)
	{
		_vector vNewPos = XMVectorSet(vWarpPos.x,vWarpPos.y,vWarpPos.z,1.f);

		// 먼저 보스 근처 위치로 이동
		m_pTransformCom->Set_State(STATE::POSITION,vNewPos);

		// 새 Navigation 기준으로 현재 Cell 다시 잡기
		m_pNavigationCom->Set_CurrentCell(vNewPos);

		// 바닥 높이 보정
		_vector vFixedPos =m_pNavigationCom->SetUp_OnNavigation(vNewPos);

		m_pTransformCom->Set_State(STATE::POSITION,vFixedPos);
	}

	GameObject::GAMEOBJECT_DESC descBossCutCamera{};

	descBossCutCamera.ObjectType = ETOUI(OBJECTTYPE::OBJECT_CAMERA);
	descBossCutCamera.m_strName = L"Boss_CutScene_Camera";
	descBossCutCamera.m_strPrototypeObjectName = L"Prototype_GameObject_BossPerformanceCamera";
	descBossCutCamera.m_strPrototypeBaseName = L"Prototype_GameObject_BossPerformanceCamera";
	descBossCutCamera.pCameraType = ETOUI(CAMERA::SUB2);
	descBossCutCamera.fSpeedPerSec = 10.f;
	descBossCutCamera.fRotationPerSec = 0.1f;

	CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(), TEXT("Prototype_GameObject_BossPerformanceCamera"), CGameInstance::Get().Get_Level(), L"CameraTag", &descBossCutCamera);

	CGameInstance::Get().Add_Camera(
		ETOUI(CAMERA::SUB2),
		dynamic_pointer_cast<Camera>(
			CGameInstance::Get().Find_Object(
				CGameInstance::Get().Get_Level(),
				L"CameraTag",
				L"Boss_CutScene_Camera"
			)
		)
	);

	return S_OK;
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

