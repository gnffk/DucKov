#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class BossMonster : public Monster
{
public:
	typedef struct tagBossMonsterDesc : public Monster::TestModelDec
	{
		_float		fMaxHP = 1000.f;
		_float		fAttackPower = 100.f;
		const uint32_t* pParentState = { nullptr };
		wstring		strModelPrototypeName;
	
	} BOSSMONSTER_DESC;


private:
	BossMonster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	BossMonster(const BossMonster& Prototype);

public:
	virtual ~BossMonster();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Turn_To_Direction(const _float3& vDirection, _float fTimeDelta);
	void Turn_To_Position(const _float3& vPosition, _float fTimeDelta);

	void Move_Direction(const _float3& vDirection, _float fTimeDelta, _float fSpeedScale = 1.f);
	void Move_Forward(_float fTimeDelta, _float fSpeedScale = 1.f);

public:

	const _float4x4* Get_SocketMatrixPtr(const _char* pSocketName);

public:
	void	SetCurrentNavMesh();
public:
	shared_ptr<Navigation> Get_Navigation() { return m_pNavigationCom; }
private:
	HRESULT Update_Animation(_float fTimeDelta);

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_UI();


private:
	void Spawn_BossPage2();
	void Last_Performance();
public:
	void Take_Damage(_float fDamage);
	void Take_Damage(_float fDamage, const _float3& vHitPos);
	void Update_HP_UI();
	void Spawn_BloodEffect(const _float3& vSpawnPos);

private:
	_bool Collider_Bullet(_float fTimeDelta);

public:
	shared_ptr<class FSM> GetFSM() { return m_pMonsterFSM; }
private:
	shared_ptr<class BossPattern> m_pBossPattern = { nullptr };
	shared_ptr<class FSM> m_pMonsterFSM = { nullptr };
	shared_ptr<class Navigation> m_pNavigationCom = { nullptr };
	shared_ptr<class Shader> m_pShaderBossCom = { nullptr };

	map<string, shared_ptr<class GameObject>> m_pUI;
	_float		m_fMaxHP = 100.f;
	_float		m_fHP = 100.f;
	_float		m_fAttackPower = 100.f;
	_float		m_fPatternRatio = 0.f;
	_bool		m_bLastPattern = false;
	_float      m_fPerformanceMove = 0.f;
	_bool		m_bNextPattern = false;	
private:
	uint32_t			m_iState = {};

	uint32_t nextAnim = 0;
	float    nextAnimPlus{ 1 };


public:
	static unique_ptr<BossMonster> Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);

	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END