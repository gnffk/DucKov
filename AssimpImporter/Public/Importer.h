#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Material.h"
#include "Bone.h"
class Importer
{
public:
	Importer();
	~Importer();


public:
	void	LoadFolder(const char* ModelFilePath, MODEL modelType);
	HRESULT Load(char* ModelFilePath, MODEL modelType);
	void Ready_Material(const aiScene* scene);
	void Ready_Bones(const aiNode* pAINode, int32_t iParentBoneIndex);
	void Load_Material(aiMaterial* material,uint32_t materialNum);

	void ProcessNonAnimMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessNonAnimNode(aiNode* node, const aiScene* scene);

	void ProcessAnimMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessAnimNode(aiNode* node, const aiScene* scene);
public:
	HRESULT ExportNonAnimBinary(const char* filePath);
	void WriteNonAnimMesh(ofstream& file, shared_ptr<Mesh> mesh);

	HRESULT ExportAnimBinary(const char* filePath);
	
	void WriteAnimMesh(ofstream& file, shared_ptr<Mesh> mesh);
	void WriteMaterial(ofstream& file, shared_ptr<Material> mesh);
	void Writebone(ofstream& file, shared_ptr<Bone> bone);


	void Clear();

public:

	int32_t Get_BoneIndex(const char* pBoneName);
public:
	int m_index{0};
	int m_iFlagModelType;
	int32_t m_iBoneIndex{-1};
	vector<shared_ptr<Bone>> Bones;
	vector<shared_ptr<Mesh>> Meshes;
	vector<shared_ptr<Material>> Materials;
	
};

