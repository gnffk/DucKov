#include "LittleMonster_RUN_State.h"

LittleMonster_RUN_State::LittleMonster_RUN_State() {

}
LittleMonster_RUN_State::~LittleMonster_RUN_State() {

}

HRESULT LittleMonster_RUN_State::Initialize() {
	return S_OK;
}
void LittleMonster_RUN_State::Enter(FSM* pFSM) {

}
void LittleMonster_RUN_State::Exit(FSM* pFSM) {

}

void LittleMonster_RUN_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void LittleMonster_RUN_State::Update(FSM* pFSM, float fDeltaTime) {


}

void LittleMonster_RUN_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<LittleMonster_RUN_State> LittleMonster_RUN_State::Create()
{
	auto	pInstance = shared_ptr<LittleMonster_RUN_State>(new LittleMonster_RUN_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : LittleMonster_RUN_State");
		return nullptr;
	}

	return pInstance;
}
