
#pragma once
#include "Client_Defines.h"

#include "LittleMonster.h"

NS_BEGIN(Client)

class LittleMonsterFSM final : public FSM
{
public:
    enum LITTLEMONSTER_STATE
    {
        IDLE = 1 << 0,
        WALK = 1 << 1,

        ATTACK = 1 << 2,
        ROLL = 1 << 3,
      
        END = 1 << 15,
    };

private:
    LittleMonsterFSM();

public:
    virtual ~LittleMonsterFSM() = default;

public:
    HRESULT Initialize(uint32_t* eModelState);

public:
    void Change_State(LITTLEMONSTER_STATE eState);
    void SetModelState(const uint32_t _eModelState) { *m_eModelState = _eModelState; }
public:
    std::shared_ptr<GameObject> Get_Monster()
    {
        return GetOwner<LittleMonster>();
    }

private:
    LITTLEMONSTER_STATE m_eCurrentState = LITTLEMONSTER_STATE::END;
    uint32_t* m_eModelState;
    _bool				m_bShift = { false };
public:
    static  shared_ptr<LittleMonsterFSM> Create(uint32_t* eModelState);
};

NS_END