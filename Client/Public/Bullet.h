#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


NS_BEGIN(Engine)

class Model;
class Shader;
class BaseCollider;
NS_END

NS_BEGIN(Client)
// 
class Bullet final : public GameObject
{

public:
	typedef struct tagBulletDesc : public GameObject::GAMEOBJECT_DESC
	{
		_float3 vStartPos = {};
		_float3 vDir = { 0.f, 0.f, 1.f };
		_float  fSpeed = 30.f;
	} BULLET_DESC;

private:
	_float3 m_vDir = { 0.f, 0.f, 1.f };
	_float  m_fSpeed = 30.f;
private:
	Bullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Bullet(const Bullet& Prototype);
public:
	virtual ~Bullet();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Ready_Components();


private:
	uint32_t			m_iData = {};

	_wstring			m_ModelComponentName;
private:

	shared_ptr<Model>			m_pModelCom = { nullptr };
	shared_ptr<Shader>			m_pShaderCom = { nullptr };



public:
	static unique_ptr<Bullet> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;


};

NS_END