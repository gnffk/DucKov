#include "FbxBuffer.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>




HRESULT FbxBuffer::Initialize_Prototype()
{
    __super::Initialize_Prototype();

   
    return S_OK;
}

HRESULT FbxBuffer::Initialize(void* pArg)
{
	
    return S_OK;

}

HRESULT FbxBuffer::Draw()
{


    return S_OK;
}

shared_ptr<Prototype> FbxBuffer::Create(shared_ptr<vector<vertex2>> vertices, shared_ptr<vector<INDEX32>> indices, ComPtr<ID3D11Buffer> VertexBuffer_, ComPtr<ID3D11Buffer> IndexBuffer_)
{
    return shared_ptr<Prototype>();
}

shared_ptr<Prototype> FbxBuffer::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<FbxBuffer>(this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : FbxBuffer");

	}

	return pInstance;
}