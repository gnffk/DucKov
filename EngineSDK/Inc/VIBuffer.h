#pragma once
#include "Component.h"

class VIBuffer : public Component
{
protected:
	VIBuffer() = default;
	virtual ~VIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);


	shared_ptr<vector<vertex2>> vertices;
	shared_ptr<INDEX32[]>		pIndex;
protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	shared_ptr<Prototype> Clone(void* pArg) = 0;
};

