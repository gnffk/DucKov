
#pragma once
#include "Client_Defines.h"
#include "FSM.h"

NS_BEGIN(Client)

class Boss_TPOSE_State : public State
{
private:
    Boss_TPOSE_State();
public:
    virtual ~Boss_TPOSE_State();
public:
    HRESULT Initialize();
public:
    virtual void Enter(FSM* pFSM);
    virtual void Exit(FSM* pFSM);

    virtual void Priority_Update(FSM* pFSM, float fDeltaTime);

    virtual void Update(FSM* pFSM, float fDeltaTime);

    virtual void Late_Update(FSM* pFSM, float fDeltaTime);


public:
    static  shared_ptr<Boss_TPOSE_State> Create();

};

NS_END
