#include "FSM.h"
#include "Player_Idle_State.h"
#include "Player_HAND_UP_AND_WALK_State.h"
#include "Player_HAND_UP_AND_LEFT_State.h"
#include "Player_HAND_UP_AND_RIGHT_State.h"
#include "Player_HAND_UP_State.h"
#include "Player_WALK_State.h"
#include "Player_LEFT_WALK_State.h"
#include "Player_RIGHT_WALK_State.h"
#include "Player_ROLL_State.h"

#include "Player_FSM.h"

Player_FSM::Player_FSM()
{
}

HRESULT Player_FSM::Initialize(uint32_t* eModelState)
{

	__super::AddState(PLAYER_STATE::IDLE, Player_Idle_State::Create());
	__super::AddState(PLAYER_STATE::HAND_UP_AND_WALK, Player_HAND_UP_AND_WALK_State::Create());
	__super::AddState(PLAYER_STATE::HAND_UP_AND_LEFT, Player_HAND_UP_AND_LEFT_State::Create());
	__super::AddState(PLAYER_STATE::HAND_UP_AND_RIGHT, Player_HAND_UP_AND_RIGHT_State::Create());
	__super::AddState(PLAYER_STATE::HAND_UP, Player_HAND_UP_State::Create());
	__super::AddState(PLAYER_STATE::WALK, Player_WALK_State::Create());
	__super::AddState(PLAYER_STATE::LEFT_WALK, Player_LEFT_WALK_State::Create());
	__super::AddState(PLAYER_STATE::RIGHT_WALK, Player_RIGHT_WALK_State::Create());
	__super::AddState(PLAYER_STATE::ROLL, Player_ROLL_State::Create());
	m_eModelState = eModelState;
	*m_eModelState = PLAYER_STATE::IDLE;
	return S_OK;
}

void Player_FSM::Change_State(PLAYER_STATE eState)
{
	auto& States = Get_States();

	auto iter = States.find(eState);

	if (iter == States.end())
		return;


	SetModelState(eState);
	__super::ChangeState(iter->second);
}

shared_ptr<Player_FSM> Player_FSM::Create(uint32_t* eModelState)
{
	auto	pInstance = shared_ptr<Player_FSM>(new Player_FSM());

	if (FAILED(pInstance->Initialize(eModelState)))
	{
		MSG_BOX("Failed to Created : Player_FSM");
		return nullptr;
	}

	return pInstance;
}