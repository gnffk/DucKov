#include "LittleMonster_Idle_State.h"


LittleMonster_Idle_State::LittleMonster_Idle_State() {

}
LittleMonster_Idle_State::~LittleMonster_Idle_State() {

}

HRESULT LittleMonster_Idle_State::Initialize() {
	return S_OK;
}
void LittleMonster_Idle_State::Enter(FSM* pFSM) {

}
void LittleMonster_Idle_State::Exit(FSM* pFSM) {
	
}

void LittleMonster_Idle_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void LittleMonster_Idle_State::Update(FSM* pFSM, float fDeltaTime) {

}

void LittleMonster_Idle_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<LittleMonster_Idle_State> LittleMonster_Idle_State::Create()
{
	auto	pInstance = shared_ptr<LittleMonster_Idle_State>(new LittleMonster_Idle_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : LittleMonster_Idle_State");
		return nullptr;
	}

	return pInstance;
}
