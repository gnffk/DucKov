#include "Player_ROLL_State.h"
#include "Player_FSM.h"
#include "GameInstance.h"

Player_ROLL_State::Player_ROLL_State() {

}
Player_ROLL_State::~Player_ROLL_State() {

}

HRESULT Player_ROLL_State::Initialize() {
	return S_OK;
}
void Player_ROLL_State::Enter(FSM* pFSM) {


}
void Player_ROLL_State::Exit(FSM* pFSM) {

}

void Player_ROLL_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_ROLL_State::Update(FSM* pFSM, float fDeltaTime) {

}

void Player_ROLL_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Player_ROLL_State> Player_ROLL_State::Create()
{
	auto	pInstance = shared_ptr<Player_ROLL_State>(new Player_ROLL_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Player_ROLL_State");
		return nullptr;
	}

	return pInstance;
}
