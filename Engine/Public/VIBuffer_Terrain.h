
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

private:
	uint32_t			m_iNumVerticesX = {};
	uint32_t			m_iNumVerticesZ = {};


public:
	static unique_ptr<VIBuffer_Terrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pHeightMapFilePath);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END