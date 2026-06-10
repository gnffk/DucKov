#include "Player_HAND_UP_State.h"
#include "Player_FSM.h"
#include "GameInstance.h"
#include "Player_Weapon.h"

Player_HAND_UP_State::Player_HAND_UP_State() {

}
Player_HAND_UP_State::~Player_HAND_UP_State() {

}

HRESULT Player_HAND_UP_State::Initialize() {
	return S_OK;
}
void Player_HAND_UP_State::Enter(FSM* pFSM) {
	auto pPlayerFSM = dynamic_cast<Player_FSM*>(pFSM);
	if (nullptr == pPlayerFSM)
		return;

	auto pPlayerObj = pPlayerFSM->Get_Player();
	auto pPlayer = dynamic_pointer_cast<Player>(pPlayerObj);
	if (nullptr == pPlayer)
		return;

	auto& weapon = pPlayer->GetPartObjectMap()[TEXT("Part_Weapon")];

	weapon->SetVisible(true);

	
}
void Player_HAND_UP_State::Exit(FSM* pFSM) {

}

void Player_HAND_UP_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_HAND_UP_State::Update(FSM* pFSM, float fDeltaTime) {
	if (CGameInstance::Get().Mouse_Pressing(MOUSEKEYSTATE::DIM_LB)) {
		auto pPlayerFSM = dynamic_cast<Player_FSM*>(pFSM);
		if (nullptr == pPlayerFSM)
			return;

		auto pPlayerObj = pPlayerFSM->Get_Player();
		auto pPlayer = dynamic_pointer_cast<Player>(pPlayerObj);
		if (nullptr == pPlayer)
			return;

		auto& weapon = pPlayer->GetPartObjectMap()[TEXT("Part_Weapon")];

		static_pointer_cast<Player_Weapon>(weapon)->Fire_Bullet();
	}
}

void Player_HAND_UP_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Player_HAND_UP_State> Player_HAND_UP_State::Create()
{
	auto	pInstance = shared_ptr<Player_HAND_UP_State>(new Player_HAND_UP_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Player_HAND_UP_State");
		return nullptr;
	}

	return pInstance;
}
