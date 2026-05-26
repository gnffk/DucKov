#include "Player_HAND_UP_State.h"
#include "Player_FSM.h"
#include "GameInstance.h"

Player_HAND_UP_State::Player_HAND_UP_State() {

}
Player_HAND_UP_State::~Player_HAND_UP_State() {

}

HRESULT Player_HAND_UP_State::Initialize() {
	return S_OK;
}
void Player_HAND_UP_State::Enter(FSM* pFSM) {


}
void Player_HAND_UP_State::Exit(FSM* pFSM) {

}

void Player_HAND_UP_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_HAND_UP_State::Update(FSM* pFSM, float fDeltaTime) {

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
