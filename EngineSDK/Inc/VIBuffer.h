#pragma once

#include "Component.h"

/* Vertex + Index Buffer */

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer abstract : public Component
{
protected:
	VIBuffer(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~VIBuffer();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Buffer>			m_pVB = { nullptr };
	ComPtr<ID3D11Buffer>			m_pIB = { nullptr };

protected:
	uint32_t					m_iNumVertexBuffers = {};
	uint32_t					m_iVertexStride = {};
	uint32_t					m_iNumVertices = {};
	uint32_t					m_iIndexStride = {};
	uint32_t					m_iNumIndices = {};
	DXGI_FORMAT					m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveType = {};



public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;

};

NS_END