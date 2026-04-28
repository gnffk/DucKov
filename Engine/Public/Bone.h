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
	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

public:
	HRESULT Initialize( int32_t iParentIndex);
	_bool Compare_Name(const _char* pBoneName) {
		return !strcmp(pBoneName, m_szName);
	}

	void Update_CombinedTransformationMatrix(const vector<shared_ptr<Bone>>& Bones);

private:
	_char			m_szName[MAX_PATH] = {  };
	_float4x4		m_TransformationMatrix = { }; /* 이 뼈만의 상태행렬 */
	_float4x4		m_CombinedTransformationMatrix = {}; /* 부모 뼈의 상태를 포함한 최종 행렬 */
	int32_t			m_iParentBoneIndex = { -1 };

public:
	static shared_ptr<Bone> Create( int32_t iParentIndex);
};

NS_END