#include "Mesh.h"

#include "GameInstance.h"



Mesh::Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : VIBuffer{pDevice, pContext}
{
}



Mesh::~Mesh()
{
}



HRESULT Mesh::Initialize(shared_ptr<vector<VTXMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices)
{
	vertices = pvertices;
	indices = pindices;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = (UINT)vertices->size();
	m_iVertexStride = sizeof(VTXMESH);

	m_iNumIndices = (UINT)indices->size();
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;





	D3D11_BUFFER_DESC           VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = static_cast<UINT>(vertices->size() * sizeof(VTXMESH));
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA          VertexInitialData{};
	VertexInitialData.pSysMem = vertices->data();

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, m_pVB.GetAddressOf()))) {

		return E_FAIL;
	}





	D3D11_BUFFER_DESC           IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = static_cast<UINT>(indices->size() * sizeof(uint32_t));
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_iIndexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;



	D3D11_SUBRESOURCE_DATA          IndexInitialData{};
	IndexInitialData.pSysMem = indices->data();

	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, m_pIB.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}

unique_ptr<Mesh> Mesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<vector<VTXMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices)
{
    auto		pInstance = unique_ptr<Mesh>(new Mesh(pDevice, pContext));

    if (FAILED(pInstance->Initialize(pvertices, pindices)))
    {
        MSG_BOX("Failed to Created : Mesh");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Mesh::Clone(void* pArg)
{
    return nullptr;
}
