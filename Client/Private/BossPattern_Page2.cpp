#include "BossPattern_Page2.h"
#include "BossMonster_Page2.h"
#include "BossMonster_Page2FSM.h"
#include "Boss_Weapon.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Player.h"
#include "Particle_System.h"
#include "LaserTrail.h"

NS_BEGIN(Client)

BossPattern_Page2::BossPattern_Page2()
{
}

HRESULT BossPattern_Page2::Initialize(shared_ptr<BossMonster_Page2> pOwner)
{
    m_pOwner = pOwner;

    m_eCurrentPattern = PATTERN_TYPE::NONE;
    m_ePatternStep = PATTERN_STEP::NONE;

    m_fPatternCoolTimer = 1.0f;

    return S_OK;
}

void BossPattern_Page2::Update(_float fTimeDelta)
{
    if (m_bRequestedDie)
    {
        if (m_eCurrentPattern != PATTERN_TYPE::DIE)
            Start_Pattern(PATTERN_TYPE::DIE);

        return;
    }

    switch (m_eCurrentPattern)
    {
    case PATTERN_TYPE::BEFORE_START:
        Update_Before_Start(fTimeDelta);
        break;

    case PATTERN_TYPE::START_APPEAR:
        Update_Start_Appear(fTimeDelta);
        break;

    case PATTERN_TYPE::LASER_CANNON:
        Update_LaserCannon(fTimeDelta);
        break;

    case PATTERN_TYPE::CHARGE_ATTACK:
        Update_ChargeAttack(fTimeDelta);
        break;

    case PATTERN_TYPE::WHIRLWIND_ATTACK:
        Update_WhirlwindAttack(fTimeDelta);
        break;

    case PATTERN_TYPE::DIE:
        Update_Die(fTimeDelta);
        break;

    default:
        Start_Pattern(PATTERN_TYPE::BEFORE_START);
        break;
    }
}

void BossPattern_Page2::Request_DiePattern()
{
    m_bRequestedDie = true;
}

void BossPattern_Page2::Select_NextPattern()
{
    auto pPlayer = Find_Player();

    if (pPlayer == nullptr)
    {
        // 없을떄 시작
        Start_Pattern(PATTERN_TYPE::BEFORE_START);
        return;
    }

    _float fDistance = Compute_Distance_To_Player();

    int iRandom = rand() % 100;



    if (fDistance <= 4.f)
    {
        if (iRandom < 35)
            Start_Pattern(PATTERN_TYPE::WHIRLWIND_ATTACK);
        else
            Start_Pattern(PATTERN_TYPE::CHARGE_ATTACK);
 
    }
    else if (fDistance <= 12.f)
    {
        if (iRandom < 30)
            Start_Pattern(PATTERN_TYPE::CHARGE_ATTACK);
        else if (iRandom < 60)
            Start_Pattern(PATTERN_TYPE::LASER_CANNON);
        else
            Start_Pattern(PATTERN_TYPE::WHIRLWIND_ATTACK);
    }
    else
    {
        if (iRandom < 45)
            Start_Pattern(PATTERN_TYPE::LASER_CANNON);
        else 
            Start_Pattern(PATTERN_TYPE::CHARGE_ATTACK);

    }
}

void BossPattern_Page2::Start_Pattern(PATTERN_TYPE ePattern)
{
    m_eCurrentPattern = ePattern;
    m_ePatternStep = PATTERN_STEP::START;

    m_fPatternTimer = 0.f;
    m_fStepTimer = 0.f;
    m_fFireTimer = 0.f;
    m_fDamageTimer = 0.f;

    m_bHasDamagedThisPattern = false;

    switch (m_eCurrentPattern)
    {
    case PATTERN_TYPE::START_APPEAR:
    {
        auto pOwner = m_pOwner.lock();
        if (nullptr == pOwner)
            return;

        auto pTransform = pOwner->GetTransform();
        if (nullptr == pTransform)
            return;

        _float3 vCurPos{};
        XMStoreFloat3( &vCurPos,pTransform->Get_State(STATE::POSITION) );

        m_vAppearTargetPos = vCurPos;


        m_vAppearStartPos = vCurPos;
        m_vAppearStartPos.y += m_fAppearHideY;

        pTransform->Set_State( STATE::POSITION, XMVectorSet( m_vAppearStartPos.x, m_vAppearStartPos.y, m_vAppearStartPos.z, 1.f) );

        m_bAppearLandingDamage = false;
        m_bCutCamReady = false;

        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);

        Setup_AppearCutSceneCamera();

        m_ePatternStep = PATTERN_STEP::START;
        m_fStepTimer = 0.f;

        break;
    }
    default:
        break;
    }
}

