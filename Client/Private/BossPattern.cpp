#include "BossPattern.h"
#include "BossMonster.h"
#include "GameInstance.h"
#include "BossMonsterFSM.h"
#include "GameObject.h"
#include "Boss_Weapon.h"



BossPattern::BossPattern()
{
}



HRESULT BossPattern::Initialize(shared_ptr<BossMonster> pOwner)
{
    m_pOwner = pOwner;

    return S_OK;
}

void BossPattern::Update(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    // 죽었으면 ROAM/CHASE/ATTACK 판단하지 말고 바로 DIE만 처리
    if (m_bDead == true)
    {
        Change_PatternState(PATTERN_STATE::DIE);
        Update_DIE(fTimeDelta);
        return;
    }

    if (m_fAttackTimer > 0.f)
        m_fAttackTimer -= fTimeDelta;

    auto pPlayer = CGameInstance::Get().Find_Object(
        CGameInstance::Get().Get_Level(),
        L"PlayerTag",
        L"Player"
    );

    PATTERN_STATE eNextState = PATTERN_STATE::ROAM;

    if (pPlayer == nullptr)
    {
        eNextState = PATTERN_STATE::ROAM;
    }
    else
    {
        _float fDistance = Compute_Distance_To_Player(pPlayer);

        if (fDistance <= m_fAttackRange && m_fAttackTimer <= 0.f)
        {
            eNextState = PATTERN_STATE::ATTACK;
        }
        else if (fDistance <= m_fDetectRange)
        {
            eNextState = PATTERN_STATE::CHASE;
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

    case PATTERN_STATE::DIE:
        Update_DIE(fTimeDelta);
        break;
    }
}

_bool BossPattern::Build_RoamPath()
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

void BossPattern::Update_Roam(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    // =====================================================
    // 목적지 도착 후 잠깐 IDLE
    // =====================================================
    if (m_bRoamIdle)
    {
        Change_BossState(BossMonsterFSM::BOSS_STATE::IDLE);

        m_fRoamIdleTimer -= fTimeDelta;

        if (m_fRoamIdleTimer <= 0.f)
        {
            m_bRoamIdle = false;
            Clear_Path();
        }

        return;
    }

    // =====================================================
    // 경로가 끝났으면 IDLE 대기 시작
    // =====================================================
    if (!m_PathPoints.empty() &&m_iCurrentPathIndex >= m_PathPoints.size())
    {
        m_bRoamIdle = true;
        m_fRoamIdleTimer = m_fRoamIdleTime;

        Change_BossState(BossMonsterFSM::BOSS_STATE::IDLE);

        return;
    }

    // =====================================================
    // 경로가 없으면 랜덤 목적지까지 A* 경로 생성
    // =====================================================
    if (m_PathPoints.empty())
    {
        if (!Build_RoamPath())
        {
            // 경로 생성 실패 시 잠깐 IDLE 후 재시도
            m_bRoamIdle = true;
            m_fRoamIdleTimer = 0.5f;

            Change_BossState( BossMonsterFSM::BOSS_STATE::IDLE);

            return;
        }
    }

    Change_BossState( BossMonsterFSM::BOSS_STATE::HAND_UP_AND_WALK);

    Follow_Path(fTimeDelta, 0.5f);
}

void BossPattern::Update_Chase(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pPlayer = CGameInstance::Get().Find_Object( CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");

    if (pPlayer == nullptr)
        return;

    auto pNavigation = pOwner->Get_Navigation();

    if (pNavigation == nullptr)
        return;

    _float fDistance = Compute_Distance_To_Player(pPlayer);

    _vector vBossPosition = pOwner->GetTransform()->Get_State(STATE::POSITION);

    _vector vPlayerPosition = pPlayer->GetTransform()->Get_State(STATE::POSITION);

    _float3 vPlayerPos{};
    XMStoreFloat3(&vPlayerPos, vPlayerPosition);

    // =====================================================
    // 총 쏠 수 있는 거리 + Player가 보이면
    // 이동보다 조준이 우선
    // =====================================================
    if (fDistance <= m_fShootRange && Can_See_Player(pPlayer))
    {
        Change_BossState( BossMonsterFSM::BOSS_STATE::HAND_UP);

        pOwner->Turn_To_Position( vPlayerPos, fTimeDelta);

        Clear_Path();

        return;
    }

    // =====================================================
    // 아직 안 보이거나 거리가 멀면 A* 추적
    // =====================================================
    Change_BossState(BossMonsterFSM::BOSS_STATE::RUN);

    m_fPathRefreshTimer -= fTimeDelta;

    if (m_PathPoints.empty() || m_iCurrentPathIndex >= m_PathPoints.size() || m_fPathRefreshTimer <= 0.f)
    {
        m_fPathRefreshTimer = m_fPathRefreshTime;

        if (pNavigation->Build_AStarPath( vBossPosition, vPlayerPosition, m_PathPoints))
        {
            m_iCurrentPathIndex = 0;

            Smooth_Path();
        }
    }

    Follow_Path(fTimeDelta, 1.5f);
}

void BossPattern::Clear_Path()
{
    m_PathPoints.clear();
    m_iCurrentPathIndex = 0;
    m_fPathRefreshTimer = 0.f;
}

void BossPattern::Change_PatternState(PATTERN_STATE eNextState)
{
    if (m_ePatternState == eNextState)
        return;
    PATTERN_STATE ePrevState = m_ePatternState;


    m_ePatternState = eNextState;

    // ROAM / ATTACK 등에서 CHASE로 처음 들어온 순간에만 재생
    if (ePrevState == PATTERN_STATE::ROAM &&
        m_ePatternState == PATTERN_STATE::CHASE)
    {
        CGameInstance::Get().PlaySoundOne(
            L"EFFECT_BOSSSOUND",
            EFFECT_BOSSMONSTER_MOUSE,
            0.5f
        );

    }
    Clear_Path();

    if (m_ePatternState != PATTERN_STATE::ROAM)
    {
        m_bRoamIdle = false;
        m_fRoamIdleTimer = 0.f;
    }
}

void BossPattern::Change_BossState(BossMonsterFSM::BOSS_STATE eState)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pFSM =dynamic_pointer_cast<BossMonsterFSM>(pOwner->GetFSM());

    if (pFSM == nullptr)
        return;

    pFSM->Change_State(eState);
}

void BossPattern::Update_Attack(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pPlayer = CGameInstance::Get().Find_Object( CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");

    if (pPlayer == nullptr)
        return;

    _vector vPlayerPosition = pPlayer->GetTransform()->Get_State(STATE::POSITION);

    _float3 vPlayerPos{};
    XMStoreFloat3(&vPlayerPos, vPlayerPosition);

    Change_BossState( BossMonsterFSM::BOSS_STATE::HAND_UP);

    pOwner->Turn_To_Position( vPlayerPos, fTimeDelta);

    if (!m_bAttacking)
    {
        m_bAttacking = true;
        m_fAttackMotionTimer = m_fAttackMotionTime;
        m_fAttackTimer = m_fAttackCoolTime;

        Clear_Path();

        auto& weapon = pOwner->GetPartObjectMap()[TEXT("Part_Weapon")];

        static_pointer_cast<Boss_Weapon>(weapon)->Fire_Bullet();


    }

    m_fAttackMotionTimer -= fTimeDelta;

    if (m_fAttackMotionTimer <= 0.f)
    {
        m_bAttacking = false;
        m_ePatternState = PATTERN_STATE::CHASE;
    }
}
void BossPattern::Update_DIE(_float fTimeDelta)
{
    Change_BossState(BossMonsterFSM::BOSS_STATE::TPOSE);
}
void BossPattern::Follow_Path(_float fTimeDelta,_float fSpeedScale)
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

            if (pNavigation->Can_MoveStraight( vOwnerPosition, vCheckPoint,  0.5f))
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

    _float3 vTargetPoint =  m_PathPoints[m_iCurrentPathIndex];

    _vector vTargetPosition = XMLoadFloat3(&vTargetPoint);

    _vector vMoveDir =vTargetPosition - vOwnerPosition;

    vMoveDir =XMVectorSetY(vMoveDir, 0.f);

    float fDistance =XMVectorGetX( XMVector3Length(vMoveDir));

    if (fDistance <= m_fWaypointReachRange)
    {
        ++m_iCurrentPathIndex;

        if (m_iCurrentPathIndex >= m_PathPoints.size())
            return;

        vTargetPoint = m_PathPoints[m_iCurrentPathIndex];

        vTargetPosition = XMLoadFloat3(&vTargetPoint);

        vMoveDir =  vTargetPosition - vOwnerPosition;

        vMoveDir = XMVectorSetY(vMoveDir, 0.f);
    }

    if (XMVectorGetX(XMVector3LengthSq(vMoveDir)) <= 0.000001f)
        return;

    vMoveDir =  XMVector3Normalize(vMoveDir);

    _float3 vDirection{};

    XMStoreFloat3(&vDirection, vMoveDir);

    pOwner->Turn_To_Direction(vDirection,fTimeDelta);

    pOwner->Move_Direction(vDirection, fTimeDelta, fSpeedScale);
}
_bool BossPattern::Can_See_Player(shared_ptr<GameObject> pPlayer)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr || pPlayer == nullptr)
        return false;

    auto pNavigation = pOwner->Get_Navigation();

    if (pNavigation == nullptr)
        return false;

    _vector vBossPosition =pOwner->GetTransform()->Get_State(STATE::POSITION);

    _vector vPlayerPosition =pPlayer->GetTransform()->Get_State(STATE::POSITION);

    return pNavigation->Can_MoveStraight(vBossPosition,vPlayerPosition,m_fSightCheckStep);
}

void BossPattern::Smooth_Path()
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

_float BossPattern::Compute_Distance_To_Player(shared_ptr<GameObject > pPlayer)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr || pPlayer == nullptr)
        return FLT_MAX;

    _vector vBoss = pOwner->GetTransform()->Get_State(STATE::POSITION);

    _vector vPlayer = pPlayer->GetTransform()->Get_State(STATE::POSITION);

    return XMVectorGetX(XMVector3Length(vPlayer - vBoss));
}

shared_ptr<BossPattern> BossPattern::Create(shared_ptr<BossMonster> pOwner)
{
    shared_ptr<BossPattern> pInstance = shared_ptr<BossPattern>(new BossPattern());

    if (FAILED(pInstance->Initialize(pOwner))) {
        MSG_BOX("FAILED BossPattern");
        return nullptr;
    }


    return pInstance;
}

