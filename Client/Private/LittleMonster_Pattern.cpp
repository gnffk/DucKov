#include "LittleMonster_Pattern.h"
#include "LittleMonster.h"
#include "GameInstance.h"
#include "LittleMonsterFSM.h"
#include "GameObject.h"
#include "LittleMonster_Weapon.h"



LittleMonster_Pattern::LittleMonster_Pattern()
{
}



HRESULT LittleMonster_Pattern::Initialize(shared_ptr<LittleMonster> pOwner)
{
    m_pOwner = pOwner;

    return S_OK;
}

void LittleMonster_Pattern::Update(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    if (m_fAttackTimer > 0.f)
        m_fAttackTimer -= fTimeDelta;

    if (m_fRollCoolTimer > 0.f)
        m_fRollCoolTimer -= fTimeDelta;

    if (m_fRollCheckTimer > 0.f)
        m_fRollCheckTimer -= fTimeDelta;

    auto pPlayer = CGameInstance::Get().Find_Object(
        CGameInstance::Get().Get_Level(),
        L"PlayerTag",
        L"Player"
    );

    PATTERN_STATE eNextState = PATTERN_STATE::ROAM;

    if (pPlayer == nullptr)
    {
        m_bAttacking = false;
        m_bRolling = false;

        eNextState = PATTERN_STATE::ROAM;
    }
    else if (m_bAttacking)
    {
        eNextState = PATTERN_STATE::ATTACK;
    }
    else if (m_bRolling)
    {
        eNextState = PATTERN_STATE::ROLL;
    }
    else
    {
        _float fDistance = Compute_Distance_To_Player(pPlayer);

        if (fDistance <= m_fAttackRange && m_fAttackTimer <= 0.f && Can_See_Player(pPlayer))
        {
            eNextState = PATTERN_STATE::ATTACK;
        }
        else if (fDistance <= m_fDetectRange)
        {
            if (Should_Roll(pPlayer))
                eNextState = PATTERN_STATE::ROLL;
            else {
                eNextState = PATTERN_STATE::CHASE;
            
            }
        }
        else
        {
            eNextState = PATTERN_STATE::ROAM;
        }
    }

    Change_PatternState(eNextState);

    switch (m_ePatternState)
    {
    case PATTERN_STATE::ROAM:
        Update_Roam(fTimeDelta);
        break;

    case PATTERN_STATE::CHASE:
        Update_Chase(fTimeDelta);
        break;

    case PATTERN_STATE::ATTACK:
        Update_Attack(fTimeDelta);
        break;

    case PATTERN_STATE::ROLL:
        Update_Roll(fTimeDelta);
        break;
    }
}
_bool LittleMonster_Pattern::Build_RoamPath()
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return false;

    auto pNavigation = pOwner->Get_Navigation();

    if (pNavigation == nullptr)
        return false;

    _vector vOwnerPosition = pOwner->GetTransform()->Get_State(STATE::POSITION);

    const float PI = 3.1415926535f;

    for (uint32_t i = 0; i < m_iRoamPathRetryCount; ++i)
    {
        float fAngle = static_cast<float>(rand() % 360) * PI / 180.f;

        float fRatio = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        float fDistance = m_fRoamMinDistance + (m_fRoamMaxDistance - m_fRoamMinDistance) * fRatio;

        _vector vRandomDir = XMVectorSet(cosf(fAngle), 0.f, sinf(fAngle), 0.f);

        _vector vGoalPosition = vOwnerPosition + vRandomDir * fDistance;

        vGoalPosition = XMVectorSetW(vGoalPosition, 1.f);

        vector<_float3> NewPath;

        if (pNavigation->Build_AStarPath(vOwnerPosition, vGoalPosition, NewPath))
        {
            if (!NewPath.empty())
            {
                m_PathPoints = NewPath;
                m_iCurrentPathIndex = 0;

                Smooth_Path();

                return true;
            }
        }
    }

    return false;
}