void BossPattern_Page2::End_Pattern()
{
    m_eCurrentPattern = PATTERN_TYPE::NONE;
    m_ePatternStep = PATTERN_STEP::NONE;

    m_fPatternTimer = 0.f;
    m_fStepTimer = 0.f;
    m_fFireTimer = 0.f;
    m_fDamageTimer = 0.f;

    Select_NextPattern();
}

void BossPattern_Page2::Update_Before_Start(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();
    if (nullptr == pOwner)
        return;

    m_fPatternTimer += fTimeDelta;

    _float fDistance = Compute_Distance_To_Player();

    if (fDistance <= m_fDetectRange)
    {
        Start_Pattern(PATTERN_TYPE::START_APPEAR);
    }

}

void BossPattern_Page2::Update_Start_Appear(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();
    if (nullptr == pOwner)
        return;

    auto pTransform = pOwner->GetTransform();
    if (nullptr == pTransform)
        return;

    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
    {
        _bool bArrived = Update_CutSceneCamera_MoveToBoss(fTimeDelta);

        if (bArrived)
        {
            m_ePatternStep = PATTERN_STEP::READY;
            m_fStepTimer = 0.f;

            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);
        }

        break;
    }

    case PATTERN_STEP::READY:
    {
        _float fRatio = m_fStepTimer / m_fAppearDropTime;

        if (fRatio > 1.f)
            fRatio = 1.f;

        // 빠르게 내려오고 마지막에 살짝 감속
        _float fEase = 1.f - powf(1.f - fRatio, 3.f);

        _float3 vNewPos = Lerp_Float3(
            m_vAppearStartPos,
            m_vAppearTargetPos,
            fEase
        );
        pTransform->Set_State(STATE::POSITION, XMVectorSet(vNewPos.x, vNewPos.y, vNewPos.z, 1.f));

        if (fRatio >= 1.f)
        {
            pTransform->Set_State(STATE::POSITION, XMVectorSet( m_vAppearTargetPos.x, m_vAppearTargetPos.y, m_vAppearTargetPos.z,1.f));

            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);

            m_ePatternStep = PATTERN_STEP::ACTIVE;
            m_fStepTimer = 0.f;
        }

        break;
    }

    case PATTERN_STEP::ACTIVE:
    {
        // 착지 직후 1회 광역 피해
        if (false == m_bAppearLandingDamage)
        {
   
            m_bAppearLandingDamage = true;
        }

        if (m_fStepTimer >= m_fAppearLandingDelay)
        {
            // 착지 연출은 끝났지만 바로 컷백하지 않고 대기 단계로 넘김

            m_ePatternStep = PATTERN_STEP::RECOVER;
            m_fStepTimer = 0.f;
            pOwner->Show_BossUI();

        }

        break;
    }

    case PATTERN_STEP::RECOVER:
    {
        // 착지 후 여운 대기
        if (m_fStepTimer >= m_fAppearAfterLandingWait)
        {
            CGameInstance::Get().Change_Camera(ETOUI(CAMERA::PLAYER));

            m_ePatternStep = PATTERN_STEP::END;
            m_fStepTimer = 0.f;
        }

        break;
    }

    case PATTERN_STEP::END:
    {
        End_Pattern();
        break;
    }

    default:
        break;
    }
}

