#pragma once
#include "Engine_Defines.h"
#include "Client_Defines.h"

NS_BEGIN(Client)


class BossPattern
{
public:
    enum class PATTERN_STATE
    {
        ROAM,
        CHASE,
        ATTACK,
        RETURN,
        DIE,
        END
    };

private:
    BossPattern();

public:
    ~BossPattern() = default;



public:
    HRESULT Initialize(shared_ptr<class BossMonster> pOwner);
    void Update(_float fTimeDelta);

private:
    void Update_Roam(_float fTimeDelta);
    void Update_Chase(_float fTimeDelta);
    void Update_Attack(_float fTimeDelta);
    void Update_DIE(_float fTimeDelta);

private:

    _float Compute_Distance_To_Player(shared_ptr<class GameObject> pPlayer);

private:
    weak_ptr<BossMonster> m_pOwner;

    PATTERN_STATE m_ePatternState = PATTERN_STATE::ROAM;



    _float3 m_vRoamDir = { 0.f, 0.f, 1.f };
    _float m_fRoamChangeTime = 2.f;
    _float m_fRoamTimer = 0.f;

private:
    vector<_float3> m_PathPoints;
    uint32_t m_iCurrentPathIndex = 0;

    _float m_fPathRefreshTime = 0.35f;
    _float m_fPathRefreshTimer = 0.f;

    _float m_fWaypointReachRange = 0.7f;
    // Roam A*
    _bool m_bRoamIdle = false;
    _float m_fRoamIdleTime = 1.5f;
    _float m_fRoamIdleTimer = 0.f;

    _float m_fRoamMinDistance = 4.f;
    _float m_fRoamMaxDistance = 10.f;

    uint32_t m_iRoamPathRetryCount = 12;
private:
    void Change_PatternState(PATTERN_STATE eNextState);
    void Clear_Path();
    void Follow_Path(_float fTimeDelta, _float fSpeedScale);
    _bool Build_RoamPath();


    void Change_BossState(BossMonsterFSM::BOSS_STATE eState);

public:
    void Pattern_Dead() { m_bDead = true; }
private:
	_bool m_bDead = false;
private:
    _float m_fDetectRange = 15.f;
    _float m_fAttackRange = 8.f;

    _float m_fAttackCoolTime = 0.1f;
    _float m_fAttackTimer = 0.f;

private:
    _float m_fShootRange = 8.f;
    _float m_fSightCheckStep = 0.5f;

    _bool m_bAttacking = false;
    _float m_fAttackMotionTime = 0.2f;
    _float m_fAttackMotionTimer = 0.f;

private:
    _bool Can_See_Player(shared_ptr<GameObject> pPlayer);
    void Smooth_Path();

public:
    static shared_ptr<BossPattern> Create(shared_ptr<class BossMonster> pOwner);

};

NS_END