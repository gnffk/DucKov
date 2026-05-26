#pragma once

#include "Engine_Defines.h"



NS_BEGIN(Engine)

class Bone final
{
private:
	Bone();
public:
	~Bone();

public:
	_matrix Get_TransformationMatrix() {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationMatrixPtr() {
		return &m_CombinedTransformationMatrix;
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(const _char* name, _float4x4 TransformMatrix, int32_t ParentBoneIndex);
	_bool Compare_Name(const _char* pBoneName) {
		return !strcmp(pBoneName, m_szName);
	}




	void Update_CombinedTransformationMatrix(const vector<shared_ptr<Bone>>& Bones, _fmatrix PreTransformMatrix);

public:

private:
	_char			m_szName[MAX_PATH] = {  };
	_float4x4		m_TransformationMatrix = { }; /* 이 뼈만의 상태행렬 */
	_float4x4		m_CombinedTransformationMatrix = {}; /* 부모 뼈의 상태를 포함한 최종 행렬 */
	int32_t			m_iParentBoneIndex = { -1 };

public:
	static shared_ptr<Bone> Create(const _char* name, _float4x4 TransformMatrix, int32_t ParentBoneIndex);
	shared_ptr<Bone> Clone();
};

NS_END