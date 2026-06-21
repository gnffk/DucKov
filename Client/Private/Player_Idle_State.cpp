#include "Player_Idle_State.h"
#include "Player_FSM.h"
#include "GameInstance.h"
#include "Player_Weapon.h"
Player_Idle_State::Player_Idle_State() {

}
Player_Idle_State::~Player_Idle_State() {

}

HRESULT Player_Idle_State::Initialize() {
	return S_OK;
}
void Player_Idle_State::Enter(FSM* pFSM) {
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
void Player_Idle_State::Exit(FSM* pFSM) {
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

void Player_Idle_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_Idle_State::Update(FSM* pFSM, float fDeltaTime) {

}

void Player_Idle_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Player_Idle_State> Player_Idle_State::Create()
{
	auto	pInstance = shared_ptr<Player_Idle_State>(new Player_Idle_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Player_Idle_State");
		return nullptr;
	}

	return pInstance;
}
