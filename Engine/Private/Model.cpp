
#include "Model.h"
#include "GameInstance.h"
#include "Material.h"
#include "Bone.h"
#include "Animation.h"
#include "Channel.h"

Model::Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : Component{ pDevice, pContext }
{
}

Model::~Model()
{
}

Model::Model(const Model& Prototype)
    : Component{ Prototype }
    , m_iNumMesh{ Prototype.m_iNumMesh }
    , m_Meshes{ Prototype.m_Meshes }
    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_iNumAnimations{ Prototype.m_iNumAnimations }
    , m_Materials{ Prototype.m_Materials }
    , m_eModelType{ Prototype.m_eModelType }
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
{
    m_Bones.reserve(Prototype.m_iBoneCount);

    for (auto& pPrototypeBone : Prototype.m_Bones)
    {
        m_Bones.emplace_back(pPrototypeBone->Clone());
    }

    m_Animations.reserve(Prototype.m_iNumAnimations);
    for (auto& pPrototypeAnim : Prototype.m_Animations)
    {
        m_Animations.emplace_back(pPrototypeAnim->Clone());
    }


}


HRESULT Model::Initialize_Prototype(uint32_t iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName, _fmatrix PreTransformMatrix)
{
    m_iLevelIndex = iLevelIndex;
    m_sModelName = modelName;
    m_eModelType = modeltype;
    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
    if (FAILED(Ready_BinaryModelFile(modelFileName))) {
        MSG_BOX("Failed to Created : BinaryModelFile");
        return E_FAIL;
    }

    if (FAILED(Ready_BinaryAnimationFile(modelFileName))) {
        MSG_BOX("Failed to Created : BinaryAnimationFile");
        return E_FAIL;
    }

    return S_OK;
 
}




HRESULT Model::Initialize(void* pArg)
{
    return S_OK;
}

unique_ptr<Model> Model::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName, _fmatrix PreTransformMatrix)
{
     auto		pInstance = unique_ptr<Model>(new Model(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(iLevelIndex, modelName, modeltype, modelFileName, PreTransformMatrix)))
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

HRESULT Model::Render()
{
    for (auto& pMesh : m_Meshes)
    {
        pMesh->Bind_Resources();
        pMesh->Render();
    }

    return S_OK;
}

HRESULT Model::Render(uint32_t iMeshIndex)
{
    m_Meshes[iMeshIndex]->Bind_Resources();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}

_bool Model::Play_Animation(_float fTimeDelta)
{
    _bool           isFinished = { false };

    /* 뼈들의 m_TransformationMatrix를 갱신해준다. */
    isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_Bones, m_isAnimLoop);

    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }

    return isFinished;

}

HRESULT Model::Bind_BoneMatrices(shared_ptr<class Shader> pShader, const _char* pConstantName, uint32_t iMeshIndex)
{
    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(m_Bones, pShader, pConstantName);
}

HRESULT Model::Bind_Materials(shared_ptr<Shader> pShader, const _char* pConstantName, uint32_t iMeshIndex, uint32_t eMaterialType, uint32_t iTextureIndex)
{
    return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()]->Bind_ShaderResource(pShader, pConstantName, eMaterialType, iTextureIndex);
}


