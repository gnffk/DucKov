#include "BossPattern_Page2.h"
#include "BossMonster_Page2.h"
#include "BossMonster_Page2FSM.h"
#include "Boss_Weapon.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Player.h"
#include "Particle_System.h"

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
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    if (pOwner->Get_Dead())
        return;

    if (m_bRequestedDie)
    {
        if (m_eCurrentPattern != PATTERN_TYPE::DIE)
            Start_Pattern(PATTERN_TYPE::DIE);

        Update_Die(fTimeDelta);
        return;
    }

    if (m_eCurrentPattern == PATTERN_TYPE::NONE)
    {
        m_fPatternCoolTimer -= fTimeDelta;

        if (m_fPatternCoolTimer <= 0.f)
            Select_NextPattern();

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

    case PATTERN_TYPE::ROLL_ATTACK:
        Update_RollAttack(fTimeDelta);
        break;

    case PATTERN_TYPE::METEOR_PATTERN:
        Update_MeteorPattern(fTimeDelta);
        break;

    case PATTERN_TYPE::DIE:
        Update_Die(fTimeDelta);
        break;

    default:
        End_Pattern();
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
        else if (iRandom < 70)
            Start_Pattern(PATTERN_TYPE::ROLL_ATTACK);
        else
            Start_Pattern(PATTERN_TYPE::CHARGE_ATTACK);
    }
    else if (fDistance <= 12.f)
    {
        if (iRandom < 30)
            Start_Pattern(PATTERN_TYPE::CHARGE_ATTACK);
        else if (iRandom < 60)
            Start_Pattern(PATTERN_TYPE::ROLL_ATTACK);
        else if (iRandom < 85)
            Start_Pattern(PATTERN_TYPE::LASER_CANNON);
        else
            Start_Pattern(PATTERN_TYPE::WHIRLWIND_ATTACK);
    }
    else
    {
        if (iRandom < 45)
            Start_Pattern(PATTERN_TYPE::LASER_CANNON);
        else if (iRandom < 75)
            Start_Pattern(PATTERN_TYPE::METEOR_PATTERN);
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

    m_Meteors.clear();

    Get_PlayerPos(m_vLockedPlayerPos);

    m_vChargeDir = Compute_Direction_To_Player();

    switch (m_eCurrentPattern)
    {
    case PATTERN_TYPE::LASER_CANNON:
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::HAND_UP);
        break;

    case PATTERN_TYPE::CHARGE_ATTACK:
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::RUN);
        break;

    case PATTERN_TYPE::WHIRLWIND_ATTACK:
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::TPOSE);
        break;

    case PATTERN_TYPE::ROLL_ATTACK:
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::ROLL);
        break;

    case PATTERN_TYPE::METEOR_PATTERN:
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::HAND_UP);
        break;

    case PATTERN_TYPE::BEFORE_START:
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::TPOSE);
        break;

    case PATTERN_TYPE::START_APPEAR:
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::TPOSE);
        break;
    case PATTERN_TYPE::DIE:
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::TPOSE);
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

    m_bHasDamagedThisPattern = false;

    m_Meteors.clear();

    Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);

    // 다음 패턴까지 대기 시간
    m_fPatternCoolTimer = 0.8f + static_cast<_float>(rand() % 100) / 100.f * 0.8f;
}

void BossPattern_Page2::Update_Before_Start(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
        m_ePatternStep = PATTERN_STEP::START;
        m_fStepTimer = 0.f;
        m_fFireTimer = 0.f;

        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::TPOSE);
        break;
    }

}

void BossPattern_Page2::Update_Start_Appear(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
        m_ePatternStep = PATTERN_STEP::READY;
        m_fStepTimer = 0.f;
        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::TPOSE);
        break;

    case PATTERN_STEP::READY:
        Lending_On_Floor(fTimeDelta);

        if (m_fStepTimer >= m_fAppearReadyTime)
        {
            m_ePatternStep = PATTERN_STEP::ACTIVE;
            m_fStepTimer = 0.f;
        }

        break;

    case PATTERN_STEP::ACTIVE:
        m_ePatternStep = PATTERN_STEP::ACTIVE;
        m_fStepTimer = 0.f;

        m_eCurrentPattern = PATTERN_TYPE::NONE;
        m_ePatternStep = PATTERN_STEP::NONE;

        m_fPatternTimer = 0.f;
        m_fStepTimer = 0.f;
        m_fFireTimer = 0.f;
        m_fDamageTimer = 0.f;

        m_bHasDamagedThisPattern = false;

        m_Meteors.clear();

        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);

        // 다음 패턴까지 대기 시간
        m_fPatternCoolTimer = 20.f;
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
        m_ePatternStep = PATTERN_STEP::READY;
        m_fStepTimer = 0.f;
        m_fFireTimer = 0.f;

        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::HAND_UP);
        break;

    case PATTERN_STEP::READY:
        Turn_To_Player(fTimeDelta);

        if (m_fStepTimer >= m_fLaserReadyTime)
        {
            m_ePatternStep = PATTERN_STEP::ACTIVE;
            m_fStepTimer = 0.f;
            m_fFireTimer = 0.f;
        }
        break;

    case PATTERN_STEP::ACTIVE:
        Turn_To_Player(fTimeDelta);

        m_fFireTimer -= fTimeDelta;

        if (m_fFireTimer <= 0.f)
        {
            m_fFireTimer = m_fLaserFireInterval;

            // 현재는 Boss_Weapon의 Bullet 연속 발사로 레이저 포 느낌을 냄.
            // 나중에 Laser 객체가 있으면 여기서 Laser 생성으로 교체하면 됨.
            Fire_BossWeapon();

            // 레이저 직선상에 있으면 데미지
            Damage_Player_If_InFront(35.f, 10.f, 8.f);
        }

        if (m_fStepTimer >= m_fLaserActiveTime)
        {
            m_ePatternStep = PATTERN_STEP::RECOVER;
            m_fStepTimer = 0.f;
            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);
        }
        break;

    case PATTERN_STEP::RECOVER:
        if (m_fStepTimer >= m_fLaserRecoverTime)
            End_Pattern();
        break;
    }
}

