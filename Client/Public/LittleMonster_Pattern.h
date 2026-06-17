
#pragma once
#include "Engine_Defines.h"
#include "Client_Defines.h"


NS_BEGIN(Client)


class LittleMonster_Pattern
{
public:
    enum class PATTERN_STATE
    {
        ROAM,
        CHASE,
        ATTACK,
        RETURN,
        ROLL,
        END
    };

private:
    LittleMonster_Pattern();

public:
    ~LittleMonster_Pattern() = default;



public:
    HRESULT Initialize(shared_ptr<class LittleMonster> pOwner);
    void Update(_float fTimeDelta);

private:
    void Update_Roam(_float fTimeDelta);
    void Update_Chase(_float fTimeDelta);
    void Update_Attack(_float fTimeDelta);

private:

    _float Compute_Distance_To_Player(shared_ptr<class GameObject> pPlayer);

private:
    weak_ptr<class LittleMonster> m_pOwner;

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
    _float m_fRollCoolTimer = 0.f;
    _float m_fRollCoolTime = 3.0f;

    _float m_fRollCheckTimer = 0.f;
    _float m_fRollCheckInterval = 0.8f;

    _float m_fRollMotionTimer = 0.f;
    _float m_fRollMotionTime = 0.45f;

    _float m_fRollChance = 0.25f;
    _float m_fRollSpeedScale = 2.8f;
    _float m_fRollProbeDistance = 2.0f;

    _float3 m_vRollDir = { 0.f, 0.f, 0.f };

    _bool m_bRolling = false;
private:
    void Update_Roll(_float fTimeDelta);
    _bool Should_Roll(shared_ptr<GameObject> pPlayer);
    void Start_Roll(shared_ptr<GameObject> pPlayer);
private:
    void Change_PatternState(PATTERN_STATE eNextState);
    void Clear_Path();
    void Follow_Path(_float fTimeDelta, _float fSpeedScale);
    _bool Build_RoamPath();


    void Change_LittleMonsterState(LittleMonsterFSM::LITTLEMONSTER_STATE eState);

private:
    _float m_fDetectRange = 15.f;
    _float m_fAttackRange = 1.f;

    _float m_fAttackCoolTime = 0.1f;
    _float m_fAttackTimer = 0.f;

private:
    _float m_fShootRange = 1.f;
    _float m_fSightCheckStep = 0.5f;

    _bool m_bAttacking = false;
    _float m_fAttackMotionTime = 0.2f;
    _float m_fAttackMotionTimer = 0.f;

private:
    _bool Can_See_Player(shared_ptr<GameObject> pPlayer);
    void Smooth_Path();

public:
    static shared_ptr<LittleMonster_Pattern> Create(shared_ptr<class LittleMonster> pOwner);

};

NS_END