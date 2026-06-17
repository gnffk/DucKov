
#include "LittleMonster_Hand_UP_State.h"

LittleMonster_HAND_UP_State::LittleMonster_HAND_UP_State() {

}
LittleMonster_HAND_UP_State::~LittleMonster_HAND_UP_State() {

}

HRESULT LittleMonster_HAND_UP_State::Initialize() {
	return S_OK;
}
void LittleMonster_HAND_UP_State::Enter(FSM* pFSM) {

}
void LittleMonster_HAND_UP_State::Exit(FSM* pFSM) {

}

void LittleMonster_HAND_UP_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void LittleMonster_HAND_UP_State::Update(FSM* pFSM, float fDeltaTime) {


}

void LittleMonster_HAND_UP_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<LittleMonster_HAND_UP_State> LittleMonster_HAND_UP_State::Create()
{
	auto	pInstance = shared_ptr<LittleMonster_HAND_UP_State>(new LittleMonster_HAND_UP_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : LittleMonster_HAND_UP_State");
		return nullptr;
	}

	return pInstance;
}
