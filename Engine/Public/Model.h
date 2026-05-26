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
	uint32_t Get_NumMeshes() const {
		return m_iNumMesh;
	}
	uint32_t Get_NumAnimation() const {
		return m_iNumAnimations;
	}
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);
public:
	virtual HRESULT Initialize_Prototype(uint32_t m_iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Render();
	HRESULT Render(uint32_t iMeshIndex);

	_bool	Play_Animation(_float fTimeDelta);

public:
	void Set_Animation(uint32_t iIndex, _bool isLoop = true, _float fBlendDuration = 0.2f);
	uint32_t Get_CurAnimationIndex() { return m_iCurrentAnimIndex; }

public:
	HRESULT Bind_BoneMatrices(shared_ptr<class Shader> pShader, const _char* pConstantName, uint32_t iMeshIndex);
	HRESULT Bind_Materials(shared_ptr<class Shader> pShader, const _char* pConstantName, uint32_t iMeshIndex, uint32_t eMaterialType, uint32_t iTextureIndex);

	public:

	static unique_ptr<Model> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName, _fmatrix PreTransformMatrix);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;


public:

	vector<shared_ptr<Mesh>>& GetMeshes() { return m_Meshes; }

	// Customize
public:
	HRESULT Set_AnimMaterial(uint32_t materialNum, const char* modelFileName);

private:
	HRESULT Ready_BinaryModelFile(const char* modelFileName);
	
	HRESULT Ready_BinaryAnimationFile(const char* modelFileName);

	HRESULT Ready_NonAnimMesh(ifstream& _file, const char* modelFileName);

	HRESULT Ready_NonAnimMaterial(ifstream& _file, const char* modelFileName);
	


	HRESULT Ready_AnimBone(ifstream& _file, const char* modelFileName);

	HRESULT Ready_AnimMesh(ifstream& _file, const char* modelFileName);

	HRESULT Ready_AnimMaterial(ifstream& _file, const char* modelFileName);



	void Apply_AnimationBlend(_float fTimeDelta);
private:
	vector<shared_ptr<class Bone>> m_Bones;
	_float4x4				m_PreTransformMatrix = {};
	uint32_t m_iBoneCount;
private:
	vector<shared_ptr<Mesh>> m_Meshes;
	uint32_t m_eModelType;
	uint32_t m_iLevelIndex;
	uint32_t m_iNumMesh{};
	wstring m_sModelName;


private:
	uint32_t						m_iNumMaterials{};
	vector<shared_ptr<class Material>>	m_Materials;

private:
	_bool							m_isAnimLoop = { true };
	_bool							m_isAnimationBlending = { false };
	_float						m_fAnimationBlendTime = {};
	_float						m_fAnimationBlendDuration = {};
	uint32_t						m_iCurrentAnimIndex = {};
	uint32_t						m_iNumAnimations = {};
	vector<_float4x4>			m_BlendStartTransformationMatrices;
	vector<shared_ptr<class Animation>>	m_Animations;
};

NS_END