void BossPattern_Page2::Update_LaserCannon(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
    {
        m_ePatternStep = PATTERN_STEP::READY;
        m_fStepTimer = 0.f;
        m_fFireTimer = 0.f;
        m_fDamageTimer = 0.f;

        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::HAND_UP);

        break;
    }

    case PATTERN_STEP::READY:
    {
        Turn_To_Player(fTimeDelta * 0.5f);

        if (m_fStepTimer >= m_fLaserReadyTime)
        {
            m_ePatternStep = PATTERN_STEP::ACTIVE;
            m_fStepTimer = 0.f;
            m_fFireTimer = 0.f;
            m_fDamageTimer = 0.f;

            Begin_BossLaser();
        }

        break;
    }

    case PATTERN_STEP::ACTIVE:
    {
        Turn_To_Player(fTimeDelta * 0.5f);


        Update_BossLaser(fTimeDelta);


        m_fDamageTimer -= fTimeDelta;

        if (m_fDamageTimer <= 0.f)
        {
            m_fDamageTimer = 0.15f;

            Damage_Player_If_InFront(35.f,4.f,8.f);
        }

        if (m_fStepTimer >= m_fLaserActiveTime)
        {
            End_BossLaser();

            m_ePatternStep = PATTERN_STEP::RECOVER;
            m_fStepTimer = 0.f;

            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);
        }

        break;
    }

    case PATTERN_STEP::RECOVER:
    {
        if (m_fStepTimer >= m_fLaserRecoverTime)
        {
            End_Pattern();
        }

        break;
    }

    default:
        break;
    }
}
void BossPattern_Page2::Update_ChargeAttack(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr || pOwner->GetTransform() == nullptr)
        return;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
    {
        m_ePatternStep = PATTERN_STEP::READY;
        m_fStepTimer = 0.f;
        m_bHasDamagedThisPattern = false;

        // 공격 준비 자세
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);

        break;
    }

    case PATTERN_STEP::READY:
    {
        // 공격 준비 중에는 플레이어 쪽 바라봄
        Turn_To_Player(fTimeDelta);

        if (m_fStepTimer >= m_fChargeReadyTime)
        {
            // 대쉬 공격 시작 순간 방향 고정
            m_vChargeDir = Compute_Direction_To_Player();

            m_ePatternStep = PATTERN_STEP::ACTIVE;
            m_fStepTimer = 0.f;
            m_bHasDamagedThisPattern = false;

            // 핵심: RUN이 아니라 HIT
            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::HIT);
        }

        break;
    }

    case PATTERN_STEP::ACTIVE:
    {
        // HIT 애니메이션 중 앞으로 짧게 대쉬
        pOwner->Turn_To_Direction(m_vChargeDir, fTimeDelta);

        pOwner->Move_Direction(m_vChargeDir,fTimeDelta,m_fChargeSpeedScale);

        // 공격 판정은 HIT 애니메이션 초반/중반에만
        if (false == m_bHasDamagedThisPattern)
        {
            if (m_fStepTimer >= m_fChargeHitStartTime &&m_fStepTimer <= m_fChargeHitEndTime)
            {
                _float fDistance = Compute_Distance_To_Player();

                if (fDistance <= m_fChargeDamageRange)
                {
                    Damage_Player_If_Close(m_fChargeDamageRange, m_fChargeDamage);

                    m_bHasDamagedThisPattern = true;
                }
            }
        }

        if (m_fStepTimer >= m_fChargeActiveTime)
        {
            m_ePatternStep = PATTERN_STEP::RECOVER;
            m_fStepTimer = 0.f;

            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);
        }

        break;
    }

    case PATTERN_STEP::RECOVER:
    {
        // 후딜 중에는 멈춰있고 플레이어만 바라보게
        Turn_To_Player(fTimeDelta);

        if (m_fStepTimer >= m_fChargeRecoverTime)
        {
            End_Pattern();
        }

        break;
    }

    default:
        break;
    }
}

void BossPattern_Page2::Update_WhirlwindAttack(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;
    m_fDamageTimer -= fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr || pOwner->GetTransform() == nullptr)
        return;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
    {
        m_ePatternStep = PATTERN_STEP::ACTIVE;
        m_fStepTimer = 0.f;
        m_fDamageTimer = 0.f;

        // 핵심: ROLL이 아니라 TPOSE
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::TPOSE);

        break;
    }

    case PATTERN_STEP::ACTIVE:
    {
        auto pTransform = pOwner->GetTransform();

        // 1. TPOSE 상태로 Y축 회전
        pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f),fTimeDelta * m_fWhirlwindRotationScale);

        // 2. 플레이어 방향으로 천천히 이동
        //    여기서 Turn_To_Direction() 쓰면 안 됨.
        _float3 vDir = Compute_Direction_To_Player();

        pOwner->Move_Direction(vDir, fTimeDelta, m_fWhirlwindSpeedScale  );

        // 3. 가까이 있으면 주기적으로 데미지
        if (m_fDamageTimer <= 0.f)
        {
            m_fDamageTimer = m_fWhirlwindDamageInterval;

            Damage_Player_If_Close(m_fWhirlwindDamageRange,m_fWhirlwindDamage);
        }

        // 4. 회오리 종료
        if (m_fStepTimer >= m_fWhirlwindActiveTime)
        {
            m_ePatternStep = PATTERN_STEP::RECOVER;
            m_fStepTimer = 0.f;

            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);
        }

        break;
    }

    case PATTERN_STEP::RECOVER:
    {
        // 후딜 중에는 플레이어 바라보게
        Turn_To_Player(fTimeDelta);

        if (m_fStepTimer >= m_fWhirlwindRecoverTime)
        {
            End_Pattern();
        }

        break;
    }

    default:
        break;
    }
}

