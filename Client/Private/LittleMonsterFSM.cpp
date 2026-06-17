#include "FSM.h"

#include "LittleMonsterFSM.h"

#include "LittleMonster_Idle_State.h"
#include "LittleMonster_HAND_UP_AND_WALK_State.h"
#include "LittleMonster_HAND_UP_AND_LEFT_State.h"
#include "LittleMonster_HAND_UP_AND_RIGHT_State.h"
#include "LittleMonster_HAND_UP_State.h"
#include "LittleMonster_WALK_State.h"
#include "LittleMonster_LEFT_WALK_State.h"
#include "LittleMonster_RIGHT_WALK_State.h"
#include "LittleMonster_ROLL_State.h"
#include "LittleMonster_RUN_State.h"
#include "LittleMonster_Attack_State.h"

LittleMonsterFSM::LittleMonsterFSM()
{
}

HRESULT LittleMonsterFSM::Initialize(uint32_t* eModelState)
{

	__super::AddState(LITTLEMONSTER_STATE::IDLE, LittleMonster_Idle_State::Create());
	__super::AddState(LITTLEMONSTER_STATE::WALK, LittleMonster_WALK_State::Create());
	__super::AddState(LITTLEMONSTER_STATE::ROLL, LittleMonster_ROLL_State::Create());
	__super::AddState(LITTLEMONSTER_STATE::ATTACK, LittleMonster_Attack_State::Create());
	m_eModelState = eModelState;
	*m_eModelState = LITTLEMONSTER_STATE::IDLE;
	return S_OK;
}


void LittleMonsterFSM::Change_State(LITTLEMONSTER_STATE eState)
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

shared_ptr<LittleMonsterFSM> LittleMonsterFSM::Create(uint32_t* eModelState)
{
	auto	pInstance = shared_ptr<LittleMonsterFSM>(new LittleMonsterFSM());

	if (FAILED(pInstance->Initialize(eModelState)))
	{
		MSG_BOX("Failed to Created : LittleMonsterFSM");
		return nullptr;
	}

	return pInstance;
}