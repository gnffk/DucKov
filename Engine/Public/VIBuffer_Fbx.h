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
	static unique_ptr<VIBuffer_Fbx> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string filePath);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;



protected:

	string m_filePath;

private:
#ifdef ENGINE_BUILD
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);

	void ProcessNode(aiNode* node, const aiScene* scene);
#endif


};

NS_END