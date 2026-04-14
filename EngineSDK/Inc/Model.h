#pragma once

#include "Mesh.h"

NS_BEGIN(Engine)

class ENGINE_DLL Model : public Component
{
private:
	Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	~Model();


public:
	virtual HRESULT Initialize_Prototype(uint32_t m_iLevelIndex, wstring modelName);
	virtual HRESULT Initialize(void* pArg) override;

public:

	static unique_ptr<Model> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iLevelIndex, wstring modelName);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;
	string WStringToString(const std::wstring& wstr);


public:
	// À¯µµ¸® ÀÖ°Ô Âô±ß
	vector<wstring>& GetMeshNames() { return meshNames; }
	
private:
	vector<wstring> meshNames;

	uint32_t m_iLevelIndex;
	wstring m_sModelName;
	int	    i_meshCount{};
private:
#ifdef ENGINE_BUILD
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);

	void ProcessNode(aiNode* node, const aiScene* scene);
#endif
	
};

NS_END