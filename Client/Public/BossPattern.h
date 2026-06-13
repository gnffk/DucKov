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

private:

    _float Compute_Distance_To_Player(shared_ptr<class GameObject> pPlayer);

private:
    weak_ptr<BossMonster> m_pOwner;

    PATTERN_STATE m_ePatternState = PATTERN_STATE::ROAM;

private:
    _float m_fDetectRange = 15.f;
    _float m_fAttackRange = 3.f;

    _float m_fAttackCoolTime = 3.f;
    _float m_fAttackTimer = 0.f;

    _float m_fRoamChangeTime = 2.f;
    _float m_fRoamTimer = 0.f;

    _float3 m_vRoamDir = { 0.f, 0.f, 1.f };




public:
    static shared_ptr<BossPattern> Create(shared_ptr<class BossMonster> pOwner);

};

NS_END