#include "Boss_Idle_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"

Boss_Idle_State::Boss_Idle_State() {

}
Boss_Idle_State::~Boss_Idle_State() {

}

HRESULT Boss_Idle_State::Initialize() {
	return S_OK;
}
void Boss_Idle_State::Enter(FSM* pFSM) {
	auto pBossMonsterFSM = dynamic_cast<BossMonsterFSM*>(pFSM);
	if (nullptr == pBossMonsterFSM)
		return;

	auto pBossMonsterObj = pBossMonsterFSM->Get_Monster();
	auto pBossMonster = dynamic_pointer_cast<BossMonster>(pBossMonsterObj);
	if (nullptr == pBossMonster)
		return;
	auto& weapon = pBossMonster->GetPartObjectMap()[TEXT("Part_Weapon")];


	weapon->SetVisible(false);
}
void Boss_Idle_State::Exit(FSM* pFSM) {
	auto pBossMonsterFSM = dynamic_cast<BossMonsterFSM*>(pFSM);
	if (nullptr == pBossMonsterFSM)
		return;

	auto pBossMonsterObj = pBossMonsterFSM->Get_Monster();
	auto pBossMonster = dynamic_pointer_cast<BossMonster>(pBossMonsterObj);
	if (nullptr == pBossMonster)
		return;
	auto& weapon = pBossMonster->GetPartObjectMap()[TEXT("Part_Weapon")];


	weapon->SetVisible(true);
}

void Boss_Idle_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_Idle_State::Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_Idle_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Boss_Idle_State> Boss_Idle_State::Create()
{
	auto	pInstance = shared_ptr<Boss_Idle_State>(new Boss_Idle_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Boss_Idle_State");
		return nullptr;
	}

	return pInstance;
}
