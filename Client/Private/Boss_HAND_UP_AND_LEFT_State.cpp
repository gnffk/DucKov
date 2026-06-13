
#include "Boss_HAND_UP_AND_LEFT_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"
Boss_HAND_UP_AND_LEFT_State::Boss_HAND_UP_AND_LEFT_State() {

}
Boss_HAND_UP_AND_LEFT_State::~Boss_HAND_UP_AND_LEFT_State() {

}

HRESULT Boss_HAND_UP_AND_LEFT_State::Initialize() {
	return S_OK;
}
void Boss_HAND_UP_AND_LEFT_State::Enter(FSM* pFSM) {

}
void Boss_HAND_UP_AND_LEFT_State::Exit(FSM* pFSM) {

}

void Boss_HAND_UP_AND_LEFT_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_HAND_UP_AND_LEFT_State::Update(FSM* pFSM, float fDeltaTime) {


}

void Boss_HAND_UP_AND_LEFT_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Boss_HAND_UP_AND_LEFT_State> Boss_HAND_UP_AND_LEFT_State::Create()
{
	auto	pInstance = shared_ptr<Boss_HAND_UP_AND_LEFT_State>(new Boss_HAND_UP_AND_LEFT_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Boss_HAND_UP_AND_LEFT_State");
		return nullptr;
	}

	return pInstance;
}
