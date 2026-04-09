#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Fbx final : public VIBuffer
{
private:
	VIBuffer_Fbx(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, string filePath);

public:
	virtual ~VIBuffer_Fbx();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static shared_ptr<VIBuffer_Fbx> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string filePath);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

	void ProcessMesh(aiMesh* mesh, const aiScene* scene);

	void ProcessNode(aiNode* node, const aiScene* scene);

protected:

	string m_filePath;
};

NS_END