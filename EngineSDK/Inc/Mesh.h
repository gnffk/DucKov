#pragma once
#include "Component.h"


NS_BEGIN(Engine)

class ENGINE_DLL Mesh final : public Component
{
private:
	Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) ;
public:
	~Mesh();



public:
	virtual HRESULT Initialize_Prototype(shared_ptr<vector<VTXTEX>> pvertices, shared_ptr<vector<uint16_t>> pindices, wstring ModelName, uint32_t m_iLevelIndex);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static unique_ptr<Mesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
		shared_ptr<vector<VTXTEX>> pvertices, shared_ptr<vector<uint16_t>> pindices, wstring ModelName, uint32_t m_iLevelIndex);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
private:
	shared_ptr<vector<VTXTEX>> vertices;
	shared_ptr<vector<uint16_t>> indices;
};

NS_END
 