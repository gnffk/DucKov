#pragma once

#include "Engine_Defines.h"
#include "Particle_System.h"

NS_BEGIN(Engine)

class Particle_Manager final
{
private:
	Particle_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	~Particle_Manager();

public:
	HRESULT Register_ParticleSystem(PARTICLE_TYPE eType,shared_ptr<Particle_System> pParticleSystem);

	HRESULT Add_Particle(PARTICLE_TYPE eType, void* pArg);

	void Clear();

private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

private:
	map<PARTICLE_TYPE, weak_ptr<Particle_System>> m_ParticleSystems;

public:
	static unique_ptr<Particle_Manager> Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);
};

NS_END