
#include "Boss_HAND_UP_AND_RIGHT_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"
Boss_HAND_UP_AND_RIGHT_State::Boss_HAND_UP_AND_RIGHT_State() {

}
Boss_HAND_UP_AND_RIGHT_State::~Boss_HAND_UP_AND_RIGHT_State() {

}

HRESULT Boss_HAND_UP_AND_RIGHT_State::Initialize() {
	return S_OK;
}
void Boss_HAND_UP_AND_RIGHT_State::Enter(FSM* pFSM) {

}
void Boss_HAND_UP_AND_RIGHT_State::Exit(FSM* pFSM) {

}

void Boss_HAND_UP_AND_RIGHT_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_HAND_UP_AND_RIGHT_State::Update(FSM* pFSM, float fDeltaTime) {


}

void Boss_HAND_UP_AND_RIGHT_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Boss_HAND_UP_AND_RIGHT_State> Boss_HAND_UP_AND_RIGHT_State::Create()
{
	auto	pInstance = shared_ptr<Boss_HAND_UP_AND_RIGHT_State>(new Boss_HAND_UP_AND_RIGHT_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Boss_HAND_UP_AND_RIGHT_State");
		return nullptr;
	}

	return pInstance;
}
