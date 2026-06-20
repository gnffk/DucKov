
#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class Particle_System abstract : public GameObject
{
public:
	typedef struct tagParticleSpawnDesc
	{
		_float3 vSpawnPos = {};
		uint32_t   iCount = 1;
		_float3 vDir = { 0.f, 1.f, 0.f };
		_float  fPower = 1.f;
	} PARTICLE_SPAWN_DESC;

protected:
	Particle_System(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext):GameObject{ pDevice, pContext }{}

	Particle_System(const Particle_System& Prototype): GameObject{ Prototype }{}

public:
	virtual ~Particle_System() = default;

public:
	virtual void Add_Particle(const PARTICLE_SPAWN_DESC& Desc) = 0;
};

NS_END