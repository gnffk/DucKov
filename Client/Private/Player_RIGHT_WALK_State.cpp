#include "Player_RIGHT_WALK_State.h"
#include "Player_FSM.h"
#include "GameInstance.h"

Player_RIGHT_WALK_State::Player_RIGHT_WALK_State() {

}
Player_RIGHT_WALK_State::~Player_RIGHT_WALK_State() {

}

HRESULT Player_RIGHT_WALK_State::Initialize() {
	return S_OK;
}
void Player_RIGHT_WALK_State::Enter(FSM* pFSM) {


}
void Player_RIGHT_WALK_State::Exit(FSM* pFSM) {

}

void Player_RIGHT_WALK_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_RIGHT_WALK_State::Update(FSM* pFSM, float fDeltaTime) {

}

void Player_RIGHT_WALK_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Player_RIGHT_WALK_State> Player_RIGHT_WALK_State::Create()
{
	auto	pInstance = shared_ptr<Player_RIGHT_WALK_State>(new Player_RIGHT_WALK_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Player_RIGHT_WALK_State");
		return nullptr;
	}

	return pInstance;
}
