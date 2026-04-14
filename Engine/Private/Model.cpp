#include "Model.h"
#include "GameInstance.h"
Model::Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : Component{ pDevice, pContext }
{
}

Model::~Model()
{
}

HRESULT Model::Initialize_Prototype(uint32_t iLevelIndex, wstring modelName)
{
    m_iLevelIndex = iLevelIndex;
    m_sModelName = modelName;
    #pragma region ASSIMP
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(WStringToString(m_sModelName),
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace
    );
    


    if (pScene == nullptr) {
        MSG_BOX("Failed to Open Assimp : Model Mesh");
        return E_FAIL;
       
    }



    if (pScene->HasMeshes()) {

        aiNode* pNode = pScene->mRootNode;
        meshNames.reserve(pScene->mNumMeshes);
        ProcessNode(pNode,pScene);
           
    }
#pragma endregion



    return S_OK;
}


void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    
    shared_ptr<vector<VTXTEX>> vertices = make_shared<vector<VTXTEX>>();
    shared_ptr<vector<uint16_t>> indices = make_shared<vector<uint16_t>>();

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
        
        vertices->emplace_back(v);
    }

    // Index
    for (UINT i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];

      
        for (UINT j = 0; j < face.mNumIndices; ++j) {
     
            indices->emplace_back(face.mIndices[j]);
        }
            
    }


   
    std::string name = std::filesystem::path(m_sModelName).stem().string();
    std::string meshName =  name + "_" + to_string(++i_meshCount);
  


    std::wstring wMeshName(meshName.begin(), meshName.end());
    std::wstring prototypeTag = L"Prototype_Component_Mesh_" + wMeshName;

    if (FAILED(CGameInstance::Get().Add_Prototype(m_iLevelIndex, prototypeTag, Mesh::Create(m_pDevice, m_pContext, vertices, indices, wMeshName,m_iLevelIndex)))) {
        MSG_BOX("Mesh Proto SAVE FAILED");

    }
    meshNames.emplace_back(wMeshName);

}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
        ProcessNode(node->mChildren[i], scene);
}




HRESULT Model::Initialize(void* pArg)
{
    return S_OK;
}

unique_ptr<Model> Model::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iLevelIndex, wstring modelName)
{
    auto		pInstance = unique_ptr<Model>(new Model(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(iLevelIndex, modelName)))
    {
        MSG_BOX("Failed to Created : Model");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Model::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<Model>(new Model(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Model");
        return nullptr;
    }

    return pInstance;
}

string Model::WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return {};

    int size = WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), -1,
        nullptr, 0,
        nullptr, nullptr);

    std::string str(size, 0);

    WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), -1,
        &str[0], size,
        nullptr, nullptr);

    str.pop_back(); 
    return str;
}

