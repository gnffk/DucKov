#pragma once
#include "Component.h"

class VIBuffer : public Component
{
protected:
	VIBuffer() = default;
public:
	virtual ~VIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Draw();


	shared_ptr<vector<vertex2>> vertices;
	shared_ptr<vector<INDEX32>> indices;

	ComPtr<ID3D11Buffer> VertexBuffer_;
	ComPtr<ID3D11Buffer> IndexBuffer_;


public:
     virtual shared_ptr<Prototype> Clone(void* pArg) = 0;
};

