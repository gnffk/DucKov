#include "FSM.h"

#include "BossMonsterFSM.h"
#include "Boss_Idle_State.h"
#include "Boss_HAND_UP_AND_WALK_State.h"
#include "Boss_HAND_UP_AND_LEFT_State.h"
#include "Boss_HAND_UP_AND_RIGHT_State.h"
#include "Boss_HAND_UP_State.h"
#include "Boss_WALK_State.h"
#include "Boss_LEFT_WALK_State.h"
#include "Boss_RIGHT_WALK_State.h"
#include "Boss_ROLL_State.h"
#include "Boss_RUN_State.h"

BossMonsterFSM::BossMonsterFSM()
{
}

HRESULT BossMonsterFSM::Initialize(uint32_t* eModelState)
{

	__super::AddState(BOSS_STATE::IDLE, Boss_Idle_State::Create());
	__super::AddState(BOSS_STATE::HAND_UP_AND_WALK, Boss_HAND_UP_AND_WALK_State::Create());
	__super::AddState(BOSS_STATE::HAND_UP_AND_LEFT, Boss_HAND_UP_AND_LEFT_State::Create());
	__super::AddState(BOSS_STATE::HAND_UP_AND_RIGHT, Boss_HAND_UP_AND_RIGHT_State::Create());
	__super::AddState(BOSS_STATE::HAND_UP, Boss_Hand_UP_State::Create());
	__super::AddState(BOSS_STATE::WALK, Boss_WALK_State::Create());
	__super::AddState(BOSS_STATE::LEFT_WALK, Boss_LEFT_WALK_State::Create());
	__super::AddState(BOSS_STATE::RIGHT_WALK, Boss_RIGHT_WALK_State::Create());
	__super::AddState(BOSS_STATE::ROLL, Boss_ROLL_State::Create());
	__super::AddState(BOSS_STATE::RUN, Boss_RUN_State::Create());
	m_eModelState = eModelState;
	*m_eModelState = BOSS_STATE::IDLE;
	return S_OK;
}



void BossMonsterFSM::Change_State(BOSS_STATE eState)
{
	// 이미 같은 상태면 무시
	if (m_eCurrentState == eState)
		return;

	auto& States = Get_States();

	auto iter = States.find(eState);

	if (iter == States.end())
		return;

	m_eCurrentState = eState;

	SetModelState(eState);

	ChangeState(iter->second);
}

shared_ptr<BossMonsterFSM> BossMonsterFSM::Create(uint32_t* eModelState)
{
	auto	pInstance = shared_ptr<BossMonsterFSM>(new BossMonsterFSM());

	if (FAILED(pInstance->Initialize(eModelState)))
	{
		MSG_BOX("Failed to Created : BossMonsterFSM");
		return nullptr;
	}

	return pInstance;
}