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
	virtual HRESULT Initialize(shared_ptr<vector<VTXANIMMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices, uint32_t materialIndex,
		uint32_t m_iNumBones, vector<uint32_t>	BoneIndices, vector<_float4x4>	BoneMatrices, vector<_float4x4>	OffsetMatrices) ;
	

	HRESULT Bind_BoneMatrices(const vector<shared_ptr<class Bone>>& Bones, shared_ptr<class Shader> pShader, const _char* pConstantName);
public:
	static unique_ptr<Mesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
		shared_ptr<vector<VTXMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices, uint32_t materialIndex);


	static unique_ptr<Mesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
		shared_ptr<vector<VTXANIMMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices, uint32_t materialIndex,
		uint32_t m_iNumBones, vector<uint32_t>	BoneIndices, vector<_float4x4>	BoneMatrices, vector<_float4x4>	OffsetMatrices);


	virtual shared_ptr<Prototype> Clone(void* pArg) override;
private:
	shared_ptr<vector<VTXMESH>> vertices;
	shared_ptr<vector<VTXANIMMESH>> animvertices;
	shared_ptr<vector<uint32_t>> indices;
	uint32_t		m_iMaterialIndex = {};


private:
	uint32_t		m_iNumBones = {}; /* 이 메시가 이용하는 뼈의 갯수. */

	/*  이 메시에 영향을 주는 뼈들의 전체뼈기준의 인덱스 */
	vector<uint32_t>	m_BoneIndices;
	vector<_float4x4>	m_BoneMatrices;
	vector<_float4x4>	m_OffsetMatrices;

};

NS_END
 