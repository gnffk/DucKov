#include "Player_RUN_State.h"
#include "Player_FSM.h"
#include "GameInstance.h"

Player_RUN_State::Player_RUN_State() {

}
Player_RUN_State::~Player_RUN_State() {

}

HRESULT Player_RUN_State::Initialize() {
	return S_OK;
}
void Player_RUN_State::Enter(FSM* pFSM) {

	CGameInstance::Get().PlaySoundLoop(L"EFFECT_Player_Walk", EFFECT_WALKPlayer, 1.f);
	
}
void Player_RUN_State::Exit(FSM* pFSM) {
	CGameInstance::Get().StopSound(EFFECT_WALKPlayer);
}

void Player_RUN_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_RUN_State::Update(FSM* pFSM, float fDeltaTime) {

}

void Player_RUN_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Player_RUN_State> Player_RUN_State::Create()
{
	auto	pInstance = shared_ptr<Player_RUN_State>(new Player_RUN_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Player_RUN_State");
		return nullptr;
	}

	return pInstance;
}
