#include "LittleMonster_WALK_State.h"

LittleMonster_WALK_State::LittleMonster_WALK_State() {

}
LittleMonster_WALK_State::~LittleMonster_WALK_State() {

}

HRESULT LittleMonster_WALK_State::Initialize() {
	return S_OK;
}
void LittleMonster_WALK_State::Enter(FSM* pFSM) {

}
void LittleMonster_WALK_State::Exit(FSM* pFSM) {

}

void LittleMonster_WALK_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void LittleMonster_WALK_State::Update(FSM* pFSM, float fDeltaTime) {


}

void LittleMonster_WALK_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<LittleMonster_WALK_State> LittleMonster_WALK_State::Create()
{
	auto	pInstance = shared_ptr<LittleMonster_WALK_State>(new LittleMonster_WALK_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : LittleMonster_WALK_State");
		return nullptr;
	}

	return pInstance;
}
