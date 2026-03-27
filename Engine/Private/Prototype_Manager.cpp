#include "Prototype_Manager.h"

Prototype_Manager::Prototype_Manager()
{
}

HRESULT Prototype_Manager::Initialize(uint32_t iNumLevels)
{
	m_pPrototypes = unique_ptr<PROTOTYPES[]>(new PROTOTYPES[iNumLevels]);

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT Prototype_Manager::Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, std::any pPrototype)
{
	if (0 == Find_Prototype(iLevelIndex, strPrototypeTag).has_value())
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, std::move(pPrototype));

	return S_OK;
}

std::any Prototype_Manager::Find_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

unique_ptr<Prototype_Manager> Prototype_Manager::Create(uint32_t iNumLevels)
{
	auto		pInstance = unique_ptr<Prototype_Manager>(new Prototype_Manager());

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : Prototype_Manager");
		return nullptr;
	}

	return pInstance;
}
