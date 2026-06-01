
#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Terrain final : public VIBuffer
{
private:
	VIBuffer_Terrain(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~VIBuffer_Terrain();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	uint32_t GetNumVerticesX() const { return m_iNumVerticesX; }
	uint32_t GetNumVerticesZ() const { return m_iNumVerticesZ; }
	shared_ptr<vector<uint32_t>>& GetIndices() { return m_indices; }
	shared_ptr<vector<VTXNORTEX>>& Getvertices() { return m_vertices; }
private:
	uint32_t			m_iNumVerticesX = {};
	uint32_t			m_iNumVerticesZ = {};

private:
	shared_ptr<vector<VTXNORTEX>> m_vertices;
	shared_ptr<vector<uint32_t>> m_indices;
public:
	static unique_ptr<VIBuffer_Terrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pHeightMapFilePath);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END