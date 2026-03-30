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

	shared_ptr<ID3D11Buffer> VertexBuffer_;
	shared_ptr<ID3D11Buffer> IndexBuffer_;


public:
	shared_ptr<Prototype> Clone(void* pArg) = 0;
};

