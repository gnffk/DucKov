#include "LittleMonster_Attack_State.h"

#include "LittleMonsterFSM.h"
#include "GameInstance.h"
#include "LittleMonster_Weapon.h"

LittleMonster_Attack_State::LittleMonster_Attack_State() {

}
LittleMonster_Attack_State::~LittleMonster_Attack_State() {

}

HRESULT LittleMonster_Attack_State::Initialize() {
	return S_OK;
}
void LittleMonster_Attack_State::Enter(FSM* pFSM) {
	auto pLittleMonsterFSM = dynamic_cast<LittleMonsterFSM*>(pFSM);
	if (nullptr == pLittleMonsterFSM)
		return;

	auto pMonsterObj = pLittleMonsterFSM->Get_Monster();
	auto pMonster = dynamic_pointer_cast<LittleMonster>(pMonsterObj);
	if (nullptr == pMonster)
		return;

	auto& weapon = pMonster->GetPartObjectMap()[TEXT("Part_Weapon")];

	weapon->SetVisible(true);
}
void LittleMonster_Attack_State::Exit(FSM* pFSM) {
	auto pLittleMonsterFSM = dynamic_cast<LittleMonsterFSM*>(pFSM);
	if (nullptr == pLittleMonsterFSM)
		return;

	auto pMonsterObj = pLittleMonsterFSM->Get_Monster();
	auto pMonster = dynamic_pointer_cast<LittleMonster>(pMonsterObj);
	if (nullptr == pMonster)
		return;

	auto& weapon = pMonster->GetPartObjectMap()[TEXT("Part_Weapon")];

	weapon->SetVisible(false);
}

void LittleMonster_Attack_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void LittleMonster_Attack_State::Update(FSM* pFSM, float fDeltaTime) {

}

void LittleMonster_Attack_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<LittleMonster_Attack_State> LittleMonster_Attack_State::Create()
{
	auto	pInstance = shared_ptr<LittleMonster_Attack_State>(new LittleMonster_Attack_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : LittleMonster_Attack_State");
		return nullptr;
	}

	return pInstance;
}
