#include "VIBuffer_Fbx.h"





VIBuffer_Fbx::VIBuffer_Fbx(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string filePath)
    : VIBuffer{ pDevice, pContext }, m_filePath{ filePath }
{
}

VIBuffer_Fbx::~VIBuffer_Fbx()
{

}


HRESULT VIBuffer_Fbx::Initialize_Prototype()
{



    
    return S_OK;
}

HRESULT VIBuffer_Fbx::Initialize(void* pArg)
{
 

    return S_OK;
}

unique_ptr<VIBuffer_Fbx> VIBuffer_Fbx::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string filePath)
{
    auto		pInstance = unique_ptr<VIBuffer_Fbx>(new VIBuffer_Fbx(pDevice, pContext, filePath));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : VIBuffer_Fbx");
        return nullptr;
    }

    return pInstance;
}


#pragma region Clone관련 다시 생각하자
shared_ptr<Prototype> VIBuffer_Fbx::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<VIBuffer_Fbx>(new VIBuffer_Fbx(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : VIBuffer_Fbx");
        return nullptr;
    }

    return pInstance;
}
#pragma endregion

void VIBuffer_Fbx::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
 

}

void VIBuffer_Fbx::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
        ProcessNode(node->mChildren[i], scene);
}