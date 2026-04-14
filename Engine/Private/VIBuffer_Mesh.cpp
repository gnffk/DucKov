#include "VIBuffer_Mesh.h"

VIBuffer_Mesh::VIBuffer_Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: VIBuffer{ pDevice, pContext }
{
}

VIBuffer_Mesh::~VIBuffer_Mesh()
{

}


HRESULT VIBuffer_Mesh::Initialize_Prototype(shared_ptr<vector<VTXTEX>> pvertex, shared_ptr<vector<uint16_t>> pindex)
{


	vertices = pvertex;
	indices = pindex;



	m_iNumVertexBuffers = 1;
	m_iNumVertices = (UINT)vertices->size();
	m_iVertexStride = sizeof(VTXTEX);

	m_iNumIndices = (UINT)indices->size();
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;





	D3D11_BUFFER_DESC           VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = static_cast<UINT>(vertices->size() * sizeof(VTXTEX));
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
	IndexBufferDesc.ByteWidth = static_cast<UINT>(indices->size() * sizeof(uint16_t));
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

HRESULT VIBuffer_Mesh::Initialize(void* pArg)
{

	return S_OK;
}

unique_ptr<VIBuffer_Mesh> VIBuffer_Mesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,
	shared_ptr<vector<VTXTEX>> pvertex, shared_ptr<vector<uint16_t>> pindex)
{
	auto		pInstance = unique_ptr<VIBuffer_Mesh>(new VIBuffer_Mesh(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype(pvertex, pindex)))
	{
		MSG_BOX("Failed to Created : VIBuffer_Mesh");
		return nullptr;
	}

	return pInstance;
}



shared_ptr<Prototype> VIBuffer_Mesh::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<VIBuffer_Mesh>(new VIBuffer_Mesh(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : VIBuffer_Mesh");
		return nullptr;
	}

	return pInstance;
}

