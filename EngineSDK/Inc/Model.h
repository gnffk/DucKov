#pragma once

#include "Mesh.h"


NS_BEGIN(Engine)

class ENGINE_DLL Model : public Component
{
private:
	Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	Model(const Model& Prototype);

public:
	~Model();



public:
	virtual HRESULT Initialize_Prototype(uint32_t m_iLevelIndex, wstring modelName, const char* modelFileName);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Render();

	
public:

	static unique_ptr<Model> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iLevelIndex, wstring modelName, const char* modelFileName);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;


public:

	vector<shared_ptr<Mesh>>& GetMeshes() { return m_Meshes; }

private:
	HRESULT Ready_BinaryModelFile(const char* modelFileName);

	HRESULT Ready_Mesh(ifstream& _file, const char* modelFileName);

	HRESULT Ready_Material(ifstream& _file, const char* modelFileName);
	
private:
	vector<shared_ptr<Mesh>> m_Meshes;

	uint32_t m_iLevelIndex;
	uint32_t m_iNumMesh{};
	wstring m_sModelName;

private:
	uint32_t						m_iNumMaterials{};
	vector<shared_ptr<class Material>>	m_Materials;

};

NS_END