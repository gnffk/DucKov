#include "Model.h"
#include "GameInstance.h"
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
 
    if (FAILED(LoadBin(modelFileName))) {
        MSG_BOX("Bin File Load FAILED");
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


HRESULT Model::LoadBin(const char* modelFileName)
{
    ifstream file(modelFileName, ios::binary);

    if (!file.is_open())
        return E_FAIL;

    FileHeader fh{};
    file.read((char*)&fh, sizeof(fh));

    if (fh.magic != ETOUI(FileHeaderType::FILEHEADER_MODEL))
        return E_FAIL;

    CHUCKHEADER ch{};
    file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(ChunkType::CHUNK_MESH))
        return E_FAIL;

    uint32_t meshCount = 0;
    file.read((char*)&meshCount, sizeof(uint32_t));

    for (uint32_t i = 0; i < meshCount; i++)
    {
        auto vertexes = make_shared<vector<VTXMESH>>();
        auto indices = make_shared<vector<uint32_t>>();

        uint32_t vCount = 0;
        uint32_t iCount = 0;

        file.read((char*)&vCount, sizeof(uint32_t));
        file.read((char*)&iCount, sizeof(uint32_t));

        vertexes->resize(vCount);
        indices->resize(iCount);

        file.read((char*)vertexes->data(), sizeof(VTXMESH) * vCount);
        file.read((char*)indices->data(), sizeof(uint32_t) * iCount);

        m_Meshes.emplace_back(Mesh::Create(m_pDevice, m_pContext, vertexes, indices));
    }

    return S_OK;
}

