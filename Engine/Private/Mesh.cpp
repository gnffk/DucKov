#include "Mesh.h"
#include "Bone.h"
#include "GameInstance.h"



Mesh::Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : VIBuffer{pDevice, pContext}
{
}



Mesh::~Mesh()
{
}



HRESULT Mesh::Initialize(shared_ptr<vector<VTXMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices, uint32_t materialIndex, _fmatrix PreTransformMatrix)
{
	vertices = pvertices;
	indices = pindices;
	m_iMaterialIndex = materialIndex;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = (UINT)vertices->size();
	m_iVertexStride = sizeof(VTXMESH);

	m_iNumIndices = (UINT)indices->size();
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	for (size_t i = 0; i < m_iNumVertices; i++)
	{

		XMStoreFloat3(&(*vertices)[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&(*vertices)[i].vPosition), PreTransformMatrix));

		XMStoreFloat3(&(*vertices)[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&(*vertices)[i].vNormal), PreTransformMatrix));

	}



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

HRESULT Mesh::Initialize( shared_ptr<vector<VTXANIMMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices, uint32_t materialIndex, 
	uint32_t iNumBones, vector<uint32_t> BoneIndices, vector<_float4x4> BoneMatrices, vector<_float4x4> OffsetMatrices)
{
	animvertices = pvertices;
	indices = pindices;
	m_iMaterialIndex = materialIndex;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = (UINT)animvertices->size();
	m_iVertexStride = sizeof(VTXANIMMESH);

	m_iNumIndices = (UINT)indices->size();
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	m_iNumBones = iNumBones;

	
	m_BoneIndices = BoneIndices;
	m_BoneMatrices = BoneMatrices;
	m_OffsetMatrices = OffsetMatrices;


	D3D11_BUFFER_DESC           VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = static_cast<UINT>(animvertices->size() * sizeof(VTXANIMMESH));
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA          VertexInitialData{};
	VertexInitialData.pSysMem = animvertices->data();

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

HRESULT Mesh::Bind_BoneMatrices(const vector<shared_ptr<Bone>>& Bones, shared_ptr<Shader> pShader, const _char* pConstantName)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
  		XMStoreFloat4x4(&m_BoneMatrices[i],
			XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	if (false == m_BoneMatrices.empty())
		pShader->Bind_Matrices(pConstantName, &m_BoneMatrices.front(), m_iNumBones);

	return S_OK;
}

unique_ptr<Mesh> Mesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<vector<VTXMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices,uint32_t materialIndex, _fmatrix PreTransformMatrix)
{
    auto		pInstance = unique_ptr<Mesh>(new Mesh(pDevice, pContext));

    if (FAILED(pInstance->Initialize(pvertices, pindices, materialIndex, PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created : Mesh");
        return nullptr;
    }

    return pInstance;
}

unique_ptr<Mesh> Mesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<vector<VTXANIMMESH>> pvertices, shared_ptr<vector<uint32_t>> pindices, 
	uint32_t materialIndex, uint32_t m_iNumBones, vector<uint32_t> BoneIndices, vector<_float4x4> BoneMatrices, vector<_float4x4> OffsetMatrices)
{
	auto		pInstance = unique_ptr<Mesh>(new Mesh(pDevice, pContext));

	if (FAILED(pInstance->Initialize(pvertices, pindices, materialIndex, m_iNumBones, BoneIndices, BoneMatrices, OffsetMatrices)))
	{
		MSG_BOX("Failed to Created : AnimMesh");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> Mesh::Clone(void* pArg)
{
    return nullptr;
}
