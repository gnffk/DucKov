#include "Player_LEFT_WALK_State.h"
#include "Player_FSM.h"
#include "GameInstance.h"

Player_LEFT_WALK_State::Player_LEFT_WALK_State() {

}
Player_LEFT_WALK_State::~Player_LEFT_WALK_State() {

}

HRESULT Player_LEFT_WALK_State::Initialize() {
	return S_OK;
}
void Player_LEFT_WALK_State::Enter(FSM* pFSM) {


}
void Player_LEFT_WALK_State::Exit(FSM* pFSM) {

}

void Player_LEFT_WALK_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_LEFT_WALK_State::Update(FSM* pFSM, float fDeltaTime) {

}

void Player_LEFT_WALK_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Player_LEFT_WALK_State> Player_LEFT_WALK_State::Create()
{
	auto	pInstance = shared_ptr<Player_LEFT_WALK_State>(new Player_LEFT_WALK_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Player_LEFT_WALK_State");
		return nullptr;
	}

	return pInstance;
}
