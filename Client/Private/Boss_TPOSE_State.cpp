
#include "Boss_TPOSE_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"
Boss_TPOSE_State::Boss_TPOSE_State() {

}
Boss_TPOSE_State::~Boss_TPOSE_State() {

}

HRESULT Boss_TPOSE_State::Initialize() {
	return S_OK;
}
void Boss_TPOSE_State::Enter(FSM* pFSM) {

}
void Boss_TPOSE_State::Exit(FSM* pFSM) {

}

void Boss_TPOSE_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_TPOSE_State::Update(FSM* pFSM, float fDeltaTime) {


}

void Boss_TPOSE_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Boss_TPOSE_State> Boss_TPOSE_State::Create()
{
	auto	pInstance = shared_ptr<Boss_TPOSE_State>(new Boss_TPOSE_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Boss_TPOSE_State");
		return nullptr;
	}

	return pInstance;
}