void LittleMonster_Pattern::Update_RoamWalkSound(_float fTimeDelta)
{
    auto pPlayer = CGameInstance::Get().Find_Object(
        CGameInstance::Get().Get_Level(),
        L"PlayerTag",
        L"Player"
    );

    if (pPlayer == nullptr)
    {
        Reset_RoamWalkSound();
        return;
    }

    _float fDistance = Compute_Distance_To_Player(pPlayer);

    if (fDistance > m_fRoamWalkSoundRange)
    {
        Reset_RoamWalkSound();
        return;
    }

    m_fRoamWalkSoundTimer -= fTimeDelta;

    if (m_fRoamWalkSoundTimer > 0.f)
        return;

    _float fRatio = fDistance / m_fRoamWalkSoundRange;

    if (fRatio < 0.f)
        fRatio = 0.f;

    if (fRatio > 1.f)
        fRatio = 1.f;

    // 가까울수록 1, 멀수록 0
    fRatio = 1.f - fRatio;

    _float fVolume = 0.15f + fRatio * 0.25f;

    CGameInstance::Get().PlaySoundOne( L"EFFECT_MONSTER_WALK",CHANNELID::EFFECT,fVolume);

    m_fRoamWalkSoundTimer = m_fRoamWalkSoundInterval;
}

void LittleMonster_Pattern::Reset_RoamWalkSound()
{
    m_fRoamWalkSoundTimer = 0.f;
}