void BossPattern_Page2::Update_Die(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    Change_BossState(BossMonster_Page2FSM::BOSS_STATE::TPOSE);

    if (m_fPatternTimer >= 1.5f)
    {
        pOwner->Set_Dead();
    }
}

void BossPattern_Page2::Fire_BossWeapon()
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto& PartObjects = pOwner->GetPartObjectMap();

    auto iter = PartObjects.find(TEXT("Part_Weapon"));

    if (iter == PartObjects.end())
        return;

    auto pWeapon = static_pointer_cast<Boss_Weapon>(iter->second);

    if (pWeapon == nullptr)
        return;

    pWeapon->Fire_Bullet();
}

void BossPattern_Page2::Turn_To_Player(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    _float3 vPlayerPos{};

    if (!Get_PlayerPos(vPlayerPos))
        return;

    pOwner->Turn_To_Position(vPlayerPos, fTimeDelta);
}

void BossPattern_Page2::Move_To_Player(_float fTimeDelta, _float fSpeedScale)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    _float3 vDir = Compute_Direction_To_Player();

    pOwner->Turn_To_Direction(vDir, fTimeDelta);
    pOwner->Move_Direction(vDir, fTimeDelta, fSpeedScale);
}

void BossPattern_Page2::Move_Away_From_Player(_float fTimeDelta, _float fSpeedScale)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    _float3 vDir = Compute_Direction_To_Player();

    vDir.x *= -1.f;
    vDir.z *= -1.f;

    pOwner->Turn_To_Direction(vDir, fTimeDelta);
    pOwner->Move_Direction(vDir, fTimeDelta, fSpeedScale);
}

shared_ptr<GameObject> BossPattern_Page2::Find_Player()
{
    return CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");
}

_bool BossPattern_Page2::Get_PlayerPos(_float3& vOutPlayerPos)
{
    auto pPlayer = Find_Player();

    if (pPlayer == nullptr)
        return false;

    if (pPlayer->GetTransform() == nullptr)
        return false;

    XMStoreFloat3(&vOutPlayerPos,pPlayer->GetTransform()->Get_State(STATE::POSITION));

    return true;
}

_float BossPattern_Page2::Compute_Distance_To_Player()
{
    auto pOwner = m_pOwner.lock();
    auto pPlayer = Find_Player();

    if (pOwner == nullptr || pPlayer == nullptr)
        return FLT_MAX;

    _vector vBoss = pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector vPlayer = pPlayer->GetTransform()->Get_State(STATE::POSITION);

    return XMVectorGetX(XMVector3Length(vPlayer - vBoss));
}

_float3 BossPattern_Page2::Compute_Direction_To_Player()
{
    _float3 vResult = { 0.f, 0.f, 1.f };

    auto pOwner = m_pOwner.lock();
    auto pPlayer = Find_Player();

    if (pOwner == nullptr || pPlayer == nullptr)
        return vResult;

    _vector vBoss = pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector vPlayer = pPlayer->GetTransform()->Get_State(STATE::POSITION);

    _vector vDir = vPlayer - vBoss;
    vDir = XMVectorSetY(vDir, 0.f);

    if (XMVectorGetX(XMVector3LengthSq(vDir)) <= 0.000001f)
        return vResult;

    vDir = XMVector3Normalize(vDir);

    XMStoreFloat3(&vResult, vDir);

    return vResult;
}

void BossPattern_Page2::Damage_Player_If_Close(_float fRange, _float fDamage)
{
    auto pPlayerObject = Find_Player();

    if (pPlayerObject == nullptr)
        return;

    auto pPlayer = dynamic_pointer_cast<Player>(pPlayerObject);

    if (pPlayer == nullptr)
        return;

    _float fDistance = Compute_Distance_To_Player();

    if (fDistance > fRange)
        return;

    _float3 vHitPos{};

    if (!Get_PlayerPos(vHitPos))
        return;

    pPlayer->Take_Damage(fDamage, vHitPos);
}

