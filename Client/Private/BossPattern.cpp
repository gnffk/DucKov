#include "BossPattern.h"
#include "BossMonster.h"
#include "GameInstance.h"
#include "BossMonsterFSM.h"
#include "GameObject.h"



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

    if (m_fAttackTimer > 0.f)
        m_fAttackTimer -= fTimeDelta;

    auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");

    if (pPlayer == nullptr)
    {
        m_ePatternState = PATTERN_STATE::ROAM;
    }
    else
    {
        _float fDistance = Compute_Distance_To_Player(pPlayer);

        // 공격 트리거
        if (fDistance <= m_fAttackRange && m_fAttackTimer <= 0.f)
        {
            m_ePatternState = PATTERN_STATE::ATTACK;
        }
        // 추적 트리거
        else if (fDistance <= m_fDetectRange)
        {
            m_ePatternState = PATTERN_STATE::CHASE;
        }
        // 감지 범위 밖
        else
        {
            m_ePatternState = PATTERN_STATE::ROAM;
        }
    }

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
    }
}

void BossPattern::Update_Roam(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    dynamic_pointer_cast<BossMonsterFSM>(pOwner->GetFSM())->Change_State(BossMonsterFSM::BOSS_STATE::WALK);

    m_fRoamTimer -= fTimeDelta;

    if (m_fRoamTimer <= 0.f)
    {
        m_fRoamTimer = m_fRoamChangeTime;

        int iDir = rand() % 4;

        switch (iDir)
        {
        case 0:
            m_vRoamDir = { 0.f, 0.f, 1.f };
            break;

        case 1:
            m_vRoamDir = { 0.f, 0.f, -1.f };
            break;

        case 2:
            m_vRoamDir = { 1.f, 0.f, 0.f };
            break;

        case 3:
            m_vRoamDir = { -1.f, 0.f, 0.f };
            break;
        }
    }
    pOwner->Turn_To_Direction(m_vRoamDir, fTimeDelta);
    pOwner->Move_Forward(fTimeDelta, 0.5f);
}

void BossPattern::Update_Chase(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;

    auto pPlayer = CGameInstance::Get().Find_Object(CGameInstance::Get().Get_Level(), L"PlayerTag", L"Player");

    if (pPlayer == nullptr)
        return;

    dynamic_pointer_cast<BossMonsterFSM>(pOwner->GetFSM())->Change_State(BossMonsterFSM::BOSS_STATE::WALK);

    _float3 vPlayerPos;

    XMStoreFloat3(&vPlayerPos, pPlayer->GetTransform()->Get_State(STATE::POSITION));
    pOwner->Turn_To_Position(vPlayerPos, fTimeDelta);
    pOwner->Move_Forward(fTimeDelta, 1.f);
}

void BossPattern::Update_Attack(_float fTimeDelta)
{
    auto pOwner = m_pOwner.lock();

    if (pOwner == nullptr)
        return;
    dynamic_pointer_cast<BossMonsterFSM>(pOwner->GetFSM())->Change_State(BossMonsterFSM::BOSS_STATE::HAND_UP);

    m_fAttackTimer = m_fAttackCoolTime;
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