void LittleMonster_Pattern::Update_Roam(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    if (m_bRoamIdle)
    {
        Change_LittleMonsterState(LittleMonsterFSM::LITTLEMONSTER_STATE::IDLE);

        Reset_RoamWalkSound();

        m_fRoamIdleTimer -= fTimeDelta;

        if (m_fRoamIdleTimer <= 0.f)
        {
            m_bRoamIdle = false;
            Clear_Path();
        }

        return;
    }

    if (!m_PathPoints.empty() && m_iCurrentPathIndex >= m_PathPoints.size())
    {
        m_bRoamIdle = true;
        m_fRoamIdleTimer = m_fRoamIdleTime;

        Change_LittleMonsterState(LittleMonsterFSM::LITTLEMONSTER_STATE::IDLE);

        Reset_RoamWalkSound();

        return;
    }

    if (m_PathPoints.empty())
    {
        if (!Build_RoamPath())
        {
            m_bRoamIdle = true;
            m_fRoamIdleTimer = 0.5f;

            Change_LittleMonsterState(LittleMonsterFSM::LITTLEMONSTER_STATE::IDLE);

            Reset_RoamWalkSound();

            return;
        }
    }

    Change_LittleMonsterState(LittleMonsterFSM::LITTLEMONSTER_STATE::WALK);

    Update_RoamWalkSound(fTimeDelta);

    Follow_Path(fTimeDelta, 0.5f);
}
void LittleMonster_Pattern::Update_Chase(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pPlayer = CGameInstance::Get().Find_Object(
        CGameInstance::Get().Get_Level(),
        L"PlayerTag",
        L"Player"
    );

    if (pPlayer == nullptr)
        return;

    auto pNavigation = pOwner->Get_Navigation();

    if (pNavigation == nullptr)
        return;

    _float fDistance = Compute_Distance_To_Player(pPlayer);

    _vector vOwnerPosition =
        pOwner->GetTransform()->Get_State(STATE::POSITION);

    _vector vPlayerPosition =
        pPlayer->GetTransform()->Get_State(STATE::POSITION);

    _float3 vPlayerPos{};
    XMStoreFloat3(&vPlayerPos, vPlayerPosition);

    // =====================================================
    // 공격 사거리 안인데 쿨타임 중이면 이동하지 말고 대기
    // 이 상태에서 쿨타임이 끝나면 Update()에서 ATTACK으로 다시 진입함
    // =====================================================
    if (fDistance <= m_fAttackRange && Can_See_Player(pPlayer))
    {
        Change_LittleMonsterState(
            LittleMonsterFSM::LITTLEMONSTER_STATE::IDLE);

        pOwner->Turn_To_Position(vPlayerPos, fTimeDelta);

        Clear_Path();

        return;
    }

    // =====================================================
    // 공격 사거리 밖이면 추적
    // =====================================================
    Change_LittleMonsterState(LittleMonsterFSM::LITTLEMONSTER_STATE::WALK);

  
    // 바로 갈 수 있으면 A* 없이 직선 추적
    if (Can_See_Player(pPlayer))
    {
        _vector vMoveDir = vPlayerPosition - vOwnerPosition;
        vMoveDir = XMVectorSetY(vMoveDir, 0.f);

        if (XMVectorGetX(XMVector3LengthSq(vMoveDir)) > 0.000001f)
        {
            vMoveDir = XMVector3Normalize(vMoveDir);

            _float3 vDir{};
            XMStoreFloat3(&vDir, vMoveDir);

            pOwner->Turn_To_Direction(vDir, fTimeDelta);
            pOwner->Move_Direction(vDir, fTimeDelta, 2.5f);
        }
        Reset_WalkSound();
        Clear_Path();

        return;
    }

    // 벽 등으로 바로 못 가면 A* 추적
    m_fPathRefreshTimer -= fTimeDelta;

    if (m_PathPoints.empty()
        || m_iCurrentPathIndex >= m_PathPoints.size()
        || m_fPathRefreshTimer <= 0.f)
    {
        m_fPathRefreshTimer = m_fPathRefreshTime;

        if (pNavigation->Build_AStarPath(
            vOwnerPosition,
            vPlayerPosition,
            m_PathPoints))
        {
            m_iCurrentPathIndex = 0;
            Smooth_Path();
        }
    }

    Follow_Path(fTimeDelta, 2.5f);
}
void LittleMonster_Pattern::Update_Attack(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pPlayer = CGameInstance::Get().Find_Object(
        CGameInstance::Get().Get_Level(),
        L"PlayerTag",
        L"Player"
    );

    if (pPlayer == nullptr)
    {
        m_bAttacking = false;

        Change_LittleMonsterState(
            LittleMonsterFSM::LITTLEMONSTER_STATE::IDLE);

        Change_PatternState(PATTERN_STATE::ROAM);
        return;
    }

    _vector vPlayerPosition =
        pPlayer->GetTransform()->Get_State(STATE::POSITION);

    _float3 vPlayerPos{};
    XMStoreFloat3(&vPlayerPos, vPlayerPosition);

    // ATTACK 상태 진입
    Change_LittleMonsterState(
        LittleMonsterFSM::LITTLEMONSTER_STATE::ATTACK);

    pOwner->Turn_To_Position(vPlayerPos, fTimeDelta);

    // =====================================================
    // ATTACK 상태에 처음 들어온 순간 딱 1번만 실행
    // =====================================================
    if (!m_bAttacking)
    {
        m_bAttacking = true;

        // 공격 애니메이션이 유지될 시간
        m_fAttackMotionTimer = m_fAttackMotionTime;

        // 다음 공격 가능 시간
        m_fAttackTimer = m_fAttackCoolTime;

        Clear_Path();

        auto& weapon =
            pOwner->GetPartObjectMap()[TEXT("Part_Weapon")];

        if (weapon != nullptr)
        {
            static_pointer_cast<LittleMonster_Weapon>(weapon)->Attack();
        }
    }

    m_fAttackMotionTimer -= fTimeDelta;

    // =====================================================
    // 공격 애니메이션 시간이 끝나면 ATTACK 상태에서 빠져나옴
    // 쿨타임이 끝날 때까지 IDLE로 대기
    // =====================================================
    if (m_fAttackMotionTimer <= 0.f)
    {
        m_bAttacking = false;

        Change_LittleMonsterState(
            LittleMonsterFSM::LITTLEMONSTER_STATE::IDLE);

        Change_PatternState(PATTERN_STATE::CHASE);
    }
}
void LittleMonster_Pattern::Clear_Path()
{
    m_PathPoints.clear();
    m_iCurrentPathIndex = 0;
    m_fPathRefreshTimer = 0.f;
}