HRESULT Model::Ready_BinaryModelFile(const char* modelFileName)
{
    ifstream file(modelFileName, ios::binary);

    if (!file.is_open())
        return E_FAIL;

    FileHeader fh{};
    file.read((char*)&fh, sizeof(fh));

     if (fh.magic != ETOUI(FILEHEADERTYPE::FILEHEADER_MODEL))
        return E_FAIL;


     switch (m_eModelType) {
     case ETOUI(MODELTYPE::NONANIM):
     {
         if (FAILED(Ready_NonAnimMesh(file, modelFileName))) {
             MSG_BOX("Ready_NonAnimMesh Load FAILED");
             file.close();
             return E_FAIL;
         }

         if (FAILED(Ready_NonAnimMaterial(file, modelFileName))) {
             MSG_BOX("Ready_NonAnimMaterial Load FAILED");
             file.close();
             return E_FAIL;
         }
     }
      
         break;

     case ETOUI(MODELTYPE::ANIM):
     {
         if (FAILED(Ready_AnimBone(file, modelFileName))) {
             MSG_BOX("Ready_AnimBone Load FAILED");
             file.close();
             return E_FAIL;
         }

         if (FAILED(Ready_AnimMesh(file, modelFileName))) {
             MSG_BOX("Ready_AnimMesh Load FAILED");
             file.close();
             return E_FAIL;
         }

         if (FAILED(Ready_AnimMaterial(file, modelFileName))) {
             MSG_BOX("Ready_AnimMaterial Load FAILED");
             file.close();
             return E_FAIL;
         }
     }
         break;
     }

  

    file.close();
    return S_OK;
}

HRESULT Model::Ready_BinaryAnimationFile(const char* modelFileName)
{
    if (m_eModelType != ETOUI(MODELTYPE::ANIM))
        return S_OK;

    std::string path = modelFileName;

    size_t pos = path.find_last_of("/\\");
    std::string directory = path.substr(0, pos + 1);
    std::string filename = path.substr(pos + 1);

    if (filename.rfind("SK_", 0) == 0)
    {
        filename.replace(0, 3, "AN_");
    }

    std::string newPath = directory + filename;


    ifstream file(newPath, ios::binary);

    if (!file.is_open())
        return E_FAIL;

    FileHeader fh{};
    file.read((char*)&fh, sizeof(fh));

    if (fh.magic != ETOUI(FILEHEADERTYPE::FILEHEADER_ANIMATION))
        return E_FAIL;

    CHUCKHEADER ch{};
    file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(CHUNCKTYPE::CHUNK_ANIMATION))
        return E_FAIL;


    uint32_t iAnimationCount = 0;
    file.read((char*)&iAnimationCount, sizeof(uint32_t));
    m_iNumAnimations = iAnimationCount;
    m_Animations.reserve(iAnimationCount);


    for (uint32_t i = 0; i < m_iNumAnimations; ++i) {
        shared_ptr<Animation> pAnimation = Animation::Create(file);
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.emplace_back(pAnimation);
    }

    file.close();

    return S_OK;
}

HRESULT Model::Ready_NonAnimMesh(ifstream& _file, const char* modelFileName)
{
   

    CHUCKHEADER ch{};
    _file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(CHUNCKTYPE::CHUNK_MESH))
        return E_FAIL;

    uint32_t meshCount = 0;
    _file.read((char*)&meshCount, sizeof(uint32_t));
    m_iNumMesh = meshCount;
    for (uint32_t i = 0; i < meshCount; i++)
    {
        auto vertexes = make_shared<vector<VTXMESH>>();
        auto indices = make_shared<vector<uint32_t>>();

        // MaterialIndex

        uint32_t materialIndex = 0;
        _file.read((char*)&materialIndex, sizeof(uint32_t));

        uint32_t vCount = 0;
        uint32_t iCount = 0;

        _file.read((char*)&vCount, sizeof(uint32_t));
        _file.read((char*)&iCount, sizeof(uint32_t));

        vertexes->resize(vCount);
        indices->resize(iCount);

        _file.read((char*)vertexes->data(), sizeof(VTXMESH) * vCount);
        _file.read((char*)indices->data(), sizeof(uint32_t) * iCount);

        m_Meshes.emplace_back(Mesh::Create(m_pDevice, m_pContext, vertexes, indices, materialIndex));
    }

    return S_OK;
}