void BossPattern_Page2::Damage_Player_If_InFront( _float fRange, _float fAngleDegree,_float fDamage)
{
    auto pOwner = m_pOwner.lock();
    auto pPlayerObject = Find_Player();

    if (pOwner == nullptr || pPlayerObject == nullptr)
        return;

    auto pPlayer = dynamic_pointer_cast<Player>(pPlayerObject);

    if (pPlayer == nullptr)
        return;

    _vector vBossPos = pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector vPlayerPos = pPlayerObject->GetTransform()->Get_State(STATE::POSITION);

    _vector vToPlayer = vPlayerPos - vBossPos;
    vToPlayer = XMVectorSetY(vToPlayer, 0.f);

    _float fDistance = XMVectorGetX(XMVector3Length(vToPlayer));

    if (fDistance > fRange)
        return;

    if (fDistance <= 0.0001f)
        return;

    vToPlayer = XMVector3Normalize(vToPlayer);

    _vector vLook = pOwner->GetTransform()->Get_State(STATE::LOOK);
    vLook = XMVectorSetY(vLook, 0.f);

    if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
        return;

    vLook = XMVector3Normalize(vLook);

    _float fDot = XMVectorGetX(XMVector3Dot(vLook, vToPlayer));
    fDot = max(-1.f, min(1.f, fDot));

    _float fAngle = XMConvertToDegrees(acosf(fDot));

    if (fAngle > fAngleDegree)
        return;

    _float3 vHitPos{};
    XMStoreFloat3(&vHitPos, vPlayerPos);

    pPlayer->Take_Damage(fDamage, vHitPos);
}

void BossPattern_Page2::Lending_On_Floor(_float fTimeDelta)
{
}

void BossPattern_Page2::Change_BossState(int iState)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pFSM = dynamic_pointer_cast<BossMonster_Page2FSM>(pOwner->Get_FSM());

    if (pFSM == nullptr)
        return;

    pFSM->Change_State(static_cast<BossMonster_Page2FSM::BOSS_STATE>(iState));
}


shared_ptr<GameObject> BossPattern_Page2::Find_PlayerCamera()
{
    return CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag",L"Player_Camera");
}

shared_ptr<GameObject> BossPattern_Page2::Find_BossCutSceneCamera()
{
    return CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(),L"CameraTag",L"Boss_CutScene_Camera");
}

void BossPattern_Page2::Setup_AppearCutSceneCamera()
{
    auto pOwner = m_pOwner.lock();
    if (pOwner == nullptr || pOwner->GetTransform() == nullptr)
        return;

    auto pPlayerCamera = Find_PlayerCamera();
    auto pCutCamera = Find_BossCutSceneCamera();

    if (pPlayerCamera == nullptr || pPlayerCamera->GetTransform() == nullptr)
        return;

    if (pCutCamera == nullptr || pCutCamera->GetTransform() == nullptr)
        return;

    auto pPlayerCamTransform = pPlayerCamera->GetTransform();
    auto pCutCamTransform = pCutCamera->GetTransform();


    pCutCamTransform->Set_State(STATE::POSITION, { 444.f,41.f, 256.f });
    XMStoreFloat3(&m_vCutCamBossViewPos, pCutCamTransform->Get_State(STATE::POSITION) );


    XMStoreFloat3(&m_vCutCamStartPos, pPlayerCamTransform->Get_State(STATE::POSITION) );
    

    pCutCamTransform->Set_State( STATE::RIGHT,  pPlayerCamTransform->Get_State(STATE::RIGHT) );

    pCutCamTransform->Set_State(STATE::UP, pPlayerCamTransform->Get_State(STATE::UP));

    pCutCamTransform->Set_State( STATE::LOOK, pPlayerCamTransform->Get_State(STATE::LOOK));

    pCutCamTransform->Set_State( STATE::POSITION,pPlayerCamTransform->Get_State(STATE::POSITION));


    CGameInstance::Get().Change_Camera(ETOUI(CAMERA::SUB2));

    m_bCutCamReady = true;
}

