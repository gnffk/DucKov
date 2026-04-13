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


#pragma region ASSIMP
    Assimp::Importer importer;
    
    const aiScene* pScene = importer.ReadFile(m_filePath,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace
    );
    


    if (pScene == nullptr) {
        MSG_BOX("Failed to Open Assimp : VIBuffer_Fbx");
        return E_FAIL;
       
    }



    if (pScene->HasMeshes()) {
        Meshes_VIBuffers  = make_shared<vector<Mesh_VIBUFFER>>();
        (Meshes_VIBuffers)->reserve(pScene->mNumMeshes);
        aiNode* pNode = pScene->mRootNode;

        ProcessNode(pNode,pScene);
           
    }
#pragma endregion


    for (auto& mesh : *Meshes_VIBuffers) {
#pragma region VERTEX_BUFFER

        D3D11_BUFFER_DESC           VertexBufferDesc{};
        VertexBufferDesc.ByteWidth = static_cast<UINT>(mesh.vertices->size() * sizeof(VTXTEX));
        VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        VertexBufferDesc.StructureByteStride = mesh.m_iVertexStride;
        VertexBufferDesc.CPUAccessFlags = 0;
        VertexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA          VertexInitialData{};
        VertexInitialData.pSysMem = mesh.vertices->data();

        if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, mesh.m_pVB.GetAddressOf()))) {

            return E_FAIL;
        }


#pragma endregion


#pragma region INDEX_BUFFER
        D3D11_BUFFER_DESC           IndexBufferDesc{};
        IndexBufferDesc.ByteWidth = static_cast<UINT>(mesh.indices->size() * sizeof(uint16_t));
        IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        IndexBufferDesc.StructureByteStride = mesh.m_iIndexStride;
        IndexBufferDesc.CPUAccessFlags = 0;
        IndexBufferDesc.MiscFlags = 0;



        D3D11_SUBRESOURCE_DATA          IndexInitialData{};
        IndexInitialData.pSysMem = mesh.indices->data();

        if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, mesh.m_pIB.GetAddressOf())))
            return E_FAIL;

#pragma endregion
    }


    
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
    Mesh_VIBUFFER Meshes{};
    Meshes.vertices = make_shared<vector<VTXTEX>>();
    Meshes.indices = make_shared<vector<uint16_t>>();

    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        //std::vector<VTXTEX> vertices;
        //std::vector<INDEX32> indices;
        VTXTEX v;
        // Position
        v.vPosition.x = mesh->mVertices[i].x;
        v.vPosition.y = mesh->mVertices[i].y;
        v.vPosition.z = mesh->mVertices[i].z;

        // Normal
        if (mesh->HasNormals())
        {
            //v.vNormal.x = mesh->mNormals[i].x;
            //v.vNormal.y = mesh->mNormals[i].y;
            //v.vNormal.z = mesh->mNormals[i].z;
        }

        // UV
        if (mesh->HasTextureCoords(0))
        {
            v.vTexcoord.x = mesh->mTextureCoords[0][i].x;
            v.vTexcoord.y = mesh->mTextureCoords[0][i].y;
        }

        // Tangent
        if (mesh->HasTangentsAndBitangents())
        {
        /*    v.vTangent.x = mesh->mTangents[i].x;
            v.vTangent.y = mesh->mTangents[i].y;
            v.vTangent.z = mesh->mTangents[i].z;*/
        }

        (*Meshes.vertices).emplace_back(v);
    }

    // Index
    for (UINT i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];

      
        for (UINT j = 0; j < face.mNumIndices; ++j) {
     
            (*Meshes.indices).emplace_back(face.mIndices[j]);
        }
            
    }


   
    Meshes.m_iNumVertexBuffers = 1;
    Meshes.m_iNumVertices = (UINT)Meshes.vertices->size();
    Meshes.m_iVertexStride = sizeof(VTXTEX);

    Meshes.m_iNumIndices = (UINT)Meshes.indices->size();
    Meshes.m_iIndexStride = 2;
    Meshes.m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    Meshes.m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    
    (*Meshes_VIBuffers).emplace_back(Meshes);

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