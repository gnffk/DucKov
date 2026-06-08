#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Instance abstract : public VIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		uint32_t			iNumInstances;
		_float3				vRange;
		_float3				vCenter;
		_float2				vSize;
	}INSTANCE_DESC;
protected:
	VIBuffer_Instance(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~VIBuffer_Instance();

public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

protected:
	ComPtr<ID3D11Buffer>			m_pVBInstance = { nullptr };
	uint32_t						m_iInstanceVertexStride = {};
	uint32_t						m_iIndexCountPerInstance = {};
	uint32_t						m_iNumInstances = {};


public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;
};

NS_END