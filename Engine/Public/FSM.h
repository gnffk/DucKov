#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class FSM;

class ENGINE_DLL State : public enable_shared_from_this<State>
{
protected:
    State();
public:
    virtual ~State();

public:
    virtual void Enter(FSM* pFSM) {}
    virtual void Exit(FSM* pFSM) {}

    virtual void Priority_Update(FSM* pFSM, float fDeltaTime) {}

    virtual void Update(FSM* pFSM, float fDeltaTime) {}

    virtual void Late_Update(FSM* pFSM, float fDeltaTime) {}


};


class ENGINE_DLL FSM
{
protected:
    FSM();

public:
    virtual ~FSM() ;

public:
    virtual void AddState(int iStateID, std::shared_ptr<class State> pState)
    {
        m_States.emplace(iStateID, pState);
    }

    virtual  void Priority_Update(float fDeltaTime)
    {
        if (m_pCurrentState)
            m_pCurrentState->Priority_Update(this, fDeltaTime);
    }

    virtual void Update(float fDeltaTime)
    {
        if (m_pCurrentState)
            m_pCurrentState->Update(this, fDeltaTime);
    }

    virtual void Late_Update(float fDeltaTime)
    {
        if (m_pCurrentState)
            m_pCurrentState->Late_Update(this, fDeltaTime);
    }

    unordered_map<int, shared_ptr<class State>>& Get_States() { return m_States; }
protected:
    void ChangeState(shared_ptr<class State> pNextState)
    {
        if (m_pCurrentState)
            m_pCurrentState->Exit(this);

        m_pCurrentState = pNextState;

        if (m_pCurrentState)
            m_pCurrentState->Enter(this);
    }

public:
    template<typename T>
    shared_ptr<T> GetOwner()
    {
        return std::static_pointer_cast<T>(m_pOwner.lock());
    }

    void SetOwner(shared_ptr<void> pOwner)
    {
        m_pOwner = pOwner;
    }

private:
    unordered_map<int, shared_ptr<class State>> m_States;

    shared_ptr<class State>   m_pCurrentState;
    weak_ptr<void>      m_pOwner;
};

NS_END