void LittleMonster_Pattern::Change_PatternState(PATTERN_STATE eNextState)
{
    if (m_ePatternState == eNextState)
        return;

    PATTERN_STATE ePrevState = m_ePatternState;

    m_ePatternState = eNextState;

    // 처음 CHASE로 진입한 순간에만 재생
    if (ePrevState != PATTERN_STATE::CHASE &&
        m_ePatternState == PATTERN_STATE::CHASE)
    {
        CGameInstance::Get().PlaySoundOne(
            L"EFFECT_LITTLE_MONSTERSOUND",
            EFFECT_LITTLEMONSTER_MOUSE,
            0.5f
        );
    }


    Clear_Path();

    if (m_ePatternState != PATTERN_STATE::ROAM &&
        m_ePatternState != PATTERN_STATE::CHASE)
    {
        //CGameInstance::Get().PlaySoundOne(L"EFFECT_LITTLE_MONSTERSOUND", EFFECT_LITTLEMONSTER_MOUSE, 0.5f);
        Reset_WalkSound();
    }

    if (m_ePatternState != PATTERN_STATE::ROAM)
    {
        m_bRoamIdle = false;
        m_fRoamIdleTimer = 0.f;
    }
}

void LittleMonster_Pattern::Change_LittleMonsterState(LittleMonsterFSM::LITTLEMONSTER_STATE eState)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pFSM = dynamic_pointer_cast<LittleMonsterFSM>(pOwner->GetFSM());

    if (pFSM == nullptr)
        return;

    pFSM->Change_State(eState);
}

void LittleMonster_Pattern::Follow_Path(_float fTimeDelta, _float fSpeedScale)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    if (m_PathPoints.empty())
        return;

    if (m_iCurrentPathIndex >= m_PathPoints.size())
        return;

    _vector vOwnerPosition = pOwner->GetTransform()->Get_State(STATE::POSITION);

    auto pNavigation = pOwner->Get_Navigation();

    if (pNavigation)
    {
        uint32_t iBestIndex = m_iCurrentPathIndex;

        for (uint32_t i = m_iCurrentPathIndex + 1; i < m_PathPoints.size(); ++i)
        {
            _vector vCheckPoint = XMLoadFloat3(&m_PathPoints[i]);

            if (pNavigation->Can_MoveStraight(vOwnerPosition, vCheckPoint, 0.5f))
            {
                iBestIndex = i;
            }
            else
            {
                break;
            }
        }

        m_iCurrentPathIndex = iBestIndex;
    }

    _float3 vTargetPoint = m_PathPoints[m_iCurrentPathIndex];

    _vector vTargetPosition = XMLoadFloat3(&vTargetPoint);

    _vector vMoveDir = vTargetPosition - vOwnerPosition;

    vMoveDir = XMVectorSetY(vMoveDir, 0.f);

    float fDistance = XMVectorGetX(XMVector3Length(vMoveDir));

    if (fDistance <= m_fWaypointReachRange)
    {
        ++m_iCurrentPathIndex;

        if (m_iCurrentPathIndex >= m_PathPoints.size())
            return;

        vTargetPoint = m_PathPoints[m_iCurrentPathIndex];

        vTargetPosition = XMLoadFloat3(&vTargetPoint);

        vMoveDir = vTargetPosition - vOwnerPosition;

        vMoveDir = XMVectorSetY(vMoveDir, 0.f);
    }

    if (XMVectorGetX(XMVector3LengthSq(vMoveDir)) <= 0.000001f)
        return;

    vMoveDir = XMVector3Normalize(vMoveDir);

    _float3 vDirection{};

    XMStoreFloat3(&vDirection, vMoveDir);

    pOwner->Turn_To_Direction(vDirection, fTimeDelta);

    pOwner->Move_Direction(vDirection, fTimeDelta, fSpeedScale);
}
_bool LittleMonster_Pattern::Can_See_Player(shared_ptr<GameObject> pPlayer)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr || pPlayer == nullptr)
        return false;

    auto pNavigation = pOwner->Get_Navigation();

    if (pNavigation == nullptr)
        return false;

    _vector vBossPosition = pOwner->GetTransform()->Get_State(STATE::POSITION);

    _vector vPlayerPosition = pPlayer->GetTransform()->Get_State(STATE::POSITION);

    return pNavigation->Can_MoveStraight(vBossPosition, vPlayerPosition, m_fSightCheckStep);
}

