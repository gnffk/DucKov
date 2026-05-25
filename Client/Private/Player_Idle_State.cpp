#include "Player_Idle_State.h"


Player_Idle_State::Player_Idle_State() {

}
Player_Idle_State::~Player_Idle_State() {

}

HRESULT Player_Idle_State::Initialize() {
	return S_OK;
}
void Player_Idle_State::Enter(FSM* pFSM) {


}
void Player_Idle_State::Exit(FSM* pFSM) {

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
