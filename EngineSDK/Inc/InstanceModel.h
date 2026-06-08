#pragma once

#include "VIBuffer_Mesh_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL InstanceModel : public Component
{
private:
	InstanceModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	InstanceModel(const InstanceModel& Prototype);

public:
	~InstanceModel();


public:
	uint32_t Get_NumMeshes() const {
		return m_iNumMesh;
	}
public:
	virtual HRESULT Initialize_Prototype(uint32_t m_iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Render();
	HRESULT Render(uint32_t iMeshIndex);
public:
	HRESULT Bind_Materials(shared_ptr<class Shader> pShader, const _char* pConstantName, uint32_t iMeshIndex, uint32_t eMaterialType, uint32_t iTextureIndex);

public:

	static unique_ptr<InstanceModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName, _fmatrix PreTransformMatrix);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

public:

	vector<shared_ptr<VIBuffer_Mesh_Instance>>& GetMeshes() { return m_Meshes; }


private:
	HRESULT Ready_BinaryModelFile(const char* modelFileName);

	HRESULT Ready_NonAnimMesh(ifstream& _file, const char* modelFileName);

	HRESULT Ready_NonAnimMaterial(ifstream& _file, const char* modelFileName);

private:

	_float4x4				m_PreTransformMatrix = {};

private:
	vector<shared_ptr<VIBuffer_Mesh_Instance>> m_Meshes;
	uint32_t m_eModelType;
	uint32_t m_iLevelIndex;
	uint32_t m_iNumMesh{};
	wstring m_sModelName;


private:
	uint32_t						m_iNumMaterials{};
	vector<shared_ptr<class Material>>	m_Materials;

private:
	vector<_float4x4> m_InstanceWorlds;

	_bool    m_isInstanceBufferDirty = true;
	uint32_t m_iInstanceCapacity = 128;

public:
	HRESULT Add_Instance(const _float3& vPosition);
	HRESULT Add_Instance(const _float3& vPosition, const _float3& vScale, _float fYaw);
	HRESULT Remove_Instance_InRadius(const _float3& vCenter, _float fRadius);
	HRESULT Clear_Instances();
	HRESULT Update_InstanceBuffer();

	uint32_t Get_InstanceCount() const {
		return static_cast<uint32_t>(m_InstanceWorlds.size());
	}

	const vector<_float4x4>& Get_InstanceWorlds() const {
		return m_InstanceWorlds;
	}
};

NS_END