HRESULT Model::Ready_NonAnimMaterial(ifstream& _file, const char* modelFileName) {


    CHUCKHEADER ch{};
    _file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(CHUNCKTYPE::CHUNK_MATERIAL))
        return E_FAIL;

    uint32_t materialCount = 0;
    _file.read((char*)&materialCount, sizeof(uint32_t));
    m_iNumMaterials = materialCount;
    m_Materials.resize(m_iNumMaterials);

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        // material번호, 텍스쳐 총 타입 카운트, 해당 종류 텍스쳐 카운트, 텍스쳐들 정보

        uint32_t materialNum = 0;
        uint32_t textureTypeCnt = 0;
        vector<vector<TEXTUREINFO>> textureTypes;
        textureTypes.clear();
        _file.read((char*)&materialNum, sizeof(uint32_t));
        _file.read((char*)&textureTypeCnt, sizeof(uint32_t));

        textureTypes.reserve(textureTypeCnt);

        for (uint32_t j = 0; j < textureTypeCnt; ++j) {


            vector<TEXTUREINFO> texes;
            uint32_t len;
            uint32_t textureCnt = 0;

            _file.read((char*)&textureCnt, sizeof(uint32_t));
            texes.resize(textureCnt);

            for (uint32_t o = 0; o < textureCnt; ++o) {
                _file.read((char*)&texes[o].m_textureType, sizeof(uint32_t));
                _file.read((char*)&texes[o].m_textureNum, sizeof(uint32_t));

          
                _file.read((char*)&len, sizeof(uint32_t));
                texes[o].File.resize(len);
                _file.read(texes[o].File.data(), len);

           
                _file.read((char*)&len, sizeof(uint32_t));
                texes[o].Ext.resize(len);
                _file.read(texes[o].Ext.data(), len);
            }

            textureTypes.emplace_back(texes);
        }
   
        
        auto  pMaterial = Material::Create(m_pDevice, m_pContext, textureTypes,  modelFileName);
        m_Materials[materialNum] = (pMaterial);


    }


    return S_OK;
}

HRESULT Model::Ready_AnimBone(ifstream& _file, const char* modelFileName)
{
    CHUCKHEADER ch{};
    _file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(CHUNCKTYPE::CHUNK_SKELETON))
        return E_FAIL;

    uint32_t iBoneCount = 0;
    _file.read((char*)&iBoneCount, sizeof(uint32_t));
    m_iBoneCount = iBoneCount;
    m_Bones.reserve(iBoneCount);



    for (uint32_t i = 0; i < iBoneCount; ++i) {
        uint32_t BoneNamesize = 0;
        _float4x4 BoneMatrix;
        uint32_t ParentBoneIndex = 0;

        XMStoreFloat4x4(&BoneMatrix, XMMatrixIdentity());

        _file.read((char*)&BoneNamesize, sizeof(uint32_t));

        _char* BoneName = new _char[BoneNamesize + 1];

        _file.read(BoneName, BoneNamesize);
        BoneName[BoneNamesize] = '\0';

        _file.read((char*)&BoneMatrix, sizeof(_float4x4));
        _file.read((char*)&ParentBoneIndex, sizeof(uint32_t));

        m_Bones.emplace_back(
            Bone::Create(BoneName, BoneMatrix, ParentBoneIndex)
        );

        delete[] BoneName;

    }

    return S_OK;
}

