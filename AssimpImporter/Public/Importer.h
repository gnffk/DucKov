#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Material.h"
#include "Bone.h"
#include "Animation.h"

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
	void Ready_Animation(const aiScene* scene);

	void Load_Material(aiMaterial* material,uint32_t materialNum);

	void ProcessNonAnimMesh(aiMesh* mesh, const aiScene* scene);
	void ProcessNonAnimNode(aiNode* node, const aiScene* scene);

	void ProcessAnimMesh(aiMesh* mesh, const aiScene* scene, string name);
	void ProcessAnimNode(aiNode* node, const aiScene* scene);

	void Load_Animaion(uint32_t iAnimaionCount, const aiAnimation* pAIAnimation);
	void Load_Channel(CHANNELDATA& ChannelData, const aiNodeAnim* pAIChannel);

public:
	HRESULT ExportNonAnimBinary(const char* filePath);
	void WriteNonAnimMesh(ofstream& file, shared_ptr<Mesh> mesh);

	 HRESULT ExportAnimMeshBinary(const char* filePath);
	 HRESULT ExportAnimationBinary(const char* filePath);
	
	void WriteAnimMesh(ofstream& file, shared_ptr<Mesh> mesh);
	void WriteMaterial(ofstream& file, shared_ptr<Material> mesh);
	void Writebone(ofstream& file, shared_ptr<Bone> bone);

	void WriteAnimation(ofstream& file, shared_ptr<Animation>Animation);




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
	vector<shared_ptr<Animation>> Animations;
};

