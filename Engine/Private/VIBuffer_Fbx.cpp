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
        aiNode* pNode = pScene->mRootNode;

        ProcessNode(pNode,pScene);
           
    }
#pragma endregion



for (auto& Mesh : *Meshes_VIBuffers) {
        m_iNumVertices += Mesh.m_iNumVertices;
        m_iNumIndices += Mesh.m_iNumIndices;
}



#pragma region VERTEX_BUFFER

    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;

    std::shared_ptr<vector<VTXTEX>> pVertices = make_shared<vector<VTXTEX>>();
    pVertices->reserve(m_iNumVertices);
    
    for (auto& mesh : *Meshes_VIBuffers) {
        for (auto vertices : *(mesh.vertices)) {
            (*pVertices).emplace_back(vertices);
        }
    }
    
    

    D3D11_SUBRESOURCE_DATA          VertexInitialData{};
    VertexInitialData.pSysMem = pVertices->data();

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER
    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;

    std::shared_ptr<vector<uint16_t>> pIndices = make_shared<vector<uint16_t>>();
    pVertices->reserve(m_iNumIndices);

    for (auto& mesh : *Meshes_VIBuffers) {
        for (auto& indices : *(mesh.indices)) {
            (*pIndices).emplace_back(indices);
        }
    }

    D3D11_SUBRESOURCE_DATA          IndexInitialData{};
    IndexInitialData.pSysMem = pIndices->data();

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

#pragma endregion


    return S_OK;
}

HRESULT VIBuffer_Fbx::Initialize(void* pArg)
{
    m_filePath = this->m_filePath;
    return S_OK;
}

shared_ptr<VIBuffer_Fbx> VIBuffer_Fbx::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, string filePath)
{
    auto		pInstance = shared_ptr<VIBuffer_Fbx>(new VIBuffer_Fbx(pDevice, pContext, filePath));

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
            v.vNormal.x = mesh->mNormals[i].x;
            v.vNormal.y = mesh->mNormals[i].y;
            v.vNormal.z = mesh->mNormals[i].z;
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
            v.vTangent.x = mesh->mTangents[i].x;
            v.vTangent.y = mesh->mTangents[i].y;
            v.vTangent.z = mesh->mTangents[i].z;
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
    Meshes.m_iNumVertices = Meshes.vertices->size();
    Meshes.m_iVertexStride = sizeof(VTXTEX);

    Meshes.m_iNumIndices = Meshes.indices->size();
    Meshes.m_iIndexStride = 2;
    Meshes.m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    Meshes.m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    

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