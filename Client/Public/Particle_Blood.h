
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Particle_Rect;
NS_END


NS_BEGIN(Client)

class Particle_Blood final : public GameObject
{
public:
	typedef struct tagParticleBloodDesc : public GameObject::GAMEOBJECT_DESC
	{
		_float3 vSpawnPos = {};
	} PARTICLE_BLOOD_DESC;

private:
	Particle_Blood(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Particle_Blood(const Particle_Blood& Prototype);
public:
	virtual ~Particle_Blood();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<VIBuffer_Particle_Rect>	m_pVIBufferCom = { nullptr };
	shared_ptr<Texture>				m_pTextureCom = { nullptr };
	shared_ptr<Shader>				m_pShaderCom = { nullptr };
	vector<_float3>					m_vVelocity;
	vector<_float4>					m_vStartPosition;
private:
	HRESULT Ready_Components();

	void Reset_Blood();

public:

	static unique_ptr<Particle_Blood> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END