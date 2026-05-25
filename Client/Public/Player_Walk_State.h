#pragma once
#include "Client_Defines.h"
#include "FSM.h"

NS_BEGIN(Client)

class Player_Walk_State : public State
{
private:
    Player_Walk_State();
public:
    virtual ~Player_Walk_State();
public:
    HRESULT Initialize();
public:
    virtual void Enter(FSM* pFSM);
    virtual void Exit(FSM* pFSM);

    virtual void Priority_Update(FSM* pFSM, float fDeltaTime);

    virtual void Update(FSM* pFSM, float fDeltaTime);

    virtual void Late_Update(FSM* pFSM, float fDeltaTime);


public:
    static  shared_ptr<Player_Walk_State> Create();

};

NS_END