void LittleMonster_Pattern::Smooth_Path()
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pNavigation =
        pOwner->Get_Navigation();

    if (pNavigation == nullptr)
        return;

    if (m_PathPoints.size() <= 2)
        return;

    vector<_float3> SmoothPoints;

    _vector vAnchor =
        pOwner->GetTransform()->Get_State(STATE::POSITION);

    uint32_t iIndex = 0;

    while (iIndex < m_PathPoints.size())
    {
        uint32_t iBestIndex = iIndex;

        for (uint32_t i = iIndex; i < m_PathPoints.size(); ++i)
        {
            _vector vCheckPoint =
                XMLoadFloat3(&m_PathPoints[i]);

            if (pNavigation->Can_MoveStraight(
                vAnchor,
                vCheckPoint,
                0.5f))
            {
                iBestIndex = i;
            }
            else
            {
                break;
            }
        }

        SmoothPoints.push_back(
            m_PathPoints[iBestIndex]);

        vAnchor =
            XMLoadFloat3(&m_PathPoints[iBestIndex]);

        iIndex =
            iBestIndex + 1;
    }

    m_PathPoints =
        SmoothPoints;

    m_iCurrentPathIndex = 0;
}

_float LittleMonster_Pattern::Compute_Distance_To_Player(shared_ptr<GameObject > pPlayer)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr || pPlayer == nullptr)
        return FLT_MAX;

    _vector vBoss = pOwner->GetTransform()->Get_State(STATE::POSITION);

    _vector vPlayer = pPlayer->GetTransform()->Get_State(STATE::POSITION);

    return XMVectorGetX(XMVector3Length(vPlayer - vBoss));
}

_bool LittleMonster_Pattern::Should_Roll(shared_ptr<GameObject> pPlayer)
{
    if (pPlayer == nullptr)
        return false;

    if (m_fRollCoolTimer > 0.f)
        return false;

    if (m_fRollCheckTimer > 0.f)
        return false;

    m_fRollCheckTimer = m_fRollCheckInterval;

    _float fDistance = Compute_Distance_To_Player(pPlayer);

    // 너무 멀면 굳이 구르지 않음
    if (fDistance > m_fAttackRange * 2.0f)
        return false;

    _float fRandom = static_cast<_float>(rand()) / static_cast<_float>(RAND_MAX);

    return fRandom <= m_fRollChance;
}

