#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Cell final : public VIBuffer
{
private:
	VIBuffer_Cell(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~VIBuffer_Cell();

public:
	virtual HRESULT Initialize_Prototype(const _float3* pPoints);
	virtual HRESULT Initialize(void* pArg);

public:
	static unique_ptr<VIBuffer_Cell> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END