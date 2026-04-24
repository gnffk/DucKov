#pragma once
#include "Component.h"
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL Mesh final : public VIBuffer
{
private:
	Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) ;
public:
	~Mesh();


public:
	uint32_t Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
public:
	virtual HRESULT Initialize(shared_ptr<vector<VTXMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices, uint32_t materialIndex) ;

public:
	static unique_ptr<Mesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
		shared_ptr<vector<VTXMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices, uint32_t materialIndex);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
private:
	shared_ptr<vector<VTXMESH>> vertices;
	shared_ptr<vector<uint32_t>> indices;
	uint32_t		m_iMaterialIndex = {};
};

NS_END
 