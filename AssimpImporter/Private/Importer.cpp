
#include "Importer.h"

Importer::Importer()
{
}

Importer::~Importer()
{
}


void Importer::LoadFolder(const char* ModelFilePath)
{
    for (const auto& entry : std::filesystem::directory_iterator(ModelFilePath))
    {
        if (!entry.is_regular_file())
            continue;

        const auto& path = entry.path();

        // 확장자 체크 (.fbx)
        if (path.extension() == ".fbx" || path.extension() == ".FBX")
        {
            string inputPath = path.string();
            string outputPath = "../../Resources/Model/Bin/" + path.stem().string() + ".bin";

            Load((char*)inputPath.c_str());
            ExportMeshBinary(outputPath.c_str());
            Clear();
        }

    
    }
}

HRESULT Importer::Load(char* ModelFilePath)
{

    m_index = 0;
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(ModelFilePath,
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
        Meshes.reserve(pScene->mNumMeshes);
        ProcessNode(pNode, pScene);

    }




    return S_OK;
}


void Importer::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{

    shared_ptr<vector<VTXMESH>> vertices = make_shared<vector<VTXMESH>>();
    shared_ptr<vector<uint32_t>> indices = make_shared<vector<uint32_t>>();


    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        VTXMESH v{};

        // Position
        v.vPosition = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        // Normal
        if (mesh->HasNormals())
        {
            v.vNormal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        // UV
        if (mesh->HasTextureCoords(0))
        {
            v.vTexcoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }

        // Tangent & Binormal
        if (mesh->HasTangentsAndBitangents())
        {
            v.vTangent = {
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z
            };

            v.vBinormal = {
                mesh->mBitangents[i].x,
                mesh->mBitangents[i].y,
                mesh->mBitangents[i].z
            };
        }

        vertices->emplace_back(v);
    }

    // Index
    for (UINT i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];

        for (UINT j = 0; j < face.mNumIndices; ++j)
        {
            indices->emplace_back(face.mIndices[j]);
        }
    }


    string _name;


    uint32_t _materialIndex;
    XMFLOAT3 _min = { FLT_MAX, FLT_MAX, FLT_MAX };
    XMFLOAT3 _max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };


    if (mesh->mName.length > 0)
        _name = mesh->mName.C_Str();
    else
        _name = "Mesh_" + to_string(++m_index);

    _materialIndex = mesh->mMaterialIndex;

    // boundingBox
    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& pos = mesh->mVertices[i];

        _min.x = min(_min.x, pos.x);
        _min.y = min(_min.y, pos.y);
        _min.z = min(_min.z, pos.z);

        _max.x = max(_max.x, pos.x);
        _max.y = max(_max.y, pos.y);
        _max.z = max(_max.z, pos.z);
    }

    shared_ptr<Mesh> fbxmesh = make_shared<Mesh>();
    fbxmesh->Set_Mesh(_name, _materialIndex, _min, _max, vertices, indices);

    Meshes.emplace_back(fbxmesh);

}

void Importer::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
        ProcessNode(node->mChildren[i], scene);
}

HRESULT Importer::ExportMeshBinary(const char* filePath)
{
    ofstream file(filePath, ios::binary);

    if (!file.is_open()) {
        MSG_BOX("no file path");
        return E_FAIL;
    }
      

    // Mesh 개수
    uint32_t meshCount = Meshes.size();
    file.write((char*)&meshCount, sizeof(uint32_t));

    for (auto& mesh : Meshes)
    {
        WriteMesh(file, mesh);
    }

    file.close();
    return S_OK;
}

void Importer::WriteMesh(ofstream& file, shared_ptr<Mesh> mesh)
{
    // Vertex Count
    uint32_t vCount = mesh->m_vertices->size();
    file.write((char*)&vCount, sizeof(uint32_t));

    // Index Count
    uint32_t iCount = mesh->m_indices->size();
    file.write((char*)&iCount, sizeof(uint32_t));

    // Vertex 데이터
    file.write((char*)mesh->m_vertices->data(), sizeof(VTXMESH) * vCount);

    // Index 데이터
    file.write((char*)mesh->m_indices->data(), sizeof(uint32_t) * iCount);
}

void Importer::Clear() {
    Meshes.clear();
}

HRESULT Importer::LoadMeshBinary(const char* filePath)
{
    ifstream file(filePath, ios::binary);

    if (!file.is_open())
        return E_FAIL;

    uint32_t meshCount = 0;
    file.read((char*)&meshCount, sizeof(uint32_t));

    for (uint32_t i = 0; i < meshCount; i++)
    {
        auto mesh = make_shared<Mesh>();

        uint32_t vCount = 0;
        uint32_t iCount = 0;

        file.read((char*)&vCount, sizeof(uint32_t));
        file.read((char*)&iCount, sizeof(uint32_t));

        mesh->m_vertices->resize(vCount);
        mesh->m_indices->resize(iCount);

        file.read((char*)mesh->m_vertices->data(), sizeof(VTXMESH) * vCount);
        file.read((char*)mesh->m_indices->data(), sizeof(uint32_t) * iCount);

        Meshes.emplace_back(mesh);
    }

    return S_OK;
}