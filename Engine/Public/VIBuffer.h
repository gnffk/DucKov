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

	UINT m_iNumVertexBuffers = 1;
	UINT m_iNumVertices = 0;
	UINT m_iVertexStride = sizeof(VTXTEX);

	UINT m_iNumIndices = 0;
	UINT m_iIndexStride = sizeof(uint16_t);
	DXGI_FORMAT m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	shared_ptr<vector<VTXTEX>> vertices;
	shared_ptr<vector<uint16_t>> indices;


public:
	virtual shared_ptr<Prototype> Clone(void* pArg) = 0;

};

NS_END