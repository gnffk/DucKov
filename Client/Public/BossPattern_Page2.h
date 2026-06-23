
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

    // 대쉬 공격
    _float m_fChargeReadyTime = 0.45f;
    _float m_fChargeActiveTime = 0.35f;
    _float m_fChargeRecoverTime = 0.65f;

    _float m_fChargeSpeedScale = 12.f;
    _float m_fChargeDamageRange = 2.5f;
    _float m_fChargeDamage = 30.f;

    _float m_fChargeHitStartTime = 0.08f;
    _float m_fChargeHitEndTime = 0.28f;

    // 회오리
    _float m_fWhirlwindActiveTime = 3.5f;
    _float m_fWhirlwindRecoverTime = 0.8f;
    _float m_fWhirlwindSpeedScale = 2.f;
    _float m_fWhirlwindRotationScale = 8.f;
    _float m_fWhirlwindDamageInterval = 0.35f;
    _float m_fWhirlwindDamageRange = 3.0f;
    _float m_fWhirlwindDamage = 15.f;



	// 등장
    _float3 m_vAppearStartPos = {};
    _float3 m_vAppearTargetPos = {};

    _float m_fAppearUpTime = 1.2f;
    _float m_fAppearLandingDelay = 0.4f;
    _float m_fAppearRecoverTime = 0.6f;
    _float m_fAppearHideY = 8.f;
    _bool  m_bAppearLandingDamage = false;

    
    _float3 m_vCutCamStartPos = {};
    _float3 m_vCutCamBossViewPos = {};
    _float3 m_vCutCamReturnStartPos = {};
    _float3 m_vCutCamReturnTargetPos = {};
    _float3 m_vCutCamLookAtPos = {};

    _float m_fCutCamMoveTime = 1.2f;
    _float m_fCutCamReturnTime = 0.8f;
    _float m_fCutCamMoveSpeed = 25.f;
    _float m_fCutCamArriveDistance = 0.15f;
    _float m_fBossViewDistance = 7.0f;
    _float m_fBossViewHeight = 2.5f;
    _float m_fBossLookAtHeight = 1.5f;

    _bool m_bCutCamReady = false;

private:
    _float m_fAppearDropTime = 0.35f;          // 확 내려오는 시간
    _float m_fAppearAfterLandingWait = 5.f;  // 착지 후 기다리는 시간

private:
    shared_ptr<GameObject> Find_PlayerCamera();
    shared_ptr<GameObject> Find_BossCutSceneCamera();
    void Setup_AppearCutSceneCamera();
    _bool Update_CutSceneCamera_MoveToBoss(_float fTimeDelta);
    void Update_CutSceneCamera_ReturnToPlayer(_float fTimeDelta);
    void Set_CameraLookAt(shared_ptr<GameObject> pCameraObject, const _float3& vEye, const _float3& vAt);
    _float SmoothStepRatio(_float fTimer, _float fDuration);
    _float3 Lerp_Float3(const _float3& vA, const _float3& vB, _float fRatio);


private:
    shared_ptr<class LaserTrail> m_pLaserTrail = nullptr;

private:
    HRESULT Ready_LaserTrail();

    void Begin_BossLaser();
    void Update_BossLaser(_float fTimeDelta);
    void End_BossLaser();

    void Get_LaserStartAndDir(_float3& vOutStartPos, _float3& vOutDir);


public:
    static shared_ptr<BossPattern_Page2> Create(shared_ptr<BossMonster_Page2> pOwner);
};

NS_END