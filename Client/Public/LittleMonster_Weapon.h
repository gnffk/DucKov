
#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class BaseCollider;
class Shader;
class Model;
NS_END


NS_BEGIN(Client)

class LittleMonster_Weapon final : public PartObject
{
public:
	typedef struct tagWeaponDesc : public PartObject::PARTOBJECT_DESC
	{
		const uint32_t* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
	}WEAPON_DESC;

private:
	LittleMonster_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	LittleMonster_Weapon(const LittleMonster_Weapon& Prototype);
public:
	virtual ~LittleMonster_Weapon();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	shared_ptr<Model>			m_pModelCom = { nullptr };
	shared_ptr<Shader>			m_pShaderCom = { nullptr };
	shared_ptr<BaseCollider>		m_pColliderCom = { nullptr };
private:
	const uint32_t* m_pParentState = { nullptr };
	const _float4x4* m_pSocketMatrix = { nullptr };
private:
	////Gun2
	_float3 m_vLocalPos = { -0.13f, 0.f, 0.075f };
	_float3 m_vLocalRot = { 171.f, -121.5f, -115.5f };
	_float3 m_vLocalScale = { 0.8f, 0.8f, 0.8f };

	_float3 m_vMuzzleLocalPos = { 1.f, 0.f, -1.f };
	_float3 m_vMuzzleWorldPos = {};
	_float3 m_vBulletDir = { 0.f, 0.f, 1.f };

#ifdef _DEBUG
private:
	void GUI_WeaponDebug();
	bool m_bShowWeaponDebug = true;
#endif
public:
	void Attack();

private:
	_float m_fFireCoolTime = 0.2f;
	_float m_fFireTimer = 0.f;

private:
	HRESULT Ready_Components();

public:
	static unique_ptr<LittleMonster_Weapon> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END