void BossPattern_Page2::Update_ChargeAttack(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
        m_ePatternStep = PATTERN_STEP::READY;
        m_fStepTimer = 0.f;
        m_bHasDamagedThisPattern = false;

        Get_PlayerPos(m_vLockedPlayerPos);
        m_vChargeDir = Compute_Direction_To_Player();

        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::HAND_UP);
        break;

    case PATTERN_STEP::READY:
        Turn_To_Player(fTimeDelta);

        if (m_fStepTimer >= m_fChargeReadyTime)
        {
            m_ePatternStep = PATTERN_STEP::ACTIVE;
            m_fStepTimer = 0.f;

            m_vChargeDir = Compute_Direction_To_Player();

            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::RUN);
        }
        break;

    case PATTERN_STEP::ACTIVE:
        pOwner->Turn_To_Direction(m_vChargeDir, fTimeDelta);
        pOwner->Move_Direction(m_vChargeDir, fTimeDelta, m_fChargeSpeedScale);

        if (!m_bHasDamagedThisPattern)
        {
            _float fDistance = Compute_Distance_To_Player();

            if (fDistance <= m_fChargeDamageRange)
            {
                Damage_Player_If_Close(m_fChargeDamageRange, m_fChargeDamage);
                m_bHasDamagedThisPattern = true;
            }
        }

        if (m_fStepTimer >= m_fChargeActiveTime)
        {
            m_ePatternStep = PATTERN_STEP::RECOVER;
            m_fStepTimer = 0.f;
            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);
        }
        break;

    case PATTERN_STEP::RECOVER:
        if (m_fStepTimer >= m_fChargeRecoverTime)
            End_Pattern();
        break;
    }
}

void BossPattern_Page2::Update_WhirlwindAttack(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;
    m_fDamageTimer -= fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
        m_ePatternStep = PATTERN_STEP::ACTIVE;
        m_fStepTimer = 0.f;
        m_fDamageTimer = 0.f;

        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::ROLL);
        break;

    case PATTERN_STEP::ACTIVE:
    {
        _float3 vDir = Compute_Direction_To_Player();

        pOwner->Turn_To_Direction(vDir, fTimeDelta);
        pOwner->Move_Direction(vDir, fTimeDelta, m_fWhirlwindSpeedScale);

        // 회오리라서 근처에 있으면 주기적으로 데미지
        if (m_fDamageTimer <= 0.f)
        {
            m_fDamageTimer = m_fWhirlwindDamageInterval;
            Damage_Player_If_Close(m_fWhirlwindDamageRange, m_fWhirlwindDamage);
        }

        if (m_fStepTimer >= m_fWhirlwindActiveTime)
        {
            m_ePatternStep = PATTERN_STEP::RECOVER;
            m_fStepTimer = 0.f;

            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);
        }
    }
    break;

    case PATTERN_STEP::RECOVER:
        if (m_fStepTimer >= m_fWhirlwindRecoverTime)
            End_Pattern();
        break;
    }
}

void BossPattern_Page2::Update_RollAttack(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
        m_ePatternStep = PATTERN_STEP::READY;
        m_fStepTimer = 0.f;
        m_bHasDamagedThisPattern = false;

        m_vChargeDir = Compute_Direction_To_Player();

        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::ROLL);
        break;

    case PATTERN_STEP::READY:
        Turn_To_Player(fTimeDelta);

        if (m_fStepTimer >= m_fRollReadyTime)
        {
            m_ePatternStep = PATTERN_STEP::ACTIVE;
            m_fStepTimer = 0.f;

            m_vChargeDir = Compute_Direction_To_Player();
        }
        break;

    case PATTERN_STEP::ACTIVE:
        pOwner->Turn_To_Direction(m_vChargeDir, fTimeDelta);
        pOwner->Move_Direction(m_vChargeDir, fTimeDelta, m_fRollSpeedScale);

        if (!m_bHasDamagedThisPattern)
        {
            _float fDistance = Compute_Distance_To_Player();

            if (fDistance <= m_fRollDamageRange)
            {
                Damage_Player_If_Close(m_fRollDamageRange, m_fRollDamage);
                m_bHasDamagedThisPattern = true;
            }
        }

        if (m_fStepTimer >= m_fRollActiveTime)
        {
            m_ePatternStep = PATTERN_STEP::RECOVER;
            m_fStepTimer = 0.f;
            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);
        }
        break;

    case PATTERN_STEP::RECOVER:
        if (m_fStepTimer >= m_fRollRecoverTime)
            End_Pattern();
        break;
    }
}

