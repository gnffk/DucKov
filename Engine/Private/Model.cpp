
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
    , m_Meshes{ Prototype.m_Meshes }
{

}


HRESULT Model::Initialize_Prototype(uint32_t iLevelIndex, wstring modelName, const char* modelFileName)
{
    m_iLevelIndex = iLevelIndex;
    m_sModelName = modelName;


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

unique_ptr<Model> Model::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iLevelIndex, wstring modelName, const char* modelFileName)
{
    auto		pInstance = unique_ptr<Model>(new Model(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(iLevelIndex, modelName, modelFileName)))
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


HRESULT Model::Ready_BinaryModelFile(const char* modelFileName)
{
    ifstream file(modelFileName, ios::binary);

    if (!file.is_open())
        return E_FAIL;

    FileHeader fh{};
    file.read((char*)&fh, sizeof(fh));

    if (fh.magic != ETOUI(FileHeaderType::FILEHEADER_MODEL))
        return E_FAIL;




    if (FAILED(Ready_Mesh(file, modelFileName))) {
        MSG_BOX("Mesh Load FAILED");
        file.close();
        return E_FAIL;
    }

    if (FAILED(Ready_Material(file, modelFileName))) {
        MSG_BOX("Material Load FAILED");
        file.close();
        return E_FAIL;
    }

    file.close();
    return S_OK;
}

HRESULT Model::Ready_Mesh(ifstream& _file, const char* modelFileName)
{
   

    CHUCKHEADER ch{};
    _file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(ChunkType::CHUNK_MESH))
        return E_FAIL;

    uint32_t meshCount = 0;
    _file.read((char*)&meshCount, sizeof(uint32_t));

    for (uint32_t i = 0; i < meshCount; i++)
    {
        auto vertexes = make_shared<vector<VTXMESH>>();
        auto indices = make_shared<vector<uint32_t>>();

        uint32_t vCount = 0;
        uint32_t iCount = 0;

        _file.read((char*)&vCount, sizeof(uint32_t));
        _file.read((char*)&iCount, sizeof(uint32_t));

        vertexes->resize(vCount);
        indices->resize(iCount);

        _file.read((char*)vertexes->data(), sizeof(VTXMESH) * vCount);
        _file.read((char*)indices->data(), sizeof(uint32_t) * iCount);

        m_Meshes.emplace_back(Mesh::Create(m_pDevice, m_pContext, vertexes, indices));
    }

    return S_OK;
}

HRESULT Model::Ready_Material(ifstream& _file, const char* modelFileName) {


    CHUCKHEADER ch{};
    _file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(ChunkType::CHUNK_MATERIAL))
        return E_FAIL;

    uint32_t materialCount = 0;
    _file.read((char*)&materialCount, sizeof(uint32_t));
    m_iNumMaterials = materialCount;
    m_Materials.reserve(m_iNumMaterials);

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {

        uint32_t materialtype = 0;
        uint32_t textureSize = 0;
        vector<TEXTUREINFO> textures;
        textures.clear();
        _file.read((char*)&materialtype, sizeof(uint32_t));
        _file.read((char*)&textureSize, sizeof(uint32_t));



        textures.reserve(textureSize);

        for (uint32_t j = 0; j < textureSize; ++j)
        {
            TEXTUREINFO tex;
            uint32_t len;
            uint32_t m_textureType;
            uint32_t m_textureNum;

            _file.read((char*)&m_textureType, sizeof(uint32_t));
            _file.read((char*)&m_textureNum, sizeof(uint32_t));

            // FilePath
            _file.read((char*)&len, sizeof(uint32_t));
            tex.File.resize(len);
            _file.read(tex.File.data(), len);

            // FileName
            _file.read((char*)&len, sizeof(uint32_t));
            tex.Ext.resize(len);
            _file.read(tex.Ext.data(), len);

            textures.emplace_back(tex);
        }

        
        auto  pMaterial = Material::Create(m_pDevice, m_pContext, materialtype, textures,  modelFileName);
        m_Materials.emplace_back(pMaterial);


    }


    return S_OK;
}