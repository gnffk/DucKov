
#include "InstanceModel.h"
#include "GameInstance.h"
#include "Material.h"


InstanceModel::InstanceModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : Component{ pDevice, pContext }
{
}

InstanceModel::~InstanceModel()
{
}

InstanceModel::InstanceModel(const InstanceModel& Prototype)
    : Component{ Prototype }
    , m_iLevelIndex{ Prototype.m_iLevelIndex }
    , m_sModelName{ Prototype.m_sModelName }
    , m_iNumMesh{ Prototype.m_iNumMesh }
    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials }
    , m_eModelType{ Prototype.m_eModelType }
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_iInstanceCapacity{ Prototype.m_iInstanceCapacity }
    , m_isInstanceBufferDirty{ true }
{
    m_Meshes.reserve(Prototype.m_Meshes.size());

    for (auto& pPrototypeMesh : Prototype.m_Meshes)
    {
        if (pPrototypeMesh == nullptr)
            continue;

        auto pClonedMesh =static_pointer_cast<VIBuffer_Mesh_Instance>(pPrototypeMesh->Clone(&m_iInstanceCapacity));

        if (pClonedMesh == nullptr)
            continue;

        m_Meshes.emplace_back(pClonedMesh);
    }

    m_InstanceWorlds.clear();
}

HRESULT InstanceModel::Initialize_Prototype(uint32_t iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName, _fmatrix PreTransformMatrix)
{
    m_iLevelIndex = iLevelIndex;
    m_sModelName = modelName;
    m_eModelType = modeltype;
    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
    if (FAILED(Ready_BinaryModelFile(modelFileName))) {
        MSG_BOX("Failed to Created : BinaryModelFile");
        return E_FAIL;
    }

    return S_OK;

}




HRESULT InstanceModel::Initialize(void* pArg)
{
    m_InstanceWorlds.clear();
    m_isInstanceBufferDirty = true;

    return S_OK;
}


unique_ptr<InstanceModel> InstanceModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iLevelIndex, wstring modelName, uint32_t modeltype, const char* modelFileName, _fmatrix PreTransformMatrix)
{
    auto		pInstance = unique_ptr<InstanceModel>(new InstanceModel(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(iLevelIndex, modelName, modeltype, modelFileName, PreTransformMatrix)))
    {
        MSG_BOX("Failed to Created : InstanceModel");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> InstanceModel::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<InstanceModel>(new InstanceModel(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : InstanceModel");
        return nullptr;
    }

    return pInstance;
}

HRESULT InstanceModel::Render()
{
    if (FAILED(Update_InstanceBuffer()))
        return E_FAIL;

    for (auto& pMesh : m_Meshes)
    {
        if (pMesh == nullptr)
            continue;

        pMesh->Bind_Resources();
        pMesh->Render();
    }

    return S_OK;
}

HRESULT InstanceModel::Render(uint32_t iMeshIndex)
{
    if (iMeshIndex >= m_Meshes.size())
        return E_FAIL;

    if (FAILED(Update_InstanceBuffer()))
        return E_FAIL;

    m_Meshes[iMeshIndex]->Bind_Resources();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}


HRESULT InstanceModel::Bind_Materials(shared_ptr<Shader> pShader, const _char* pConstantName, uint32_t iMeshIndex, uint32_t eMaterialType, uint32_t iTextureIndex)
{
    return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()]->Bind_ShaderResource(pShader, pConstantName, eMaterialType, iTextureIndex);
}


HRESULT InstanceModel::Ready_BinaryModelFile(const char* modelFileName)
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
    }



    file.close();
    return S_OK;
}


HRESULT InstanceModel::Ready_NonAnimMesh(ifstream& _file, const char* modelFileName)
{
    CHUCKHEADER ch{};
    _file.read((char*)&ch, sizeof(ch));

    if (ch.type != ETOUI(CHUNCKTYPE::CHUNK_MESH))
        return E_FAIL;

    uint32_t meshCount = 0;
    _file.read((char*)&meshCount, sizeof(uint32_t));

    m_iNumMesh = meshCount;
    m_Meshes.reserve(meshCount);

    for (uint32_t i = 0; i < meshCount; i++)
    {
        auto vertexes = make_shared<vector<VTXMESH>>();
        auto indices = make_shared<vector<uint32_t>>();

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

        auto pMesh = VIBuffer_Mesh_Instance::Create(m_pDevice,m_pContext,vertexes,indices,materialIndex,XMLoadFloat4x4(&m_PreTransformMatrix));

        if (pMesh == nullptr)
            return E_FAIL;

        m_Meshes.emplace_back(pMesh);
    }

    return S_OK;
}
HRESULT InstanceModel::Ready_NonAnimMaterial(ifstream& _file, const char* modelFileName) {


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


        auto  pMaterial = Material::Create(m_pDevice, m_pContext, textureTypes, modelFileName);
        m_Materials[materialNum] = (pMaterial);


    }


    return S_OK;
}
HRESULT InstanceModel::Add_Instance(const _float3& vPosition)
{
    return Add_Instance(
        vPosition,
        _float3{ 1.f, 1.f, 1.f },
        0.f);
}

