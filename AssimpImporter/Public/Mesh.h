#pragma once
#include "pch.h"


class Mesh
{
public:
	Mesh();

	~Mesh();



	HRESULT Set_Mesh(string _name, uint32_t _materialIndex, XMFLOAT3 _min, XMFLOAT3 _max, shared_ptr<vector<VTXMESH>> _vertices, shared_ptr<vector<uint32_t>> _indices);
	HRESULT Set_AnimMesh(string _name, uint32_t _materialIndex, XMFLOAT3 _min, XMFLOAT3 _max, shared_ptr<vector<VTXANIMMESH>> _vertices, shared_ptr<vector<uint32_t>> _indices);
	string m_name;


	uint32_t m_materialIndex;

	XMFLOAT3 m_min;
	XMFLOAT3 m_max;

	shared_ptr<vector<VTXMESH>> m_vertices;
	shared_ptr<vector<VTXANIMMESH>> m_animvertices;
	shared_ptr<vector<uint32_t>> m_indices;



	uint32_t		m_iNumBones = {}; /* 이 메시가 이용하는 뼈의 갯수. */

	/*  이 메시에 영향을 주는 뼈들의 전체뼈기준의 인덱스 */
	shared_ptr<vector<uint32_t>>	m_BoneIndices;
	shared_ptr<vector<XMFLOAT4X4>>	m_BoneMatrices;
	shared_ptr<vector<XMFLOAT4X4>>	m_OffsetMatrices;
};

