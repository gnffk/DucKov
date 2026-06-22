
#pragma once
#include "Engine_Defines.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class BossPattern_Page2
{
public:
    enum class PATTERN_TYPE
    {
        NONE,
        BEFORE_START,
        START_APPEAR,
        LASER_CANNON,
        CHARGE_ATTACK,
        WHIRLWIND_ATTACK,
        ROLL_ATTACK,
        METEOR_PATTERN,

        DIE
    };

    enum class PATTERN_STEP
    {
        NONE,

        START,
        READY,
        ACTIVE,
        RECOVER,
        END
    };

private:
    typedef struct tagMeteorDesc
    {
        _float3 vTargetPos = {};
        _float  fTimer = 0.f;
        _bool   bHit = false;
    } METEOR_DESC;

private:
    BossPattern_Page2();

public:
    ~BossPattern_Page2() = default;

public:
    HRESULT Initialize(shared_ptr<BossMonster_Page2> pOwner);
    void Update(_float fTimeDelta);

public:
    void Request_DiePattern();

private:
    void Select_NextPattern();
    void Start_Pattern(PATTERN_TYPE ePattern);
    void End_Pattern();

private:
    void Update_Before_Start(_float fTimeDelta);
    void Update_Start_Appear(_float fTimeDelta);
    void Update_LaserCannon(_float fTimeDelta);
    void Update_ChargeAttack(_float fTimeDelta);
    void Update_WhirlwindAttack(_float fTimeDelta);
    void Update_RollAttack(_float fTimeDelta);
    void Update_MeteorPattern(_float fTimeDelta);
    void Update_Die(_float fTimeDelta);

private:
    void Fire_BossWeapon();
    void Turn_To_Player(_float fTimeDelta);
    void Move_To_Player(_float fTimeDelta, _float fSpeedScale);
    void Move_Away_From_Player(_float fTimeDelta, _float fSpeedScale);
    void Damage_Player_If_Close(_float fRange, _float fDamage);
    void Damage_Player_If_InFront(_float fRange, _float fAngleDegree, _float fDamage);
	void Lending_On_Floor(_float fTimeDelta);

public:
	PATTERN_TYPE Get_CurrentPattern() const { return m_eCurrentPattern; }
private:
    shared_ptr<GameObject> Find_Player();
    _bool Get_PlayerPos(_float3& vOutPlayerPos);
    _float Compute_Distance_To_Player();
    _float3 Compute_Direction_To_Player();

private:
    void Change_BossState(int iState);

private:
    weak_ptr<BossMonster_Page2> m_pOwner;

private:
    PATTERN_TYPE m_eCurrentPattern = PATTERN_TYPE::NONE;
    PATTERN_STEP m_ePatternStep = PATTERN_STEP::NONE;

private:
    _float m_fPatternTimer = 0.f;
    _float m_fPatternCoolTimer = 1.5f;

    _float m_fAppearReadyTime = 0.f;

    _float m_fStepTimer = 0.f;
    _float m_fFireTimer = 0.f;
    _float m_fDamageTimer = 0.f;

private:
    _float3 m_vLockedPlayerPos = {};
    _float3 m_vChargeDir = {};

private:
    vector<METEOR_DESC> m_Meteors;

private:
    _bool m_bRequestedDie = false;
    _bool m_bHasDamagedThisPattern = false;

private:
    // 패턴 공통 값
    _float m_fDetectRange = 35.f;

    // 레이저 포
    _float m_fLaserReadyTime = 0.8f;
    _float m_fLaserActiveTime = 1.5f;
    _float m_fLaserRecoverTime = 0.5f;
    _float m_fLaserFireInterval = 0.12f;

    // 돌진
    _float m_fChargeReadyTime = 0.5f;
    _float m_fChargeActiveTime = 1.0f;
    _float m_fChargeRecoverTime = 0.5f;
    _float m_fChargeSpeedScale = 3.2f;
    _float m_fChargeDamageRange = 2.0f;
    _float m_fChargeDamage = 30.f;

    // 회오리
    _float m_fWhirlwindActiveTime = 2.5f;
    _float m_fWhirlwindRecoverTime = 0.6f;
    _float m_fWhirlwindSpeedScale = 1.7f;
    _float m_fWhirlwindDamageInterval = 0.35f;
    _float m_fWhirlwindDamageRange = 2.5f;
    _float m_fWhirlwindDamage = 15.f;

    // 구르기
    _float m_fRollReadyTime = 0.25f;
    _float m_fRollActiveTime = 0.9f;
    _float m_fRollRecoverTime = 0.45f;
    _float m_fRollSpeedScale = 3.8f;
    _float m_fRollDamageRange = 2.2f;
    _float m_fRollDamage = 25.f;

    // 메테오
    _float m_fMeteorMoveAwayTime = 1.0f;
    _float m_fMeteorGrowTime = 1.0f;
    _float m_fMeteorCastTime = 3.0f;
    _float m_fMeteorRecoverTime = 1.0f;
    _float m_fMeteorSpawnInterval = 0.45f;
    _float m_fMeteorDropDelay = 0.8f;
    _float m_fMeteorDamageRange = 2.8f;
    _float m_fMeteorDamage = 35.f;

public:
    static shared_ptr<BossPattern_Page2> Create(shared_ptr<BossMonster_Page2> pOwner);
};

NS_END