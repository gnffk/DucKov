
#include "Boss_Hand_UP_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"
Boss_Hand_UP_State::Boss_Hand_UP_State() {

}
Boss_Hand_UP_State::~Boss_Hand_UP_State() {

}

HRESULT Boss_Hand_UP_State::Initialize() {
	return S_OK;
}
void Boss_Hand_UP_State::Enter(FSM* pFSM) {

}
void Boss_Hand_UP_State::Exit(FSM* pFSM) {

}

void Boss_Hand_UP_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_Hand_UP_State::Update(FSM* pFSM, float fDeltaTime) {


}

void Boss_Hand_UP_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Boss_Hand_UP_State> Boss_Hand_UP_State::Create()
{
	auto	pInstance = shared_ptr<Boss_Hand_UP_State>(new Boss_Hand_UP_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Boss_Hand_UP_State");
		return nullptr;
	}

	return pInstance;
}
