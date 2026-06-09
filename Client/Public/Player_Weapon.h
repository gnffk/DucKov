#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class BaseCollider;
class Shader;
class Model;
NS_END


NS_BEGIN(Client)

class Player_Weapon final : public PartObject
{
public:
	typedef struct tagWeaponDesc : public PartObject::PARTOBJECT_DESC
	{
		const uint32_t* pParentState = { nullptr };
		const _float4x4* pSocketMatrix = { nullptr };
	}WEAPON_DESC;

private:
	Player_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Player_Weapon(const Player_Weapon& Prototype);
public:
	virtual ~Player_Weapon();

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
	//Gun2
	_float3 m_vLocalPos = { -0.06f, 0.15f, 0.27f };
	_float3 m_vLocalRot = { 360.f, -148.5f, -14.5f };
	_float3 m_vLocalScale = { 1.f, 1.f, 1.f };

	_float3 m_vMuzzleLocalPos = { 1.f, 0.f, -1.f };
	_float3 m_vMuzzleWorldPos = {};
	_float3 m_vBulletDir = { 0.f, 0.f, 1.f };

private:
	void Fire_Bullet();



private:
	HRESULT Ready_Components();
#ifdef _DEBUG
private:
	void GUI_PlayerWeapon();
#endif
public:
	static unique_ptr<Player_Weapon> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END