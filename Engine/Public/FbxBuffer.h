#pragma once
#include "VIBuffer.h"

class FbxBuffer final : public VIBuffer
{
protected:
	FbxBuffer() = default;
	

public:
	virtual ~FbxBuffer() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Draw();


public:
	static shared_ptr<Prototype> Create(shared_ptr<vector<vertex2>> vertices, shared_ptr<vector<INDEX32>> indices,ComPtr<ID3D11Buffer> VertexBuffer_,ComPtr<ID3D11Buffer> IndexBuffer_);
	shared_ptr<Prototype> Clone(void* pArg) override;
};

