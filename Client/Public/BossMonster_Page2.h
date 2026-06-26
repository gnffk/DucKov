#pragma once

#include "Client_Defines.h"
#include "Monster.h"

NS_BEGIN(Engine)
class Navigation;
class BaseCollider;
NS_END

NS_BEGIN(Client)

class BossMonster_Page2 final : public Monster
{
public:
    typedef struct tagBossMonsterPage2Desc : public Monster::TestModelDec
    {
        _float3 vSpawnPos = {};
        _float3 vSpawnLook = { 0.f, 0.f, 1.f };
    } BOSSMONSTER_PAGE2_DESC;

private:
    BossMonster_Page2(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    BossMonster_Page2(const BossMonster_Page2& Prototype);

public:
    virtual ~BossMonster_Page2();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT Render_Shadow() override;
public:
    const _float4x4* Get_SocketMatrixPtr(const _char* pSocketName);

public:
    void Take_Damage(_float fDamage);
    void Take_Damage(_float fDamage, const _float3& vHitPos);

private:
    HRESULT Ready_PartObjects();
    HRESULT Ready_UI();
    HRESULT Update_Animation(_float fTimeDelta);

private:
    void SetCurrentNavMesh();
    void Update_HP_UI();
    void Spawn_BloodEffect(const _float3& vSpawnPos);
    _bool Collider_Bullet(_float fTimeDelta);

public:
    void Show_BossUI();

public:
    void Turn_To_Direction(const _float3& vDirection, _float fTimeDelta);
    void Turn_To_Position(const _float3& vPosition, _float fTimeDelta);
    void Move_Direction(const _float3& vDirection, _float fTimeDelta, _float fSpeedScale = 1.f);
    void Move_Forward(_float fTimeDelta, _float fSpeedScale = 1.f);


public:
	shared_ptr<class BossMonster_Page2FSM> Get_FSM() { return m_pMonsterFSM; }

private:
    uint32_t m_iState = {};
    _float nextAnimPlus = 1.f;
    uint32_t nextAnim = 0;

private:
    shared_ptr<class BossMonster_Page2FSM> m_pMonsterFSM = nullptr;
    shared_ptr<class BossPattern_Page2> m_pBossPattern = nullptr;
    shared_ptr<class Navigation> m_pNavigationCom = nullptr;
    shared_ptr<class Shader> m_pShaderBossCom = { nullptr };

private:
    map<string, shared_ptr<class GameObject>> m_pUI;

private:
    _float m_fMaxHP = 1800.f;
    _float m_fHP = 1800.f;
    _float m_fAttackPower = 20.f;

private:
    _bool m_bHit = false;
    _float m_fHitTime = 0.f;

public:
    static unique_ptr<BossMonster_Page2> Create(
        ComPtr<ID3D11Device> pDevice,
        ComPtr<ID3D11DeviceContext> pContext
    );

    virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END