_bool BossPattern_Page2::Update_CutSceneCamera_MoveToBoss(_float fTimeDelta)
{
    if (false == m_bCutCamReady)
        return false;

    auto pCutCamera = Find_BossCutSceneCamera();

    if (pCutCamera == nullptr || pCutCamera->GetTransform() == nullptr)
        return false;

    auto pTransform = pCutCamera->GetTransform();

    _vector vCurPos = pTransform->Get_State(STATE::POSITION);

    _vector vTargetPos = XMVectorSet( m_vCutCamBossViewPos.x, m_vCutCamBossViewPos.y, m_vCutCamBossViewPos.z, 1.f);

    _vector vDir = vTargetPos - vCurPos;

    _float fDistance = XMVectorGetX(XMVector3Length(vDir));

    if (fDistance <= 1.f)
    {
        pTransform->Set_State(STATE::POSITION, vTargetPos);
        return true;
    }

    vDir = XMVector3Normalize(vDir);

    pTransform->Move(vDir, fTimeDelta, 1.f);

    return false;
}

void BossPattern_Page2::Update_CutSceneCamera_ReturnToPlayer(_float fTimeDelta)
{
    auto pCutCamera = Find_BossCutSceneCamera();
    auto pPlayerCamera = Find_PlayerCamera();

    if (pCutCamera == nullptr || pCutCamera->GetTransform() == nullptr)
        return;

    if (pPlayerCamera == nullptr || pPlayerCamera->GetTransform() == nullptr)
        return;

    XMStoreFloat3( &m_vCutCamReturnTargetPos, pPlayerCamera->GetTransform()->Get_State(STATE::POSITION));

    _float fRatio = SmoothStepRatio(m_fStepTimer, m_fCutCamReturnTime);

    _float3 vCamPos = Lerp_Float3( m_vCutCamReturnStartPos,  m_vCutCamReturnTargetPos,  fRatio );

    pCutCamera->GetTransform()->Set_State(STATE::POSITION,XMVectorSet(vCamPos.x, vCamPos.y, vCamPos.z, 1.f) );

    // 복귀 중에는 Player Camera가 바라보는 방향을 따라가게 함
    _vector vPlayerCamLook = pPlayerCamera->GetTransform()->Get_State(STATE::LOOK);
    _vector vAt = XMLoadFloat3(&vCamPos) + XMVector3Normalize(vPlayerCamLook) * 10.f;

    _float3 vLookAt{};
    XMStoreFloat3(&vLookAt, vAt);

    Set_CameraLookAt( pCutCamera, vCamPos, vLookAt );
}

void BossPattern_Page2::Set_CameraLookAt( shared_ptr<GameObject> pCameraObject, const _float3& vEye, const _float3& vAt)
{
    if (pCameraObject == nullptr || pCameraObject->GetTransform() == nullptr)
        return;

    _vector vEyeVec = XMLoadFloat3(&vEye);
    _vector vAtVec = XMLoadFloat3(&vAt);

    _vector vLook = vAtVec - vEyeVec;

    if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
        return;

    vLook = XMVector3Normalize(vLook);

    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Cross(vUp, vLook);

    if (XMVectorGetX(XMVector3LengthSq(vRight)) <= 0.000001f)
        return;

    vRight = XMVector3Normalize(vRight);
    vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    auto pTransform = pCameraObject->GetTransform();

    pTransform->Set_State(STATE::RIGHT, vRight);
    pTransform->Set_State(STATE::UP, vUp);
    pTransform->Set_State(STATE::LOOK, vLook);
    pTransform->Set_State(STATE::POSITION, XMVectorSet(vEye.x, vEye.y, vEye.z, 1.f));
}

_float BossPattern_Page2::SmoothStepRatio(_float fTimer, _float fDuration)
{
    if (fDuration <= 0.f)
        return 1.f;

    _float fRatio = fTimer / fDuration;

    if (fRatio < 0.f)
        fRatio = 0.f;

    if (fRatio > 1.f)
        fRatio = 1.f;

    return fRatio * fRatio * (3.f - 2.f * fRatio);
}

_float3 BossPattern_Page2::Lerp_Float3(const _float3& vA,const _float3& vB,_float fRatio)
{
    _float3 vResult{};

    vResult.x = vA.x + (vB.x - vA.x) * fRatio;
    vResult.y = vA.y + (vB.y - vA.y) * fRatio;
    vResult.z = vA.z + (vB.z - vA.z) * fRatio;

    return vResult;
}

