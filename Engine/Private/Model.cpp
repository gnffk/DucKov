
#include "Model.h"
#include "GameInstance.h"
#include "Material.h"

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
    , m_Materials{ Prototype.m_Materials }
{

}


HRESULT Model::Initialize_Prototype(uint32_t iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName)
{
    m_iLevelIndex = iLevelIndex;
    m_sModelName = modelName;
    m_eModelType = modeltype;

    if (FAILED(Ready_BinaryModelFile(modelFileName))) {
        MSG_BOX("Failed to Created : BinaryModelFile");
        return E_FAIL;
    }

    return S_OK;
 
}




HRESULT Model::Initialize(void* pArg)
{
    return S_OK;
}

unique_ptr<Model> Model::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName)
{
    auto		pInstance = unique_ptr<Model>(new Model(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(iLevelIndex, modelName, modeltype, modelFileName)))
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

         break;
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
    m_Materials.reserve(m_iNumMaterials);

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

        for (uint32_t i = 0; i < textureTypeCnt; ++i) {


            vector<TEXTUREINFO> texes;
            uint32_t len;
            uint32_t textureCnt = 0;

            _file.read((char*)&textureCnt, sizeof(uint32_t));
            texes.resize(textureCnt);

            for (uint32_t j = 0; j < textureCnt; ++j) {
                _file.read((char*)&texes[j].m_textureType, sizeof(uint32_t));
                _file.read((char*)&texes[j].m_textureNum, sizeof(uint32_t));

          
                _file.read((char*)&len, sizeof(uint32_t));
                texes[j].File.resize(len);
                _file.read(texes[j].File.data(), len);

           
                _file.read((char*)&len, sizeof(uint32_t));
                texes[j].Ext.resize(len);
                _file.read(texes[j].Ext.data(), len);
            }

            textureTypes.emplace_back(texes);
        }
   
        
        auto  pMaterial = Material::Create(m_pDevice, m_pContext, materialNum, textureTypes,  modelFileName);
        m_Materials.emplace_back(pMaterial);


    }


    return S_OK;
}

HRESULT Model::Ready_AnimMesh(ifstream& _file, const char* modelFileName)
{
    return S_OK;
}

HRESULT Model::Ready_AnimMaterial(ifstream& _file, const char* modelFileName)
{
    return S_OK;
}
