#include "Mesh.h"
#include "Mesh.h"
#include "GameInstance.h"



Mesh::Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : Component{pDevice, pContext}
{
}



Mesh::~Mesh()
{
}


HRESULT Mesh::Initialize_Prototype(shared_ptr<vector<VTXTEX>> pvertices, shared_ptr<vector<uint16_t>> pindices, wstring ModelName, uint32_t m_iLevelIndex)
{
    std::wstring prototypeTag = L"Prototype_Component_VIBuffer_Mesh_" + ModelName;

    if (FAILED(CGameInstance::Get().Add_Prototype(m_iLevelIndex, prototypeTag, VIBuffer_Mesh::Create(m_pDevice, m_pContext, pvertices, pindices)))) {
        return E_FAIL;
    }
    return S_OK;
}


HRESULT Mesh::Initialize(void* pArg)
{
	return S_OK;
}

unique_ptr<Mesh> Mesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, shared_ptr<vector<VTXTEX>> pvertices, shared_ptr<vector<uint16_t>> pindices, wstring ModelName, uint32_t m_iLevelIndex)
{
    auto		pInstance = unique_ptr<Mesh>(new Mesh(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(pvertices, pindices, ModelName, m_iLevelIndex)))
    {
        MSG_BOX("Failed to Created : Mesh");
        return nullptr;
    }

    return pInstance;
}

shared_ptr<Prototype> Mesh::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<Mesh>(new Mesh(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : Mesh");
        return nullptr;
    }

    return pInstance;
}
