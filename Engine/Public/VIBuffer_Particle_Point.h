#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Particle_Point final : public VIBuffer_Instance
{
public:
	typedef struct tagParticleInstanceDesc final : public VIBuffer_Instance::INSTANCE_DESC
	{
		_float2			vSpeed = {};
		_float2			vLifeTime = {};
		_bool			isLoop = { false };
		_float3			vPivot = {};
	}PARTICLE_INSTANCE_DESC;
protected:
	VIBuffer_Particle_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~VIBuffer_Particle_Point();

public:
	virtual HRESULT Initialize_Prototype(void* pArg) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

public:
	void Drop(_float fTimeDelta);
	void Rise(_float fTimeDelta);
	void Spread(_float fTimeDelta);
	void Smoke_Stay(_float fTimeDelta);

private:
	shared_ptr<VTXINSTANCE_PARTICLE[]>			m_pInstanceData = { nullptr };
	shared_ptr<_float[]>						m_pSpeeds = { nullptr };
	_bool										m_isLoop = { false };
	_float3										m_vPivot = {};
	D3D11_BUFFER_DESC							m_InstanceBufferDesc = {};

public:

	static unique_ptr<VIBuffer_Particle_Point> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
};

NS_END