HRESULT BossPattern_Page2::Ready_LaserTrail()
{

    if (m_pLaserTrail != nullptr)
        return S_OK;


    auto pFindObject =CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(),TEXT("Effect"),TEXT("Boss_LaserTrail") );

    if (pFindObject != nullptr)
    {
        m_pLaserTrail = dynamic_pointer_cast<LaserTrail>(pFindObject);

        if (m_pLaserTrail == nullptr)
            return E_FAIL;

        m_pLaserTrail->Set_Active(false);

        return S_OK;
    }

    LaserTrail::LASERTRAIL_DESC Desc{};

    Desc.ObjectType = ETOUI(OBJECTTYPE::OBJECT_STATIC);
    Desc.m_strName = L"Boss_LaserTrail";
    Desc.m_strPrototypeObjectName = L"Prototype_GameObject_LaserTrail";
    Desc.m_strPrototypeBaseName = L"LaserTrail";
    Desc.pCameraType = ETOUI(CAMERA::NONE);
    Desc.fSpeedPerSec = 0.f;
    Desc.fRotationPerSec = 0.f;

    Desc.fLength = 35.f;
    Desc.fWidth = 0.35f;
    Desc.fFollowPower = 8.f;
    Desc.fWavePower = 0.8f;

    if (FAILED(CGameInstance::Get().Add_GameObject_toLayer(CGameInstance::Get().Get_Level(),TEXT("Prototype_GameObject_LaserTrail"),CGameInstance::Get().Get_Level(),TEXT("Effect"),&Desc)))
    {
        return E_FAIL;
    }

    pFindObject = CGameInstance::Get().Find_Object( CGameInstance::Get().Get_Level(),TEXT("Effect"),TEXT("Boss_LaserTrail") );

    if (pFindObject == nullptr)
        return E_FAIL;

    m_pLaserTrail =dynamic_pointer_cast<LaserTrail>(pFindObject);

    if (m_pLaserTrail == nullptr)
        return E_FAIL;

    m_pLaserTrail->Set_Active(false);

    return S_OK;
}

void BossPattern_Page2::Get_LaserStartAndDir(_float3& vOutStartPos,_float3& vOutDir)
{
    auto pOwner = m_pOwner.lock();


    auto& PartObjects = pOwner->GetPartObjectMap();

    auto pBossTransform = pOwner->GetTransform();

    _vector vBossPos = pBossTransform->Get_State(STATE::POSITION);

    _vector vLook = pBossTransform->Get_State(STATE::LOOK);

    vLook = XMVectorSetY(vLook, 0.f);

    if (XMVectorGetX(XMVector3LengthSq(vLook)) <= 0.000001f)
        vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    else
        vLook = XMVector3Normalize(vLook);

    _vector vStartPos = vBossPos + vLook * 2.5f +XMVectorSet(0.f, 3.f, 0.f, 0.f);

    XMStoreFloat3(&vOutStartPos, vStartPos);
    XMStoreFloat3(&vOutDir, vLook);
}

void BossPattern_Page2::Begin_BossLaser()
{
    if (m_pLaserTrail == nullptr)
    {
        if (FAILED(Ready_LaserTrail()))
            return;
    }

    _float3 vStartPos{};
    _float3 vDir{};

    Get_LaserStartAndDir(vStartPos, vDir);

    m_pLaserTrail->Set_Active(true);
    m_pLaserTrail->Update_Laser(vStartPos, vDir, 0.f);
}

void BossPattern_Page2::Update_BossLaser(_float fTimeDelta)
{
    if (m_pLaserTrail == nullptr)
        return;

    _float3 vStartPos{};
    _float3 vDir{};

    Get_LaserStartAndDir(vStartPos, vDir);

    m_pLaserTrail->Update_Laser(vStartPos,vDir,fTimeDelta);
}

void BossPattern_Page2::End_BossLaser()
{
    if (m_pLaserTrail == nullptr)
        return;

    m_pLaserTrail->Set_Active(false);
}


shared_ptr<BossPattern_Page2> BossPattern_Page2::Create(shared_ptr<BossMonster_Page2> pOwner)
{
    shared_ptr<BossPattern_Page2> pInstance = shared_ptr<BossPattern_Page2>(new BossPattern_Page2());

    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX("FAILED BossPattern_Page2");
        return nullptr;
    }

    return pInstance;
}

NS_END