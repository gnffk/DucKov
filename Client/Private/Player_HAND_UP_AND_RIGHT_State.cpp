#include "Player_HAND_UP_AND_RIGHT_State.h"

Player_HAND_UP_AND_RIGHT_State::Player_HAND_UP_AND_RIGHT_State() {

}
Player_HAND_UP_AND_RIGHT_State::~Player_HAND_UP_AND_RIGHT_State() {

}

HRESULT Player_HAND_UP_AND_RIGHT_State::Initialize() {
	return S_OK;
}
void Player_HAND_UP_AND_RIGHT_State::Enter(FSM* pFSM) {


}
void Player_HAND_UP_AND_RIGHT_State::Exit(FSM* pFSM) {

}

void Player_HAND_UP_AND_RIGHT_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_HAND_UP_AND_RIGHT_State::Update(FSM* pFSM, float fDeltaTime) {

}

void Player_HAND_UP_AND_RIGHT_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Player_HAND_UP_AND_RIGHT_State> Player_HAND_UP_AND_RIGHT_State::Create()
{
	auto	pInstance = shared_ptr<Player_HAND_UP_AND_RIGHT_State>(new Player_HAND_UP_AND_RIGHT_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Player_HAND_UP_AND_RIGHT_State");
		return nullptr;
	}

	return pInstance;
}
