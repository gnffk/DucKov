#pragma once
#include "pch.h"


class Mesh
{
public:
	Mesh();

	~Mesh();



	HRESULT Set_Mesh(string _name, uint32_t _materialIndex, XMFLOAT3 _min, XMFLOAT3 _max, shared_ptr<vector<VTXMESH>> _vertices, shared_ptr<vector<uint32_t>> _indices);
	string m_name;


	uint32_t m_materialIndex;

	XMFLOAT3 m_min;
	XMFLOAT3 m_max;

	shared_ptr<vector<VTXMESH>> m_vertices;
	shared_ptr<vector<uint32_t>> m_indices;
};

