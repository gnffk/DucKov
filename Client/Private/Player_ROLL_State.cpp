#include "Player_ROLL_State.h"
#include "Player_FSM.h"
#include "GameInstance.h"

Player_ROLL_State::Player_ROLL_State() {

}
Player_ROLL_State::~Player_ROLL_State() {

}

HRESULT Player_ROLL_State::Initialize() {
	return S_OK;
}
void Player_ROLL_State::Enter(FSM* pFSM) {

    m_fCurrentSpeed = 5.f;
}
void Player_ROLL_State::Exit(FSM* pFSM) {

}

void Player_ROLL_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Player_ROLL_State::Update(FSM* pFSM, float fDeltaTime) {
    if (nullptr == pFSM)
        return;

    auto pPlayerFSM = dynamic_cast<Player_FSM*>(pFSM);
    if (nullptr == pPlayerFSM)
        return;

    auto pPlayerObj = pPlayerFSM->Get_Player();
    auto pPlayer = dynamic_pointer_cast<Player>(pPlayerObj);
    if (nullptr == pPlayer)
        return;

    auto pTransform = pPlayer->GetTransform();
    if (nullptr == pTransform)
        return;

    _vector vRollDir = pPlayer->Get_RollDir();

    if (XMVector3Equal(vRollDir, XMVectorZero()))
        return;

    vRollDir = XMVector3Normalize(vRollDir);



    m_fCurrentSpeed -= 45.f * fDeltaTime;

    if (m_fCurrentSpeed < 0.f)
        m_fCurrentSpeed = 0.f;


    pTransform->Move(vRollDir, fDeltaTime , m_fCurrentSpeed);
}

void Player_ROLL_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Player_ROLL_State> Player_ROLL_State::Create()
{
	auto	pInstance = shared_ptr<Player_ROLL_State>(new Player_ROLL_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Player_ROLL_State");
		return nullptr;
	}

	return pInstance;
}
