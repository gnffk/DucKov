#include "Player_Walk_State.h"


Player_Walk_State::Player_Walk_State() {

}
Player_Walk_State::~Player_Walk_State() {

}

HRESULT Player_Walk_State::Initialize() {
	return S_OK;
}
void Player_Walk_State::Enter(FSM* pFSM) {


}
void Player_Walk_State::Exit(FSM* pFSM) {

}

void Player_Walk_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_Walk_State::Update(FSM* pFSM, float fDeltaTime) {

}

void Player_Walk_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Player_Walk_State> Player_Walk_State::Create()
{
	auto	pInstance = shared_ptr<Player_Walk_State>(new Player_Walk_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Player_Walk_State");
		return nullptr;
	}

	return pInstance;
}
