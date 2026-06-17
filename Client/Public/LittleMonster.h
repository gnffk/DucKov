
#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class LittleMonster : public Monster
{
public:
	typedef struct tagLittleMonsterDesc : public Monster::TestModelDec
	{
		_float		fMaxHP = 100.f;
		_float		fAttackPower = 10.f;
		const uint32_t* pParentState = { nullptr };
		wstring		strModelPrototypeName;

	} LITTLEMONSTER_DESC;

private:
	LittleMonster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	LittleMonster(const LittleMonster& Prototype);

public:
	virtual ~LittleMonster();

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
	void Spawn_DieBox();
private:
	_bool Collider_Bullet(_float fTimeDelta);
public:
	shared_ptr<class FSM> GetFSM() { return m_pMonsterFSM; }
private:
	shared_ptr<class LittleMonster_Pattern> m_pLittleMonsterPattern = { nullptr };
	shared_ptr<class FSM> m_pMonsterFSM = { nullptr };
	shared_ptr<class Navigation> m_pNavigationCom = { nullptr };
	map<string, shared_ptr<class GameObject>> m_pUI;

	_float		m_fMaxHP = 100.f;
	_float		m_fHP = 100.f;
	_float		m_fAttackPower = 10.f;

private:
	uint32_t			m_iState = {};

	uint32_t nextAnim = 0;
	float    nextAnimPlus{ 1 };

public:
	void Take_Damage(_float fDamage);
	void Update_HP_UI();
public:
	static unique_ptr<LittleMonster> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END