#pragma once
#include "Client_Defines.h"

#include "Player.h"

NS_BEGIN(Client)

class Player_FSM final : public FSM
{
public:
	enum PLAYER_STATE
	{
		IDLE = 0x00000001,
		RUN = 0x00000002,
		WALK = 0x00000004,
		RIGHT_WALK = 0x00000008,
		LEFT_WALK = 0x00000010,
		TPOSE = 0x00000020,
		HAND_UP = 0x00000040,
		HAND_UP_AND_RUN = 0x00000080,
		HAND_UP_AND_WALK = 0x00000100,
		HAND_UP_AND_RIGHT = 0x00000200,
		HAND_UP_AND_LEFT = 0x00000400,
		ROLL = 0x00000800,
		HIT = 0x00001000,
		HIT_REVERSE = 0x00002000,

		END = 0xffffffff
	};
private:
    Player_FSM();

public:
    virtual ~Player_FSM() = default;

public:
    HRESULT Initialize(uint32_t* eModelState);

public:
    void Change_State(PLAYER_STATE eState);
	void SetModelState(const uint32_t _eModelState) { *m_eModelState = _eModelState; }
public:
    std::shared_ptr<GameObject> Get_Player()
    {
        return GetOwner<Player>();
    }

private:
	PLAYER_STATE m_eCurrentState = PLAYER_STATE::END;
	 uint32_t * m_eModelState;
public:
    static  shared_ptr<Player_FSM> Create(uint32_t* eModelState);
};

NS_END