void BossPattern_Page2::Update_MeteorPattern(_float fTimeDelta)
{
    m_fPatternTimer += fTimeDelta;
    m_fStepTimer += fTimeDelta;
    m_fFireTimer -= fTimeDelta;

    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    switch (m_ePatternStep)
    {
    case PATTERN_STEP::START:
        m_ePatternStep = PATTERN_STEP::READY;
        m_fStepTimer = 0.f;

        m_Meteors.clear();

        Change_BossState(BossMonster_Page2FSM::BOSS_STATE::RUN);
        break;

    case PATTERN_STEP::READY:
        // 플레이어 반대 방향으로 멀리 이동
        Move_Away_From_Player(fTimeDelta, 2.0f);

        if (m_fStepTimer >= m_fMeteorMoveAwayTime)
        {
            m_ePatternStep = PATTERN_STEP::ACTIVE;
            m_fStepTimer = 0.f;
            m_fFireTimer = 0.f;

            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::HAND_UP);
        }
        break;

    case PATTERN_STEP::ACTIVE:
    {
        Turn_To_Player(fTimeDelta);

        // 처음 1초 동안 커짐
        if (m_fStepTimer <= m_fMeteorGrowTime)
        {
            _float fRatio = m_fStepTimer / m_fMeteorGrowTime;
            fRatio = max(0.f, min(1.f, fRatio));

            _float fScale = 1.f + fRatio * 1.5f;

            // Transform에 Set_Scale이 있으면 사용
            pOwner->GetTransform()->Set_Scale(fScale, fScale, fScale);
        }

        // 거대화 이후 메테오 생성
        if (m_fStepTimer >= m_fMeteorGrowTime)
        {
            m_fFireTimer -= fTimeDelta;

            if (m_fFireTimer <= 0.f)
            {
                m_fFireTimer = m_fMeteorSpawnInterval;

                _float3 vPlayerPos{};

                if (Get_PlayerPos(vPlayerPos))
                {
                    // 약간 랜덤하게 주변에 떨어뜨림
                    _float fRandX = (static_cast<_float>(rand() % 200) / 100.f - 1.f) * 2.5f;
                    _float fRandZ = (static_cast<_float>(rand() % 200) / 100.f - 1.f) * 2.5f;

                    METEOR_DESC Meteor{};
                    Meteor.vTargetPos = {
                        vPlayerPos.x + fRandX,
                        vPlayerPos.y,
                        vPlayerPos.z + fRandZ
                    };
                    Meteor.fTimer = m_fMeteorDropDelay;
                    Meteor.bHit = false;

                    m_Meteors.push_back(Meteor);

                    // 경고 이펙트 느낌
                    Particle_System::PARTICLE_SPAWN_DESC Desc{};
                    Desc.vSpawnPos = Meteor.vTargetPos;
                    Desc.iCount = 15;
                    Desc.fPower = 0.4f;

                    CGameInstance::Get().Add_Particle(PARTICLE_TYPE::BLOOD, &Desc);
                }
            }
        }

        // 메테오 낙하 처리
        for (auto& Meteor : m_Meteors)
        {
            if (Meteor.bHit)
                continue;

            Meteor.fTimer -= fTimeDelta;

            if (Meteor.fTimer <= 0.f)
            {
                Meteor.bHit = true;

                Particle_System::PARTICLE_SPAWN_DESC Desc{};
                Desc.vSpawnPos = Meteor.vTargetPos;
                Desc.iCount = 60;
                Desc.fPower = 2.5f;

                CGameInstance::Get().Add_Particle(PARTICLE_TYPE::BLOOD, &Desc);

                _float3 vPlayerPos{};

                if (Get_PlayerPos(vPlayerPos))
                {
                    _vector vA = XMLoadFloat3(&Meteor.vTargetPos);
                    _vector vB = XMLoadFloat3(&vPlayerPos);

                    _float fDistance = XMVectorGetX(XMVector3Length(vB - vA));

                    if (fDistance <= m_fMeteorDamageRange)
                    {
                        Damage_Player_If_Close(999.f, m_fMeteorDamage);
                    }
                }
            }
        }

        if (m_fStepTimer >= m_fMeteorCastTime)
        {
            m_ePatternStep = PATTERN_STEP::RECOVER;
            m_fStepTimer = 0.f;

            // 원래 크기로 복구
            pOwner->GetTransform()->Set_Scale(1.f, 1.f, 1.f);

            Change_BossState(BossMonster_Page2FSM::BOSS_STATE::IDLE);
        }
    }
    break;

    case PATTERN_STEP::RECOVER:
        if (m_fStepTimer >= m_fMeteorRecoverTime)
            End_Pattern();
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