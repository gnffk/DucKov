#include "FSM.h"
#include "Player_Idle_State.h"
#include "Player_Walk_State.h"
#include "Player_FSM.h"

Player_FSM::Player_FSM()
{
}

HRESULT Player_FSM::Initialize(uint32_t* eModelState)
{

	__super::AddState(PLAYER_STATE::IDLE, Player_Idle_State::Create());
	__super::AddState(PLAYER_STATE::WALK, Player_Walk_State::Create());
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