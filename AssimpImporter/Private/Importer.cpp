
#include "Importer.h"

Importer::Importer()
{
}

Importer::~Importer()
{
}


void Importer::LoadFolder(const char* ModelFilePath, MODEL modelType)
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
            std::string modelName = path.stem().string();

            std::string basePath = "../../Resources/Model/";

            if (modelName.rfind("CH_", 0) == 0)  
            {
                basePath += "Character/";
            }
            else
            {
                basePath += "Bin/";
            }

            // 최종 디렉토리
            std::string dirPath = basePath + modelName;

            // 폴더 생성
            std::filesystem::create_directories(dirPath);

            // 최종 파일 경로
            std::string outputPath = dirPath + "/" + modelName + ".bin";
        

            Load((char*)inputPath.c_str(), modelType);
            ExportBinary(outputPath.c_str());
            Clear();
        }

    
    }
}

HRESULT Importer::Load(char* ModelFilePath, MODEL modelType)
{

    m_index = 0;
    Assimp::Importer importer;
    uint32_t        iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };



    if (MODEL::NONANIM == modelType)
        iFlag |= aiProcess_PreTransformVertices;



    const aiScene* pScene = importer.ReadFile(ModelFilePath, iFlag);

    if (pScene == nullptr) {
        MSG_BOX("Failed to Open Assimp : Model Mesh");
        return E_FAIL;

    }



    if (pScene->HasMeshes()) {

        aiNode* pNode = pScene->mRootNode;
        Meshes.reserve(pScene->mNumMeshes);
        ProcessNode(pNode, pScene);

    }


    if (pScene->HasMaterials()) {
        Ready_Material(pScene);
    }




    return S_OK;
}

void Importer::Ready_Material(const aiScene* scene) {
    uint32_t NumMaterials = scene->mNumMaterials;
    Materials.reserve(NumMaterials);

    for (size_t i = 0; i < NumMaterials; i++)
    {
        Load_Material(scene->mMaterials[i],i);
       
      
    }
}

void Importer::Load_Material(aiMaterial* material, uint32_t materialNum)
{
    shared_ptr<Material> fbxmaterial = make_shared<Material>();
    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        uint32_t		iNumTextures = material->GetTextureCount(static_cast<aiTextureType>(i));
        
        if (iNumTextures == 0) {
            continue;
        }
        
       vector<TEXTUREINFO> textureDummy;
       textureDummy.resize(iNumTextures);

        for (size_t j = 0; j < iNumTextures; j++)
        {

            char	szFileName[MAX_PATH] = { };
            char	szExt[MAX_PATH] = { };

            aiString		strTexturePath = {};

            material->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath);

            _splitpath_s(strTexturePath.C_Str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            textureDummy[j].m_textureType = i;
            textureDummy[j].m_textureNum = j;
            textureDummy[j].File = szFileName;
            textureDummy[j].Ext = szExt;
        }

      
        fbxmaterial->m_textures.push_back(textureDummy);
    }
    fbxmaterial->m_materialNum = materialNum;

    Materials.emplace_back(fbxmaterial);
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

    mesh->mMaterialIndex;


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

HRESULT Importer::ExportBinary(const char* filePath)
{
    ofstream file(filePath, ios::binary);

    if (!file.is_open()) {
        MSG_BOX("no file path");
        return E_FAIL;
    }
    FileHeader fh;
    fh.magic = ETOUI(FileHeaderType::FILEHEADER_MODEL);
    fh.version = 1;
    file.write((char*)&fh, sizeof(fh));

    //---------------------------------------------------Mesh-------------------------------------------------------------------//
    ChunkHeader ch;
    ch.type = ChunkType::CHUNK_MESH;

    uint32_t totalSize = 0;
    totalSize += sizeof(uint32_t);
    for (auto& mesh : Meshes)
    {
        uint32_t vCount = mesh->m_vertices->size();
        uint32_t iCount = mesh->m_indices->size();

        totalSize += sizeof(uint32_t) * 2;
        totalSize += sizeof(VTXMESH) * vCount;
        totalSize += sizeof(uint32_t) * iCount;
    }

    ch.size = totalSize;

    file.write((char*)&ch, sizeof(ch));

    // 기존 코드 그대로
    uint32_t meshCount = Meshes.size();
    file.write((char*)&meshCount, sizeof(uint32_t));

    for (auto& mesh : Meshes)
    {
        WriteMesh(file, mesh);
    }

    //---------------------------------------------------Material-------------------------------------------------------------------//

    ch.type = ChunkType::CHUNK_MATERIAL;
    totalSize = 0;

    totalSize += sizeof(uint32_t); // materialCount

    for (auto& mat : Materials)
    {
        totalSize += sizeof(uint32_t); // materialtype
        totalSize += sizeof(uint32_t); // textureNum
        totalSize += sizeof(TEXTUREINFO) * mat->m_textures.size();
    }

    // ChunkHeader 쓰기
    file.write((char*)&ch, sizeof(ch));

    // Material 개수
    uint32_t materialCount = Materials.size();
    file.write((char*)&materialCount, sizeof(uint32_t));

    // 실제 데이터
    for (auto& mat : Materials)
    {
        WriteMaterial(file, mat);
    }


    file.close();
    return S_OK;
}

void Importer::WriteMesh(ofstream& file, shared_ptr<Mesh> mesh)
{
    // MaterialIndex

    uint32_t vMaterialIndex = mesh->m_materialIndex;
    file.write((char*)&vMaterialIndex, sizeof(uint32_t));

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

void Importer::WriteMaterial(ofstream& file, shared_ptr<Material> mat) {
    // material번호, 텍스쳐 총 타입 카운트, 해당 종류 텍스쳐 카운트, 텍스쳐들 정보
    file.write((char*)&mat->m_materialNum, sizeof(uint32_t));

    uint32_t textureTypeCount = mat->m_textures.size();
    file.write((char*)&textureTypeCount, sizeof(uint32_t));

    for (auto& texs : mat->m_textures)
    {
        uint32_t textureCount = texs.size();
        file.write((char*)&textureCount, sizeof(uint32_t));

        for (auto& tex : texs) {
            file.write((char*)&tex.m_textureType, sizeof(uint32_t));
            file.write((char*)&tex.m_textureNum, sizeof(uint32_t));

            uint32_t len;


            len = tex.File.size();
            file.write((char*)&len, sizeof(uint32_t));
            file.write(tex.File.c_str(), len);


            len = tex.Ext.size();
            file.write((char*)&len, sizeof(uint32_t));
            file.write(tex.Ext.c_str(), len);
        }
    }
}

void Importer::Clear() {
    Meshes.clear();
    Materials.clear();

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