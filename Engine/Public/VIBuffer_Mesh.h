#pragma once
#include "VIBuffer.h"
NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Mesh final : public VIBuffer
{
private:
	VIBuffer_Mesh(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~VIBuffer_Mesh();

public:
	virtual HRESULT Initialize_Prototype(shared_ptr<vector<VTXTEX>> pvertex, shared_ptr<vector<uint16_t>> pindex);
	virtual HRESULT Initialize(void* pArg);

	

public:
	static unique_ptr<VIBuffer_Mesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<vector<VTXTEX>> pvertex, shared_ptr<vector<uint16_t>> pindex);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END