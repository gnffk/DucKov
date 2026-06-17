
#include "LittleMonster_HAND_UP_AND_RIGHT_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"
LittleMonster_HAND_UP_AND_RIGHT_State::LittleMonster_HAND_UP_AND_RIGHT_State() {

}
LittleMonster_HAND_UP_AND_RIGHT_State::~LittleMonster_HAND_UP_AND_RIGHT_State() {

}

HRESULT LittleMonster_HAND_UP_AND_RIGHT_State::Initialize() {
	return S_OK;
}
void LittleMonster_HAND_UP_AND_RIGHT_State::Enter(FSM* pFSM) {

}
void LittleMonster_HAND_UP_AND_RIGHT_State::Exit(FSM* pFSM) {

}

void LittleMonster_HAND_UP_AND_RIGHT_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void LittleMonster_HAND_UP_AND_RIGHT_State::Update(FSM* pFSM, float fDeltaTime) {


}

void LittleMonster_HAND_UP_AND_RIGHT_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<LittleMonster_HAND_UP_AND_RIGHT_State> LittleMonster_HAND_UP_AND_RIGHT_State::Create()
{
	auto	pInstance = shared_ptr<LittleMonster_HAND_UP_AND_RIGHT_State>(new LittleMonster_HAND_UP_AND_RIGHT_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : LittleMonster_HAND_UP_AND_RIGHT_State");
		return nullptr;
	}

	return pInstance;
}
