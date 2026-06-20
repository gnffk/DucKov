#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Particle_Rect final : public VIBuffer_Instance
{
public:
	typedef struct tagParticleInstanceDesc final : public VIBuffer_Instance::INSTANCE_DESC
	{
		_float2			vSpeed = {};
		_float2			vLifeTime = {};
		_bool			isLoop = { false };
	}PARTICLE_INSTANCE_DESC;
protected:
	VIBuffer_Particle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~VIBuffer_Particle_Rect();

public:
	virtual HRESULT Initialize_Prototype(void* pArg) override;
	virtual HRESULT Initialize(void* pArg) override;



public:
	void Drop(_float fTimeDelta);
	void Add_Spray(const _float3& vSpawnPos, uint32_t iCount);
	void Update_Spray(_float fTimeDelta);


private:
	shared_ptr<VTXINSTANCE_PARTICLE[]>			m_pInstanceData = { nullptr };
	shared_ptr<_float[]>						m_pSpeeds = { nullptr };
	_bool										m_isLoop = { false };

	vector<_float3>								m_vVelocity;
	vector<_bool>								m_vActive;


public:
	static unique_ptr<VIBuffer_Particle_Rect> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;


};

NS_END