HRESULT InstanceModel::Add_Instance(const _float3& vPosition,const _float3& vScale,_float fYaw)
{
    _matrix matScale = XMMatrixScaling(vScale.x,vScale.y,vScale.z);

    _matrix matRot = XMMatrixRotationY(fYaw);

    _matrix matTrans = XMMatrixTranslation(vPosition.x,vPosition.y,vPosition.z);

    _float4x4 World{};
    XMStoreFloat4x4(&World, matScale * matRot * matTrans);

    m_InstanceWorlds.emplace_back(World);
    m_isInstanceBufferDirty = true;

    return S_OK;
}

HRESULT InstanceModel::Remove_Instance_InRadius(const _float3& vCenter,_float fRadius)
{
    const _float fRadiusSq = fRadius * fRadius;

    auto iter = remove_if(
        m_InstanceWorlds.begin(),
        m_InstanceWorlds.end(),
        [&](const _float4x4& World)
        {
            _float x = World._41;
            _float z = World._43;

            _float dx = x - vCenter.x;
            _float dz = z - vCenter.z;

            _float distSq = dx * dx + dz * dz;

            return distSq <= fRadiusSq;
        });

    if (iter != m_InstanceWorlds.end())
    {
        m_InstanceWorlds.erase(iter, m_InstanceWorlds.end());
        m_isInstanceBufferDirty = true;
    }

    return S_OK;
}

HRESULT InstanceModel::Clear_Instances()
{
    m_InstanceWorlds.clear();
    m_isInstanceBufferDirty = true;

    return S_OK;
}

HRESULT InstanceModel::Update_InstanceBuffer()
{
    if (m_isInstanceBufferDirty == false)
        return S_OK;

    for (auto& pMesh : m_Meshes)
    {
        if (pMesh == nullptr)
            continue;

        if (FAILED(pMesh->Update_InstanceBuffer(m_InstanceWorlds)))
            return E_FAIL;
    }

    m_isInstanceBufferDirty = false;

    return S_OK;
}

HRESULT InstanceModel::Save_Instances_JSON(const char* pFilePath)
{
    if (pFilePath == nullptr)
        return E_FAIL;

    nlohmann::json root;

    root["type"] = "TREE_INSTANCE";
    root["version"] = 1;
    root["count"] = static_cast<uint32_t>(m_InstanceWorlds.size());
    root["instances"] = nlohmann::json::array();

    for (const auto& World : m_InstanceWorlds)
    {
        nlohmann::json mat = nlohmann::json::array();

        mat.push_back(World._11);
        mat.push_back(World._12);
        mat.push_back(World._13);
        mat.push_back(World._14);

        mat.push_back(World._21);
        mat.push_back(World._22);
        mat.push_back(World._23);
        mat.push_back(World._24);

        mat.push_back(World._31);
        mat.push_back(World._32);
        mat.push_back(World._33);
        mat.push_back(World._34);

        mat.push_back(World._41);
        mat.push_back(World._42);
        mat.push_back(World._43);
        mat.push_back(World._44);

        root["instances"].push_back(mat);
    }

    std::ofstream file(pFilePath);

    if (!file.is_open())
        return E_FAIL;

    file << root.dump(4);
    file.close();

    return S_OK;
}

HRESULT InstanceModel::Load_Instances_JSON(const char* pFilePath)
{
    if (pFilePath == nullptr)
        return E_FAIL;

    std::ifstream file(pFilePath);

    if (!file.is_open())
        return E_FAIL;

    nlohmann::json root = nlohmann::json::parse(file, nullptr, false);

    if (root.is_discarded())
        return E_FAIL;

    if (!root.contains("type") || root["type"] != "TREE_INSTANCE")
        return E_FAIL;

    if (!root.contains("version") || root["version"] != 1)
        return E_FAIL;

    if (!root.contains("instances") || !root["instances"].is_array())
        return E_FAIL;

    vector<_float4x4> LoadedWorlds;
    LoadedWorlds.reserve(root["instances"].size());

    for (const auto& mat : root["instances"])
    {
        if (!mat.is_array() || mat.size() != 16)
            return E_FAIL;

        for (uint32_t i = 0; i < 16; ++i)
        {
            if (!mat[i].is_number())
                return E_FAIL;
        }

        _float4x4 World{};

        World._11 = mat[0].get<float>();
        World._12 = mat[1].get<float>();
        World._13 = mat[2].get<float>();
        World._14 = mat[3].get<float>();

        World._21 = mat[4].get<float>();
        World._22 = mat[5].get<float>();
        World._23 = mat[6].get<float>();
        World._24 = mat[7].get<float>();

        World._31 = mat[8].get<float>();
        World._32 = mat[9].get<float>();
        World._33 = mat[10].get<float>();
        World._34 = mat[11].get<float>();

        World._41 = mat[12].get<float>();
        World._42 = mat[13].get<float>();
        World._43 = mat[14].get<float>();
        World._44 = mat[15].get<float>();

        LoadedWorlds.emplace_back(World);
    }

    // 빈 파일로 기존 인스턴스를 날리는 것 방지
    if (LoadedWorlds.empty())
        return E_FAIL;

    m_InstanceWorlds = std::move(LoadedWorlds);
    m_isInstanceBufferDirty = true;

    return S_OK;
}