void LittleMonster_Pattern::Start_Roll(shared_ptr<GameObject> pPlayer)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr || pPlayer == nullptr)
        return;

    _vector vOwnerPosition = pOwner->GetTransform()->Get_State(STATE::POSITION);
    _vector vPlayerPosition = pPlayer->GetTransform()->Get_State(STATE::POSITION);

    _vector vToPlayer = vPlayerPosition - vOwnerPosition;
    vToPlayer = XMVectorSetY(vToPlayer, 0.f);

    if (XMVectorGetX(XMVector3LengthSq(vToPlayer)) <= 0.000001f)
        vToPlayer = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    else
        vToPlayer = XMVector3Normalize(vToPlayer);

    _float3 vToPlayerFloat{};
    XMStoreFloat3(&vToPlayerFloat, vToPlayer);

    _float fSide = (rand() % 2 == 0) ? -1.f : 1.f;

    _vector vSideDir = XMVectorSet(
        -vToPlayerFloat.z * fSide,
        0.f,
        vToPlayerFloat.x * fSide,
        0.f
    );

    // 완전 옆구르기보다, 살짝 플레이어 방향도 섞어서 자연스럽게
    _vector vRollDir = XMVector3Normalize(vSideDir * 0.75f + vToPlayer * 0.25f);

    auto pNavigation = pOwner->Get_Navigation();

    if (pNavigation != nullptr)
    {
        _vector vProbePosition = vOwnerPosition + vRollDir * m_fRollProbeDistance;
        vProbePosition = XMVectorSetW(vProbePosition, 1.f);

        if (!pNavigation->Can_MoveStraight(vOwnerPosition, vProbePosition, 0.3f))
        {
            vSideDir *= -1.f;
            vRollDir = XMVector3Normalize(vSideDir * 0.75f + vToPlayer * 0.25f);
        }
    }

    XMStoreFloat3(&m_vRollDir, vRollDir);

    m_bRolling = true;
    m_fRollMotionTimer = m_fRollMotionTime;
    m_fRollCoolTimer = m_fRollCoolTime;

    Clear_Path();
}
void LittleMonster_Pattern::Update_Roll(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pPlayer = CGameInstance::Get().Find_Object(
        CGameInstance::Get().Get_Level(),
        L"PlayerTag",
        L"Player"
    );

    if (pPlayer == nullptr)
    {
        m_bRolling = false;
        Change_PatternState(PATTERN_STATE::ROAM);
        return;
    }

    if (!m_bRolling)
    {
        Start_Roll(pPlayer);
    }

    Change_LittleMonsterState(LittleMonsterFSM::LITTLEMONSTER_STATE::ROLL);

    pOwner->Turn_To_Direction(m_vRollDir, fTimeDelta);
    pOwner->Move_Direction(m_vRollDir, fTimeDelta, m_fRollSpeedScale);

    m_fRollMotionTimer -= fTimeDelta;

    if (m_fRollMotionTimer <= 0.f)
    {
        m_bRolling = false;
        Change_PatternState(PATTERN_STATE::CHASE);
    }
}

void LittleMonster_Pattern::Update_WalkSound(
    _float fTimeDelta,
    _float fInterval,
    _float fBaseVolume
)
{
    auto pPlayer = CGameInstance::Get().Find_Object(
        CGameInstance::Get().Get_Level(),
        L"PlayerTag",
        L"Player"
    );

    if (pPlayer == nullptr)
    {
        Reset_WalkSound();
        return;
    }

    _float fDistance = Compute_Distance_To_Player(pPlayer);

    // 플레이어가 너무 멀면 발소리 안 남
    if (fDistance > m_fWalkSoundRange)
    {
        Reset_WalkSound();
        return;
    }

    m_fWalkSoundTimer -= fTimeDelta;

    if (m_fWalkSoundTimer > 0.f)
        return;

    // 가까울수록 볼륨 증가
    _float fRatio = fDistance / m_fWalkSoundRange;

    if (fRatio < 0.f)
        fRatio = 0.f;

    if (fRatio > 1.f)
        fRatio = 1.f;

    fRatio = 1.f - fRatio;

    _float fVolume = fBaseVolume + fRatio * 0.25f;

    CGameInstance::Get().PlaySoundOne(
        L"EFFECT_MONSTER_WALK",
        CHANNELID::EFFECT,
        fVolume
    );

    m_fWalkSoundTimer = fInterval;
}

void LittleMonster_Pattern::Reset_WalkSound()
{
    m_fWalkSoundTimer = 0.f;
}




shared_ptr<LittleMonster_Pattern> LittleMonster_Pattern::Create(shared_ptr<LittleMonster> pOwner)
{
    shared_ptr<LittleMonster_Pattern> pInstance = shared_ptr<LittleMonster_Pattern>(new LittleMonster_Pattern());

    if (FAILED(pInstance->Initialize(pOwner))) {
        MSG_BOX("FAILED LittleMonster_Pattern");
        return nullptr;
    }


    return pInstance;
}

