

#pragma once
#include "Client_Defines.h"
#include "FSM.h"

NS_BEGIN(Client)

class LittleMonster_HAND_UP_AND_WALK_State : public State
{
private:
    LittleMonster_HAND_UP_AND_WALK_State();
public:
    virtual ~LittleMonster_HAND_UP_AND_WALK_State();
public:
    HRESULT Initialize();
public:
    virtual void Enter(FSM* pFSM);
    virtual void Exit(FSM* pFSM);

    virtual void Priority_Update(FSM* pFSM, float fDeltaTime);

    virtual void Update(FSM* pFSM, float fDeltaTime);

    virtual void Late_Update(FSM* pFSM, float fDeltaTime);


public:
    static  shared_ptr<LittleMonster_HAND_UP_AND_WALK_State> Create();

};

NS_END