HRESULT Model::Ready_AnimMesh(ifstream& _file, const char* modelFileName)
{
    CHUCKHEADER ch{};
    _file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(CHUNCKTYPE::CHUNK_MESH))
        return E_FAIL;

    uint32_t meshCount = 0;
    _file.read((char*)&meshCount, sizeof(uint32_t));
    m_iNumMesh = meshCount;
    for (uint32_t i = 0; i < meshCount; i++)
    {
        auto vertexes = make_shared<vector<VTXANIMMESH>>();
        auto indices = make_shared<vector<uint32_t>>();

        // MaterialIndex

        uint32_t materialIndex = 0;
        _file.read((char*)&materialIndex, sizeof(uint32_t));

        uint32_t vCount = 0;
        uint32_t iCount = 0;

        _file.read((char*)&vCount, sizeof(uint32_t));
        _file.read((char*)&iCount, sizeof(uint32_t));

        vertexes->resize(vCount);
        indices->resize(iCount);

        _file.read((char*)vertexes->data(), sizeof(VTXANIMMESH) * vCount);
        _file.read((char*)indices->data(), sizeof(uint32_t) * iCount);

        uint32_t numBones = 0;
        _file.read((char*)&numBones, sizeof(uint32_t));


        uint32_t numBonesIndices = 0;
        uint32_t numBonesMatrices = 0;
        uint32_t numBonesOffsetMatices = 0;
        _file.read((char*)&numBonesIndices, sizeof(uint32_t));
        _file.read((char*)&numBonesMatrices, sizeof(uint32_t));
        _file.read((char*)&numBonesOffsetMatices, sizeof(uint32_t));

        vector<uint32_t> BonesIndices;
        vector<_float4x4> BonesMatrices;
        vector<_float4x4> BonesOffsetMatices;
    
        BonesIndices.resize(numBonesIndices);
        BonesMatrices.resize(numBonesMatrices);
        BonesOffsetMatices.resize(numBonesOffsetMatices);

        _file.read((char*)BonesIndices.data(), sizeof(uint32_t) * numBonesIndices);
        _file.read((char*)BonesMatrices.data(), sizeof(_float4x4) * numBonesMatrices);
        _file.read((char*)BonesOffsetMatices.data(), sizeof(_float4x4) * numBonesOffsetMatices);

        m_Meshes.emplace_back(Mesh::Create(m_pDevice, m_pContext, vertexes, indices, materialIndex, numBones, BonesIndices, BonesMatrices, BonesOffsetMatices));
    }

    return S_OK;
}

HRESULT Model::Ready_AnimMaterial(ifstream& _file, const char* modelFileName)
{
    CHUCKHEADER ch{};
    _file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(CHUNCKTYPE::CHUNK_MATERIAL))
        return E_FAIL;

    uint32_t materialCount = 0;
    _file.read((char*)&materialCount, sizeof(uint32_t));
    m_iNumMaterials = materialCount;
    m_Materials.resize(m_iNumMaterials);

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        // material번호, 텍스쳐 총 타입 카운트, 해당 종류 텍스쳐 카운트, 텍스쳐들 정보

        uint32_t materialNum = 0;
        uint32_t textureTypeCnt = 0;
        vector<vector<TEXTUREINFO>> textureTypes;
   
        textureTypes.clear();
        _file.read((char*)&materialNum, sizeof(uint32_t));
        _file.read((char*)&textureTypeCnt, sizeof(uint32_t));

        textureTypes.reserve(TextureType_END);
        for (uint32_t j = 0; j < textureTypeCnt; ++j) {

            vector<TEXTUREINFO> texes;
            uint32_t len;
            uint32_t textureCnt = 0;
         
            _file.read((char*)&textureCnt, sizeof(uint32_t));
            texes.resize(textureCnt);

            for (uint32_t o = 0; o < textureCnt; ++o) {

                _file.read((char*)&texes[o].m_textureType, sizeof(uint32_t));
                _file.read((char*)&texes[o].m_textureNum, sizeof(uint32_t));


                _file.read((char*)&len, sizeof(uint32_t));
                texes[o].File.resize(len);
                _file.read(texes[o].File.data(), len);


                _file.read((char*)&len, sizeof(uint32_t));
                texes[o].Ext.resize(len);
                _file.read(texes[o].Ext.data(), len);
            }

            

            textureTypes.emplace_back(texes);
        }


        auto  pMaterial = Material::Create(m_pDevice, m_pContext, textureTypes, modelFileName);
        m_Materials[materialNum] = (pMaterial);


    }


    return S_OK;
}

