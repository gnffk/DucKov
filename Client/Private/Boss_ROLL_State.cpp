
#include "Boss_ROLL_State.h"
#include "BossMonsterFSM.h"
#include "GameInstance.h"
#include "Boss_Weapon.h"
Boss_ROLL_State::Boss_ROLL_State() {

}
Boss_ROLL_State::~Boss_ROLL_State() {

}

HRESULT Boss_ROLL_State::Initialize() {
	return S_OK;
}
void Boss_ROLL_State::Enter(FSM* pFSM) {

}
void Boss_ROLL_State::Exit(FSM* pFSM) {

}

void Boss_ROLL_State::Priority_Update(FSM* pFSM, float fDeltaTime) {

}

void Boss_ROLL_State::Update(FSM* pFSM, float fDeltaTime) {

    //if (nullptr == pFSM)
    //    return;

    //auto pBossFSM = dynamic_cast<BossMonsterFSM*>(pFSM);
    //if (nullptr == pBossFSM)
    //    return;

    //auto pMonsterObj = pBossFSM->Get_Monster();
    //auto pBoss= dynamic_pointer_cast<BossMonster>(pMonsterObj);
    //if (nullptr == pBoss)
    //    return;

    //auto pTransform = pBoss->GetTransform();
    //if (nullptr == pTransform)
    //    return;

    //_vector vRollDir = pBoss->Get_RollDir();

    //if (XMVector3Equal(vRollDir, XMVectorZero()))
    //    return;

    //vRollDir = XMVector3Normalize(vRollDir);



    //m_fCurrentSpeed -= 1.f * fDeltaTime;

    //if (m_fCurrentSpeed < 0.f)
    //    m_fCurrentSpeed = 0.f;

    //auto Nav = pBossFSM->GetNavigation();

    //pTransform->Move(vRollDir, fDeltaTime, Nav, m_fCurrentSpeed);
}

void Boss_ROLL_State::Late_Update(FSM* pFSM, float fDeltaTime) {

}

shared_ptr<Boss_ROLL_State> Boss_ROLL_State::Create()
{
	auto	pInstance = shared_ptr<Boss_ROLL_State>(new Boss_ROLL_State());

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Boss_ROLL_State");
		return nullptr;
	}

	return pInstance;
}
