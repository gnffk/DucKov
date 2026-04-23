#include "Mesh.h"

Mesh::Mesh() {

}

Mesh::~Mesh() {

}

HRESULT Mesh::Set_Mesh(string _name, uint32_t _materialIndex, XMFLOAT3 _min, XMFLOAT3 _max, shared_ptr<vector<VTXMESH>> _vertices, shared_ptr<vector<uint32_t>> _indices)
{
	m_name = _name;
	m_materialIndex = _materialIndex;
	m_min = _min;
	m_max = _max;
	m_vertices = _vertices;
	m_indices = _indices;

	return S_OK;
}


