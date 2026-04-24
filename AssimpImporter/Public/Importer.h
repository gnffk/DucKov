#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Material.h"

class Importer
{
public:
	Importer();
	~Importer();


public:
	void	LoadFolder(const char* ModelFilePath);
	HRESULT Load(char* ModelFilePath);
	void Ready_Material(const aiScene* scene);
	void Load_Material(aiMaterial* material,uint32_t materialNum);

	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessNode(aiNode* node, const aiScene* scene);
public:
	HRESULT ExportBinary(const char* filePath);
	void WriteMesh(ofstream& file, shared_ptr<Mesh> mesh);
	void WriteMaterial(ofstream& file, shared_ptr<Material> mesh);

	void Clear();

public:
	HRESULT LoadMeshBinary(const char* filePath);
public:
	int m_index{0};
	vector<shared_ptr<Mesh>> Meshes;
	vector<shared_ptr<Material>> Materials;
	
};

