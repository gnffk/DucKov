#pragma once
#include "pch.h"
#include "Mesh.h"

class Importer
{
public:
	Importer();
	~Importer();


public:
	void	LoadFolder(const char* ModelFilePath);
	HRESULT Load(char* ModelFilePath);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessNode(aiNode* node, const aiScene* scene);
public:
	HRESULT ExportMeshBinary(const char* filePath);
	void WriteMesh(ofstream& file, shared_ptr<Mesh> mesh);

	void Clear();

public:
	HRESULT LoadMeshBinary(const char* filePath);
public:
	int m_index{0};
	vector<shared_ptr<Mesh>> Meshes;
	
};

