
#include "Boss_HIT_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"
Boss_HIT_State::Boss_HIT_State() {

}
Boss_HIT_State::~Boss_HIT_State() {

}

HRESULT Boss_HIT_State::Initialize() {
	return S_OK;
}
void Boss_HIT_State::Enter(FSM* pFSM) {

}
void Boss_HIT_State::Exit(FSM* pFSM) {

}

void Boss_HIT_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_HIT_State::Update(FSM* pFSM, float fDeltaTime) {


}

void Boss_HIT_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Boss_HIT_State> Boss_HIT_State::Create()
{
	auto	pInstance = shared_ptr<Boss_HIT_State>(new Boss_HIT_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Boss_HIT_State");
		return nullptr;
	}

	return pInstance;
}
