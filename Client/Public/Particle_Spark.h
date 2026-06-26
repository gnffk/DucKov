#pragma once

#include "Client_Defines.h"
#include "Particle_System.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Particle_Rect;
NS_END

NS_BEGIN(Client)

class Particle_Spark final : public Particle_System
{
public:
	typedef struct tagParticlSparkDesc : public GameObject::GAMEOBJECT_DESC
	{
		_float3 vSpawnPos = {};
	} PARTICLE_SPARK_DESC;

private:
	Particle_Spark(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Particle_Spark(const Particle_Spark& Prototype);

public:
	virtual ~Particle_Spark();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Add_Particle(const PARTICLE_SPAWN_DESC& Desc) override;

private:
	HRESULT Ready_Components();

private:
	shared_ptr<VIBuffer_Particle_Rect>	m_pVIBufferCom = { nullptr };
	shared_ptr<Texture>					m_pTextureCom = { nullptr };
	shared_ptr<Texture>					m_pNoiseTextureCom = { nullptr };
	shared_ptr<Texture>					m_pMaskTextureCom = { nullptr };
	shared_ptr<Texture>					m_pNormalTextureCom = { nullptr };
	shared_ptr<Shader>					m_pShaderCom = { nullptr };
private:
	_float m_fTimeAcc = 0.f;

public:
	static unique_ptr<Particle_Spark> Create(
		ComPtr<ID3D11Device> pDevice,
		ComPtr<ID3D11DeviceContext> pContext
	);

	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END