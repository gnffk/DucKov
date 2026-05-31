
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
            else if (modelName.rfind("SK_", 0) == 0) {
                basePath += "Skeleton/";
            }
            else if (modelName.rfind("SM_", 0) == 0) {
                basePath += "StaticMesh/";
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

            if (modelType == MODEL::NONANIM) {
                ExportNonAnimBinary(outputPath.c_str());
            }
            if (modelType == MODEL::ANIM) {
                ExportAnimMeshBinary(outputPath.c_str());

                std::string path = outputPath;

                size_t pos = path.find_last_of("/\\");
                std::string directory = path.substr(0, pos + 1);
                std::string filename = path.substr(pos + 1);

                if (filename.rfind("SK_", 0) == 0)
                {
                    filename.replace(0, 3, "AN_");
                }

                std::string newPath = directory + filename;

                ExportAnimationBinary(newPath.c_str());

            }

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

    
    if(modelType == MODEL::NONANIM) {
        if (pScene->HasMeshes()) {

            aiNode* pNode = pScene->mRootNode;
            Meshes.reserve(pScene->mNumMeshes);
            ProcessNonAnimNode(pNode, pScene);

        }


        if (pScene->HasMaterials()) {
            Ready_Material(pScene);
        }
    }


    if(modelType == MODEL::ANIM) {
        if (nullptr != pScene->mRootNode) {
            uint32_t boneMap = 0;

          
            Ready_Bones(pScene->mRootNode, -1);
        }

        if (pScene->HasMeshes()) {

            aiNode* pNode = pScene->mRootNode;
            Meshes.reserve(pScene->mNumMeshes);
            ProcessAnimNode(pNode, pScene);

        }


        if (pScene->HasMaterials()) {
            Ready_Material(pScene);
        }


        if (pScene->HasAnimations()) {
            Ready_Animation(pScene);
        }
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

void Importer::Ready_Bones(const aiNode* pAINode, int32_t iParentBoneIndex)
{
    auto    pBone = make_shared<Bone>();

    pBone->m_name = pAINode->mName.C_Str();

    memcpy(&(pBone->m_TransformationMatrix), &pAINode->mTransformation, sizeof(XMFLOAT4X4));

    pBone->m_patrentBoneIndex = iParentBoneIndex; 

    Bones.push_back(pBone);

    int32_t iParentIndex = Bones.size() - 1;

    for (uint32_t i = 0; i < pAINode->mNumChildren; ++i)
    {
        Ready_Bones(pAINode->mChildren[i], iParentIndex);
    }

}

void Importer::Ready_Animation(const aiScene* scene)
{
    Animations.reserve(scene->mNumAnimations);
    for (size_t i = 0; i < scene->mNumAnimations; i++)
    {

        Animations.emplace_back(make_shared<Animation>());

        Load_Animaion(i, scene->mAnimations[i]);
    }

}

void Importer::Load_Material(aiMaterial* material, uint32_t materialNum)
{
    shared_ptr<Material> fbxmaterial = make_shared<Material>();
    fbxmaterial->m_materialNum = materialNum;
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


    Materials.emplace_back(fbxmaterial);
}

void Importer::ProcessNonAnimMesh(aiMesh* mesh, const aiScene* scene)
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

void Importer::ProcessNonAnimNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessNonAnimMesh(mesh, scene);
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
        ProcessNonAnimNode(node->mChildren[i], scene);
}

void Importer::ProcessAnimMesh(aiMesh* mesh, const aiScene* scene, string name)
{

    string _name;
    
    _name = name;

    //if (mesh->mName.length > 0)
    //    _name = mesh->mName.C_Str();
    //else
    //    _name = "Mesh_" + to_string(++m_index);


    uint32_t _materialIndex;
    XMFLOAT3 _min = { FLT_MAX, FLT_MAX, FLT_MAX };
    XMFLOAT3 _max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    uint32_t m_iNumBones = mesh->mNumBones;
    shared_ptr<vector<uint32_t>> m_Boneindices = make_shared<vector<uint32_t>>();
    shared_ptr<vector<XMFLOAT4X4>> m_BoneMatrices = make_shared<vector<XMFLOAT4X4>>();
    shared_ptr<vector<XMFLOAT4X4>> m_OffsetMatrices = make_shared<vector<XMFLOAT4X4>>();
    shared_ptr<vector<VTXANIMMESH>> vertices = make_shared<vector<VTXANIMMESH>>();
    shared_ptr<vector<uint32_t>> indices = make_shared<vector<uint32_t>>();

    m_Boneindices->resize(m_iNumBones);
    m_BoneMatrices->resize(m_iNumBones);
    m_OffsetMatrices->resize(m_iNumBones);
    shared_ptr<Mesh> fbxmesh = make_shared<Mesh>();





    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        VTXANIMMESH v{};

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



    if (0 == m_iNumBones)
    {
        m_iNumBones = 1;

        int32_t        iBoneIndex = { -1 };

        iBoneIndex = Get_BoneIndex(_name.data());

        if (-1 == iBoneIndex)
            return;

        XMFLOAT4X4       OffsetMatrix;
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

        m_Boneindices->push_back(iBoneIndex);
        m_OffsetMatrices->push_back(OffsetMatrix);
        m_BoneMatrices->resize(iBoneIndex);
    }
    else {
        for (size_t i = 0; i < m_iNumBones; i++)
        {
            aiBone* pAIBone = mesh->mBones[i];

            XMFLOAT4X4   OffsetMatrix;
            memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(XMFLOAT4X4));

            XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
            (*m_OffsetMatrices)[i]=(OffsetMatrix);


            int32_t    iBoneIndex = Get_BoneIndex(pAIBone->mName.C_Str());
            if (-1 == iBoneIndex)
                return;

            (*m_Boneindices)[i]= (iBoneIndex);

            for (size_t j = 0; j < pAIBone->mNumWeights; j++)
            {
                if (0 == (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
                {
                    (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
                    (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
                }

                else if (0 == (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
                {
                    (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
                    (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
                }

                else if (0 == (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
                {
                    (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
                    (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
                }

                else if (0 == (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
                {
                    (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
                    (*vertices)[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
                }
            }
        }
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

   
    fbxmesh->Set_AnimMesh(_name, _materialIndex, _min, _max, vertices, indices);
    fbxmesh->m_iNumBones = m_iNumBones;
    fbxmesh->m_BoneIndices = m_Boneindices;
    fbxmesh->m_BoneMatrices = m_BoneMatrices;
    fbxmesh->m_OffsetMatrices = m_OffsetMatrices;


    Meshes.emplace_back(fbxmesh);


}

void Importer::ProcessAnimNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        string name = node->mName.C_Str();
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessAnimMesh(mesh, scene, name);
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
        ProcessAnimNode(node->mChildren[i], scene);
}

void Importer::Load_Animaion(uint32_t iAnimaionCount, const aiAnimation* pAIAnimation)
{
    Animations[iAnimaionCount]->m_AnimationData.AnimationDuration = pAIAnimation->mDuration;
    Animations[iAnimaionCount]->m_AnimationData.AnimtaionTickPerSecond = pAIAnimation->mTicksPerSecond;
    Animations[iAnimaionCount]->m_AnimationData.ChannelCount = pAIAnimation->mNumChannels;

    Animations[iAnimaionCount]->m_AnimationData.Channels = make_shared<vector<CHANNELDATA>>();
    Animations[iAnimaionCount]->m_AnimationData.Channels->reserve(pAIAnimation->mNumChannels);


    for (size_t i = 0; i < pAIAnimation->mNumChannels; i++)
    {
        CHANNELDATA ChanelData;

        Load_Channel(ChanelData, pAIAnimation->mChannels[i]);
        Animations[iAnimaionCount]->m_AnimationData.Channels->emplace_back(ChanelData);
    }

}

void Importer::Load_Channel(CHANNELDATA& ChannelData, const aiNodeAnim* pAIChannel)
{
    ChannelData.BoneIndex = Get_BoneIndex(pAIChannel->mNodeName.C_Str());
    if (-1 == ChannelData.BoneIndex) {
        MSG_BOX("Chanel FAILED LOAD");
        return;
    }
        

    ChannelData.KeyFrameCount = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    ChannelData.KeyFrameCount = max(ChannelData.KeyFrameCount, pAIChannel->mNumPositionKeys);

    XMFLOAT3     vScale = {};
    XMFLOAT4     vRotation = {};
    XMFLOAT3     vTranslation = {};

    ChannelData.KeyFrames = make_shared<vector< KEYFRAME>>();

    ChannelData.KeyFrames->reserve(ChannelData.KeyFrameCount);

    for (size_t i = 0; i < ChannelData.KeyFrameCount; i++)
    {
        KEYFRAME            KeyFrame = {};

        if (i < pAIChannel->mNumScalingKeys) // 만약에 더 큰 값이 들어올떄 그 전의 값으로 마지막껄 채워준다.
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof vScale);
            KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
        }

        if (i < pAIChannel->mNumRotationKeys)
        {
            // memcpy(&vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof vRotation);
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
            KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
        }

        if (i < pAIChannel->mNumPositionKeys)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof vTranslation);
            KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;

        ChannelData.KeyFrames->emplace_back(KeyFrame);
    }

}

HRESULT Importer::ExportNonAnimBinary(const char* filePath)
{
    ofstream file(filePath, ios::binary);

    if (!file.is_open()) {
        MSG_BOX("no file path");
        return E_FAIL;
    }
    FileHeader fh;
    fh.magic = ETOUI(FILEHEADERTYPE::FILEHEADER_MODEL);
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
        WriteNonAnimMesh(file, mesh);
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

void Importer::WriteNonAnimMesh(ofstream& file, shared_ptr<Mesh> mesh)
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

HRESULT Importer::ExportAnimMeshBinary(const char* filePath)
{
    ofstream file(filePath, ios::binary);

    if (!file.is_open()) {
        MSG_BOX("no file path");
        return E_FAIL;
    }
    FileHeader fh;
    fh.magic = ETOUI(FILEHEADERTYPE::FILEHEADER_MODEL);
    fh.version = 1;
    file.write((char*)&fh, sizeof(fh));
    //-------------------------------------------------Bone----------------------------------------------------------------------//
    ChunkHeader ch;
    ch.type = ChunkType::CHUNK_SKELETON;
    file.write((char*)&ch, sizeof(ch));

    uint32_t BoneCount = Bones.size();
    file.write((char*)&BoneCount, sizeof(uint32_t));

    for (auto& bone : Bones)
    {
        Writebone(file, bone);
    }

    //---------------------------------------------------Mesh-------------------------------------------------------------------//

    ch.type = ChunkType::CHUNK_MESH;



    file.write((char*)&ch, sizeof(ch));

    // 기존 코드 그대로
    uint32_t meshCount = Meshes.size();
    file.write((char*)&meshCount, sizeof(uint32_t));

    for (auto& mesh : Meshes)
    {
        WriteAnimMesh(file, mesh);
    }

    //---------------------------------------------------Material-------------------------------------------------------------------//

    ch.type = ChunkType::CHUNK_MATERIAL;

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

HRESULT Importer::ExportAnimationBinary(const char* filePath)
{
    ofstream file(filePath, ios::binary);

    if (!file.is_open()) {
        MSG_BOX("no file path");
        return E_FAIL;
    }
    FileHeader fh;
    fh.magic = ETOUI(FILEHEADERTYPE::FILEHEADER_ANIMATION);
    fh.version = 1;
    file.write((char*)&fh, sizeof(fh));

    //---------------------------------------------------Animaiton-------------------------------------------------------------------//
    ChunkHeader ch;
    ch.type = ChunkType::CHUNK_ANIMATION;

    // ChunkHeader 쓰기
    file.write((char*)&ch, sizeof(ch));

    // Animation 개수
    uint32_t AnimCount = Animations.size();
    file.write((char*)&AnimCount, sizeof(uint32_t));

    // 실제 데이터
    for (auto& Anim : Animations)
    {
        WriteAnimation(file, Anim);
    }


    file.close();
    return S_OK;
}

void Importer::WriteAnimMesh(ofstream& file, shared_ptr<Mesh> mesh)
{
    // MaterialIndex
    uint32_t vMaterialIndex = mesh->m_materialIndex;
    file.write((char*)&vMaterialIndex, sizeof(uint32_t));

    // Vertex Count
    uint32_t vCount = mesh->m_animvertices->size();
    file.write((char*)&vCount, sizeof(uint32_t));

    // Index Count
    uint32_t iCount = mesh->m_indices->size();
    file.write((char*)&iCount, sizeof(uint32_t));

    // Vertex 데이터
    file.write((char*)mesh->m_animvertices->data(), sizeof(VTXANIMMESH) * vCount);

    // Index 데이터
    file.write((char*)mesh->m_indices->data(), sizeof(uint32_t) * iCount);


    // Mesh가 이용하는 뼈의 개수
    file.write((char*)&mesh->m_iNumBones, sizeof(uint32_t));

    // BoneIndices Count
    uint32_t BoneIndicesCount = mesh->m_BoneIndices->size();
    file.write((char*)&BoneIndicesCount, sizeof(uint32_t));

    // BoneMatrices Count
    uint32_t BoneMatricesCount = mesh->m_BoneMatrices->size();
    file.write((char*)&BoneMatricesCount, sizeof(uint32_t));

    // OffsetMatrices Count
    uint32_t OffsetMatricesCount = mesh->m_OffsetMatrices->size();
    file.write((char*)&OffsetMatricesCount, sizeof(uint32_t));

    // BoneIndices 데이터
    file.write((char*)mesh->m_BoneIndices->data(), sizeof(uint32_t) * BoneIndicesCount);

    // BoneMatrices 데이터
    file.write((char*)mesh->m_BoneMatrices->data(), sizeof(XMFLOAT4X4) * BoneMatricesCount);

    // OffsetMatrices 데이터
    file.write((char*)mesh->m_OffsetMatrices->data(), sizeof(XMFLOAT4X4) * OffsetMatricesCount);



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

void Importer::Writebone(ofstream& file, shared_ptr<Bone> bone)
{
    uint32_t len;
    len = bone->m_name.size();
    file.write((char*)&len, sizeof(uint32_t));
    file.write(bone->m_name.c_str(), len);
    file.write((char*)(&bone->m_TransformationMatrix), sizeof(XMFLOAT4X4));
    file.write((char*)&bone->m_patrentBoneIndex, sizeof(uint32_t));

}

void Importer::WriteAnimation(ofstream& file, shared_ptr<Animation> Animation)
{
    float AnimationDuration = Animation->m_AnimationData.AnimationDuration;
    float AnimtaionTickPerSecond = Animation->m_AnimationData.AnimtaionTickPerSecond;

    file.write((char*)&AnimationDuration, sizeof(float));
    file.write((char*)&AnimtaionTickPerSecond, sizeof(float));

    uint32_t  ChannelCount = Animation->m_AnimationData.ChannelCount;
    file.write((char*)&ChannelCount, sizeof(uint32_t));

    for (int i = 0; i < Animation->m_AnimationData.ChannelCount; ++i) {
        int32_t  BoneIndex = (*Animation->m_AnimationData.Channels)[i].BoneIndex;
        uint32_t  KeyFrameCount = (*Animation->m_AnimationData.Channels)[i].KeyFrameCount;

        file.write((char*)&BoneIndex, sizeof(int32_t));
        file.write((char*)&KeyFrameCount, sizeof(uint32_t));

        for (int j = 0; j < (*Animation->m_AnimationData.Channels)[i].KeyFrameCount; ++j) {
            XMFLOAT3  vScale = (*(*Animation->m_AnimationData.Channels)[i].KeyFrames)[j].vScale;
            XMFLOAT4  vRotation = (*(*Animation->m_AnimationData.Channels)[i].KeyFrames)[j].vRotation;
            XMFLOAT3  vTranslation = (*(*Animation->m_AnimationData.Channels)[i].KeyFrames)[j].vTranslation;
            float  fTrackPosition = (*(*Animation->m_AnimationData.Channels)[i].KeyFrames)[j].fTrackPosition;


            file.write((char*)&vScale, sizeof(XMFLOAT3));
            file.write((char*)&vRotation, sizeof(XMFLOAT4));
            file.write((char*)&vTranslation, sizeof(XMFLOAT3));
            file.write((char*)&fTrackPosition, sizeof(float));
          
        }
    }
}

void Importer::Clear() {
    Bones.clear();
    Meshes.clear();
    Materials.clear();
    Animations.clear();
}


int32_t Importer::Get_BoneIndex(const char* pBoneName)
{
    int32_t iBoneIndex = { 0 };
    auto    iter = find_if(Bones.begin(), Bones.end(), [&](shared_ptr<Bone> pBone)->bool
        {
            if (true == pBone->Compare_Name(pBoneName))
                return true;

            ++iBoneIndex;

            return false;
        });

    if (iter == Bones.end())
        return -1;

    return iBoneIndex;
}