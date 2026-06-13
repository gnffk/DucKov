#include "Boss_RIGHT_WALK_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"
Boss_RIGHT_WALK_State::Boss_RIGHT_WALK_State() {

}
Boss_RIGHT_WALK_State::~Boss_RIGHT_WALK_State() {

}

HRESULT Boss_RIGHT_WALK_State::Initialize() {
	return S_OK;
}
void Boss_RIGHT_WALK_State::Enter(FSM* pFSM) {

}
void Boss_RIGHT_WALK_State::Exit(FSM* pFSM) {

}

void Boss_RIGHT_WALK_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_RIGHT_WALK_State::Update(FSM* pFSM, float fDeltaTime) {


}

void Boss_RIGHT_WALK_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Boss_RIGHT_WALK_State> Boss_RIGHT_WALK_State::Create()
{
	auto	pInstance = shared_ptr<Boss_RIGHT_WALK_State>(new Boss_RIGHT_WALK_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Boss_RIGHT_WALK_State");
		return nullptr;
	}

	return pInstance;
}
