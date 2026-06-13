#include "Boss_RUN_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"
Boss_RUN_State::Boss_RUN_State() {

}
Boss_RUN_State::~Boss_RUN_State() {

}

HRESULT Boss_RUN_State::Initialize() {
	return S_OK;
}
void Boss_RUN_State::Enter(FSM* pFSM) {

}
void Boss_RUN_State::Exit(FSM* pFSM) {

}

void Boss_RUN_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_RUN_State::Update(FSM* pFSM, float fDeltaTime) {


}

void Boss_RUN_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Boss_RUN_State> Boss_RUN_State::Create()
{
	auto	pInstance = shared_ptr<Boss_RUN_State>(new Boss_RUN_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Boss_RUN_State");
		return nullptr;
	}

	return pInstance;
}
